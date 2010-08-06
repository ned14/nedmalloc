import os, sys, platform

Import("env")

# Build the nedmalloc DLL
libobjects = env.SharedObject("nedmalloc.c", CPPDEFINES=env['CPPDEFINES']+["NEDMALLOC_DLL_EXPORTS"], CCFLAGS=env['CCFLAGS']+env['CCFLAGSFORNEDMALLOC'])
if sys.platform=='win32':
    libobjects+=env.SharedObject("winpatcher_nedmalloc", "winpatcher.c", CPPDEFINES=env['CPPDEFINES']+["NEDMALLOC_DLL_EXPORTS"])
    libobjects+=env.RES("nedmalloc.res", "nedmalloc_dll.rc")
nedmalloclib = env.SharedLibrary(env['NEDMALLOCLIBRARYNAME'], source = libobjects)
if sys.platform=='win32': env.AddPostAction(nedmalloclib, 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2')
nedmallocliblib = nedmalloclib
if sys.platform=='win32':
    #env.AddPreAction(env.AddPreAction(nedmalloclib, "pgomgr /clear ${VARIANT}/nedmalloc.pgd"), "pgomgr /merge ${VARIANT}/nedmalloc.pgd")
    nedmallocliblib=nedmalloclib[1]

if False and sys.platform=='win32':
    # Build the UMPA DLL
    libobjects=env.SharedObject("winpatcher_umpa", "winpatcher.c", CPPDEFINES=env['CPPDEFINES']+["USERMODEPAGEALLOCATOR_DLL_EXPORTS"], CCFLAGS=env['CCFLAGS']+env['CCFLAGSFORNEDMALLOC'])
    libobjects+=env.RES("nedmalloc.res", "nedmalloc_dll.rc")
    umpalib = env.SharedLibrary(env['UMPALIBRARYNAME'], source = libobjects)
    env.AddPostAction(umpalib, 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2')
    env.Depends(nedmallocliblib, umpalib)

# Test program
sources = [ "test.c" ]
objects = env.Object(source = sources) # + [nedmallocliblib]
testlibs=[nedmallocliblib]
testprogram = env.Program("test", source = objects, LINKFLAGS=[], LIBS = testlibs)

# PGO program
sources = [ "make_pgos.c" ]
objects = env.Object(source = sources) # + [nedmallocliblib]
testlibs=[nedmallocliblib]
env.Depends(testprogram, env.Program("make_pgos", source = objects, LINKFLAGS=[], LIBS = testlibs))

# Scaling program
sources = [ "scalingtest.cpp" ]
objects = env.Object(source = sources) # + [nedmallocliblib]
env.Depends(testprogram, env.Program("scalingtest", source = objects, LINKFLAGS=[]))

Default(testprogram)
Return("nedmalloclib")
