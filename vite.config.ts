import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
export default defineConfig({plugins:[react()],test:{exclude:['dist-server/**','node_modules/**']},build:{outDir:'dist-web'}});
