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

//creating possible state for the MESI protocol
enum MESI_STATE{
M,
E,
S,
I
};

enum NAddress{
READ = 0, //Read data request to L1 data cache
WRITE, //Write data request to L1 data cache
FETCH, //Instruction fetch (a read request to L1 instruction cache)
INVALIDATE, //invalidate command from L2
REQUEST, //data request from L2 (in response to snoop)
CLEAR = 8, // clear the cache and reset all state (and statistic)
PRINT = 9, //print contents and state of the cache (allow subsequent trace activity)
};

//creating a a enum variable that will contain the necessary tag, cache data and the current state of the cache
struct address{
	unsigned int tag;
	unsigned int index;
	unsigned int byteOffest;
};

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
		return readingData = 1;
}

int writeDataL1(){
		return writingData = 1;
}

int instFetch(){

};

int invComL2(){
		return invalidateData = 1;

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

