//Group: Carina Rammelkamp, Andro Stotts, Peng Wang
//CS154, Project 3
#include "cachesim.h"
#define debug 1

/*You have a struct that contains all of the information for one cache. In this function, you create the cache and initialize it, returning a pointer to the struct. Because you are determining the struct, you return a void * to our main. Type 0 is a direct-mapped cache. Type 1 is a pseudo-associative cache. Type 2 is a 4-way set associative cache.*/


void *createAndInitialize(int blocksize, int cachesize, int type){
	Cache *cache= malloc(sizeof(Cache));

	//initialize fields
	cache->misscount=0;
	cache->accesscount=0;
	cache->accesstime=0;
	cache->blocksize = blocksize;
	cache->cachesize = cachesize;
	cache->type = type;
	if(type ==0){
		//calculate number of blocks for each set
		int test = logbase2(blocksize);
		if(debug) printf("number of bits: %d\n", test);
		cache->numBlocksPerSet = cachesize >> logbase2(blocksize);
		if(debug) printf("number of blocks: %d\n", cache->numBlocksPerSet);
		cache->set = malloc(sizeof(Set*)*(cache->numBlocksPerSet));
		initializeSet(cache->set, cache->numBlocksPerSet);
	}
	else if(type == 1){
		int test = logbase2(blocksize);
		if(debug) printf("number of bits: %d\n", test);
		cachesize >>= 1;
		cache->numBlocksPerSet = cachesize >> logbase2(blocksize);
		if(debug) printf("number of blocks: %d\n", cache->numBlocksPerSet);
		cache->set = malloc(sizeof(Set*)*(cache->numBlocksPerSet));
		cache->set1 = malloc(sizeof(Set*)*(cache->numBlocksPerSet));
		initializeSet(cache->set, cache->numBlocksPerSet);
		initializeSet(cache->set1, cache->numBlocksPerSet);
	}
	return (void*) cache;
}

/*In this function, we access the cache with a particular address. If the address results in a hit, return 1. If it is a miss, return 0.*/
int accessCache(void *cache, int address){
	Cache *c = (Cache *)cache;
	int tag = 0;
 	int index = 0;
	getTagAndIndex(c, &tag, &index, address);
	
	if(debug) printf("DEBUG: after decode tag: %d index: %d\n", tag, index);
	c->accesscount++;	

	if(c->type == 0){
		c->accesstime++;
		if(isMiss(c->set, tag, index)){
			fetch(c ,c->set, index, tag, c->set1);	
			return 0;
		}
		else{
			return 1;
		}
		
	}
	if(c->type == 1){
		c->accesstime++;
		int ret = isMiss(c->set, tag, index);
		if(ret == 2){
			fetch(c, c->set, index, tag, c->set1);
			return 0;
		}
		else if(ret == 1){
			c->accesstime++;
			ret = isMiss(c->set1, tag, index);
			if(ret == 2){
				fetch(c, c->set1, tag, index, c->set);
				return 0;
			}
			else if(ret == 1){
				if(c->set[index]->MRU)
					fetch(c, c->set1, tag, index, c->set);
				else
					fetch(c, c->set, tag, index, c->set1);
				return 0;	
			}
			else
				return 1;
		}
		else
			return 1;
	}
	return -1;
}

/*This returns the number of misses that have occurred so far*/
int missesSoFar(void *cache){
	return ((Cache*)cache)->misscount;

}

/*This returns the number of accesses that have occurred so far*/
int accessesSoFar(void *cache){
	return ((Cache*)cache)->accesscount;
}

/*This returns the total number of cycles that all of the accesses have taken so far.*/
int totalAccessTime(void *cache){
	return ((Cache*)cache)->accesstime;
}

int logbase2(int n) {
	if (n==0)
		return -1;
	n--;
	int logValue = 0;
	while (n) {
		logValue++;
		n >>= 1;
	}
	return logValue;
 }


void initializeSet(Set** set, int num){
	int i;	
	if(debug) printf("malloc %d blocks for set\n", num);
	for(i=0;i < num;i++){
		set[i]= malloc(sizeof(Set));
		set[i]->valid = 0;
		set[i]->tag = 0;
		set[i]->MRU = 0;
	}
}

void printSet(Set **set, int num){
	int j;
	for(j=0; j<num; j++){
		printf("index: %d valid = %d tag = %d MRU = %d\n", j,set[j]->valid, set[j]->tag, set[j]->MRU);	
	}
}

int isMiss(Set **set, int tag, int index){
	//if valid bit 1
	if(debug) printf("DEBUG: checking at index %d valid bit = %d tag = %d\n", index, set[index]->valid, set[index]->tag);

	if(set[index]->valid){
		//if tag found
		if(set[index]->tag==tag)
			return 0;	// this is no miss		
		//tag not found
		else
			return 1;	// this is a miss on tag		
	}
	//else valid bit 0 (MISS)
	else
		return 2;		// this is a miss on valid
}
void getTagAndIndex(Cache *c, int *tag, int *index, int address){
	int indexBits = logbase2(c->numBlocksPerSet);	
	int blockOffsetBits = logbase2(c->blocksize)-2;
	int byteOffsetBits = 2;
	int tagBits = 32 - indexBits - blockOffsetBits - byteOffsetBits;
	if(debug){
		printf("indexBits: %d\n",indexBits); 
		printf("blockOffsetBits: %d\n", blockOffsetBits);
		printf("byteOffsetBits: %d\n",byteOffsetBits);
		printf("tagBits: %d\n",tagBits);
	}

	*tag = address >> (32 - tagBits);
	*index = address >> (byteOffsetBits+blockOffsetBits) & ((1 << (indexBits))-1);
}

void fetch(Cache *c, Set **s, int index, int tag, Set **s1){
	s[index]->tag = tag;
	s[index]->valid = 1;
	s[index]->MRU = 1;
	//printSet(c->set, c->numBlocksPerSet);
	c->misscount++;
	c->accesstime+=100;

	if(c->type > 0)
		s1[index]->MRU = 0;
	printSet(s, c->numBlocksPerSet);
	printf("\n");
	printSet(s1, c->numBlocksPerSet);
}
