/*
 ============================================================================
 Name        : ece585_cache_fp.c
 Authors     : Kai Roy, Nick Allmeyer, Daisy Perez, Jesus Zavala, Kamal Smith
 Version     :
 Copyright   : Your copyright notice
 Description :
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/****************************************************************************
** Macros
****************************************************************************/
// Trace File
#define TRACECHARLEN 9
#define TRACEDATALEN 8
#define MAXLEN 1000


/****************************************************************************
** Cache Structure
****************************************************************************/
#define DWAYNUM 8
#define IWAYNUM 4
#define DCACHESIZE 16384
#define ICACHESIZE 16384
#define BYTEOFFSETSIZE 64

enum MESI_STATE{M, E, S, I};

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

struct address {
	char hex[MAXLEN];
	unsigned int tag;
	unsigned int index;
	unsigned int byteOffset;
};

// Create instruction and data cache
union byteLine dCache[DCACHESIZE];
union byteLine iCache[ICACHESIZE];

struct address backup;


/****************************************************************************
** Globals
****************************************************************************/
// Trace File
int Tracen = 16;											//n
int TraceData [TRACEDATALEN] = {16,16,16,16,16,16,16,16};	//address data

// Mode control
int mode = 0;
int miss = 0;
int wb = 0;

enum commands{
	READ = 0, 			//Read data request to L1 data cache
	WRITE = 1, 			//Write data request to L1 data cache
	FETCH = 2, 			//Instruction fetch (a read request to L1 instruction cache)
	INVALID = 3, 	//invalidate command from L2
	REQUEST = 4, 		//data request from L2 (in response to snoop)
	CLEAR = 8, 			//clear the cache and reset all state (and statistic)
	PRINT = 9, 			//print contents and state of the cache (allow subsequent trace activity)
};


/****************************************************************************
** Function Prototypes
****************************************************************************/
// Cache / Cache Commands
void initCache(union byteLine *cache, int cacheSize, char type);
int findInvalidWay(union byteLine var, char type);
int readFromDCache(struct address addr);
int writeToDCache(struct address addr, int data);
int instrFetch(struct address addr);
int invalidL2(struct address addr);
int dataReqL2(struct address addr);
void clearCache();
int printCache(struct address addr);

// Trace File Handling
int TextToDecimal(char value);
void TraceDataHandler(char TraceLine[]);
int ExtractTag(int TD0, int TD1, int TD2);				//Trace data elements 0-2
int ExtractIndex(int TD3, int TD4, int TD5, int TD6);	//Trace data elements 3-6
int ExtractByteOffset(int TD6, int TD7);				//Trace data elements 6-7
int MaskLower(int TD6);									//Masks the lower two bits of element 6
int containshex(char string[]);                         //checks if a hex value is present in a string
int ishexval(char val);                                  //verifies is a character is a hex value

// LRU
int LRU_Data(struct address addr);
int LRU_Instr(struct address addr);
void print_byteline_state(const char* message, union byteLine var, char type);

// MESI
int MESI_Protocol(struct address addr, int way, char type, int snoopBit, int readingData, int writingData, int invalidateData);
char getStateName(enum MESI_STATE state);


