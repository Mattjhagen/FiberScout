import Database from 'better-sqlite3';
const file=process.env.FIBERSCOUT_DB||'./data/fiberscout-v11.sqlite';
const db=new Database(file,{readonly:true});
const integrity=db.pragma('integrity_check',{simple:true});
const foreignKeys=db.pragma('foreign_key_check');
console.log(JSON.stringify({database:file,integrity,foreignKeyErrors:foreignKeys.length},null,2));
if(integrity!=='ok'||foreignKeys.length)process.exitCode=1;
db.close();
