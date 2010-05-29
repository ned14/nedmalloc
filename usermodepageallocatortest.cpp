/* usermodepageallocatortest.cpp
Does some unit testing of the user mode page allocator internal functions
(C) 2010 Niall Douglas
*/

#define _CRT_SECURE_NO_WARNINGS 1	/* Don't care about MSVC warnings on POSIX functions */
#include <stdio.h>
#include <stdlib.h>
#include "nedmalloc.h"

#define LOOPS 4
#define ALLOCATIONS 4096

#ifdef _MSC_VER
/*#pragma optimize("g", off)*/	/* Useful for debugging */
#endif

#if !defined(USE_NEDMALLOC_DLL)
#include "nedmalloc.c"
#elif defined(WIN32)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#ifdef WIN32
typedef unsigned __int64 usCount;
static usCount GetUsCount()
{
	static LARGE_INTEGER ticksPerSec;
	static double scalefactor;
	LARGE_INTEGER val;
	if(!scalefactor)
	{
		if(QueryPerformanceFrequency(&ticksPerSec))
			scalefactor=ticksPerSec.QuadPart/1000000000000.0;
		else
			scalefactor=1;
	}
	if(!QueryPerformanceCounter(&val))
		return (usCount) GetTickCount() * 1000000000;
	return (usCount) (val.QuadPart/scalefactor);
}
#else
#include <sys/time.h>

typedef unsigned long long usCount;
static usCount GetUsCount()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return ((usCount) tv.tv_sec*1000000000000LL)+tv.tv_usec*1000000LL;
}
#endif

using namespace nedalloc;
using namespace std;

