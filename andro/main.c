#include "cachesim.h"
void printCache(void *cache){
	
	int i;
	for(i=0;i < (1<<((Cache*)cache)->numsets); i++){
		printf("Set0: I: %d V: %d T: %d\n",i,((Cache*)cache)->set[i]->valid, ((Cache*)cache)->set[i]->tag);
		printf("Set1: I: %d V: %d T: %d\n",i,((Cache*)cache)->set1[i]->valid, ((Cache*)cache)->set1[i]->tag);
	}

}


int main (int argc, char *argv[]){

	Cache *cache = createAndInitialize(8, 64, 1);
	//printCache(cache);

	int addresses[] = { 0, 4, 8, 12, 64, 68, 72, 76, 0, 4, 8 };
	int ret;
	int i;
	for (i = 0; i < sizeof(addresses) / sizeof(int); i++)
	{	
		
		int address = addresses[i];
		//printf("%d: Hit? %d\n",addresses[i],ret = accessCache(cache, addresses[i]));
		int ret=accessCache(cache, address);
		//printf("\nCACHE after accessing %d Hit: %d\n",address, ret);
		//printCache(cache);
	}

	/*
	accessCache(cache, 1024);
	exit(0);

	int num = 0;
	while(num < 13){
		printf("%d: Hit? %d\n",num,accessCache(cache, num));
		num = num+4;
	}
	num = 64;
	while(num < 77){
		printf("%d: Hit? %d\n",num,accessCache(cache, num));
		num = num+4;
	}

	num = 0;
	while(num < 13){
		printf("%d: Hit? %d\n",num,accessCache(cache, num));
		num = num+4;
	}

	num = 0;
	while(num < 44){
		printf("%d: Hit? %d\n",num,accessCache(cache, num));
		num = num+4;
	}
	num=64;
	while(num < 109){
		printf("%d: Hit? %d\n",num,accessCache(cache, num));
		num = num+4;
	}*/

	int miss = missesSoFar(cache);
	int access = accessesSoFar(cache);
	int time = totalAccessTime(cache);
	
	//printf("\n%d Accesses, %d Misses\n",access, miss);
	//printf("Total access time: %d\n",time);
	return 0;
}
