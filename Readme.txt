nedalloc v1.06 ?:
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

by Niall Douglas (http://www.nedprod.com/programs/portable/nedmalloc/)

Enclosed is nedalloc, an alternative malloc implementation for multiple
threads without lock contention based on dlmalloc v2.8.4. It is more
or less a newer implementation of ptmalloc2, the standard allocator in
Linux (which is based on dlmalloc v2.7.0) but also contains a per-thread
cache for maximum CPU scalability.

It is licensed under the Boost Software License which basically means
you can do anything you like with it. This does not apply to the malloc.c.h
file which remains copyright to others.

It has been tested on win32 (x86), win64 (x64), Linux (x64), FreeBSD (x64)
and Apple Mac OS X (x86). It works very well on all of these and is very
significantly faster than the system allocator on Windows and FreeBSD. If
you are using a recent Apple Mac OS X then you probably won't see much
improvement (and kudos to Apple for adopting an excellent allocator).

By literally dropping in this allocator as a replacement for your system
allocator, you can see real world improvements of up to three times in normal
code!

Table of Contents:
  A: How to use
  B: Notes
  C: Speed Comparisons
  D: Troubleshooting
  E: Changelog

A. To use:
-=-=-=-=-=
Drop in nedmalloc.h, nedmalloc.c and malloc.c.h into your project.
Configure using the instructions in nedmalloc.h. Make sure that you call
neddisablethreadcache() for every pool you use on thread exit, and don't
forget neddisablethreadcache(0) for the system pool if necessary. Run and
enjoy!

To test, compile test.c. It will run a comparison between your system
allocator and nedalloc and tell you how much faster nedalloc is. It also
serves as an example of usage.

B. Notes:
-=-=-=-=-
If you want the very latest version of this allocator, get it from the
TnFOX SVN repository at svn://svn.berlios.de/viewcvs/tnfox/trunk/src/nedmalloc

Because of how nedalloc allocates an mspace per thread, it can cause
severe bloating of memory usage under certain allocation patterns.
You can substantially reduce this wastage by setting MAXTHREADSINPOOL
or the threads parameter to nedcreatepool() to a fraction of the number of
threads which would normally be in a pool at once. This will reduce
bloating at the cost of an increase in lock contention. If allocated size
is less than THREADCACHEMAX, locking is avoided 90-99% of the time and
if most of your allocations are below this value, you can safely set
MAXTHREADSINPOOL to one.

You will suffer memory leakage unless you call neddisablethreadcache()
per pool for every thread which exits. This is because nedalloc cannot
portably know when a thread exits and thus when its thread cache can
be returned for use by other code. Don't forget pool zero, the system pool.

Equally if you use nedmalloc from a DLL which you later kick out of
memory, you will leak memory if you don't disable all thread caches for
all pools (as per the preceding paragraph), destroy all thread pools
using neddestroypool() and destroy the system pool using neddestroysyspool().

For C++ type allocation patterns (where the same sizes of memory are
regularly allocated and deallocated as objects are created and destroyed),
the threadcache always benefits performance. If however your allocation
patterns are different, searching the threadcache may significantly slow
down your code - as a rule of thumb, if cache utilisation is below 80%
(see the source for neddisablethreadcache() for how to enable debug
printing in release mode) then you should disable the thread cache for
that thread. You can compile out the threadcache code by setting
THREADCACHEMAX to zero.

C. Speed comparisons:
-=-=-=-=-=-=-=-=-=-=-
See Benchmarks.xls for details.

The enclosed test.c can do two things: it can be a torture test or a speed
test. The speed test is designed to be a representative synthetic
memory allocator test. It works by randomly mixing allocations with frees
with half of the allocation sizes being a two power multiple less than
512 bytes (to mimic C++ stack instantiated objects) and the other half
being a simple random value less than 16Kb. 

The real world code results are from Tn's TestIO benchmark. This is a
heavily multithreaded and memory intensive benchmark with a lot of branching
and other stuff modern processors don't like so much. As you'll note, the
test doesn't show the benefits of the threadcache mostly due to the saturation
of the memory bus being the limiting factor.

D. Troubleshooting:
-=-=-=-=-==-=-=-=-=
I get a quite a few bug reports about code not working properly under nedmalloc.
I do not wish to sound presumptuous, however in an overwhelming majority of cases the
problem is in your application code and not nedmalloc (see below for all the bugs
reported and fixed since 2006). Some of the largest corporations and IT deployments
in the world use nedmalloc, and it has been very heavily stress tested on everything
from 32 processor SMP clusters right through to root DNS servers, ATM machine networks
and embedded operating systems requiring a very high uptime.

In particular, just because it just happens to appear to work under the system
allocator does not mean that your application is not riddled with memory corruption
and non-ANSI usage of the API! And usually this is not your code's fault, but rather
the third party libraries being used.

Even though debugging an application for memory errors is a true black art made
possible only with a great deal of patience, intuition and skill, here is a checklist
for things to do before reporting a bug in nedmalloc:

1. Make SURE you try nedmalloc from SVN HEAD. For around six months of 2007 I kept
getting the same report of a bug long fixed in SVN HEAD.

2. Make use of nedmalloc's internal debug routines. Try turning on full sanity
checks by #define FULLSANITYCHECKS 1. Also make use of all the assertion checking
performed when DEBUG is defined. A lot of bug reports are made before running under
a debug build where an assertion trip clearly showed the problem. Lastly, try
changing the thread cache by #defining THREADCACHEMAX - this fundamentally changes
how the memory allocator behaves: if everything is fine with the thread cache fully
on or fully off, then this strongly suggests the source of your problem.

3. Make SURE you are matching allocations and frees belonging to nedmalloc.
nedmalloc does not patch itself in to replace the system allocator (if you want that,
try the Hoard memory allocator) because it is intended to coexist with any or
all other memory allocators. Attempting to free a block not allocated by nedmalloc
will end badly, similarly passing one of nedmalloc's blocks to another allocator
will likely also end badly. I have inserted as many assertion and debug checks for
this possibility as I can think of (further suggestions are welcome), but no system
can ever be watertight. If you're using C++, I would use its strong template type
system to have the compiler guarantee membership of a memory pointer - see the Boost
libraries, or indeed my own TnFOX portability toolkit (http://www.nedprod.com/TnFOX/).

4. If you're still having problems, or more likely your code runs absolutely fine
under debug builds but trips up under release which suggests a timing bug, it is
time to deploy heavyweight tools. Under Linux, you should use valgrind. Under Windows,
there is an excellent commercial tool called Glowcode (http://www.glowcode.com/).
Any programming team serious on quality should ALWAYS run their projects through
these tools before each and every release anyway - you would be amazed at what you
miss during all other testing.

5. Lastly, in the worst case scenario, consider hiring in a memory debugging
expert. There are quite a few on the market and they often are authors of memory
allocators. Wolfram Gloger (the author of ptmalloc) provides consulting services.
My own consulting company ned Productions Ltd. may be able to provide such a
service depending on our current workload.

I hope that these tips help. And I urge anyone considering simply dropping
back to the system allocator as a quick fix to reconsider: squashing memory bugs
often brings with it significant extra benefits in performance and reliability.
It may cost a lot extra now but it usually will save itself many times its cost
over the next few years. I know of one large multinational corporation who saved
hundreds of millions of dollars due to the debugging of their system software
performed when trying to get it working with nedmalloc - they found one bug in
nedmalloc but over a hundred in their own code, and in the process improved
performance threefold which saved an expensive hardware upgrade and deployment.
Fixing memory bugs now tends to be worth it in the long run.

E. ChangeLog:
-=-=-=-=-=-=-
v1.06 ?:
 * { 1079 } Fixed misdeclaration of struct mallinfo as C++ type. Thanks to
James Mansion for reporting this.
 * { 1082 } Fixed dlmalloc bug which caused header corruption to mmap()
allocations when running under multiple threads
 * { 1088 } Fixed assertion failure for nedblksize() with latest dlmalloc.
Thanks to Anteru for reporting this.
 * { 1088 } Added neddestroysyspool(). Thanks to Lars Wehmeyer for
suggesting this.
 * { 1088 } Fixed thread id high bit set bug causing SIGABRT on Mac OS X.
Thanks to Chris Dillman for reporting this.
 * { 1094 } Integrated dlmalloc v2.8.4 final.
 * { 1095 } Added nedtrimthreadcache(). Thanks to Hayim Hendeles for
suggesting this.
 * { 1095 } Fixed silly assertion of null pointer dereference. Thanks to
Ullrich Heinemann for reporting this.
 * { 1096 } Fixed lots of level 4 warnings on MSVC. Thanks to Anteru for
suggesting this.
 * { 1098 } Improved non-nedmalloc block detection to 6.25% probability
of being wrong. Thanks to Advanced Research Associates for sponsoring this.

v1.05 15th June 2008:
 * { 1042 } Added error check for TLSSET() and TLSFREE() macros. Thanks to
Markus Elfring for reporting this.
 * { 1043 } Fixed a segfault when freeing memory allocated using
nedindependent_comalloc(). Thanks to Pavel Vozenilek for reporting this.

v1.04 14th July 2007:
 * Fixed a bug with the new optimised implementation that failed to lock
on a realloc under certain conditions.
 * Fixed lack of thread synchronisation in InitPool() causing pool corruption
 * Fixed a memory leak of thread cache contents on disabling. Thanks to Earl
Chew for reporting this.
 * Added a sanity check for freed blocks being valid.
 * Reworked test.c into being a torture test.
 * Fixed GCC assembler optimisation misspecification

v1.04alpha_svn915 7th October 2006:
 * Fixed failure to unlock thread cache list if allocating a new list failed.
Thanks to Dmitry Chichkov for reporting this. Futher thanks to Aleksey Sanin.
 * Fixed realloc(0, <size>) segfaulting. Thanks to Dmitry Chichkov for
reporting this.
 * Made config defines #ifndef so they can be overriden by the build system.
Thanks to Aleksey Sanin for suggesting this.
 * Fixed deadlock in nedprealloc() due to unnecessary locking of preferred
thread mspace when mspace_realloc() always uses the original block's mspace
anyway. Thanks to Aleksey Sanin for reporting this.
 * Made some speed improvements by hacking mspace_malloc() to no longer lock
its mspace, thus allowing the recursive mutex implementation to be removed
with an associated speed increase. Thanks to Aleksey Sanin for suggesting this.
 * Fixed a bug where allocating mspaces overran its max limit. Thanks to
Aleksey Sanin for reporting this.

v1.03 10th July 2006:
 * Fixed memory corruption bug in threadcache code which only appeared with >4
threads and in heavy use of the threadcache.

v1.02 15th May 2006:
 * Integrated dlmalloc v2.8.4, fixing the win32 memory release problem and
improving performance still further. Speed is now up to twice the speed of v1.01
(average is 67% faster).
 * Fixed win32 critical section implementation. Thanks to Pavel Kuznetsov
for reporting this.
 * Wasn't locking mspace if all mspaces were locked. Thanks to Pavel Kuznetsov
for reporting this.
 * Added Apple Mac OS X support.

v1.01 24th February 2006:
 * Fixed multiprocessor scaling problems by removing sources of cache sloshing
 * Earl Chew <earl_chew <at> agilent <dot> com> sent patches for the following:
   1. size2binidx() wasn't working for default code path (non x86)
   2. Fixed failure to release mspace lock under certain circumstances which
      caused a deadlock

v1.00 1st January 2006:
 * First release
