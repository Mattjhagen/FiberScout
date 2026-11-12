import {describe,it,expect} from 'vitest';
import {leadPatchSchema,loginSchema,idSchema} from '../server/validation.js';
describe('security validation',()=>{
  it('rejects unknown lead fields and oversized notes',()=>{expect(leadPatchSchema.safeParse({notes:'x'.repeat(2001)}).success).toBe(false);expect(leadPatchSchema.safeParse({notes:'ok',secret:'bad'}).success).toBe(false)});
  it('accepts only bounded update values',()=>{expect(leadPatchSchema.safeParse({follow_up_status:'contacted',follow_up_date:'2026-11-12'}).success).toBe(true);expect(leadPatchSchema.safeParse({follow_up_status:'erase'}).success).toBe(false)});
  it('rejects malformed identifiers and login bodies',()=>{expect(idSchema.safeParse('1 OR 1=1').success).toBe(false);expect(loginSchema.safeParse({username:'a',password:'p',extra:true}).success).toBe(false)});
});