/****************************************************************************
** Function: Main
** Version: v1.0.0
** Description: C Main Function
****************************************************************************/
int main(void) {
	clearCache();
	struct address addr;
	int step = 0;
	int state = 0;

	char fileName[50];
	int choice;

	//User Input here
	printf("\nEnter your Trace File: ");
	scanf("%s", fileName);

	FILE *fp;   //file pointer
	fp = fopen(fileName, "r");
	if (fp == NULL)
	{
		printf("ERROR: Could not open file\n");
		exit(1);
	}


	do {
		//Display the options for the mainMenu
		printf("\nPlease select an operating mode");
		printf("\n0: Mode 0");
		printf("\n1: Mode 1");
		printf("\n2: Exit program");

		//User Input here
		printf("\nEnter your choice: ");
		scanf("%d", &choice);

		//Choices for the menu
		if(choice == 0){
			mode = 0;
			break;
		}
		else if(choice == 1){
			mode = 1;
			break;
		}
		else if(choice == 2){
			printf("\nThe program will now exit");
			exit(1);
		}
		else {

			printf("\nYou have not entered a valid input\n");
			printf("Please Try again\n");
			continue;
		}
	} while (choice != 3);

	//string to store a given trace file line
	char line[MAXLEN];

	while (fgets(line, MAXLEN, fp) != NULL) //fgets terminates string line with '\0'
	{
		line[strcspn(line, "\n")] = 0; //remove newline character
		if (!containshex(line)) //ignore any blank lines
		{
			continue;
		}
//		step++;
		strcpy(addr.hex, line+2);

		//Verifying TraceData
		TraceDataHandler(line);
//		printf("Step %d\t n: %x\n", step, Tracen);
//		printf("Address: ");
//		for (int i = 0; i < 8; i++)
//		{
//			printf("%x",TraceData[i]);
//		}
//		printf("\n");

		//Storing Data in the struct storage
		addr.tag = ExtractTag(TraceData[0], TraceData[1], TraceData[2]);
		addr.index = ExtractIndex(TraceData[3], TraceData[4], TraceData[5], TraceData[6]);
		addr.byteOffset = ExtractByteOffset(TraceData[6], TraceData[7]);


		//Verifying the data stored properly
//		printf("Tag: %x\n", addr.tag);
//		printf("Index: %x\n", addr.index);
//		printf("ByteOffset: %x\n", addr.byteOffset);
//		printf("\n");

		switch (Tracen) {
			case READ:		readFromDCache(addr); break;
			case WRITE:		writeToDCache(addr, 1); break;
			case FETCH:		instrFetch(addr); break;
			case INVALID:	invalidL2(addr); break;
			case REQUEST:	dataReqL2(addr); break;
			case CLEAR:		clearCache(); break;
			case PRINT:		printCache(addr); break;
			default:		printf("ERROR: TRACE\n"); break;
		}
//		printCache(addr);
//		if(step == 8 && !state) {
//			step = 0;
//			state = 1;
//		}
	}

	if (fclose(fp) == EOF)
	{
		printf("ERROR: File close not successful\n");
	}

	return EXIT_SUCCESS;
}


/****************************************************************************
** Function: initCache
** Authors: Kai Roy
** Version: v1.0.0
** Description: Initializes all valid bits in cache to 0.
****************************************************************************/
void initCache(union byteLine *cache, int cacheSize, char type) {
	// Loop through every cache line
	for (int i = 0; i < cacheSize; i++){
		// Loop through every way/block
		if (type == 'i') {		// Instruction cache
			for (int j = 0; j < IWAYNUM; j++) {
				cache[i].iWay[j].mesi = I;					// Change to MESI
				cache[i].iWay[j].LRU = -1;
			}
		} else {				// Data cache
			for (int j = 0; j < DWAYNUM; j++) {
				cache[i].dWay[j].mesi = I;
				cache[i].dWay[j].LRU = -1;
			}
		}
	}
	return;
}


