/*
ECE 585 Final Project
Trace file data handling

Nick Allmeyer
ECE 585
Winter 2024
*/

/*Header files*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*Macros*/
#define TRACECHARLEN 9
#define TRACEDATALEN 8
#define MAXLEN 1000

/*Global variables*/
int Tracen = 16;											//n
int TraceData [TRACEDATALEN] = {16,16,16,16,16,16,16,16};	//address data

/*Function prototypes*/
int TextToDecimal(char value);
void TraceDataHandler(char TraceLine[]);
int ExtractTag(int TD0, int TD1, int TD2);				//Trace data elements 0-2
int ExtractIndex(int TD3, int TD4, int TD5, int TD6);	//Trace data elements 3-6
int ExtractByteOffset(int TD6, int TD7);				//Trace data elements 6-7
int MaskLower(int TD6);									//Masks the lower two bits of element 6
int containshex(char string[]);                         //checks if a hex value is present in a string
int ishexval(char val);                                  //verifies is a character is a hex value

/*Struct(s)*/
typedef struct
{
	unsigned int tag;
	unsigned int index;
	unsigned int byteOffset;
}address;


int main()
{
	FILE *fp;   //file pointer
	fp = fopen("TraceFile.txt", "r");
	if (fp == NULL)
    {
        printf("ERROR: Could not open file\n");
        exit(1);
    }
    //string to store a given trace file line
    char line[MAXLEN];

    while (fgets(line, MAXLEN, fp) != NULL) //fgets terminates string line with '\0'
    {
        line[strcspn(line, "\n")] = 0; //remove newline character
        if (!containshex(line)) //ignore any blank lines
        {
            continue;
        }

        //Verifying TraceData
        TraceDataHandler(line);
        printf("n: %x\n", Tracen);
        printf("Address: ");
        for (int i = 0; i < 8; i++)
        {
            printf("%x",TraceData[i]);
        }
        printf("\n");

        //Storing Data in the struct storage
        address storage;
        storage.tag = ExtractTag(TraceData[0], TraceData[1], TraceData[2]);
        storage.index = ExtractIndex(TraceData[3], TraceData[4], TraceData[5], TraceData[6]);
        storage.byteOffset = ExtractByteOffset(TraceData[6], TraceData[7]);

        //Verifying the data stored properly
        printf("Tag: %x\n", storage.tag);
        printf("Index: %x\n", storage.index);
        printf("ByteOffset: %x\n", storage.byteOffset);
        printf("\n");
    }

    if (fclose(fp) == EOF)
    {
        printf("ERROR: File close not successful\n");
    }

    return 0;
}


/**********************/
/*Function Definitions*/
/**********************/

/*Converts a character to an integer*/
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

/*Function converts a single line of text from the trace file and stores
the n value in a global integer and the address in an array of integers*/
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

/*Masks the lower two bits of element 6*/
int MaskLower(int TD6)
{
	return (0x3 & TD6);
}

/*Extracts the Tag from the TraceData array*/
int ExtractTag(int TD0, int TD1, int TD2)
{
	return (TD0 * pow(16,2)) + (TD1 * pow(16,1)) + (TD2 * pow(16,0));
}

/*Extracts the index from the Trace Data array*/
int ExtractIndex(int TD3, int TD4, int TD5, int TD6)
{
	int temp;
	temp = (TD3 * pow(16,3)) + (TD4 * pow(16,2)) + (TD5 * pow(16,1)) + (TD6 * pow(16,0));
	return (temp >> 2);
}

/*Extracts the byte offset from the TraceData array*/
int ExtractByteOffset(int TD6, int TD7)
{
	return (MaskLower(TD6) * pow(16,1)) + (TD7 * pow(16,0));
}

int containshex(char string[])  //checks a string for a hex value
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

int ishexval(char val)  //verifies is a character is a hex value
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
