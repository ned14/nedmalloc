/* test.cpp
An example of how to use nedalloc in C++
(C) 2010 Niall Douglas
*/

#include <stdio.h>
#include <stdlib.h>
#include "nedmalloc.h"

#ifdef _MSC_VER
/*#pragma optimize("g", off)*/	/* Useful for debugging */
#endif

#if !defined(USE_NEDMALLOC_DLL)
#include "nedmalloc.c"
#elif defined(WIN32)
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

using namespace nedalloc;

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
	printf("\nPress a key to trim\n");
	getchar();
	nedmalloc_trim(0);
#ifdef _MSC_VER
	printf("\nPress a key to end\n");
	getchar();
#endif
	return 0;
}
