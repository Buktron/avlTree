// File for DEMONSTRATION PURPOSES ONLY.
// Not to be shared with any other student.
// Annotate with lots of comments (by yourself) for easier understanding!!
// Copying and pasting any part of this file WILL BE CONSIDERED PLAGIARISM.
// Using this file to attain better understanding of this problem
// and designing a solution is acceptable.
// DIRECT MODIFICATION of this file is PROHIBITED. Doing so WILL BE CONSIDERED PLAGIARISM.
// This file is NOT A MODEL SOLUTION, but AN EDUCATIONAL RESOURCE. Treat it as such.
// Dwaipayan Chakraborty, 2017-08-03.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BUFFER_SIZE 500

typedef struct airPdata{
char *LocID;
char *fieldName;
char *city;
float longitude;
float latitude;
} airPdata;



typedef struct lListAirPdata
{
  airPdata *airportPtr;
  struct lListAirPdata *nextAirport;
}lListAirPdata;


void parseLine(char *line, airPdata *apd);
void deleteStruct(airPdata *apd);

float sexag2decimal(char *degreeString);
void sortByLocID(lListAirPdata *airports, int length);
void sortByLatitude(lListAirPdata *airports, int length);


int main (int argc, char *argv[])
{

    FILE *fid;
    char buffer[BUFFER_SIZE];

    int fileLength = 0, skipHeader = 0, i, nHelipad = 0, headExists = 0, j = 0, k = 0;
    if(argc==3)
    {
        fid = fopen(argv[1], "r");
        if(fid==NULL)
        {
            printf("FILE ERROR: File %s not found. Aborting.\n", argv[1]);
            return 22;
        }
    }
    else
    {
        printf("EINVAL: Incorrect number of input parameters. Please use correct syntax.\n");
        printf("Syntax: ./hw3 <input file> <sortParameter>\n");
        return 22;
    }


    while(fgets(buffer, BUFFER_SIZE, fid) != NULL )
    {
        if (buffer[0] < '0' || buffer[0] > '9')
        {
            skipHeader++;
            continue;
        }
        fileLength++;
    }

    rewind(fid);



    airPdata *apData;

    apData = malloc(sizeof(airPdata)*fileLength);
    if(apData == NULL)
    {
        printf("EAGAIN: Memory allocation failed. Try again.\n");
        return 11;
    }

    for(i = (0 - skipHeader); i < fileLength; i++)
    {

        fgets(buffer, BUFFER_SIZE, fid);

        if (buffer[0] < '0' || buffer[0] > '9')
          continue;

        if(buffer[strlen(buffer) - 1] == '\n')
          buffer[strlen(buffer)-1] = '\0';

        parseLine(buffer, apData+i);

    }

    fclose(fid);

    lListAirPdata *pHead = NULL;

    pHead = malloc(sizeof(lListAirPdata));
    if (pHead == NULL)
    {
        printf("EAGAIN: Memory allocation failed. Try again.\n");
        return 11;
    }
    else
    {
        while (headExists < 1)
        {
            if ((((apData+k)->LocID[0] <= '9') && ((apData+k)->LocID[0] >= '0')) ||
                (((apData+k)->LocID[1] <= '9') && ((apData+k)->LocID[1] >= '0')) ||
                (((apData+k)->LocID[2] <= '9') && ((apData+k)->LocID[2] >= '0')) ||
                (((apData+k)->latitude < 0.00) && ((apData+k)->latitude > 31.00)))
            {
                nHelipad++;
                k++;
            }

            else
            {
                pHead->airportPtr = (apData+k);
                pHead->nextAirport = NULL;
                headExists=1;
            }
        }

        lListAirPdata *currentAirport = pHead;

        for(k = (nHelipad + 1); k < fileLength; k++)
        {
            while (currentAirport->nextAirport != NULL)
            {
                currentAirport = currentAirport->nextAirport;
            }

            if ((((apData+k)->LocID[0] <= '9') && ((apData+k)->LocID[0] >= '0')) || (((apData+k)->LocID[1] <= '9') && ((apData+k)->LocID[1] >= '0')) || (((apData+k)->LocID[2] <= '9') && ((apData+k)->LocID[2] >= '0')) || (((apData+k)->latitude < 0.00) && ((apData+k)->latitude > 31.00)))
            {
                nHelipad++;
            }
            else
            {
                currentAirport->nextAirport = malloc(sizeof(lListAirPdata));
                currentAirport->nextAirport->airportPtr = (apData+k);
                currentAirport->nextAirport->nextAirport = NULL;
            }
        }
    }


    fileLength -= nHelipad;


    char sortArg;
    sortArg = argv[2][0];

    if (sortArg == 'N' || sortArg == 'n')
    {
    sortByLatitude(pHead, fileLength);
    }
    else if (sortArg == 'A' || sortArg == 'a')
    {
    sortByLocID(pHead, fileLength);
    }
    else
    {
        printf("EINVAL: Argument invalid.\nValid arguments are A/N/a/n.\n");
    }


    printf("Code, Name, City, Latitude , Longitude\n");
    lListAirPdata *current = pHead;
    while (current != NULL)
    {

            printf("%s,%s,%s,%.4f,%.4f\n", (current)->airportPtr->LocID,(current)->airportPtr->fieldName,
            (current)->airportPtr->city,(current)->airportPtr->latitude,(current)->airportPtr->longitude);

            current = current->nextAirport;
    }



    for(j = 0; j < fileLength; j++)
    {
        deleteStruct(apData+j);
    }
    free(apData);

    return 0;

}


