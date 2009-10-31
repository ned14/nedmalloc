import os, sys

env = Environment(ENV=os.environ)
AddOption('--DEBUG', dest='debug', help='enable debug build')
builddefines=[]
buildoptions=[]
libraries=[]
linkoptions=[]
if env.GetOption('debug'):
    builddefines+=["DEBUG", "_DEBUG"]
else:
    builddefines+=["NDEBUG"]
if sys.platform=='win32':
    builddefines+=["WIN32", "_WINDOWS", "UNICODE", "_UNICODE"]
    if env.GetOption('debug'):
        buildoptions+=["/Od"]
    else:
        buildoptions+=["/O2"]
    libraries+=["psapi", "user32"]
    linkoptions+=[]
else:
    builddefines+=[]
    if env.GetOption('debug'):
        buildoptions+=["-O0 -g"]
    else:
        buildoptions+=["-O2"]
    libraries+=["pthread"]
    linkoptions+=[]

# DLL first
libsources = [ "nedmalloc.c" ]
if sys.platform=='win32': libsources+=["winpatcher.c"]
VariantDir
libobjects = env.SharedObject(source = libsources, CPPDEFINES = builddefines+["NEDMALLOC_DLL_EXPORTS"], CCFLAGS = buildoptions)
nedmalloclib = env.SharedLibrary(target="nedmalloc", source = libobjects, LIBS=libraries, LINKFLAGS=linkoptions)
if sys.platform=='win32': nedmalloclib=nedmalloclib[1]

# Test program
sources = [ "test.c" ]
objects = env.Object(source = sources, CPPDEFINES = builddefines, CCFLAGS = buildoptions) + [nedmalloclib]
testprogram = env.Program(target = "test", source = objects, LIBS=libraries)

Default(testprogram)
