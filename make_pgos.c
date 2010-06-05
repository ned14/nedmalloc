/* make_pgos.c
Simply runs through each of the most common code paths for PGO purposes
(C) 2010 Niall Douglas
*/

#define _CRT_SECURE_NO_WARNINGS 1	/* Don't care about MSVC warnings on POSIX functions */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "nedmalloc.h"

#define RECORDS 100000

int main(void)
{
	int n;
	void *mem[RECORDS];
	printf("nedmalloc PGO maker\n"
		   "-=-=-=-=-=-=-=-=-=-\n");
	printf("Small allocations\n");
	for(n=0; n<RECORDS; n++)
	{
		mem[n]=nedmalloc(64);
		if(n>0) mem[n-1]=nedrealloc(mem[n-1], 96);
		if(n>1) nedfree(mem[n-2]);
	}

	printf("Medium allocations\n");
	for(n=0; n<RECORDS/100; n++)
	{
		mem[n]=nedmalloc(64*1024);
		if(n>0) mem[n-1]=nedrealloc(mem[n-1], 96*1024);
		if(n>1) nedfree(mem[n-2]);
	}

	printf("Large allocations\n");
	for(n=0; n<RECORDS/1000; n++)
	{
		mem[n]=nedmalloc(4*1024*1024);
		if(n>0) mem[n-1]=nedrealloc(mem[n-1], 6*1024*1024);
		if(n>1) nedfree(mem[n-2]);
	}

	return 0;
}