int PatchInNedmallocDLL(void);
int main(void)
{
#if defined(WIN32) && defined(USE_NEDMALLOC_DLL)
	PatchInNedmallocDLL();
#endif

#ifdef WIN32
	{	/* Force load of user32.dll so we can debug */
		BOOL v;
		SystemParametersInfo(SPI_GETBEEP, 0, &v, 0);
	}
#endif

	{
    unsigned int seed=1;
    static usCount times[ALLOCATIONS+1];
    static unsigned opcounts[ALLOCATIONS+1];
    static struct { void *addr; size_t size; } addrsize[ALLOCATIONS];
    int m, n, i;
    unsigned opcount;
    usCount start, end;
    size_t bytesallocated=0;

    printf("sizeof(RegionStorage_t)=%d\n", sizeof(RegionStorage_t));
    printf("sizeof(region_node_t)=%d\n", sizeof(region_node_t));
    printf("REGIONSPERSTORAGE=%d\n", REGIONSPERSTORAGE);
    assert(OSHavePhysicalPageSupport());
    printf("System memory pressure is %lf\n", OSSystemMemoryPressure());
    srand(1);
  #ifdef NDEBUG
    printf("\nPress to go ...\n");
    getchar();
    printf("Working ...\n");
  #endif
    // First test: give me a large stretch of memory, and then be very mean to it
    if(1)
    {
      const size_t regionsize=4*1024*1024;
      const size_t pagesinregion=regionsize/PAGE_SIZE;
      size_t allocations=0, releases=0;
      void *region;
      start=GetUsCount();
      region=AllocatePages(0, regionsize, 0);
      end=GetUsCount();
      printf("Allocating %uMb (%u pages) takes %lf ms\n", regionsize/1024/1024, pagesinregion, (end-start)/1000000000.0);
      start=GetUsCount();
      for(m=1; m<ALLOCATIONS; m++)
      {
        for(i=0; i<LOOPS; i++)
        {
          size_t pageoffset=rand() & (pagesinregion-1), pagelength=(rand() & (pagesinregion-1))/2, size;
          void *addr;
          if(pageoffset+pagelength>=pagesinregion) pagelength=pagesinregion-1-pageoffset;
          addr=(void *)((size_t) region + pageoffset*PAGE_SIZE);
          size=pagelength*PAGE_SIZE;
          if(m & 1)
          {
            ReleasePages(addr, size);
            releases+=pagelength;
          }
          else
          {
            AllocatePages(addr, size, 0);
            allocations+=pagelength;
          }
        }
      }
      end=GetUsCount();
      printf("allocate=%u, release=%u takes %lf ms (%lf ops/sec)\n", allocations, releases, (end-start)/1000000000.0, (allocations+releases)/((end-start)/1000000000000.0));
      start=GetUsCount();
      ReleasePages(region, regionsize);
      end=GetUsCount();
      printf("Releasing %uMb (%u pages) takes %lf ms\n", regionsize/1024/1024, pagesinregion, (end-start)/1000000000.0);
    }
    // Second test: allocate and randomly free lots of varying lengths
    for(m=1; m<ALLOCATIONS; m++)
    {
      opcount=0;
      start=GetUsCount();
      // Allocate and free
      for(i=0; i<LOOPS; i++)
      {
        for(n=0; n<m; n++)
        {
          unsigned r=rand()<<2;
          if(!addrsize[n].addr)
          { // Allocate
            size_t length=(r<<10) & 0xffff;
            length&=~(PAGE_SIZE-1);
            if(length<PAGE_SIZE) length=PAGE_SIZE;
            if(!(addrsize[n].addr=userpage_malloc(length, 0)))
            {
              assert(0);
              abort();
            }
            addrsize[n].size=length;
            bytesallocated+=length;
            opcount++;
          }
          else if(r&4)
          { // 50% Free
            userpage_free(addrsize[n].addr, addrsize[n].size);
            bytesallocated-=addrsize[n].size;
            addrsize[n].addr=0;
            opcount++;
          }
        }
      }
      end=GetUsCount();
      times[m]=end-start;
      opcounts[m]=opcount;
#ifdef NDEBUG
      if(!(m & 15))
#endif
      printf("%d allocations (%uMb) takes %lf ms (%lf ops/sec)\n", m, bytesallocated/1024/1024, times[m]/1000000000.0, opcounts[m]/(times[m]/1000000000000.0));
    }
    opcount=0;
    start=GetUsCount();
    // Free anything remaining
    for(n=0; n<ALLOCATIONS; n++)
    {
      if(addrsize[n].addr)
      {
        userpage_free(addrsize[n].addr, addrsize[n].size);
        addrsize[n].addr=0;
        opcount++;
      }
    }
    end=GetUsCount();
    times[n]=end-start;
    opcounts[n]=opcount;
    printf("Time taken to free %d blocks = %lf secs\n", opcount, (end-start)/1000000000000.0);
    // Make sure everything has been emptied correctly
    {
      region_node_t *r;
      REGION_FOREACH(r, regionL_tree_s, &lower.regiontreeL)
      {
        printf("*** Free block remaining %p - %p\n", r->start, r->end);
      }
      REGION_FOREACH(r, regionL_tree_s, &lower.regiontreeL)
      {
        printf("*** Free block remaining %p - %p\n", r->start, r->end);
      }
      REGION_FOREACH(r, regionA_tree_s, &upper.regiontreeA)
      {
        printf("*** Allocated block remaining %p - %p\n", r->start, r->end);
      }
      REGION_FOREACH(r, regionA_tree_s, &upper.regiontreeA)
      {
        printf("*** Allocated block remaining %p - %p\n", r->start, r->end);
      }
      assert(REGION_EMPTY(&lower.regiontreeA));
      assert(REGION_EMPTY(&lower.regiontreeL));
      assert(REGION_EMPTY(&upper.regiontreeA));
      assert(REGION_EMPTY(&upper.regiontreeL));
    }
    CheckFreeRegionNodes(&regionstorage);
    assert(!regionstorage);
    {
	    FILE *oh;
      double opcount=0;
      for(n=1; n<ALLOCATIONS; n++)
        opcount+=opcounts[n]/(times[n]/1000000000000.0);
      printf("\nAverage opcount over %u allocations = %lf\n", ALLOCATIONS, opcount/ALLOCATIONS);

	    oh=fopen("results.csv", "w");
	    fprintf(oh, "\"Time (ms)\",\"Ops/sec\"\n");
	    for(n=1; n<=ALLOCATIONS; n++)
	    {
		    fprintf(oh, "%lf,%lf\n", times[n]/1000000000.0, opcounts[n]/(times[n]/1000000000000.0));
	    }
	    fclose(oh);
    }

#ifdef _MSC_VER
		printf("\nPress a key to end\n");
		getchar();
#endif
	}
	return 0;
}
