/*
 ============================================================================
 Name        : ece585_cache_fp.c
 Author      : Kai Roy, Daisy Perez-Ruiz, Kamal Smith, Nicholas Allmeyer, Jesus Zavala
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>


//Cache Structure
#define DWAYNUM 3
#define IWAYNUM 4
#define DCACHESIZE 4
#define ICACHESIZE 4

enum MESI{M, E, S, I};

struct way {
	unsigned short valid;
	unsigned int tag;

	enum MESI mesi;
	unsigned int LRU;
};

union byteLine {
	struct way dWay[DWAYNUM];
	struct way iWay[IWAYNUM];
};



//Function Definitions
void initCache(union byteLine *cache, int cacheSize, char type);
int findInvalidWay(union byteLine var, char type);


//Main Function
int main(void) {
	union byteLine dCache[DCACHESIZE];
	union byteLine iCache[ICACHESIZE];

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


// Initializes all valid bits
void initCache(union byteLine *cache, int cacheSize, char type) {
	for (int i = 0; i < cacheSize; i++){
		if (type == 'i') {
			for (int j = 0; j < IWAYNUM; j++) {
				cache[i].iWay[j].valid = 0;
			}
		} else {
			for (int j = 0; j < DWAYNUM; j++) {
				cache[i].dWay[j].valid = 0;
			}
		}
	}
	return;
}


//Check if all ways are valid
//Return location of the first invalid way
//Return -1 if there are no invalid ways
int findInvalidWay(union byteLine var, char type){
	// instr cache Mode
	if (type == 'i') {
		for (int i = 0; i < IWAYNUM; i ++) {
			if (!var.iWay[i].valid)
				return i;
		}
	} else { // data cache mode
		for (int i = 0; i < IWAYNUM; i ++) {
			if (!var.dWay[i].valid)
				return i;
		}
	}
	return -1;
}



