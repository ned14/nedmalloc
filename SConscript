import os, sys, platform

Import("env")

# Build the DLL
libobjects = env.SharedObject("nedmalloc.c", CPPDEFINES=env['CPPDEFINES']+["NEDMALLOC_DLL_EXPORTS"], CCFLAGS=env['CCFLAGS']+env['CCFLAGSFORNEDMALLOC'])
if sys.platform=='win32':
    libobjects+=env.SharedObject("winpatcher.c", CPPDEFINES=env['CPPDEFINES']+["NEDMALLOC_DLL_EXPORTS"])
    libobjects+=env.RES("nedmalloc.res", "nedmalloc_dll.rc")
nedmalloclib = env.SharedLibrary(env['LIBRARYNAME'], source = libobjects)
if sys.platform=='win32': env.AddPostAction(nedmalloclib, 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2')
nedmallocliblib = nedmalloclib
if sys.platform=='win32':
    #env.AddPreAction(env.AddPreAction(nedmalloclib, "pgomgr /clear ${VARIANT}/nedmalloc.pgd"), "pgomgr /merge ${VARIANT}/nedmalloc.pgd")
    nedmallocliblib=nedmalloclib[1]

# Test program
sources = [ "test.c" ]
objects = env.Object(source = sources) # + [nedmallocliblib]
testlibs=[nedmallocliblib]
testprogram = env.Program("test", source = objects, LINKFLAGS=[], LIBS = testlibs)

# PGO program
sources = [ "make_pgos.c" ]
objects = env.Object(source = sources) # + [nedmallocliblib]
testlibs=[nedmallocliblib]
testprogram = env.Program("make_pgos", source = objects, LINKFLAGS=[], LIBS = testlibs)


Default(testprogram)
Return("nedmalloclib")
