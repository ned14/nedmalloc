nedalloc v1.02 ?:
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

by Niall Douglas (http://www.nedprod.com/programs/portable/nedmalloc/)

Enclosed is nedalloc, an alternative malloc implementation for multiple
threads without lock contention based on dlmalloc v2.8.4. It is more
or less a newer implementation of ptmalloc2, the standard allocator in
Linux (which is based on dlmalloc v2.7.0) but also contains a per-thread
cache for maximum CPU scalability.

It is licensed under the Boost Software License which basically means
you can do anything you like with it. This does not apply to the malloc.c.h
file which remains copyright to others.

It has been tested on win32 (x86), win64 (x64), Linux (x64) and
FreeBSD (x64). It works very well on all of these and is very significantly
faster than the system allocator on all of these platforms.

By literally dropping in this allocator as a replacement for your system
allocator, you can see real world improvements of up to three times in normal
code!

To use:
-=-=-=-
Drop in nedmalloc.h, nedmalloc.c and malloc.c.h into your project.
Configure using the instructions in nedmalloc.h. Run and enjoy.

To test, compile test.c. It will run a comparison between your system
allocator and nedalloc and tell you how much faster nedalloc is. It also
serves as an example of usage.

Notes:
-=-=-=
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
be returned for use by other code.

For C++ type allocation patterns (where the same sizes of memory are
regularly allocated and deallocated as objects are created and destroyed),
the threadcache always benefits performance. If however your allocation
patterns are different, searching the threadcache may significantly slow
down your code - as a rule of thumb, if cache utilisation is below 80%
(see the source for neddisablethreadcache() for how to enable debug
printing in release mode) then you should disable the thread cache for
that thread. You can compile out the threadcache code by setting
THREADCACHEMAX to zero.

Speed comparisons:
-=-=-=-=-=-=-=-=-=
See Benchmarks.xls for details.

The enclosed test.c is designed to be a representative synthetic
memory allocator test. It works by randomly mixing allocations with frees
with half of the allocation sizes being a two power multiple less than
512 bytes (to mimic C++ stack instantiated objects) and the other half
being a simple random value less than 16Kb. 

The real world code results are from Tn's TestIO benchmark. This is a
heavily multithreaded and memory intensive benchmark with a lot of branching
and other stuff modern processors don't like so much. As you'll note, the
test doesn't show the benefits of the threadcache mostly due to the saturation
of the memory bus being the limiting factor.

ChangeLog:
-=-=-=-=-=
v1.02 ?:
 * Integrated dlmalloc v2.8.4, fixing the win32 memory release problem and
improving performance still further

v1.01 24th February 2006:
 * Fixed multiprocessor scaling problems by removing sources of cache sloshing
 * Earl Chew <earl_chew <at> agilent <dot> com> sent patches for the following:
   1. size2binidx() wasn't working for default code path (non x86)
   2. Fixed failure to release mspace lock under certain circumstances which
      caused a deadlock

v1.00 1st January 2006:
 * First release
