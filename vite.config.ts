import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
export default defineConfig({plugins:[react()],server:{host:'127.0.0.1',proxy:{'/api':'http://127.0.0.1:8787'}},test:{exclude:['dist-server/**','node_modules/**']},build:{outDir:'dist-web'}});
