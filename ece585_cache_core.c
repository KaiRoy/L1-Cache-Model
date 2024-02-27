/*
 ============================================================================
 Name        : ece585_cache_core.c
 Author      : Kai Roy
 Version     : 
 Copyright   : Your copyright notice
 Description : 
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>


/****************************************************************************
** Cache Structure
****************************************************************************/
#define DWAYNUM 8
#define IWAYNUM 4
#define DCACHESIZE 16384
#define ICACHESIZE 16384
#define BYTEOFFSETSIZE 64

enum MESI{M, E, S, I};

struct way {
	unsigned short valid;
	unsigned int tag;
	enum MESI mesi;
	unsigned int LRU;
	unsigned short data[BYTEOFFSETSIZE];
};

union byteLine {
	struct way dWay[DWAYNUM];
	struct way iWay[IWAYNUM];
};

struct address {
	unsigned int tag;
	unsigned int index;
	unsigned int byteOffest;
}



/****************************************************************************
** Function Definitions
****************************************************************************/
void initCache(union byteLine *cache, int cacheSize, char type);
int findInvalidWay(union byteLine var, char type);
int readFromDCache(union byteLine *cache, struct address addr);
int writeToDCache(union byteLine *cache, struct address addr, int data);
int instrFetch(union byteLine *cache, struct address addr);
int invalidL2(union byteLine *cache, struct address addr);
int dataReqL2(union byteLine *cache, struct address addr);
int clearCache(union byteLine *cache);
int printCache(union byteLine *cache);


/****************************************************************************
** Function: Main
** Version: v1.0.0
** Description: C Main Function
****************************************************************************/
int main(void) {
	// Create instruction and data cache
	union byteLine dCache[DCACHESIZE];
	union byteLine iCache[ICACHESIZE];

	// Initialize instruction and data cache
	initCache(dCache, DCACHESIZE, 'd');
	initCache(iCache, ICACHESIZE, 'i');

	dCache[0].dWay[0].valid = 1;
	dCache[0].dWay[1].valid = 1;
	dCache[1].dWay[1].valid = 1;
	dCache[2].dWay[0].valid = 1;
	dCache[2].dWay[1].valid = 1;
	dCache[2].dWay[2].valid = 1;

	int test0 = findInvalidWay(dCache[0], 'd');	// 1
	int test1 = findInvalidWay(dCache[1], 'd');	// 0
	int test2 = findInvalidWay(dCache[2], 'd');	// -1

	printf("!!!Hello World!!!\n"); /* prints !!!Hello World!!! */
	printf("Test 0: %d\n", test0);
	printf("Test 1: %d\n", test1);
	printf("Test 2: %d\n", test2);
	return EXIT_SUCCESS;
}


/****************************************************************************
** Function: initCache
** Version: v1.0.0
** Description: Initializes all valid bits in cache to 0. 
****************************************************************************/
void initCache(union byteLine *cache, int cacheSize, char type) {
	// Loop through every cache line
	for (int i = 0; i < cacheSize; i++){
		// Loop through every way/block
		if (type == 'i') {		// Instruction cache
			for (int j = 0; j < IWAYNUM; j++) {
				cache[i].iWay[j].valid = 0;
			}
		} else {				// Data cache
			for (int j = 0; j < DWAYNUM; j++) {
				cache[i].dWay[j].valid = 0;
			}
		}
	}
	return;
}


/****************************************************************************
** Function: findInvalidWay
** Version: v1.0.0
** Description: Check if all ways are valid. Return -1 if there are no invalid
** ways, else return the location of the first invalid way.
****************************************************************************/
int findInvalidWay(union byteLine var, char type){
	// Loop through every way/block
	if (type == 'i') {			// Instruction cache
		for (int i = 0; i < IWAYNUM; i ++) {
			if (!var.iWay[i].valid)		// Check valid bit
				return i;
		}
	} else { 					// Data cache
		for (int i = 0; i < IWAYNUM; i ++) {
			if (!var.dWay[i].valid)		// Check valid bit
				return i;
		}
	}
	return -1;
}


/****************************************************************************
** Function: readFromDCache
** Version: v1.0.0
** Description: Read data request to L1 data cache
****************************************************************************/
int readFromDCache(union byteLine *cache, struct address addr) {

}


/****************************************************************************
** Function: writeToDCache
** Version: v1.0.0
** Description: Write data request to L1 data cache
****************************************************************************/
int writeToDCache(union byteLine *cache, struct address addr, int data) {

}


/****************************************************************************
** Function: instrFetch
** Version: v1.0.0
** Description: instruction fetch (a read request to L1 instruction cache)
****************************************************************************/
int instrFetch(union byteLine *cache, struct address addr) {

}


/****************************************************************************
** Function: invalidL2
** Version: v1.0.0
** Description: Invalidate command from L2
****************************************************************************/
int invalidL2(union byteLine *cache, struct address addr); {

}


/****************************************************************************
** Function: dataReqL2
** Version: v1.0.0
** Description: Data request from L2 (in response to snoop)
****************************************************************************/
int dataReqL2(union byteLine *cache, struct address addr) {

}


/****************************************************************************
** Function: clearCache
** Version: v1.0.0
** Description: Clear the cache and reset all state (and statistics)
****************************************************************************/
int clearCache(union byteLine *cache) {

}


/****************************************************************************
** Function: printCache
** Version: v1.0.0
** Description: Print contents and state of the cache (allow subsequent trace 
** activity)
****************************************************************************/
int printCache(union byteLine *cache) {

}