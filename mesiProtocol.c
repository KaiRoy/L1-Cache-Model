/*
Author: Jesus Zavala
Description: MESI Protocol
Class: ECE 585 - Winter 2024

*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

//creating constants for the code - which will be used for the address
#define addrConst 8

//addy will contain the address data that we will use for the MESI
int addy [addrConst] = {16,16,16,16,16,16,16,16};


//creating global variables
bool snoopBit = 0; //Creating a boolean snoop variable that will update to true/1 if there are a write detected.
//will get the following global variables from the N address
bool readingData = 0; //get value from readDataL1
bool writingData = 0; //get value from writeDataL1
bool invalidateData = 0; //get value from invComL2
bool simulatingL2cache = 0;  //creating a place holder for deciding if we want to simulate the L2 cache.


//creating possible state for the MESI protocol
enum MESI_STATE{
M,
E,
S,
I
};

enum NAddress{
READ = 0, //Read data request to L1 data cache
WRITE = 1, //Write data request to L1 data cache
FETCH = 2, //Instruction fetch (a read request to L1 instruction cache)
INVALIDATE = 4, //invalidate command from L2
REQUEST = 5, //data request from L2 (in response to snoop)
CLEAR = 8, // clear the cache and reset all state (and statistic)
PRINT = 9, //print contents and state of the cache (allow subsequent trace activity)
};

//creating a a enum variable that will contain the necessary tag, cache data and the current state of the cache
struct address{
	unsigned int tag;
	unsigned int index;
	unsigned int byteOffest;
};

struct way {
	unsigned short valid;
	unsigned int tag;
	enum MESI_STATE mesi;
	int LRU;
	unsigned short data[BYTEOFFSETSIZE];
};

union byteLine {
	struct way dWay[DWAYNUM];
	struct way iWay[IWAYNUM];
};


// Create instruction and data cache
union byteLine dCache[DCACHESIZE];
union byteLine iCache[ICACHESIZE];

// creating the portotype functions
int MESI_Protocol(char mesiState, enum MESI_STATE state);

//Need to create some functions that will get the data

int readDataL1(/*Add Details*/);
int writeDataL1(/**/);
int instFetch();
int invComL2();
int dataRqstL2();
void clearCache();
void printCont();




//need to create a function that  will pick other functions
//It will all depend which N address is selected from the trace file

void reading_N_Adress(enum NAddress N){
   switch(N){
        case READ: readDataL1(/*Add Details*/); break;
        case WRITE: writeDataL1(/*Add Details*/); break;
        case FETCH: instFetch(/*Add Details*/); break;
        case INVALIDATE: invComL2(/*Add Details*/);break;
        case REQUEST: dataRqstL2(/*Add Details*/);break;
        case CLEAR: clearCache (/*Add Details*/);break;
        case PRINT: printCont(/*Add details*/); break;
        default: printf("N address not found!"); break;

      }
}

