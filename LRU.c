/*****************************************************************************************
File: LRU.c

Purpose: To keep track of the LRU values for each called tag.

Assumptions: that 111 is the MRU

*****************************************************************************************/



#include <stdio.h>

#define NUM_CACHE_ENTRIES    4
#define MRU_VAL             7

typedef struct way {
    unsigned short valid;
    unsigned int tag;
    int LRU;  
} way_t;

way_t cache[NUM_CACHE_ENTRIES];

void LRU(int request, unsigned int req_tag) {
    if (request >= 0 && request < 10) {		//For the commands 0-9 listed in the assignment
        int located = -1;					//Default value of -1
        
        // Check for cache hit
        for (int j = 0; j < NUM_CACHE_ENTRIES; j++) {
            if (cache[j].tag == req_tag && cache[j].valid == 1) {
                located = j;
                break;
            }
        }
       	// Tag found, update LRU for accessed tag 
        if (located != -1) { 
            cache[located].LRU = MRU_VAL;  // Set LRU to MRU_VAL for accessed tag
            // Decrement LRU values for other tags
            for (int i = 0; i < NUM_CACHE_ENTRIES; i++) {
                if (cache[i].valid == 1 && i != located && cache[i].LRU > 0) {
                    cache[i].LRU--;
                }
            }
        } else {
            // Tag not found, find LRU entry or replace existing tag
            int lru_loc = 0;
            for (int i = 1; i < NUM_CACHE_ENTRIES; i++) {
                if (cache[i].LRU < cache[lru_loc].LRU) {
                    lru_loc = i;
                }
            }
            // Check if the requested tag is already present
            for (int i = 0; i < NUM_CACHE_ENTRIES; i++) {
                if (cache[i].tag == req_tag && cache[i].valid == 1) {
                    lru_loc = i; // Update location to the existing tag
                    break;
                }
            }
            cache[lru_loc].valid = 1;
            cache[lru_loc].tag = req_tag;  // Update tag to the requested tag
            cache[lru_loc].LRU = MRU_VAL;  // Set LRU to MRU_VAL for the new entry or existing tag
            // Decrement LRU values for other valid entries
            for (int i = 0; i < NUM_CACHE_ENTRIES; i++) {
                if (cache[i].valid == 1 && i != lru_loc && cache[i].LRU > 0) {
                    cache[i].LRU--;
                }
            }
        }
    }
}

void print_cache_state(const char* message) {
    printf("%s\n", message);
    for (int i = 0; i < NUM_CACHE_ENTRIES; i++) {
        printf("Entry %d: Valid: %d, Tag: %u, LRU: %d\n", i, cache[i].valid, cache[i].tag, cache[i].LRU);
    }
    printf("\n");
}

int main() {
    int request;   // Request type 0-9
    int req_data;  // Requested data

    // Initialize cache entries (all invalid initially)
    for (int i = 0; i < NUM_CACHE_ENTRIES; i++) {
        cache[i].valid = 0;
        cache[i].tag = 0;
        cache[i].LRU = -1; // Initialize LRU to -1
    }

    print_cache_state("Initial state:");

    LRU(1, 10);
    print_cache_state("Test1:");

    LRU(2, 20);
    print_cache_state("Test2:");

    LRU(3, 30);
    print_cache_state("Test3:");

    LRU(4, 10); // Same tag as Test1
    print_cache_state("Test4:");
    
    LRU(4, 50); // Same tag as Test1
    print_cache_state("Test5:");
    return 0;
}
