/* unittests.cpp
Catch various bugs reported over the years
(C) 2012 Niall Douglas
*/

#define NEDMALLOCDEPRECATED
#define NEDMALLOC_DEBUG 1
#define FULLSANITYCHECKS

#include "nedmalloc.h"
#include <stdio.h>
#include <vector>

#if !defined(USE_NEDMALLOC_DLL)
#include "nedmalloc.c"
#endif

int main(void)
{
  using namespace std;
  using namespace nedalloc;
  // Crash when nedcreatepool is called in the fifth time
  // https://github.com/ned14/nedmalloc/issues/7
  printf("Testing: Crash when nedcreatepool is called in the fifth time ...\n");
  {
    vector<nedpool *> pools;
    size_t count=0;
    for(size_t n=1; n<256; n<<=1)
    {
      size_t oldsize=pools.size();
      pools.resize(n);
      for(size_t m=0; m<n; m++)
      {
        if(!pools[m])
        {
          printf("Creating pool %u ...\n", count+1);
          if(!(pools[m]=nedcreatepool(0, 0))) abort();
          count++;
        }
      }
      for(size_t m=0; m<n/2; m++)
      {
        size_t i=rand() % n;
        if(pools[i])
        {
          printf("Destroying pool %u ...\n", count);
          neddestroypool(pools[i]);
          pools[i]=0;
          count--;
        }
      }
    }
    for(size_t n=0; n<pools.size(); n++)
    {
      if(pools[n])
      {
        printf("Destroying pool %u ...\n", count);
        neddestroypool(pools[n]);
      }
    }
  }

  return 0;
}