//Create functions for reading the value
int readDataL1(){

    int checkingWay = -1; //need to initialize to -1 since this will be used to check the way in the cache
        //checking to see if the cache is empty or not
        for(int i = 0; i < DWAYNUM; i++){
            if(dCache[addr.index].dWay[i].tag == 0)
                checkingWay = i; //we will assign the checkingWay the current way number
        }

        //we have checked that current way, thus we will need to update the current way with the new information and change the MESI state to 'E' (EXLCUSIVE).
        if(checkingWay >= 0){
            //**Need to add code for adding the index value to the current way
            dCache[addr.index].dWay[checkingWay].mesi = 'E'; //need to change it to E state since the first write will be write through

            if(simulatingL2cache){
                printf("Read from L2 < %X >\n", addr);
            }
        }
        //if the checkingWay does not see an empty way, we need to check for MISS
        else if (checkingWay < 0){
                if ((checkingWay < 0) && (dCache[addr.index].dWay[checkingWay].mesi != 'I')){
                    //We checked the array, we see that it is not empty (checkingWay will be -1) and it is not currently invalid
                    //**need to add code to replace the LRU and evict it
                    if(simulatingL2cache){
                        printf("Read from L2 < %X >\n", addr);
                    }
                }
                else{
                    printf("MESI State is invalid.");
                    return -1;
                }
        }
        //We got a read HIT, we need to move to the next state that is given in the state diagram
        else{
                 //checking for the current MESI state for the index **WILL PROBABLY NEED TO ADD CODE ON WHAT TO DO WITH CACHE DETAILS
                switch (dCache[addr.index].dWay[checkingWay].mesi){
                    case 'M': dCache[addr.index].dWay[checkingWay].mesi = 'M'; break;//If we are reading and current state is 'M' (MODIFIED) we will go back to 'M'
                    case 'E': dCache[addr.index].dWay[checkingWay].mesi = 'S'; break;//If we are reading and current state is 'E' (EXCLUSIVE) we will go back to 'S' --> this is from the project explanation (considering this as read from other processor)
                    case 'S': dCache[addr.index].dWay[checkingWay].mesi = 'S'; break;//If we are reading and current state is 'S' (SHARED) we will go back to 'S'
                    case 'I': dCache[addr.index].dWay[checkingWay].mesi = 'S'; break;//If we are reading and current state is 'I' (INVALID) we will go to 'S'
                }
        }
return 0;
}

int writeDataL1(union byteLine *cache, struct address addr, int data){
    int checkingWay = -1; //need to initialize to -1 since this will be used to check the way in the cache

        //checking to see if the cache is empty or not
        for(int i = 0; i < DWAYNUM; i++){
            if(dCache[addr.index].dWay[i].tag == 0)
                checkingWay = i; //we will assign the checkingWay the current way number
        }

        //we have checked that current way, thus we will need to update the current way with the new information and change the MESI state to 'M' (MODIFIED).
        if(checkingWay >= 0){
            //**Need to add code for adding the cache details - such as what information is transferred over to the tags, index and offset.
            dCache[addr.index].dWay[checkingWay].mesi = 'M'; //need to change it to E state since the first write will be write through

            if(simulatingL2cache){
                printf("Write from L2 < %X >\n", addr);
            }
        }
                //if the checkingWay does not see an empty index, we need to check for MISS
        else if (checkingWay < 0){
                if ((checkingWay < 0) && (dCache[addr.index].dWay[checkingWay].mesi != 'I')){
                    //We checked the cache, we see that it is not empty (checkingWay will be -1) and it is not currently invalid
                    //**need to add code to replace the LRU and evict it
                    dCache[addr.index].dWay[checkingWay].mesi = 'E';
                    if(simulatingL2cache){
                        printf("Write from L2 < %X >\n", addr);
                    }
                }
                //NOT SURE IF I NEED TO ADD when mesi == 'I'
                else{
                    printf("MESI State is invalid.");
                    return -1;
                }
        }
                //We got a read HIT, we need to move to the next state that is given in the state diagram
        else{
                 //checking for the current MESI state for the index **WILL PROBABLY NEED TO ADD CODE ON WHAT TO DO WITH CACHE DETAILS
                switch (dCache[addr.index].dWay[checkingWay].mesi){
                    case 'M': dCache[addr.index].dWay[checkingWay].mesi = 'M'; break;//If we are reading and current state is 'M' (MODIFIED) we will go back to 'M'
                    case 'E': dCache[addr.index].dWay[checkingWay].mesi = 'M';
                                if(simulatingL2cache){
                                    printf("Read for ownership from L2 < %X >\n", addr);
                                }
                                    break;//If we are reading and current state is 'E' (EXCLUSIVE) we will go back to 'S' --> this is from the project explanation (considering this as read from other processor)
                    case 'S': dCache[addr.index].dWay[checkingWay].mesi = 'M'; break;//If we are reading and current state is 'S' (SHARED) we will go back to 'S'. WILL NEED TO ADD INVALIDATE TO THIS
                    case 'I': dCache[addr.index].dWay[checkingWay].mesi = 'M'; break;//If we are reading and current state is 'I' (INVALID) we will go to 'S'. WILL NEED TO ADD RFO TO THIS
                }
        }
return 0;
}
}

