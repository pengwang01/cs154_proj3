#include "cachesim.h"
#define debug 1
int main (int argc, char *argv[]){

	Cache *cache = createAndInitialize(8, 64, 1);

	int num = 0;
	while(num < 45){
		printf("%d: Hit? %d\n",num,accessCache(cache, num));
		if(debug) printf("------------------------------------------\n");
		num = num+4;
	}
	num=64;
	while(num < 109){
		printf("%d: Hit? %d\n",num,accessCache(cache, num));
		if(debug) printf("------------------------------------------\n");
		num = num+4;
	}

	num = 0;
	while(num < 33){
		printf("%d: Hit? %d\n",num,accessCache(cache, num));
		if(debug) printf("------------------------------------------\n");
		num = num+4;
	}

	int miss = missesSoFar(cache);
	int access = accessesSoFar(cache);
	int time = totalAccessTime(cache);
	
	printf("\n%d Accesses, %d Misses\n",access, miss);
	printf("Total access time: %d\n",time);
	
	//int test = logbase2(125);
	//printf( "%i",test);
	return 0;
}
