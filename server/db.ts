import Database from 'better-sqlite3'; import fs from 'node:fs'; import path from 'node:path';
const file=process.env.FIBERSCOUT_DB||'./data/fiberscout-v11.sqlite'; fs.mkdirSync(path.dirname(file),{recursive:true});
export const db=new Database(file); db.pragma('journal_mode = WAL'); db.pragma('foreign_keys = ON');
export function migrate(){db.exec('CREATE TABLE IF NOT EXISTS schema_migrations(version TEXT PRIMARY KEY, applied_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP)');const sql=fs.readFileSync(path.resolve(process.cwd(),'db/migrations/001_initial.sql'),'utf8'); const version='001_initial'; if(!db.prepare('SELECT 1 FROM schema_migrations WHERE version=?').get(version)){db.exec(sql);db.prepare('INSERT OR IGNORE INTO schema_migrations(version) VALUES(?)').run(version);}}
migrate();
