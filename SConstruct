import os, sys, platform

env = Environment(ENV=os.environ)
AddOption('--debugbuild', dest='debug', help='enable debug build')
AddOption('--force32', dest='force32', help='force 32 bit build on 64 bit machine')
AddOption('--sse', dest='sse', nargs=1, type='int', help='set SSE used (0-4) on 32 bit x86')
architecture="generic"
env['CPPDEFINES']=[]
env['CCFLAGS']=[]
env['LIBS']=[]
env['LINKFLAGS']=[]

# Am I in a 32 or 64 bit environment? Note that not specifying --sse doesn't set any x86 or x64 specific options
# so it's good to go for ANY platform
if sys.platform=="win32":
    if not env.GetOption('force32') and os.environ.has_key('LIBPATH') and -1!=os.environ['LIBPATH'].find("\\amd64"):
        architecture="x64"
    else:
        architecture="x86"
        if   env.GetOption('sse')==1: env['CCFLAGS']+=[ "/arch:SSE" ]
        elif env.GetOption('sse')>=2: env['CCFLAGS']+=[ "/arch:SSE2" ]
        if   env.GetOption('sse')>=3: env['CPPDEFINES']+=[("__SSE3__", 1)]
        if   env.GetOption('sse')>=4: env['CPPDEFINES']+=[("__SSE4__", 1)]
else:
    if not env.GetOption('force32') and ('x64' in platform.machine() or 'x86_64' in platform.machine()):
        architecture="x64"
    elif 'i386' in platform.machine():
        architecture="x86"
        if env.GetOption('sse'):
            env['CCFLAGS']+=["-mfpmath=sse"]
            if env.GetOption('sse')>1: env['CCFLAGS']+=["-msse%s" % str(env.GetOption('sse'))]
            else: env['CCFLAGS']+=["-msse"]

# Am I building a debug or release build?
if env.GetOption('debug'):
    env['CPPDEFINES']+=["DEBUG", "_DEBUG"]
    variant=architecture+"/Debug"
else:
    env['CPPDEFINES']+=["NDEBUG"]
    variant=architecture+"/Release"

# Am I building for Windows or POSIX?
if sys.platform=='win32':
    env['CPPDEFINES']+=["WIN32", "_WINDOWS", "UNICODE", "_UNICODE"]
    env['CCFLAGS']+=["/GF"]     # Eliminate duplicate strings
    env['CCFLAGS']+=["/Gy"]     # Seperate COMDATs
    if env.GetOption('debug'):
        env['CCFLAGS']+=["/Od", "/MDd"]
    else:
        env['CCFLAGS']+=["/O2", "/MD", "/GL"]
    env['LIBS']+=["psapi", "user32"]
    env['LINKFLAGS']+=["/LTCG:PGUPDATE"]
else:
    env['CPPDEFINES']+=[]
    if env.GetOption('debug'):
        env['CCFLAGS']+=["-O0 -g"]
    else:
        env['CCFLAGS']+=["-O2 -g"]
    env['LIBS']+=["pthread"]
    env['LINKFLAGS']+=[]

# Build
env['VARIANT']=variant
SConscript("SConscript", variant_dir=variant, duplicate=False, exports="env")
