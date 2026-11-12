import 'dotenv/config';
import express from 'express';
import session from 'express-session';
import helmet from 'helmet';
import multer from 'multer';
import fs from 'node:fs';
import path from 'node:path';
import {db} from './db.js';
import {SqliteSessionStore} from './session-store.js';
import {ensureAdmin,login,csrf,requireAuth,requireCsrf} from './auth.js';
import {parseWigle} from './wigle.js';
import {rebuildAggregates} from './aggregate.js';
import {idSchema,importLabelSchema,leadPatchSchema,loginSchema} from './validation.js';

if(process.env.NODE_ENV==='production'&&(!process.env.ADMIN_PASSWORD_BCRYPT||!process.env.SESSION_SECRET))throw new Error('Production requires ADMIN_PASSWORD_BCRYPT and SESSION_SECRET');
ensureAdmin();
rebuildAggregates();
const app=express();
const port=Number(process.env.PORT||8787);
const dataDir=path.dirname(process.env.FIBERSCOUT_DB||'./data/fiberscout-v11.sqlite');
fs.mkdirSync(dataDir,{recursive:true});
app.set('trust proxy',process.env.TRUST_PROXY==='1'?1:false);
app.use(helmet({contentSecurityPolicy:{directives:{defaultSrc:["'self'"],baseUri:["'self'"],fontSrc:["'self'","data:"],imgSrc:["'self'","data:","blob:","https://*.tile.openstreetmap.org"],scriptSrc:["'self'"],styleSrc:["'self'","'unsafe-inline'"],connectSrc:["'self'"],objectSrc:["'none'"],frameAncestors:["'none'"],formAction:["'self'"]}}}));
app.use(express.json({limit:'100kb'}));app.use(express.urlencoded({extended:false,limit:'20kb'}));
app.use(session({store:new SqliteSessionStore(db),secret:process.env.SESSION_SECRET||'development-only-change-me',resave:false,saveUninitialized:false,cookie:{httpOnly:true,sameSite:'lax',secure:process.env.NODE_ENV==='production'||process.env.TRUST_PROXY==='1',maxAge:8*60*60*1000}}));

type Rate={count:number;first:number};const loginRates=new Map<string,Rate>();const WINDOW=15*60*1000,MAX_FAILURES=5;
function clientKey(req:express.Request){return req.ip||'unknown';}
function rateCheck(key:string){const now=Date.now(),r=loginRates.get(key);if(!r||now-r.first>WINDOW){loginRates.set(key,{count:0,first:now});return true;}return r.count<MAX_FAILURES;}
function rateFail(key:string){const r=loginRates.get(key)||{count:0,first:Date.now()};r.count++;loginRates.set(key,r);}
function rateClear(key:string){loginRates.delete(key);}
function bad(res:express.Response,message:string,status=400){return res.status(status).json({error:message});}
function parsedId(value:string|string[]){const x=idSchema.safeParse(Array.isArray(value)?value[0]:value);return x.success?x.data:null;}

