// g++ nedmalloctester3.c -o nedmalloctester3 -O3 -s -pthread

#ifndef WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define USE_LOCKS 1
#define USE_DL_PREFIX 1
#define NDEBUG
#define NO_NED_NAMESPACE

#include "nedmalloc.h"
#include "nedmalloc.c"

#define malloc_vpool nedmalloc
#define free_vpool nedfree
#define realloc_vpool nedrealloc

/*#define malloc_vpool malloc
#define free_vpool free
#define realloc_vpool realloc*/


#define TESTMEMMAX 1024*1024*2

void ** test=NULL;

int div_w=8; // block size to be sure that we touching pointers allocated from different thread ID-s

void malt(int thread){
	for(int iteracio=1; iteracio<80; iteracio+=4){
		for(int i=0; i<TESTMEMMAX; i++){
			if(((i/div_w)%10)!=thread) continue;        // 10 thread
			// printf("%d\n", i);
			test[i]=realloc_vpool(test[i], iteracio);
			memset(test[i], 1, iteracio);
		}
	}
}

void *malt2(void * threadid){ malt(1); }
void *malt3(void * threadid){ malt(2); }
void *malt4(void * threadid){ malt(3); }
void *malt5(void * threadid){ malt(4); }
void *malt6(void * threadid){ malt(5); }
void *malt7(void * threadid){ malt(6); }
void *malt8(void * threadid){ malt(7); }
void *malt9(void * threadid){ malt(8); }
void *malt10(void * threadid){ malt(9); }

void MallocStabTest3(){
	printf("test 3 begins...\n");

	test=(void**) malloc_vpool(128+(TESTMEMMAX*sizeof(void*)));
	for(int i=0; i<(TESTMEMMAX); i++) test[i]=NULL;

	for(int Z=0; Z<100; Z++){
		div_w=2+(rand()%40);  // random block size to be sure that we touching pointers allocated from different thread ID-s

		pthread_t TMP2=0;
		pthread_t TMP3=0;
		pthread_t TMP4=0;
		pthread_t TMP5=0;
		pthread_t TMP6=0;
		pthread_t TMP7=0;
		pthread_t TMP8=0;
		pthread_t TMP9=0;
		pthread_t TMP10=0;

		pthread_create(&TMP2, NULL, malt2, NULL);
		pthread_create(&TMP3, NULL, malt3, NULL);
		pthread_create(&TMP4, NULL, malt4, NULL);
		pthread_create(&TMP5, NULL, malt5, NULL);
		pthread_create(&TMP6, NULL, malt6, NULL);
		pthread_create(&TMP7, NULL, malt7, NULL);
		pthread_create(&TMP8, NULL, malt8, NULL);
		pthread_create(&TMP9, NULL, malt9, NULL);
		pthread_create(&TMP10, NULL, malt10, NULL);

		malt(0);

		pthread_join(TMP2, NULL);
		pthread_join(TMP3, NULL);
		pthread_join(TMP4, NULL);
		pthread_join(TMP5, NULL);
		pthread_join(TMP6, NULL);
		pthread_join(TMP7, NULL);
		pthread_join(TMP8, NULL);
		pthread_join(TMP9, NULL);
		pthread_join(TMP10, NULL);

		printf("%d percent finished\n", Z);
	}

	for(int i=0; i<(TESTMEMMAX); i++) if(test[i]) free_vpool(test[i]);
	free_vpool(test);
	printf("success.\n");
}

int main(){
	MallocStabTest3();
}
#else
int main(){
	return 0;
}
#endif
