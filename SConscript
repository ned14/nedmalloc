import os, sys, platform

Import("env")

# Build the DLL
libsources = [ "nedmalloc.c" ]
if sys.platform=='win32': libsources+=["winpatcher.c"]
libobjects = env.SharedObject(source = libsources, CPPDEFINES=env['CPPDEFINES']+["NEDMALLOC_DLL_EXPORTS"])
nedmalloclib = env.SharedLibrary("nedmalloc", source = libobjects)
if sys.platform=='win32':
    #env.AddPreAction(env.AddPreAction(nedmalloclib, "pgomgr /clear ${VARIANT}/nedmalloc.pgd"), "pgomgr /merge ${VARIANT}/nedmalloc.pgd")
    nedmalloclib=nedmalloclib[1]

# Test program
sources = [ "test.c" ]
objects = env.Object(source = sources) + [nedmalloclib]
testprogram = env.Program("test", source = objects, LINKFLAGS=[])

Default(testprogram)