/****************************************************************************
** Function: findInvalidWay
** Authors: Kai Roy
** Version: v1.0.0
** Description: Check if all ways are valid. Return -1 if there are no invalid
** ways, else return the location of the lru invalid way.
****************************************************************************/
int findInvalidWay(union byteLine var, char type){
	int lru_loc = 0;
	int invalid = 0;

	// Loop through every way/block
	if (type == 'i') {
		// Instruction cache
		for (int i = 0; i < DWAYNUM; i++) {
			if (!invalid) {			// For first invalid
				if (var.iWay[i].mesi == I) {			// Check for invalid
					lru_loc = i;
					invalid = 1;
				}
			} else {				// For proceeding invalids
				if (var.iWay[i].mesi == I && (var.iWay[i].LRU < var.iWay[lru_loc].LRU)) {
					lru_loc = i;
				}
			}
		}
	} else {
		// Data cache
		for (int i = 0; i < DWAYNUM; i++) {
			if (!invalid) {			// For first invalid
				if (var.dWay[i].mesi == I) {			// Check for invalid
					lru_loc = i;
					invalid = 1;
				}
			} else {				// For proceeding invalids
				if (var.dWay[i].mesi == I && (var.dWay[i].LRU < var.dWay[lru_loc].LRU)) {
					lru_loc = i;
				}
			}
		}
	}

	if (!invalid) {
		return -1;
	} else {
		return lru_loc;
	}
}


/****************************************************************************
** Function: readFromDCache
** Authors: Kai Roy
** Version: v1.0.0
** Description: Read data request to L1 data cache
****************************************************************************/
int readFromDCache(struct address addr) {
	//LRU_Data
		//Checks for hit
		//Check for Invalid first, then LRU of Invalid
		//Checks for LRU
	//MESI Change I -> E -> S

	int way;
	enum MESI_STATE state;
	way = LRU_Data(addr);
	state = MESI_Protocol(addr, way, 'd', 0, 1, 0, 0);

	if (miss) {
		if (mode)
			printf("Read from L2 %s\n", addr.hex);
		miss = 0;
	}

	return 0;
}


/****************************************************************************
** Function: writeToDCache
** Authors: Kai Roy
** Version: v1.0.0
** Description: Write data request to L1 data cache
****************************************************************************/
int writeToDCache(struct address addr, int data) {
	//LRU_Data
			//Checks for hit
			//Check for Invalid first, then LRU of Invalid
			//Checks for LRU
	//MESI Change(s) I -> M, E -> M, ???
	int way;
	enum MESI_STATE state;
	way = LRU_Data(addr);
	state = MESI_Protocol(addr, way, 'd', 0, 0, 1, 0);

	if (miss) {
		if (mode)
			printf("Read for Ownership from L2 %s\n", addr.hex);
		miss = 0;
	}

	return 0;
}


/****************************************************************************
** Function: instrFetch
** Authors: Kai Roy
** Version: v1.0.0
** Description: instruction fetch (a read request to L1 instruction cache)
****************************************************************************/
int instrFetch(struct address addr) {
	//LRU_Instr
			//Checks for hit
			//Check for Invalid first, then LRU of Invalid
			//Checks for LRU
	//MESI Change(s) I -> E -> S ???
	int way;
	enum MESI_STATE state;
	way = LRU_Instr(addr);
	state = MESI_Protocol(addr, way, 'i', 0, 0, 1, 0);
	if (miss) {
		if (mode)
			printf("Read from L2 %s to Instruction Cache\n", addr.hex);
		miss = 0;
	}

	return EXIT_SUCCESS;
}


/****************************************************************************
** Function: invalidL2
** Authors: Kai Roy
** Version: v1.0.0
** Description: Invalidate command from L2
****************************************************************************/
int invalidL2(struct address addr) {
	// Find Tag, save way
	// MESI Change "Any State" -> I
	enum MESI_STATE state;
	int way = -1;					//Default value of -1
	int located = -1;

	// Check for cache hit
	for (int j = 0; j < DWAYNUM; j++) {														// Change to MESI Check, move this check outside of this func?
		if (dCache[addr.index].dWay[j].tag == addr.tag && dCache[addr.index].dWay[j].mesi != I) {
			located = j;
			break;
		}
	}
	if (located != -1) {
		way = LRU_Data(addr);
		state = MESI_Protocol(addr, way, 'd', 0, 0, 0, 1);
		return EXIT_SUCCESS;
	} else {
		printf("\nError - Tag does not exist for invalidate\n\n");
		return EXIT_FAILURE;
	}
}