int instFetch(){

};

int invComL2(){
//might be the same as the dataRqstL2 where we will need to set the cache line to invalid since we are simulating the L2 cache communication?

}

int dataRqstL2(union byteLine *cache, struct address addr){  // Check the data cache
    for (int i = 0; i < DWAYNUM; ++i) {
        if(dCache[addr.index].dWay[i].tag == iCache[addr.index].iWay[i].tag) { //Compares to find tag of address to invalidate
            switch (dCache[addr.index].dWay[i].mesi) {
                case 'M': dCache[addr.index].dWay[i].mesi = 'I'; //changes MESI bit set to invalidate
                case 'E': dCache[addr.index].dWay[i].mesi = 'I'; //changes MESI bit set to invalidate
                case 'S': dCache[addr.index].dWay[i].mesi = 'I'; //changes MESI bit set to invalidate
                case 'I': return 0; 		    //do nothing, already invalid
            }
        }
    }
    return 0;
};

void clearCache(union byteLine *cache){ //clearing the cache line, will set all states to invalid
    for(int i = 0; i < IWAYNUM; i++){ //setting the instruction cache to invalid
        cache->iWay[i].mesi = 'I';
 }
    for(int j = 0; j < DWAYNUM; j++){ //setting the data cache to invalid
        cache->dWay[j].mesi = 'I';
    }
};

void printCont(){
};

//NEED TO CREATE functions that will output the character for the MESI

//IF reading, we do not need to update snooping
//IF writing, we need to update snooping


/**********************************************************************************************
************The function below will follow the state diagram that is shown in class************
**********************************************************************************************/

int MESI_Protocol(char mesiState, enum MESI_STATE state){
    //creating a local variable for the MESI character
    char S = mesiState;

//Start of INVALID transitions
    if ((state = 'I') && (snoopBit = 1) && (readingData = 1)){ //If the current state of the cache is invalid but we are trying to read and the snooping found data in another processor
        state = 'S'; //will change the state to SHARED and return the MESI character
        S = "S";
        return S;

    }else if((state = 'I') && (snoopBit = 0) && (readingData = 1)){ //If current state of cache is invalid, the snooping did not find anything but we are still trying to read.
        state = 'E'; //will change the state to EXCLUSIVE and return the MESI character
        S = "E";
        return S;

    }else if((state = 'I') && (writingData = 1)){ //If current state of the cache is invalid, and the writing bit is currently 1.
        state = 'M'; //will change the state to MODIFIED and return the MESI character
        S = "M";
        return S;
//End of INVALID transitions

//Start of EXCLUSIVE  transitions
    }else if((state = 'E') && (readingData = 1)){
        state = 'E';
        S = "E";
        return S;

    }else if ((state = 'E')&& (writingData = 1)){
        state = 'M';
        S = "M";
        return S;
//End of EXCLUSIVE transitions

//Start of SHARED transitions
    }else if((state = 'S') && (readingData = 1)){
        state = 'S';
        S = "S";
        return S;
    }else if ((state = 'S') && (invalidateData = 1)){
        state = 'M';
        S = "M";
        return S;
//End of Exclusive transitions

//Start of MODIFIED transitions
    }else if((state = 'M') && (readingData = 1)){
        state = 'M';
        S = "M";
        return S;
    }else if((state = 'M') && (writingData = 1)){
        state = 'M';
        S = "M";
        return S;
//end of MODIFIED transitions

    }else{
     printf("State not found - ERROR encountered.");
     exit(EXIT_FAILURE);
    }

};

