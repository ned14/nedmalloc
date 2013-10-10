import os, sys, platform
def bitscanrev(x, c=-1):
    return bitscanrev(x/2, c+1) if x else c
	
architectures = ["generic"]

env = Environment()
#print env['TOOLS']

architecture="generic"
ARMcrosscompiler=False
if env['CC']=='cl':
	# We're on windows
	if os.environ.has_key('LIBPATH'):
		if -1!=os.environ['LIBPATH'].find("\\amd64"):
			architecture="x64"
		elif -1!=os.environ['LIBPATH'].find("\\arm"):
			architecture="ARMv7"
		else:
			architecture="x86"
		architectures.append(architecture)
else:
	# We're on POSIX, so ask the compiler what it thinks it's building
	output=''
	try:
		output=subprocess.check_output([env['CXX'], "--target-help"])
	except:
		try:
			output=subprocess.check_output([env['CXX'], "--version"])
		except:
			output=platform.machine()
	if 'armelf' in output:
		architectures+=["ARMv7"]
		architecture="ARMv7"
	# Choose Intel in preference as otherwise it's probably a cross-compiler
	if 'x64' in output or 'x86_64' in output:
		architectures+=["x86", "x64"]
		architecture="x64"
	else:
		for a in ['i386', 'i486', 'i586', 'i686']:
			if a in output:
				architectures+=["x86"]
				architecture="x86"
				break
	# If not ARMv7 support, see if there is a cross-compiler
	if "ARMv7" not in architectures:
		output=''
		try:
			output=subprocess.check_output(['arm-linux-gnueabi-g++', "--target-help"])
		except:
			try:
				output=subprocess.check_output(['arm-linux-gnueabi-g++', "--version"])
			except:
				output=platform.machine()
		if 'armelf' in output:
			architectures+=["ARMv7"]
			ARMcrosscompiler=True
print "*** Architectures detected as being available:", architectures
print "*** Minimum architecture is:", architecture

if sys.platform=="win32" and 'INCLUDE' not in os.environ:
    env = Environment(tools=['mingw', 'msvs'])
#print env['TOOLS']
AddOption('--postfix', dest='postfix', nargs=1, default='_test', help='appends a string to the DLL name')
AddOption('--debugbuild', dest='debug', action='store_const', const=1, help='enable debug build')
AddOption('--optdebugbuild', dest='debug', action='store_const', const=2, help='enable optimised debug build')
AddOption('--static', dest='static', nargs='?', const=True, help='build a static library rather than shared library')
AddOption('--pgo', dest='pgo', nargs='?', const=True, help='build PGO instrumentation')
AddOption('--debugprint', dest='debugprint', nargs='?', const=True, help='enable lots of debug printing (windows only)')
AddOption('--fullsanitychecks', dest='fullsanitychecks', nargs='?', const=True, help='enable full sanity checking on every memory op')
AddOption('--useclang', dest='useclang', nargs=1, type='str', help='use clang if it is available')
AddOption('--analyze', dest='analyze', nargs='?', const=True, help='have the compiler do static analysis')
AddOption('--usegcc', dest='usegcc', nargs=1, type='str', help='use gcc if it is available')
AddOption('--usethreadsanitize', dest='usethreadsanitize', nargs='?', const=True, help='use thread sanitiser')
AddOption('--usegcov', dest='usegcov', nargs='?', const=True, help='use GCC coverage')
AddOption('--force32', dest='force32', help='force 32 bit build on 64 bit machine')
AddOption('--archs', dest='archs', nargs=1, type='str', default='min', help='which architectures to build, comma separated. all means all. Defaults to min.')
if 'x86' in architectures:
	AddOption('--sse', dest='sse', nargs=1, type='int', default=2, help='set SSE used (0-4) on 32 bit x86. Defaults to 2 (SSE2).')
	AddOption('--avx', dest='avx', nargs=1, type='int', default=0, help='set AVX used (0-2) on x86/x64. Defaults to 0 (No AVX).')
if 'ARMv7' in architectures:
	AddOption('--fpu', dest='fpu', nargs=1, type='str', default='neon', help='sets FPU used on ARMv7. Defaults to neon.')
	AddOption('--thumb', dest='thumb', nargs='?', const=True, help='generate ARMv7 Thumb instructions instead of normal.')
if env.GetOption('archs')!='min' and env.GetOption('archs')!='all':
	archs=env.GetOption('archs').split(',')
	for arch in archs:
		assert arch in architectures
	architectures=archs
