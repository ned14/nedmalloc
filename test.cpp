/* test.cpp
An example of how to use nedalloc in C++
(C) 2010 Niall Douglas
*/

#include <stdio.h>
#include <stdlib.h>
#include "nedmalloc.h"
#include <vector>
#include <tuple>

#ifdef _MSC_VER
/*#pragma optimize("g", off)*/	/* Useful for debugging */
#endif

#if !defined(USE_NEDMALLOC_DLL)
#include "nedmalloc.c"
#elif defined(WIN32)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#ifdef _MSC_VER
#define MEMALIGNED(v) __declspec(align(v))
#elif defined(__GNUC__)
#define MEMALIGNED(v) __attribute__ ((aligned(v)))
#else
#define MEMALIGNED(v)
#endif

using namespace nedalloc;
using namespace std;
using namespace std::tr1;

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

	/* This is the classic usage scenario: simply give the
	STL collection class a nedallocator of the same type */
	vector<int, nedallocator<int> > anyvector1;

	/* What if we are allocating SSE/AVX vectors and we
	need the array always allocated on a 16 byte boundary? */
	struct SSEVectorType
	{
#if defined(_M_X64) || defined(__x86_64__) || (defined(_M_IX86) && _M_IX86_FP>=1) || (defined(__i386__) && defined(__SSE__))
		operator __m128 &() { return *(__m128 *)this; }
#endif
		tuple<float, float, float, float> tup;
		SSEVectorType()
		{
			size_t myaddr=(size_t) this;
			if(myaddr & 15)
			{
				printf("SSEVectorType lives at %p when it ought to be 16 byte aligned!\n", this);
				abort();
			}
		}
	};
	vector<SSEVectorType, nedallocator<SSEVectorType, nedalignment<16> > > SSEvector1(5);

	/* Or if you couldn't be bothered typing so much, or you
	would like to genericise the use of STL nedallocator<>, use
	the nedallocate<> convenience template which will take any
	arbitrary STL container. */
	//nedallocate<vector, SSEVectorType, nedalignment<16> >::type SSEvector2;

	printf("\nPress a key to trim\n");
	getchar();
	nedmalloc_trim(0);
#ifdef _MSC_VER
	printf("\nPress a key to end\n");
	getchar();
#endif
	return 0;
}