/****************************************************************************
** Function: dataReqL2
** Authors: Kai Roy
** Version: v1.0.0
** Description: Data request from L2 (in response to snoop)
****************************************************************************/
int dataReqL2(struct address addr) {
	// Write back data (if state = M)
	// P2 modifies then goes to E
	// MESI Change All States -> I
	enum MESI_STATE state;
	int way = -1;					//Default value of -1

	// Find Tag
	way = LRU_Data(addr);

	// Print Information

//	if (mode) {
//		printf("1 Return data to L2 %s\n", addr.hex);
//	}

	if (way != -1) {
		state = MESI_Protocol(addr, way, 'd', 0, 0, 0, 1);
		return EXIT_SUCCESS;
	} else {
		return EXIT_FAILURE;
	}
}


/****************************************************************************
** Function: clearCache
** Authors: Kai Roy
** Version: v1.0.0
** Description: Clear the cache and reset all state (and statistics)
****************************************************************************/
void clearCache() {
	//MESI Change -> all to invalid
	//Reset LRU -> set to -1
	//Set all tags to 0
	initCache(dCache, DCACHESIZE, 'd');
	initCache(iCache, ICACHESIZE, 'i');
	for (int i = 0; i < DCACHESIZE; i++) {
		for (int j = 0; j < DWAYNUM; j++) {
			dCache[i].dWay[j].tag = 0;
		}
	}
	for (int i = 0; i < DCACHESIZE; i++) {
		for (int j = 0; j < DWAYNUM; j++) {
			iCache[i].iWay[j].tag = 0;
		}
	}

	return;
}


/****************************************************************************
** Function: printCache
** Authors: Kai Roy
** Version: v1.0.0
** Description: Print contents and state of the cache (allow subsequent trace
** activity) (prints only the byteline in question, much like in the FP
** explanation document.
****************************************************************************/
int printCache(struct address addr) {
	// Print the contents and state of the byteline

	// 				|	Print Way#... 		|
	printf("index\t| ");
	for (int i = 0; i < DWAYNUM; i++) {
		printf("Way %d\t| ", i+1);
	}
	// Print index 	|	Print Tags... 		|
	printf("\n%x \t| ", addr.index);
	for (int i = 0; i < DWAYNUM; i++) {
		printf("%03x\t| ", dCache[addr.index].dWay[i].tag);
	}
	// Print "LRU"	| 	Print LRU Bits...	|
	printf("\nLRU\t| ");
	for (int i = 0; i < DWAYNUM; i++) {
		printf("%d  \t| ", dCache[addr.index].dWay[i].LRU);
	}
	// print "MESI" |	print MESI state...	|
	printf("\nMESI\t| ");
	for (int i = 0; i < DWAYNUM; i++) {
		printf("%c  \t| ", getStateName(dCache[addr.index].dWay[i].mesi));
	}
	printf("\n\n");

	return EXIT_SUCCESS;
}


/****************************************************************************
** Function: TextToDecimal
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Converts a character to an integer
****************************************************************************/
int TextToDecimal(char value)
{
	switch (value)
		{
			case '0':
				return 0x0;	//decimal 0	//4'b0000
				break;
			case '1':
				return 0x1;	//decimal 1	//4'b0001
				break;
			case '2':
				return 0x2;	//decimal 2	//4'b0010
				break;
			case '3':
				return 0x3;	//decimal 3	//4'b0011
				break;
			case '4':
				return 0x4;	//decimal 4	//4'b0100
				break;
			case '5':
				return 0x5;	//decimal 5	//4'b0101
				break;
			case '6':
				return 0x6;	//decimal 6	//4'b0110
				break;
			case '7':
				return 0x7;	//decimal 7	//4'b0111
				break;
			case '8':
				return 0x8;	//decimal 8	//4'b1000
				break;
			case '9':
				return 0x9;	//decimal 9	//4'b1001
				break;
			case 'a':
			case 'A':
				return 0xA;	//decimal 10	//4'b1010
				break;
			case 'b':
			case 'B':
				return 0xB;	//decimal 11	//4'b1011
				break;
			case 'c':
			case 'C':
				return 0xC;	//decimal 12	//4'b1100
				break;
			case 'd':
			case 'D':
				return 0xD;	//decimal 13	//4'b1101
				break;
			case 'e':
			case 'E':
				return 0xE;	//decimal 14	//4'b1110
				break;
			case 'f':
			case 'F':
				return 0xF;	//decimal 15	//4'b1111
				break;
			default:	//if this occurs, bad data was put into the trace file
				printf("ERROR: Invalid data from trace file\n");
				exit(1);
				break;
		}
}


