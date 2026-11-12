import {z} from 'zod';
export const loginSchema=z.object({username:z.string().trim().min(1).max(80),password:z.string().min(1).max(256)}).strict();
export const leadPatchSchema=z.object({follow_up_status:z.enum(['new','contacted','due','closed']).optional(),follow_up_date:z.string().regex(/^$|^\d{4}-\d{2}-\d{2}$/).optional(),notes:z.string().max(2000).optional(),neighborhood:z.string().trim().max(120).optional()}).strict().refine(x=>Object.keys(x).length>0,{message:'no editable fields'});
export const idSchema=z.coerce.number().int().positive();
export const importLabelSchema=z.string().trim().min(1).max(120).optional();
