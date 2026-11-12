import Database from 'better-sqlite3'; import fs from 'node:fs'; import path from 'node:path';
const file=process.env.FIBERSCOUT_DB||'./data/fiberscout-v11.sqlite'; fs.mkdirSync(path.dirname(file),{recursive:true});
export const db=new Database(file); db.pragma('journal_mode = WAL'); db.pragma('foreign_keys = ON'); db.pragma('busy_timeout = 5000');
export function migrate(){db.exec('CREATE TABLE IF NOT EXISTS schema_migrations(version TEXT PRIMARY KEY, applied_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP)');for(const version of ['001_initial','002_import_staging']){const sql=fs.readFileSync(path.resolve(process.cwd(),`db/migrations/${version}.sql`),'utf8');if(!db.prepare('SELECT 1 FROM schema_migrations WHERE version=?').get(version)){const tx=db.transaction(()=>{db.exec(sql);db.prepare('INSERT INTO schema_migrations(version) VALUES(?)').run(version);});tx();}}}
function enforceRetention(){const days=Number(process.env.RAW_OBSERVATION_RETENTION_DAYS||90);if(Number.isFinite(days)&&days>0)db.prepare("DELETE FROM wifi_observations WHERE created_at < datetime('now', ?)").run(`-${Math.floor(days)} days`);}
migrate();enforceRetention();