/****************************************************************************
** Function: TraceDataHandler
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Function converts a single line of text from the trace file and
** stores the n value in a global integer and the address in an array of integers
****************************************************************************/
void TraceDataHandler(char TraceLine[])	//TraceLine is a the address of the first element of a string of unknown length
{
	//Place all character hex values into a string. Ignore all whitespace
	char TraceText[TRACECHARLEN] = {'x','x','x','x','x','x','x','x','x'};	//initialize to an impossible value
	int hexcount = 0;	//counts the number of hex values in the trace line being examined line (min 1, max 9)
	int i = 0;
	int j = 0;

	while (TraceLine[i] != '\0')	//No '\0' in a text file, so this is a while(1). I will keep it this way to allow easy debugging with hard coded strings
	{
	    if (!ishexval(TraceLine[i]))
		{
			i = i + 1; //ignore whitespace, focus only on numerical data
			continue;
		}
		TraceText[j] = TraceLine[i];
		i = i + 1;
		j = j + 1;
		hexcount = hexcount + 1;
		if (hexcount == 9)	//if we get more than 32 bits of data in a given line of the trace file
		{
		    TraceLine[i+1] = '\0';
			break;					//ignore data over 32 bits (more than 9 hex values)
		}
	}

	//get n
	Tracen = TextToDecimal(TraceText[0]);
	//check that n is 1,2,3,4,8, or 9
	if (Tracen == 5 || Tracen == 6 || Tracen == 7 || Tracen > 9)
    {
        printf("ERROR: n must be 1, 2, 3, 4, 8, or 9\n");
        exit(1);
    }

	//accounting for n in the hexcount
	hexcount  = hexcount - 1;
	int k = TRACEDATALEN - hexcount;

	for (i = 0; i < k; i = i + 1)
	{
		TraceData[i] = 0;	//put leading zeros in our address data int array if necessary
	}

	j =0;
	for (i = k; i < (hexcount + k); i = i + 1)	//fill the rest of the elements with the hex values
	{
		TraceData[i] = TextToDecimal(TraceText[j+1]);
		j = j + 1;
	}
}


/****************************************************************************
** Function: MaskLower
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Masks the lower two bits of element 6
****************************************************************************/
int MaskLower(int TD6)
{
	return (0x3 & TD6);
}


/****************************************************************************
** Function: ExtractTag
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Extracts the Tag from the TraceData array
****************************************************************************/
int ExtractTag(int TD0, int TD1, int TD2)
{
	return (TD0 * pow(16,2)) + (TD1 * pow(16,1)) + (TD2 * pow(16,0));
}


/****************************************************************************
** Function: ExtractIndex
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Extracts the index from the Trace Data array
****************************************************************************/
int ExtractIndex(int TD3, int TD4, int TD5, int TD6)
{
	int temp;
	temp = (TD3 * pow(16,3)) + (TD4 * pow(16,2)) + (TD5 * pow(16,1)) + (TD6 * pow(16,0));
	return (temp >> 2);
}


/****************************************************************************
** Function: ExtractByteOffset
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Extracts the byte offset from the TraceData array
****************************************************************************/
int ExtractByteOffset(int TD6, int TD7)
{
	return (MaskLower(TD6) * pow(16,1)) + (TD7 * pow(16,0));
}


