nedalloc v1.01 ? 2006:
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

by Niall Douglas (http://www.nedprod.com/programs/portable/nedmalloc/)

Enclosed is nedalloc, an alternative malloc implementation for multiple
threads without lock contention based on dlmalloc v2.8.3. It is more
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

Known Bugs:
-=-=-=-=-=-
dlmalloc v2.8.3 currently has a problem where it doesn't properly release
memory back to the system, especially under win32.

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
1. The enclosed test.c is designed to be a representative synthetic
memory allocator test. It works by randomly mixing allocations with frees
with half of the allocation sizes being a two power multiple less than
512 bytes (to mimic C++ stack instantiated objects) and the other half
being a simple random value less than 16Kb. See Benchmarks.xls for
details.


2. Results from Tn's TestIO benchmark. This is a heavily multithreaded
and memory intensive benchmark with a lot of branching and other stuff
modern processors don't like so much. These figures are for SVN rev 532.
As you'll note, the memory allocator isn't that much of a bottleneck here.

Microsoft Windows XP SP2 on x86 uniprocessor
Win32 allocator: 109.4Mb/sec
ptmalloc2:       164.3Mb/sec
nedmalloc:       166.7Mb/sec

As you can see, nedmalloc is slightly faster but testing shows that this
is due to the threadcache. If disabled, one only gets ~160Mb/sec showing
how ptmalloc2 is slightly more efficient as a raw allocator.


ChangeLog:
-=-=-=-=-=
v1.01 ?:
 * Fixed multiprocessor scaling problems by removing sources of cache sloshing
 * Earl Chew <earl_chew <at> agilent <dot> com> sent patches for the following:
   1. size2binidx() wasn't working for default code path (non x86)
   2. Fixed failure to release mspace lock under certain circumstances which
      caused a deadlock

v1.00 1st January 2006:
 * First release
