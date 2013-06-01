//Group: Carina Rammelkamp, Andro Stotts, Peng Wang
//CS154, Project 3
#include "cachesim.h"
#define DEBUG 0
/*You have a struct that contains all of the information for one cache. In this function, you create the cache and initialize it, returning a pointer to the struct. Because you are determining the struct, you return a void * to our main. Type 0 is a direct-mapped cache. Type 1 is a pseudo-associative cache. Type 2 is a 4-way set associative cache.*/

void *createAndInitialize(int blocksize, int cachesize, int type){
	if(DEBUG) printf("blocksize: %d, cachesize: %d\n", blocksize, cachesize);
	Cache *cache=malloc(sizeof(Cache));
	cache->blocksize = blocksize;
	cache->cachesize = cachesize;
	cache->type = type;
	cache->misscount=0;
	cache->accesscount=0;histor
	cache->accesstime=0;
	int numcacheblocks = calculatesets(blocksize,cachesize); //128
	if(type ==0){
		cache->numsets = logbase2(numcacheblocks);
		cache->set = calloc(numcacheblocks,sizeof(Set*));
		initializeSet(cache->set, cache->numsets);	
	}
	else if(type ==1){
		cache->numsets = logbase2(numcacheblocks>>1);
		cache->set = calloc(numcacheblocks>>1,sizeof(Set*));
		cache->set1 = calloc(numcacheblocks>>1,sizeof(Set*));	
		initializeSet(cache->set, cache->numsets);
		initializeSet(cache->set1, cache->numsets);
	}
	else if(type ==2){
		cache->numsets = logbase2(numcacheblocks >> 2);//logbase2(32)=5
		cache->set = calloc(numcacheblocks >> 2,sizeof(Set*));
		cache->set1 = calloc(numcacheblocks >> 2,sizeof(Set*));
		cache->set2 = calloc(numcacheblocks >> 2,sizeof(Set*));
		cache->set3 = calloc(numcacheblocks >> 2,sizeof(Set*));
		initializeSet(cache->set, cache->numsets);
		initializeSet(cache->set1, cache->numsets);
		initializeSet(cache->set2, cache->numsets);
		initializeSet(cache->set3, cache->numsets);	
	}
	else{
		printf("ERROR: incorrect type\n");
		exit(-1);
	}

	return (void*) cache;
}