void parseLine(char *line, airPdata *apd)
{

    int i = 0, j = 0, commas=0;

    char *latBuffer;
	char *lonBuffer;

    while(commas<15)
    {
        while(*(line+i)!=',')
        {
            i++;
        }

        // strncpy does not append a '\0' to the end of the copied sub-string, so we will
        // replace the comma with '\0'.
        *(line+i) = '\0';

        switch (commas)
        {
        case 1:   //Grab the second "field" - Location ID
            apd->LocID = malloc(sizeof(char)*(i-j+1));
            if(apd->LocID==NULL)
            {
                printf("malloc failed to initialize airPdata.LocID.\n");
                exit(-1);
            }
            strncpy(apd->LocID, line+j, i-j+1);
            break;
        case 2:   //Grab the third "field" - Field Name
            apd->fieldName = malloc(sizeof(char)*(i-j+1));
            if(apd->fieldName==NULL)
            {
                printf("malloc failed to initialize airPdata.fieldName.\n");
                exit(-1);
            }
            strncpy(apd->fieldName, line+j, i-j+1);
            break;
        case 3:   //Grab the fourth "field" - City
            apd->city = malloc(sizeof(char)*(i-j+1));
            if(apd->city==NULL)
            {
                printf("malloc failed to initialize airPdata.city.\n");
                exit(-1);
            }
            strncpy(apd->city, line+j, i-j+1);
            break;
		case 8:   //Grab the ninth "field" - Latitude (sexagesimal string)

			latBuffer = malloc(sizeof(char)*(i-j+1));
			if(latBuffer==NULL){
				printf("malloc failed to initialize latBuffer.\n");
				exit(-1);
			}

			strncpy(latBuffer, line+j, i-j+1);
			apd->latitude = sexag2decimal(latBuffer);

			free(latBuffer);

			break;
		case 9:   //Grab the tenth "field" - Longitude (sexagesimal string)

			lonBuffer = malloc(sizeof(char)*(i-j+1));
			if(lonBuffer==NULL){
				printf("malloc failed to initialize lonBuffer.\n");
				exit(-1);
			}

			strncpy(lonBuffer, line+j, i-j+1);
			apd->longitude = sexag2decimal(lonBuffer);

			free(lonBuffer);

			break;

		}

        j=++i;
        commas++;
    }
}


void deleteStruct(airPdata *apd)
{
    free(apd->city);
    free(apd->fieldName);
    free(apd->LocID);
}



float sexag2decimal(char *degreeString)
{

    char *tok;
    char *tmpInBuffer = degreeString;
    float degrees, minutes, seconds;
    float degreeDecimal;

    if (degreeString == NULL)
    {
        return 0.0;
    }

    tok = strsep(&tmpInBuffer, "-");
    degrees = atof(tok);
    tok = strsep(&tmpInBuffer, "-");
    minutes = atof(tok);
    tok = strsep(&tmpInBuffer, "NESW");
    seconds = atof(tok);

    if (degrees > 99.00 || degrees < 0.00 || minutes > 59.00 || minutes < 0.00 || seconds > 59.9999 || seconds < 0.00)
        return 0.0;


    if (degreeString[0] == '0')
    {
        degreeDecimal = (-degrees) - (minutes/60.00) - (seconds/3600.00);
    }
    else
    {
        degreeDecimal = degrees + (minutes/60.00) + (seconds/3600.00);
    }


    return degreeDecimal;

}



void sortByLatitude(lListAirPdata *airports, int length)
{
  // BubbleSort is used, replacement with other sorting algorithms
  // having better time comlexity is optional, but strongly recommended.
  // Play around with the code to understand it better!!
    struct airPdata *swap = malloc(sizeof(airPdata));
    struct lListAirPdata *tmp = malloc(sizeof(lListAirPdata));
    int i, j;
    for (i = 0; i < (length - 1); i++)
    {

        tmp = (airports->nextAirport);

        for (j = 0; j < (length - i - 1); j++)
        {

            if ((airports->airportPtr->latitude) > (tmp->airportPtr->latitude))
            {
                swap = (airports->airportPtr);
                (airports->airportPtr) = (tmp->airportPtr);
                (tmp->airportPtr) = swap;

            }
            tmp = (tmp->nextAirport);
        }
        airports = (airports->nextAirport);
    }
    free(tmp);

}


void sortByLocID(lListAirPdata *airports, int length)
{
  // BubbleSort is used, replacement with other sorting algorithms
  // having better time comlexity is optional, but strongly recommended.
  // Play around with the code to understand it better!!
    struct airPdata *swap = malloc(sizeof(airPdata));
    struct lListAirPdata *tmp = malloc(sizeof(lListAirPdata));
    int i, j;
    for (i = 0; i < (length - 1); i++)
    {

        tmp = (airports->nextAirport);

        for (j = 0; j < (length - i - 1); j++)
        {
            if (strcmp((airports->airportPtr->LocID) , (tmp->airportPtr->LocID)) > 0)
            {
                swap = (airports->airportPtr);
                (airports->airportPtr) = (tmp->airportPtr);
                (tmp->airportPtr) = swap;

            }
            tmp = (tmp->nextAirport);
        }
        airports = (airports->nextAirport);
    }
    free(tmp);
}
