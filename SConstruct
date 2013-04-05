# vim: ft=python

env=DefaultEnvironment()

env = Environment(
                    CCFLAGS = '-std=c++11 -g',
                    LINKFLAGS = '-std=c++11 -lpthread -lboost_serialization -lboost_system -lboost_chrono -lpqxx'
                 )

SConscript('src/SConscript.model', variant_dir='build', duplicate=0, exports = ['env'])
SConscript('src/SConscript.controller', variant_dir='build', duplicate=0, exports = ['env'])
SConscript('src/SConscript.view', variant_dir='build', duplicate=0, exports = ['env'])
SConscript('test/SConscript.test', variant_dir='build_test', duplicate=0, exports = ['env'])
