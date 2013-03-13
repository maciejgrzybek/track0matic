# vim: ft=python

env=DefaultEnvironment()

env = Environment(
                    CCFLAGS = '-std=c++0x -g',
                    LINKFLAGS = '-std=c++0x -lpthread -lboost_serialization -lpqxx'
                 )

SConscript('src/SConscript.model', variant_dir='build', duplicate=0, exports = ['env'])
SConscript('src/SConscript.controller', variant_dir='build', duplicate=0, exports = ['env'])
SConscript('src/SConscript.view', variant_dir='build', duplicate=0, exports = ['env'])
SConscript('test/SConscript.test', variant_dir='build_test', duplicate=0, exports = ['env'])
