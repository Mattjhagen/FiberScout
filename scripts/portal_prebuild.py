Import('env')
import runpy
runpy.run_path(env.subst('$PROJECT_DIR') + '/scripts/build_portal.py', run_name='__main__')
