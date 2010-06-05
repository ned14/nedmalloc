/* make_pgos.c
Simply runs through each of the most common code paths for PGO purposes
(C) 2010 Niall Douglas
*/

#define _CRT_SECURE_NO_WARNINGS 1	/* Don't care about MSVC warnings on POSIX functions */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "nedmalloc.h"

int main(void)
{
	void *mem1, *mem2;
	printf("nedmalloc PGO maker\n"
		   "-=-=-=-=-=-=-=-=-=-\n");
	printf("Small allocations\n");
	mem1=nedmalloc(64);
	mem2=nedrealloc(mem1, 96);
	nedfree(mem2);

	printf("Medium allocations\n");
	mem1=nedmalloc(65536);
	mem2=nedrealloc(mem1, 256*1024);
	nedfree(mem2);

	printf("Large allocations\n");
	mem1=nedmalloc(4*1024*1024);
	mem2=nedrealloc(mem1, 8*1024*1024);
	nedfree(mem2);

	return 0;
}