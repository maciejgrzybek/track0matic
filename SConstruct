# vim: ft=python

env=DefaultEnvironment()

env = Environment(
                    CCFLAGS = '-std=c++11 -g -O0',
                    LINKFLAGS = '-std=c++11 -lpqxx -lpthread -lboost_chrono -lboost_program_options -lboost_serialization -lboost_system -lboost_thread'
                 )

Qt4Dir = '/usr/include/qt4'

qtEnv = env.Clone()
qtEnv['QT4DIR'] = Qt4Dir
qtEnv.Tool('qt4')

qtEnv.EnableQt4Modules([
                        'QtGui',
                        'QtCore'
                       ])


thirdparty = env.SConscript('src/SConscript.3rdparty', variant_dir='build', duplicate=0, exports = ['env'])
common = env.SConscript('src/SConscript.common', variant_dir='build', duplicate=0, exports = ['env'])
model = env.SConscript('src/SConscript.model', variant_dir='build', duplicate=0, exports = ['env'])
controller = env.SConscript('src/SConscript.controller', variant_dir='build', duplicate=0, exports = ['env'])
view = env.SConscript('src/SConscript.view', variant_dir='build', duplicate=0, exports = ['qtEnv'])
final = env.SConscript('src/SConscript.main', variant_dir='build', duplicate=0, exports = ['qtEnv'])
env.Depends(final,model)
env.Depends(final,controller)
env.Depends(view,common)
env.Depends(final,view)
env.Depends(view,thirdparty) # for static db driver (map drawing)
env.SConscript('test/SConscript.test', variant_dir='build_test', duplicate=0, exports = ['env'])
