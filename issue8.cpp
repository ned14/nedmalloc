#define USE_LOCKS 1
#define USE_DL_PREFIX 1
#define DEBUG 1
#define NO_NED_NAMESPACE
//#define INSECURE 1
//#define DEBUG 1
#define NEDMALLOCDEPRECATED

#define NEDMALLOC_DEBUG 1
//#define FULLSANITYCHECKS

//#define THREADCACHEMAX 0

//#define USE_MAGIC_HEADERS 1
//#define USE_ALLOCATOR 0
// #define MSPACES 1
// #define ONLY_MSPACES 1

#include "nedmalloc.c"


void MallocStabTest(){
    printf("test\n");
    void ** test=(void**)nedmalloc(1024*1024*16*sizeof(void**));
    for(int i=0;i<(1024*1024*16);i++) test[i]=NULL;
    for(int j=0;j<10000000;j++){
        int testbyte=rand()%((1024*1024*16)-128);

        for(int w=0;w<127;w++){
            if(test[testbyte+w]){
                nedfree(test[testbyte+w]);
                test[testbyte+w]=NULL;
            }
        }

        test[testbyte++]=nedmalloc(10);
        test[testbyte++]=nedmalloc(40);
        test[testbyte++]=nedmalloc(100);

        test[testbyte+2]=nedmalloc(100);
        test[testbyte+1]=nedmalloc(40);
        test[testbyte]=nedmalloc(7);

        testbyte+=3;

        void * mallblocks[7];
        size_t mallsz[7]={10,20,30,40,50,60,70};
        nedindependent_comalloc(7, mallsz, mallblocks);

        test[testbyte++]=mallblocks[0];
        test[testbyte++]=mallblocks[6];
        test[testbyte++]=mallblocks[2];
        test[testbyte++]=mallblocks[5];
        test[testbyte++]=mallblocks[4];
        test[testbyte++]=mallblocks[3];
        test[testbyte++]=mallblocks[1];

    }
    for(int i=0;i<(1024*1024*16);i++) if(test[i]) nedfree(test[i]);
    nedfree(test);
    printf("end\n");
}


int main(void)
{
  MallocStabTest();
  getchar();
  return 0;
}
