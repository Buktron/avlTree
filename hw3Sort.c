/*“I [Jabari Long] ([Ja257871]) affirm that this program is entirely my own work and that I have
neither developed my code together with any another person, nor copied any code from any other person,
nor permitted my code to be copied or otherwise used by any other person, nor have I copied, modified, or
otherwise used program code that I have found in any external source, including but not limited to, online
sources. I acknowledge that any violation of the above terms will be treated as academic dishonesty.”*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "airPdata.h"

typedef struct lListAirPdata
{
  airPdata *airPtr;
  struct lListAirPdata *nextAirport;
}lListAirPdata;

void parseLine(char *line, airPdata *air1);
float sexag2decimal(char *degreeString);
void sortByLocID(lListAirPdata *airports, int length);
void sortByLatitude(lListAirPdata *airports, int length);
void deleteStruct(airPdata *air1);

int main (int argc, char **argv)
{

	// Declare input buff and other parameters
	FILE *fp;
	int i = 0, j = 0, k = 0, count = 0, lineSkipper = 0, heliPad = 0, header = 0;
	char buff[1024];  
	char *tmp, *tmp1;

	if(argc == 3)
	{
		fp = fopen(argv[1], "r");
		if(fp==NULL){
			fprintf(stderr, "ERROR: File %s failed to open. Aborting.\n", argv[1]);
			return 0;
		}
	}
	else
	{
		fprintf(stderr,"ERROR: Incorect number of input parameters.\n");
	}
	
    // Determine length of the file.
	while(fgets(buff, 1024, fp) != NULL )
    {
        if (buff[0] < '0' || buff[0] > '9')
    {
        header++;
        continue;
    }
        count++;
    }
	rewind(fp);
	
	// Declare a struct array and allocate memory.
	airPdata *air;

	air = malloc(sizeof(airPdata)*count);
	if(air==NULL)
	{
		fprintf(stderr, "ERROR: Memory allocation for airPdata array failed. Aborting.\n");
		return 0;
	}

	for(i = (0 - header); i < count; i++)
	{
		fgets(buff, 1024, fp);
        if (buff[0] < '0' || buff[0] > '9')
        {
            continue;
        }

		if(buff[strlen(buff) - 1] == '\n') 
            buff[strlen(buff)-1] = '\0';
		
		parseLine(buff, air+i);
	}

	fclose(fp);

	lListAirPdata *airHead = NULL;

    airHead = malloc(sizeof(lListAirPdata));
    if (airHead == NULL)
    {
        fprintf(stderr, "ERROR: Memory allocation failed. Try again.\n");
        return 11;
    }
    else
    {
        while (header < 1)
        {
            if ((((air+k)->LocID[0] <= '9') && ((air+k)->LocID[0] >= '0')) ||
                (((air+k)->LocID[1] <= '9') && ((air+k)->LocID[1] >= '0')) ||
                (((air+k)->LocID[2] <= '9') && ((air+k)->LocID[2] >= '0')) ||
                (((air+k)->latitude < 0.00) && ((air+k)->latitude > 31.00)))
            {
                heliPad++;
                k++;
            }

            else
            {
                airHead->airPtr = (air+k);
                airHead->nextAirport = NULL;
                header=1;
            }
        }

        lListAirPdata *currentPort = airHead;

        for(k = (heliPad + 1); k < count; k++)
        {
            while (currentPort->nextAirport != NULL)
            {
                currentPort = currentPort->nextAirport;
            }

            if ((((air+k)->LocID[0] <= '9') && ((air+k)->LocID[0] >= '0')) || 
                (((air+k)->LocID[1] <= '9') && ((air+k)->LocID[1] >= '0')) || 
                (((air+k)->LocID[2] <= '9') && ((air+k)->LocID[2] >= '0')) || 
                (((air+k)->latitude < 0.00) && ((air+k)->latitude > 31.00)))
            {
                heliPad++;
            }
            else
            {
                currentPort->nextAirport = malloc(sizeof(lListAirPdata));
                currentPort->nextAirport->airPtr = (air+k);
                currentPort->nextAirport->nextAirport = NULL;
            }
        }
    }


    count -= heliPad;

    char sortArg;
    sortArg = argv[2][0];

    if (sortArg == 'N' || sortArg == 'n')
    {
    sortByLatitude(airHead, count);
    }
    else if (sortArg == 'A' || sortArg == 'a')
    {
    sortByLocID(airHead, count);
    }
    else
    {
        fprintf(stderr, "ERROR: Argument invalid.\n");
    }

    printf("code,name,city,lat,lon\n");

    lListAirPdata *current = airHead;
    
    while (current != NULL)
    {
        printf("%s,%s,%s,%.4f,%.4f\n", (current)->airPtr->LocID,(current)->airPtr->fieldName, 
        (current)->airPtr->city,(current)->airPtr->latitude,(current)->airPtr->longitude);
        current = current->nextAirport;
    }

	// Free the memory used for fields of the structs.
	for(j = 0; j < count; j++)
	{
		deleteStruct(air+j);
	}

	free(air);
	return 0;

}


void parseLine(char *line, airPdata *air)
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
            air->LocID = malloc(sizeof(char)*(i-j+1));
            if(air->LocID==NULL)
            {
                printf("malloc failed to initialize LocID.\n");
                exit(-1);
            }
            strncpy(air->LocID, line+j, i-j+1);
            break;
        case 2:   //Grab the third "field" - Field Name
            air->fieldName = malloc(sizeof(char)*(i-j+1));
            if(air->fieldName==NULL)
            {
                printf("malloc failed to initialize fieldName.\n");
                exit(-1);
            }
            strncpy(air->fieldName, line+j, i-j+1);
            break;
        case 3:   //Grab the fourth "field" - City
            air->city = malloc(sizeof(char)*(i-j+1));
            if(air->city==NULL)
            {
                printf("malloc failed to initialize city.\n");
                exit(-1);
            }
            strncpy(air->city, line+j, i-j+1);
            break;
		case 8:   //Grab the ninth "field" - Latitude (sexagesimal string)

			latBuffer = malloc(sizeof(char)*(i-j+1));
			if(latBuffer==NULL){
				printf("malloc failed to initialize latBuffer.\n");
				exit(-1);
			}

			strncpy(latBuffer, line+j, i-j+1);
			air->latitude = sexag2decimal(latBuffer);

			free(latBuffer);

			break;
		case 9:   //Grab the tenth "field" - Longitude (sexagesimal string)

			lonBuffer = malloc(sizeof(char)*(i-j+1));
			if(lonBuffer==NULL){
				printf("malloc failed to initialize lonBuffer.\n");
				exit(-1);
			}

			strncpy(lonBuffer, line+j, i-j+1);
			air->longitude = sexag2decimal(lonBuffer);

			free(lonBuffer);

			break;

		}

        j=++i;
        commas++;
    }
}


void deleteStruct(airPdata *air1)
{
	free(air1->city);
	free(air1->fieldName);
	free(air1->LocID);
}
float sexag2decimal(char *degreeString)
{
	float segaDegree, degrees, minutes, seconds;     
	char *temp = degreeString;

	if(degreeString == NULL)
	{
	return 0.0;
	}

	degrees = atof(strsep(&temp, "-"));
	minutes = atof(strsep(&temp, "-"));
	seconds = atof(strsep(&temp, "NESW")); 
	segaDegree = degrees + (minutes/60.0) + (seconds/3600.0);
	
    if (degrees > 99.00 || degrees < 0.00 || minutes > 59.00 || minutes <
    0.00 || seconds > 59.9999 || seconds < 0.00)
    {
        return 0;
    }

	if(degreeString[0] == '0')
	{
		segaDegree = (0.0 - segaDegree);
	}
	else
	{
		segaDegree = (segaDegree + 0.0);
	}

	return segaDegree;
}

void sortByLatitude(lListAirPdata *airports, int length)
{
	struct airPdata *air1 = malloc(sizeof(airPdata));
    struct lListAirPdata *tmp = malloc(sizeof(lListAirPdata));
    int i, j;
    for (i = 0; i < (length - 1); i++)
    {

        tmp = (airports->nextAirport);

        for (j = 0; j < (length - i - 1); j++)
        {

            if ((airports->airPtr->latitude) > (tmp->airPtr->latitude))
            {
                air1 = (airports->airPtr);
                (airports->airPtr) = (tmp->airPtr);
                (tmp->airPtr) = air1;

            }
            tmp = (tmp->nextAirport);
        }
        airports = (airports->nextAirport);
    }
    free(tmp);

}
void sortByLocID(lListAirPdata *airports, int length)
{
    struct airPdata *air1 = malloc(sizeof(airPdata));
    struct lListAirPdata *tmp = malloc(sizeof(lListAirPdata));
    int i, j;
    for (i = 0; i < (length - 1); i++)
    {

        tmp = (airports->nextAirport);

        for (j = 0; j < (length - i - 1); j++)
        {
            if (strcmp((airports->airPtr->LocID) , (tmp->airPtr->LocID)) > 0)
            {
                air1 = (airports->airPtr);
                (airports->airPtr) = (tmp->airPtr);
                (tmp->airPtr) = air1;

            }
            tmp = (tmp->nextAirport);
        }
        airports = (airports->nextAirport);
    }
    free(tmp);
}