/*In this function, we access the cache with a particular address. If the address results in a hit, return 1. If it is a miss, return 0.*/
int accessCache(void *cache, int address){
	((Cache*)cache)->accesscount++;
	int numsets = ((Cache*)cache)->numsets; //indices
	int offsetSize = logbase2(((Cache*)cache)->blocksize);
	int tagsize = 32-numsets-offsetSize;
	unsigned int tag = (address >> (32-tagsize)) & ((1<<tagsize)-1);
	
	int index = (address >> offsetSize) & ((1<<numsets)-1);
	int byteOffset=address & ((1<<offsetSize)-1);
	int type = ((Cache*)cache)->type;
	if(DEBUG){
		printf("ACCESSCACHE: ");
		printf("*numsets: %d, ",numsets); 
		printf("*Blocksize: %d, ", ((Cache*)cache)->blocksize);
		printf("*tagsize: %d, ",tagsize);
		printf("*offsetSize: %d, ",offsetSize);
		printf("*Tag: %d, ",tag);
		printf("*Index: %d, ",index);
		printf("*ByteOffset: %d, ",byteOffset);
		printf("---------------------------------------\n");
	}
	
	if(type == 0)
	{
		((Cache*)cache)->accesstime++;	
		int misscount = missOrHit(((Cache*)cache)->set[index]->valid, ((Cache*)cache)->set[index]->tag, tag);
		
		//if MISS
		if(misscount != 1){
			((Cache*)cache)->set[index]->tag = tag;
			((Cache*)cache)->set[index]->valid = 1;
			((Cache*)cache)->misscount++;
			((Cache*)cache)->accesstime+=100;
			misscount = 0;
		}
		return misscount;
	}
	else if (type ==1) //Pseudo Cache
	{
		((Cache*)cache)->accesstime++;
		//FIRST CACHE
		int misscount = missOrHit(((Cache*)cache)->set[index]->valid, ((Cache*)cache)->set[index]->tag, tag);
		//if tag does not match
		if(misscount == 2){
			((Cache*)cache)->accesstime++;
			 misscount = missOrHit(((Cache*)cache)->set1[index]->valid, ((Cache*)cache)->set1[index]->tag, tag);


			//SECOND CACHE
			if(misscount == 0){ //valid bit is zero
				((Cache*)cache)->accesstime+=100;
				((Cache*)cache)->set1[index]->valid = 1;
				((Cache*)cache)->misscount++;

				//swap tags
				((Cache*)cache)->set1[index]->tag = ((Cache*)cache)->set[index]->tag;
				((Cache*)cache)->set[index]->tag = tag;
				return 0;
			}
			else if (misscount == 2)
			{
				((Cache*)cache)->accesstime+=100;
				((Cache*)cache)->misscount++;

				//swap tags
				((Cache*)cache)->set1[index]->tag = ((Cache*)cache)->set[index]->tag;
				((Cache*)cache)->set[index]->tag = tag;
				return 0;
			}
			else {
				//swap tags
				((Cache*)cache)->set1[index]->tag = ((Cache*)cache)->set[index]->tag;
				((Cache*)cache)->set[index]->tag = tag;
				return 1;
			}

		}
		//if valid bit is zero
		else if (misscount ==0){
			((Cache*)cache)->accesstime++;
			((Cache*)cache)->accesstime+=100;
			((Cache*)cache)->misscount++;
			((Cache*)cache)->set[index]->valid = 1;
			((Cache*)cache)->set[index]->tag = tag;
			return 0;		
		}
		else{
			return 1;
		}
			 
 	
	}	

	else if (type ==2) //4-way-----------------------------------------------------------
	{
		((Cache*)cache)->accesstime += 3;

		//first check
		int misscount = missOrHit(((Cache*)cache)->set[index]->valid, ((Cache*)cache)->set[index]->tag, tag);

		if(misscount == 0){ //NOT VALID ON 1ST
			((Cache*)cache)->accesstime+=100;
			((Cache*)cache)->misscount++;	
			((Cache*)cache)->set[index]->tag = tag;
			((Cache*)cache)->set[index]->valid =1;	
			return 0;		
		}
		else if(misscount == 2) //BAD TAG ON 1ST
		{
			misscount = missOrHit(((Cache*)cache)->set1[index]->valid, ((Cache*)cache)->set1[index]->tag, tag);
			
			if(misscount == 0){ //NOT VALID ON 2ND
				((Cache*)cache)->accesstime+=100;
				((Cache*)cache)->misscount++;	
				((Cache*)cache)->set1[index]->valid =1;	
			
				//swap
				((Cache*)cache)->set1[index]->tag = ((Cache*)cache)->set[index]->tag;
				((Cache*)cache)->set[index]->tag = tag;
				return 0;
			}
			else if(misscount ==2){//BAD TAG ON 2ND
				misscount = missOrHit(((Cache*)cache)->set2[index]->valid, ((Cache*)cache)->set2[index]->tag, tag);

				if(misscount == 0){ //NOT VALID ON 3RD
					((Cache*)cache)->accesstime+=100;
					((Cache*)cache)->misscount++;	
					((Cache*)cache)->set2[index]->valid =1;	
			
					//swap
					((Cache*)cache)->set2[index]->tag = ((Cache*)cache)->set1[index]->tag;
					((Cache*)cache)->set1[index]->tag = ((Cache*)cache)->set[index]->tag;
					((Cache*)cache)->set[index]->tag = tag;
					return 0;
				}
				else if(misscount ==2){//BAD TAG ON 3RD 
					misscount = missOrHit(((Cache*)cache)->set3[index]->valid, ((Cache*)cache)->set3[index]->tag, tag);
					
					if(misscount != 1){ //NOT VALID ON 4TH or BAD TAG (MISS)
						((Cache*)cache)->accesstime+=100;
						((Cache*)cache)->misscount++;	
						((Cache*)cache)->set3[index]->valid =1;	
			
						//swap
						((Cache*)cache)->set3[index]->tag = ((Cache*)cache)->set2[index]->tag;
						((Cache*)cache)->set2[index]->tag = ((Cache*)cache)->set1[index]->tag;
						((Cache*)cache)->set1[index]->tag = ((Cache*)cache)->set[index]->tag;
						((Cache*)cache)->set[index]->tag = tag;
						return 0;
					}
					else {//HIT ON 4TH
		   	 			//swap
						((Cache*)cache)->set3[index]->tag = ((Cache*)cache)->set2[index]->tag;
						((Cache*)cache)->set2[index]->tag = ((Cache*)cache)->set1[index]->tag;
						((Cache*)cache)->set1[index]->tag = ((Cache*)cache)->set[index]->tag;
						((Cache*)cache)->set[index]->tag = tag;
				  		return 1;		
					}
				}
				else {//HIT ON 3RD
		   	 		//swap
					((Cache*)cache)->set2[index]->tag = ((Cache*)cache)->set1[index]->tag;
					((Cache*)cache)->set1[index]->tag = ((Cache*)cache)->set[index]->tag;
					((Cache*)cache)->set[index]->tag = tag;
				  	return 1;		
				}

			}
			else {//HIT ON 2ND
	   	 		//swap
				((Cache*)cache)->set1[index]->tag = ((Cache*)cache)->set[index]->tag;
				((Cache*)cache)->set[index]->tag = tag;
			  	return 1;		
			}
		}
		else //HIT ON 1ST
			return 1;	 	
	}
	//Should NEVER reach here	
	printf("ERROR: Wrong type given\n");
	exit (-1);
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

int calculatesets(int blocksize, int cachesize){
	if(DEBUG){
		printf("CALCULATESETS: \n");	
		printf("blocksize: %d\n",blocksize);
		printf("cachesize: %d\n",cachesize);
		printf("log2blocksize: %d\n",logbase2(blocksize));
		printf("cachesize/blocksize: %d\n",cachesize>>logbase2(blocksize));
		printf("roundedup cachesize/blocksize: %d\n",roundUp(cachesize>>logbase2(blocksize)));
	}	
	return roundUp(cachesize >> logbase2(blocksize));
}

int logbase2(int n) {

	if (n==0)
		return -1;
	int logValue = -1;
	while (n) {
		logValue++;
		n >>= 1;
	}

	return logValue;
 }

void initializeSet(Set** set, int numsets){
	
	int i;	
	for(i=0;i < (1<<numsets);i++){
		set[i]=malloc(sizeof(Set));
		set[i]->valid = 0;
		set[i]->tag = 0;
	}
}

int missOrHit(int valid, unsigned int cachetag, unsigned int tag)
{
	//if valid bit 1
	if(valid){
		//if tag found
		if(cachetag==tag){
			return 1;
		}
		//TAG NOT FOUND -- (MISS)
		else{
			return 2;
		}
	}
	//else valid bit 0 (MISS)
	else{
		return 0;
	}
	//Should never reach here
	printf("ERROR: reached unreachable location in missOrHit");
	return -1;
}

int roundUp(int num)
{

	if (num < 0)
		return 0;
	num--;
	num |= num >> 1;
	num |= num >> 2;
	num |= num >> 4;
	num |= num >> 8;
	num |= num >> 16;	
	return num + 1;
}