app.post('/api/login',(req,res)=>{const parsed=loginSchema.safeParse(req.body);if(!parsed.success)return bad(res,'invalid login request');const key=clientKey(req);if(!rateCheck(key))return res.status(429).set('Retry-After','900').json({error:'too many login attempts'});if(!login(parsed.data.username,parsed.data.password)){rateFail(key);return bad(res,'invalid credentials',401);}rateClear(key);req.session.regenerate(err=>{if(err)return bad(res,'session error',500);req.session.user=parsed.data.username;csrf(req);res.json({ok:true});});});
app.post('/api/logout',requireAuth,requireCsrf,(req,res)=>req.session.destroy(err=>err?bad(res,'logout failed',500):res.status(204).end()));
app.get('/api/session',(req,res)=>res.json({authenticated:!!req.session.user}));
const api=[requireAuth,requireCsrf];
app.get('/api/csrf',...api,(req,res)=>res.json({csrf:csrf(req)}));
app.get('/api/overview',...api,(_req,res)=>{const q=(s:string)=>((db.prepare(s).get() as any)?.n||0);res.json({surveySessions:q('SELECT COUNT(*) n FROM survey_sessions'),observations:q('SELECT COUNT(*) n FROM wifi_observations'),areas:q('SELECT COUNT(*) n FROM neighborhood_aggregates'),canvassingSessions:q('SELECT COUNT(*) n FROM canvassing_sessions'),leads:q('SELECT COUNT(*) n FROM opt_in_leads WHERE deleted_at IS NULL'),followUps:q("SELECT COUNT(*) n FROM opt_in_leads WHERE deleted_at IS NULL AND follow_up_status IN ('new','due')")});});
app.get('/api/neighborhoods',...api,(_req,res)=>res.json(db.prepare('SELECT * FROM neighborhood_aggregates ORDER BY opt_in_leads DESC,unique_access_points DESC').all()));
app.get('/api/leads',...api,(_req,res)=>res.json(db.prepare('SELECT id,first_name,last_name,contact,address,preferred_contact,interested,consent,neighborhood,source,follow_up_status,follow_up_date,notes,created_at FROM opt_in_leads WHERE deleted_at IS NULL ORDER BY created_at DESC').all()));
app.patch('/api/leads/:id',...api,(req,res)=>{const id=parsedId(req.params.id);if(!id)return bad(res,'invalid lead id');const parsed=leadPatchSchema.safeParse(req.body);if(!parsed.success)return bad(res,'invalid lead update');const keys=Object.keys(parsed.data) as Array<keyof typeof parsed.data>;const set=keys.map(k=>`${k}=?`).join(',');const result=db.prepare(`UPDATE opt_in_leads SET ${set} WHERE id=? AND deleted_at IS NULL`).run(...keys.map(k=>parsed.data[k]),id);if(!result.changes)return bad(res,'lead not found',404);res.json({ok:true});});
app.delete('/api/leads/:id',...api,(req,res)=>{const id=parsedId(req.params.id);if(!id)return bad(res,'invalid lead id');const erase=db.transaction((leadId:number)=>{const found=db.prepare('SELECT id FROM opt_in_leads WHERE id=?').get(leadId);if(!found)return false;db.prepare('DELETE FROM lead_events WHERE lead_id=?').run(leadId);db.prepare('DELETE FROM opt_in_leads WHERE id=?').run(leadId);rebuildAggregates();return true;});if(!erase(id))return bad(res,'lead not found',404);res.status(204).end();});
app.get('/api/imports',...api,(_req,res)=>res.json(db.prepare('SELECT id,filename,status,bytes,rows_seen,rows_imported,rows_invalid,rows_duplicate,error,survey_session_id,created_at,completed_at FROM import_jobs ORDER BY created_at DESC LIMIT 100').all()));
const upload=multer({dest:path.join(dataDir,'uploads'),limits:{fileSize:Number(process.env.UPLOAD_MAX_BYTES||26214400),files:1},fileFilter:(_req,file,cb)=>cb(null,/\.(csv|wiglecsv|gz)$/i.test(file.originalname)||file.mimetype==='text/csv')});
app.post('/api/imports',...api,upload.single('file'),async(req,res)=>{if(!req.file)return bad(res,'CSV file required');const labelResult=importLabelSchema.safeParse(req.body.label);if(!labelResult.success)return bad(res,'invalid session label');const label=labelResult.data||req.file.originalname;let jobId:number|bigint=0,sessionId:number|bigint=0;try{const ids=db.transaction(()=>{const job=db.prepare("INSERT INTO import_jobs(filename,status,bytes) VALUES(?,?,?)").run(req.file!.originalname,'running',req.file!.size);const survey=db.prepare("INSERT INTO survey_sessions(label) VALUES(?)").run(label);db.prepare('UPDATE import_jobs SET survey_session_id=? WHERE id=?').run(survey.lastInsertRowid,job.lastInsertRowid);return {jobId:job.lastInsertRowid,sessionId:survey.lastInsertRowid};})();jobId=ids.jobId;sessionId=ids.sessionId;let duplicates=0;const stage=db.prepare(`INSERT INTO wifi_observation_staging(import_job_id,observed_at,bssid,ssid,auth_mode,channel,frequency_mhz,rssi,latitude,longitude,altitude_m,accuracy_m,network_type,source_row_hash) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?)`);const summary=await parseWigle(req.file.path,o=>{try{stage.run(jobId,o.observedAt,o.bssid,o.ssid,o.authMode,o.channel,o.frequencyMhz,o.rssi,o.latitude,o.longitude,o.altitude_m,o.accuracyM,o.networkType,o.sourceRowHash);}catch(e:any){if(e?.code==='SQLITE_CONSTRAINT_UNIQUE')duplicates++;else throw e;}});db.transaction(()=>{db.prepare(`INSERT INTO wifi_observations(survey_session_id,observed_at,bssid,ssid,auth_mode,channel,frequency_mhz,rssi,latitude,longitude,altitude_m,accuracy_m,network_type,source_row_hash) SELECT ?,observed_at,bssid,ssid,auth_mode,channel,frequency_mhz,rssi,latitude,longitude,altitude_m,accuracy_m,network_type,source_row_hash FROM wifi_observation_staging WHERE import_job_id=?`).run(sessionId,jobId);db.prepare('UPDATE survey_sessions SET observation_count=?,valid_count=?,invalid_count=?,duplicate_count=? WHERE id=?').run(summary.rowsSeen,summary.rowsImported-duplicates,summary.rowsInvalid,duplicates,sessionId);db.prepare("UPDATE import_jobs SET status='complete',rows_seen=?,rows_imported=?,rows_invalid=?,rows_duplicate=?,completed_at=CURRENT_TIMESTAMP WHERE id=?").run(summary.rowsSeen,summary.rowsImported-duplicates,summary.rowsInvalid,duplicates,jobId);db.prepare('DELETE FROM wifi_observation_staging WHERE import_job_id=?').run(jobId);rebuildAggregates();})();res.json({jobId:Number(jobId),...summary,rowsImported:summary.rowsImported-duplicates,rowsDuplicate:duplicates});}catch(e){if(jobId){try{db.transaction(()=>{db.prepare('DELETE FROM wifi_observation_staging WHERE import_job_id=?').run(jobId);db.prepare("UPDATE import_jobs SET status='failed',error='import failed',survey_session_id=NULL WHERE id=?").run(jobId);db.prepare('DELETE FROM survey_sessions WHERE id=?').run(sessionId);})();}catch{}}return bad(res,'import failed',400);}finally{if(req.file)fs.rmSync(req.file.path,{force:true});}});
app.delete('/api/imports/:id',...api,(req,res)=>{const id=parsedId(req.params.id);if(!id)return bad(res,'invalid import id');const deleted=db.transaction((importId:number)=>{const row=db.prepare('SELECT survey_session_id FROM import_jobs WHERE id=?').get(importId) as any;if(!row)return false;db.prepare('DELETE FROM import_jobs WHERE id=?').run(importId);db.prepare('DELETE FROM survey_sessions WHERE id=?').run(row.survey_session_id);rebuildAggregates();return true;})(id);if(!deleted)return bad(res,'import not found',404);res.status(204).end();});
app.use(express.static('dist-web'));app.use((req,res,next)=>req.path.startsWith('/api/')?next():res.sendFile(path.resolve('dist-web/index.html')));
app.use((err:any,_req:express.Request,res:express.Response,_next:express.NextFunction)=>{if(err?.code==='LIMIT_FILE_SIZE')return bad(res,'uploaded file is too large',413);if(err?.name==='MulterError')return bad(res,'invalid upload',400);return bad(res,'internal server error',500);});
app.listen(port,'127.0.0.1',()=>console.log(`FiberScout dashboard listening on http://127.0.0.1:${port}`));
