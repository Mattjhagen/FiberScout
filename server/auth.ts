import bcrypt from 'bcryptjs'; import crypto from 'node:crypto'; import {Request,Response,NextFunction} from 'express'; import {db} from './db.js';
export function ensureAdmin(){const hash=process.env.ADMIN_PASSWORD_BCRYPT;if(!hash) return;const user=process.env.ADMIN_USERNAME||'admin';db.prepare('INSERT INTO admin_users(username,password_hash) VALUES(?,?) ON CONFLICT(username) DO UPDATE SET password_hash=excluded.password_hash').run(user,hash);}
export function login(user:string,password:string){const row=db.prepare('SELECT * FROM admin_users WHERE username=?').get(user) as any;return row&&bcrypt.compareSync(password,row.password_hash);}
export function csrf(req:Request){if(!req.session.csrf)req.session.csrf=crypto.randomBytes(24).toString('hex');return req.session.csrf;}
export function requireAuth(req:Request,res:Response,next:NextFunction){if(req.session.user)next();else res.status(401).json({error:'authentication required'});}
export function requireCsrf(req:Request,res:Response,next:NextFunction){if(['GET','HEAD','OPTIONS'].includes(req.method)||req.get('x-csrf-token')===req.session.csrf)next();else res.status(403).json({error:'csrf validation failed'});}
declare module 'express-session' {interface SessionData {user?:string;csrf?:string;}}