/****************************************************************************
** Function: containshex
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Checks a string for a hex value
****************************************************************************/
int containshex(char string[])
{
    int does = 0;
    for (int i = 0; i < strlen(string); i++)
    {
        if (does == 1)
        {
            break;
        }
        switch(string[i])
        {
            case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'a':
			case 'A':
			case 'b':
			case 'B':
			case 'c':
			case 'C':
			case 'd':
			case 'D':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
                does = 1;
                break;
			default:	//no hex value in string
				does = 0;
				break;
        }
    }
    return does;
}


/****************************************************************************
** Function: ishexval
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Verifies is a character is a hex value
****************************************************************************/
int ishexval(char val)
{
    switch(val)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'a':
        case 'A':
        case 'b':
        case 'B':
        case 'c':
        case 'C':
        case 'd':
        case 'D':
        case 'e':
        case 'E':
        case 'f':
        case 'F':
            return 1;
            break;
        default:	//no hex value in string
            return 0;
            break;
        }
}


/****************************************************************************					!!! Have to change how the cache function !!!
** Function: LRU_Data																			!!! This func uses a typedef global for the cache !!!
** Authors: Daisy Perez, Modified by Kai Roy
** Version: v1.0.0
** Description: LRU Handling for Data Cache
****************************************************************************/
int LRU_Data(struct address addr) {
    int MRU_VAL = DWAYNUM - 1;
	int located = -1;					//Default value of -1
	miss = 0;

	// Check for cache hit
	for (int j = 0; j < DWAYNUM; j++) {														// Change to MESI Check, move this check outside of this func?
		if (dCache[addr.index].dWay[j].tag == addr.tag && dCache[addr.index].dWay[j].mesi != I) {
			located = j;
			break;
		}
	}

	if (located != -1) {
		// Tag found, update LRU for accessed tag

		// Set LRU to MRU_VAL for accessed tag
		int old = dCache[addr.index].dWay[located].LRU;
		dCache[addr.index].dWay[located].LRU = MRU_VAL;

		// Decrement LRU values for other tags
		for (int i = 0; i < DWAYNUM; i++) {
			if (/*dCache[addr.index].dWay[i].mesi == 1 &&*/ i != located && dCache[addr.index].dWay[i].LRU > old) {
				dCache[addr.index].dWay[i].LRU--;
			}
		}

		return located;
	} else {
		// Tag not found,
		// find Invalid way
			// If multiple invalid ways, priorities LRU invalid
			// If no invalid ways, next
		// find LRU entry or replace existing tag
		miss = 1;

		int lru_loc = 0;
		int invalid = findInvalidWay(dCache[addr.index], 'd');

		if (invalid == -1) {
			// No invalid ways, find LRU

			for (int i = 1; i < DWAYNUM; i++) {
				if (dCache[addr.index].dWay[i].LRU < dCache[addr.index].dWay[lru_loc].LRU) {
					lru_loc = i;
				}
			}

			// !!! Do I need this function? !!!
//				// Check if the requested tag is already present
//				for (int i = 0; i < DWAYNUM; i++) {
//					if (dCache[addr.index].dWay[i].tag == addr.tag && dCache[addr.index].dWay[i].valid == 1) {
//						lru_loc = i; // Update location to the existing tag
//						break;
//					}
//				}
		} else {
			// Invalid Way
			lru_loc = invalid;
		}

		int old = dCache[addr.index].dWay[lru_loc].LRU;
		backup.tag = dCache[addr.index].dWay[lru_loc].tag;
		backup.index = addr.index;

		if (dCache[addr.index].dWay[lru_loc].mesi == M)
			wb = 1;
		dCache[addr.index].dWay[lru_loc].mesi = I;
		dCache[addr.index].dWay[lru_loc].tag = addr.tag;  // Update tag to the requested tag
		dCache[addr.index].dWay[lru_loc].LRU = MRU_VAL;  // Set LRU to MRU_VAL for the new entry or existing tag

		// Decrement LRU values for other valid entries
		for (int i = 0; i < DWAYNUM; i++) {
			if (/*dCache[addr.index].dWay[i].valid == 1 &&*/ i != lru_loc && dCache[addr.index].dWay[i].LRU > old) {
				dCache[addr.index].dWay[i].LRU--;
			}
		}

		return lru_loc;
	}

    return -1;
}