if architecture=='x64' and env.GetOption('force32'): architecture='x86'
AddOption('--replacesystemallocator', dest='replacesystemallocator', nargs='?', const=True, help='replace all usage of the system allocator in the process when loaded')
AddOption('--tolerant', dest='tolerant', nargs=1, default=1, help='enable tolerance of the system allocator (not guaranteed). On by default.')
AddOption('--magicheaders', dest='magicheaders', nargs='?', const=True, help='enable magic headers (guaranteed tolerance of the system allocator)')
AddOption('--useallocator', dest='useallocator', nargs=1, type='int', default=1, help='which allocator to use')
AddOption('--uselocks', dest='uselocks', nargs=1, type='int', default=1, help='which form of locking to use')
AddOption('--nospinlocks', dest='nospinlocks', nargs='?', const=True, help='use system mutexs rather than CPU spinlocks')
AddOption('--largepages', dest='largepages', nargs='?', const=True, help='enable large page support')
AddOption('--fastheapdetection', dest='fastheapdetection', nargs='?', const=True, help='enable fast system-specific heap detection')
AddOption('--maxthreadsinpool', dest='maxthreadsinpool', nargs=1, type='int', help='sets how much memory bloating to cause for more performance')
AddOption('--defaultgranularity', dest='defaultgranularity', nargs=1, type='int', help='sets how much memory to claim or release from the system at one time')
AddOption('--threadcachemax', dest='threadcachemax', nargs=1, type='string', help='sets what allocations should use the threadcache')
AddOption('--threadcachemaxbins', dest='threadcachemaxbins', nargs=1, type='int', help='sets the threadcache binning')
AddOption('--threadcachemaxfreespace', dest='threadcachemaxfreespace', nargs=1, type='int', help='sets when the threadcache should be garbage collected')
AddOption('--adminuac', dest='adminuac', nargs='?', const=True, help='Windows only. Link with UAC:requireAdmin to force all test apps to run as Administrator')

