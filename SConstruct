# vim: ft=python

env=DefaultEnvironment()

env = Environment(
                    CCFLAGS = '-std=c++11 -g',
                    LINKFLAGS = '-std=c++11 -lpqxx -lpthread -lboost_chrono -lboost_serialization -lboost_system -lboost_thread'
                 )

model = env.SConscript('src/SConscript.model', variant_dir='build', duplicate=0, exports = ['env'])
controller = env.SConscript('src/SConscript.controller', variant_dir='build', duplicate=0, exports = ['env'])
view = env.SConscript('src/SConscript.view', variant_dir='build', duplicate=0, exports = ['env'])
final = env.SConscript('src/SConscript.main', variant_dir='build', duplicate=0, exports = ['env'])
env.Depends(final,model)
env.Depends(final,controller)
env.Depends(final,view)
env.SConscript('test/SConscript.test', variant_dir='build_test', duplicate=0, exports = ['env'])