/****************************************************************************					!!! Have to change how the cache function !!!
** Function: LRU_Instr																				!!! This func uses a typedef global for the cache !!!
** Authors: Daisy Perez, Modified by Kai Roy
** Version: v1.0.0
** Description: LRU Handling for INstruction Cache
****************************************************************************/
int LRU_Instr(struct address addr) {
    int MRU_VAL = IWAYNUM - 1;
	int located = -1;					//Default value of -1
	miss = 0;

	// Check for cache hit
	for (int j = 0; j < IWAYNUM; j++) {														// Change to MESI Check, move this check outside of this func?
		if (iCache[addr.index].iWay[j].tag == addr.tag && iCache[addr.index].iWay[j].mesi != I) {
			located = j;
			break;
		}
	}

	if (located != -1) {
		// Tag found, update LRU for accessed tag

		// Set LRU to MRU_VAL for accessed tag
		int old = iCache[addr.index].iWay[located].LRU;
		iCache[addr.index].iWay[located].LRU = MRU_VAL;

		// Decrement LRU values for other tags
		for (int i = 0; i < IWAYNUM; i++) {
			if (/*dCache[addr.index].dWay[i].mesi == 1 &&*/ i != located && iCache[addr.index].iWay[i].LRU > old) {
				iCache[addr.index].iWay[i].LRU--;
			}
		}

		return located;
	} else {
		// Tag not found,
		// find Invalid way
			// If multiple invalid ways, priorities LRU invalid
			// If no invalid ways, next
		// find LRU entry or replace existing tag
		miss = 1;

		int lru_loc = 0;
		int invalid = findInvalidWay(iCache[addr.index], 'd');

		if (invalid == -1) {
			// No invalid ways, find LRU

			for (int i = 1; i < IWAYNUM; i++) {
				if (iCache[addr.index].iWay[i].LRU < iCache[addr.index].iWay[lru_loc].LRU) {
					lru_loc = i;
				}
			}

			// !!! Do I need this function? !!!
//				// Check if the requested tag is already present
//				for (int i = 0; i < DWAYNUM; i++) {
//					if (dCache[addr.index].dWay[i].tag == addr.tag && dCache[addr.index].dWay[i].valid == 1) {
//						lru_loc = i; // Update location to the existing tag
//						break;
//					}
//				}
		} else {
			// Invalid Way
			lru_loc = invalid;
		}

		int old = iCache[addr.index].iWay[lru_loc].LRU;
		iCache[addr.index].iWay[lru_loc].mesi = I;
		iCache[addr.index].iWay[lru_loc].tag = addr.tag;  // Update tag to the requested tag
		iCache[addr.index].iWay[lru_loc].LRU = MRU_VAL;  // Set LRU to MRU_VAL for the new entry or existing tag

		// Decrement LRU values for other valid entries
		for (int i = 0; i < IWAYNUM; i++) {
			if (/*dCache[addr.index].dWay[i].valid == 1 &&*/ i != lru_loc && iCache[addr.index].iWay[i].LRU > old) {
				iCache[addr.index].iWay[i].LRU--;
			}
		}

		return lru_loc;
	}

    return -1;
}


