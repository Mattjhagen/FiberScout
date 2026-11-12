import session from 'express-session';
import type {Database} from 'better-sqlite3';

export class SqliteSessionStore extends session.Store {
  private db: Database;
  constructor(db: Database) {
    super(); this.db=db;
    db.exec('CREATE TABLE IF NOT EXISTS sessions (sid TEXT PRIMARY KEY, sess TEXT NOT NULL, expire INTEGER NOT NULL)');
    db.prepare('DELETE FROM sessions WHERE expire <= ?').run(Date.now());
  }
  get(sid:string,cb:(err:any,sess?:session.SessionData|null)=>void){try{const row=this.db.prepare('SELECT sess,expire FROM sessions WHERE sid=?').get(sid) as any;if(!row||row.expire<=Date.now())return cb(null,null);cb(null,JSON.parse(row.sess));}catch(e){cb(e);}}
  set(sid:string,sess:session.SessionData,cb:(err?:any)=>void){try{const expiry=sess.cookie?.expires?new Date(sess.cookie.expires).getTime():Date.now()+8*60*60*1000;this.db.prepare('INSERT INTO sessions(sid,sess,expire) VALUES(?,?,?) ON CONFLICT(sid) DO UPDATE SET sess=excluded.sess,expire=excluded.expire').run(sid,JSON.stringify(sess),expiry);cb();}catch(e){cb(e);}}
  destroy(sid:string,cb:(err?:any)=>void){try{this.db.prepare('DELETE FROM sessions WHERE sid=?').run(sid);cb();}catch(e){cb(e);}}
  touch(sid:string,sess:session.SessionData,cb:(err?:any)=>void){try{const expiry=sess.cookie?.expires?new Date(sess.cookie.expires).getTime():Date.now()+8*60*60*1000;this.db.prepare('UPDATE sessions SET sess=?,expire=? WHERE sid=?').run(JSON.stringify(sess),expiry,sid);cb();}catch(e){cb(e);}}
}