# Force scons to always use absolute paths in everything (helps debuggers to find source files)
env['CCCOM']   =    env['CCCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
env['SHCCCOM'] =  env['SHCCCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
env['CXXCOM']  =   env['CXXCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
env['SHCXXCOM']= env['SHCXXCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
env['CPPPATH']=[]
env['CPPDEFINES']=[]
env['CPPFLAGS']=[]
env['CCFLAGS']=[]
env['CXXFLAGS']=[]
env['LIBS']=[]
env['LIBPATH']=[]
env['RPATH']=[]
env['LINKFLAGS']=[]
env['CCFLAGSFORNEDMALLOC']=[]
env['NEDMALLOCLIBRARYNAME']="nedmalloc"+('_ptchg' if env.GetOption('replacesystemallocator') else '')+env.GetOption('postfix')
env['UMPALIBRARYNAME']="nedumpa"+('_ptchg' if env.GetOption('replacesystemallocator') else '')+env.GetOption('postfix')
if env.GetOption('debugprint'): env['CPPDEFINES']+=["USE_DEBUGGER_OUTPUT"]
if env.GetOption('fullsanitychecks'): env['CPPDEFINES']+=["FULLSANITYCHECKS"]
if env.GetOption('replacesystemallocator'): env['CPPDEFINES']+=["REPLACE_SYSTEM_ALLOCATOR"]
if env.GetOption('tolerant'): env['CPPDEFINES']+=["ENABLE_TOLERANT_NEDMALLOC"]
if env.GetOption('magicheaders'): env['CPPDEFINES']+=["USE_MAGIC_HEADERS"]
env['CPPDEFINES']+=[("USE_ALLOCATOR",env.GetOption('useallocator'))]
env['CPPDEFINES']+=[("USE_LOCKS",env.GetOption('uselocks'))]
if env.GetOption('nospinlocks'): env['CPPDEFINES']+=[("USE_SPIN_LOCKS",0)]
if env.GetOption('largepages'): env['CPPDEFINES']+=["ENABLE_LARGE_PAGES"]
if env.GetOption('fastheapdetection'): env['CPPDEFINES']+=["ENABLE_FAST_HEAP_DETECTION"]
if env.GetOption('maxthreadsinpool'): env['CPPDEFINES']+=[("MAXTHREADSINPOOL",env.GetOption('maxthreadsinpool'))]
if env.GetOption('defaultgranularity'): env['CPPDEFINES']+=[("DEFAULT_GRANULARITY",env.GetOption('defaultgranularity'))]
if env.GetOption('threadcachemax'):
    threadcachemax=int(env.GetOption('threadcachemax'))
    if threadcachemax<=32: threadcachemax=0
    env['CPPDEFINES']+=[("THREADCACHEMAX",threadcachemax)]
    if not env.GetOption('threadcachemaxbins') and threadcachemax:
        maxbins=bitscanrev(threadcachemax)-4;
        print "THREADCACHEMAX set but not THREADCACHEMAXBINS, so auto-setting THREADCACHEMAXBINS =", maxbins
        env['CPPDEFINES']+=[("THREADCACHEMAXBINS",maxbins)]
if env.GetOption('threadcachemaxbins'): env['CPPDEFINES']+=[("THREADCACHEMAXBINS",env.GetOption('threadcachemaxbins'))]
if env.GetOption('threadcachemaxfreespace'): env['CPPDEFINES']+=[("THREADCACHEMAXFREESPACE",env.GetOption('threadcachemaxfreespace'))]
env['CPPDEFINES']+=[("NEDMALLOCDEPRECATED", "")]

# Am I in a 32 or 64 bit environment? Note that not specifying --sse doesn't set any x86 or x64 specific options
# so it's good to go for ANY platform
if sys.platform=="win32" and 'INCLUDE' in os.environ:
    # Even the very latest scons still screws this up :(
    env['ENV']['INCLUDE']=os.environ['INCLUDE']
    env['ENV']['LIB']=os.environ['LIB']
    env['ENV']['PATH']=os.environ['PATH']
else:
    if sys.platform=="win32":
        env['ENV']['PATH']=os.environ['PATH']
        env['CPPDEFINES']+=["WIN32"]
    # Test the build environment
    def CheckHaveClang(cc):
        cc.Message("Checking if clang is available ...")
        try:
            temp=env['CC']
        except:
            temp=[]
        cc.env['CC']=env.GetOption('useclang')
        result=cc.TryLink('int main(void) { return 0; }\n', '.c')
        cc.env['CC']=temp
        cc.Result(result)
        return result
    def CheckHaveGCC(cc):
        cc.Message("Checking if gcc is available ...")
        try:
            temp=env['CC']
        except:
            temp=[]
        cc.env['CC']=env.GetOption('usegcc')
        result=cc.TryLink('int main(void) { return 0; }\n', '.c')
        cc.env['CC']=temp
        cc.Result(result)
        return result
    def CheckHaveVisibility(cc):
        cc.Message("Checking for symbol visibility support ...")
        try:
            temp=cc.env['CPPFLAGS']
        except:
            temp=[]
        cc.env['CPPFLAGS']=temp+["-fvisibility=hidden"]
        result=cc.TryCompile('struct __attribute__ ((visibility("default"))) Foo { int foo; };\nint main(void) { Foo foo; return 0; }\n', '.cpp')
        cc.env['CPPFLAGS']=temp
        cc.Result(result)
        return result
    def CheckHaveOpenMP(cc):
        cc.Message("Checking for OpenMP support ...")
        try:
            temp=cc.env['CPPFLAGS']
        except:
            temp=[]
        cc.env['CPPFLAGS']=temp+["-fopenmp"]
        result=cc.TryCompile('#include <omp.h>\nint main(void) { return 0; }\n', '.cpp')
        cc.env['CPPFLAGS']=temp
        cc.Result(result)
        return result
    def CheckHaveCPP11Features(cc):
        cc.Message("Checking if can enable C++11 features ...")
        try:
            temp=cc.env['CPPFLAGS']
        except:
            temp=[]
        cc.env['CPPFLAGS']=temp+["-std=c++11"]
        result=cc.TryCompile('struct Foo { static const int gee=5; Foo(const char *) { } Foo(Foo &&a) { } };\nint main(void) { Foo foo(__func__); static_assert(Foo::gee==5, "Death!"); return 0; }\n', '.cpp')
        cc.env['CPPFLAGS']=temp
        cc.Result(result)
        return result
    def CheckHaveValgrind(cc):
        cc.Message("Checking if valgrind is available ...")
        result=cc.TryLink('#include <valgrind/valgrind.h>\nint main(void) { VALGRIND_CREATE_MEMPOOL(0,0,0); return 0; }\n', '.c')
        cc.Result(result)
        return result
    conf=Configure(env, { "CheckHaveClang" : CheckHaveClang, "CheckHaveGCC" : CheckHaveGCC, "CheckHaveVisibility" : CheckHaveVisibility, "CheckHaveOpenMP" : CheckHaveOpenMP, "CheckHaveCPP11Features" : CheckHaveCPP11Features, "CheckHaveValgrind" : CheckHaveValgrind } )
    if env.GetOption('useclang') and conf.CheckHaveClang():
        env['CC']="clang"
        env['CXX']=env.GetOption('useclang')
        if env.GetOption('analyze'): env['CPPFLAGS']+=["--analyze"]
    if env.GetOption('usegcc') and conf.CheckHaveGCC():
        env['CC']="gcc"
        env['CXX']=env.GetOption('usegcc')
    if env.GetOption('usethreadsanitize'):
        env['CPPFLAGS']+=["-fsanitize=thread", "-fPIC"]
        env['LINKFLAGS']+=["-fsanitize=thread"]
        env['LIBS']+=["tsan"]
    if env.GetOption('usegcov'):
        env['CPPFLAGS']+=["-fprofile-arcs", "-ftest-coverage"]
        env['LIBS']+=["gcov"]
        env['CPPDEFINES']+=["NDEBUG"] # Disables some code only there to aid debugger display
    if not conf.CheckLib("rt", "clock_gettime") and not conf.CheckLib("c", "clock_gettime"):
        print "WARNING: Can't find clock_gettime() in librt or libc, code may not fully compile if your system headers say that this function is available"
    if conf.CheckHaveVisibility():
        env['CPPFLAGS']+=["-fvisibility=hidden"]        # All symbols are hidden unless marked otherwise
        env['CXXFLAGS']+=["-fvisibility-inlines-hidden" # All inlines are always hidden
                             ]
    else:
        print "Disabling -fvisibility support"

    if conf.CheckHaveOpenMP():
        env['CPPFLAGS']+=["-fopenmp"]
        env['LINKFLAGS']+=["-fopenmp"]
    else:
        print "Disabling OpenMP support"

    if conf.CheckHaveCPP11Features():
        env['CXXFLAGS']+=["-std=c++11"]
    else:
        print "Disabling C++11 support"

    if conf.CheckHaveValgrind():
        env['CPPDEFINES']+=["HAVE_VALGRIND"]
    else:
        print "Disabling valgrind support"

    env=conf.Finish()

# Build
nedmalloclib=None
buildvariants={}
for arch in architectures:
    for buildtype in ["Debug", "Release"]:
        env['VARIANT']=arch+"/"+buildtype
        Export(importedenv=env, ARMcrosscompiler=ARMcrosscompiler)
        nedmalloclibvariant=env.SConscript("SConscript", variant_dir=env['VARIANT'], duplicate=False)
        buildvariants[(buildtype, architecture)]=nedmalloclibvariant

if env.GetOption('archs')=='min':
	print "*** Build variant preferred by environment is", "Debug" if env.GetOption("debug") else "Release", architecture, "using compiler", env['CC']
	nedmalloclib=buildvariants[("Debug" if env.GetOption("debug") else "Release", architecture)]
	#print(nedmalloclib)
	Default([x[0] for x in nedmalloclib.values()])
else:
	#print(buildvariants)
	nedmalloclib=[x.values()[0][0] for x in buildvariants.values()]
	#print(nedmalloclib)
	Default(nedmalloclib)

# Set up the MSVC project files
if 'win32'==sys.platform:
    includes = [ "nedmalloc.h", "malloc.c.h", "nedmalloc.c", "usermodepageallocator.c"]
    variants = []
    projs = {}
    for buildvariant, output in buildvariants.items():
        variant = buildvariant[0]+'|'+("Win32" if buildvariant[1]=="x86" else buildvariant[1])
        variants+=[variant]
        for program, builditems in output.items():
            if not program in projs: projs[program]={}
            projs[program][variant]=builditems
    variants.sort()
    msvsprojs = []
    for program, items in projs.items():
        buildtargets = items.items()
        buildtargets.sort()
        #print buildtargets
        #print [str(x[1][0][0]) for x in buildtargets]
        msvsprojs+=env.MSVSProject(program+env['MSVSPROJECTSUFFIX'], srcs=items.values()[0][1], incs=includes, misc="Readme.html", buildtarget=[x[1][0][0] for x in buildtargets], runfile=[str(x[1][0][0]) for x in buildtargets], variant=[x[0] for x in buildtargets], auto_build_solution=0)
    msvssolution = env.MSVSSolution("nedmalloc.sln", projects=msvsprojs, variant=variants)
    Depends(msvssolution, msvsprojs)
    Alias("msvcproj", msvssolution)

Return("nedmalloclib")