/****************************************************************************					!!! Have to change how the cache function !!!
** Function: print_byteline_state																	!!! This func uses a typedef global for the cache !!!
** Authors: Daisy Perez, modified by Kai Roy
** Version: v1.0.0
** Description:
****************************************************************************/
void print_byteline_state(const char* message, union byteLine var,  char type) {
    printf("%s\n", message);

    if (type == 'i') {
    	for (int i = 0; i < IWAYNUM; i++) {
    				printf("Entry %d: Valid: %d, Tag: %u, LRU: %d\n", i, var.iWay[i].valid, var.iWay[i].tag, var.iWay[i].LRU);
    			}
    } else {
		for (int i = 0; i < DWAYNUM; i++) {
			printf("Entry %d: Valid: %d, Tag: %u, LRU: %d\n", i, var.dWay[i].valid, var.dWay[i].tag, var.dWay[i].LRU);
		}
    }
    printf("\n");
}


/****************************************************************************
** Function: MESI_Protocol
** Authors: Jesus Zavala, modified by Kai Roy
** Version: v1.0.0
** Description: The function follows the state diagram that is shown in class
****************************************************************************/
int MESI_Protocol(struct address addr, int way, char type, int snoopBit, int readingData, int writingData, int invalidateData) {
    //Create and load a local variable with the MESI state from the way.
	enum MESI_STATE state;
	if (type == 'i') {
		state = iCache[addr.index].iWay[way].mesi;
	} else {
		state = dCache[addr.index].dWay[way].mesi;
	}

//Start of INVALID transitions
    if 		 ((state == I) && (snoopBit == 1) && (readingData == 1)){
        state = S;
        if (mode && wb) {
			printf("Return data to L2 %x\n", (1048576*backup.tag + 64*backup.index));
			wb = 0;
        }

    }else if ((state == I) && (snoopBit == 0) && (readingData == 1)){
        state = E;
        if (mode && wb) {
			printf("Return data to L2 %x\n", (1048576*backup.tag + 64*backup.index));
			wb = 0;
        }

    }else if ((state == I) && (writingData == 1)){
        state = M;
        if (mode && wb) {
        	printf("Return data to L2 %x\n", (1048576*backup.tag + 64*backup.index));
        	wb = 0;
        }
        if (mode)
        	printf("Write to L2 %s\n", addr.hex);

//End of INVALID transitions

//Start of EXCLUSIVE  transitions
	}else if ((state == E) && (invalidateData == 1)){
		state = I;

    }else if ((state == E) && (readingData == 1)){
        state = S;			// In this model, we assume that any read is from another processor, thus E -> S

    }else if ((state == E) && (writingData == 1)){
        state = M;
        if (mode)
        	printf("Write to L2 %s\n", addr.hex);
//End of EXCLUSIVE transitions

//Start of SHARED transitions
    }else if ((state == S) && (invalidateData == 1)){
        state = M;
        if (mode)
        	printf("Write to L2 %s\n", addr.hex);

    }else if ((state == S) && (readingData == 1)){
        state = S;
//End of Exclusive transitions

//Start of MODIFIED transitions
    }else if ((state == M) && (invalidateData == 1)){
    	state = I;
    	if (mode)
    		printf("Return data to L2 %s\n", addr.hex);

    }else if ((state == M) && (readingData == 1)){
        state = M;			// In this model, there is no M->S transition

    }else if ((state == M) && (writingData == 1)){
        state = M;
//end of MODIFIED transitions

    }else if (invalidateData){
    	state = I;
    }else{
		 printf("State not found - ERROR encountered.");
		 exit(EXIT_FAILURE);
    }

    // Update the MESI state inside the way
	if (type == 'i') {
		iCache[addr.index].iWay[way].mesi = state;
	} else {
		dCache[addr.index].dWay[way].mesi = state;
	}

	return state;
};

char getStateName(enum MESI_STATE state) {
	switch (state) {
		case M: return 'M';
		case E: return 'E';
		case S: return 'S';
		case I: return 'I';
	}

	return 'X';
}
