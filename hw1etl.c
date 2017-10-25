/*“I [] ([]) affirm that this program is entirely my own work and that I have
neither developed my code together with any another person, nor copied any code from any other person,
nor permitted my code to be copied or otherwise used by any other person, nor have I copied, modified, or
otherwise used program code that I have found in any external source, including but not limited to, online
sources. I acknowledge that any violation of the above terms will be treated as academic dishonesty.”*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include "airPdata.h"

typedef struct lListAirPdata
{
  airPdata *airPtr;
  struct lListAirPdata *nextAirport;
}lListAirPdata;

int convertValue(char x);
void parseLine(char *line, airPdata *air1);
void deleteStruct(airPdata *air1);
float sexag2decimal(char *degreeString);
void sortByLocID(lListAirPdata *airports, int length);
void sortByLatitude(lListAirPdata *airports, int length);

/*argument fname will call the file by reference when 
typing ./etl filename.csv or whatever.csv*/
int main(int argc, char** argv)
{
	int i = 0, j = 0, k = 0, count = 0, lineSkipper = 0, dataLength = 0, heliPad = 0, header = 0;
	char buff[1024];  
	char *tmp, *tmp1;
	char *latitudeBuff, *longitudeBuff;

    FILE *fp;
	if(argc ==3)
	{
		// File pointer 
		fp = fopen(argv[1], "r");
		if(fp == NULL)
		{
			fprintf(stderr, "etl ERROR: File '%s' not found.\n", argv[1]);
			return 0; // or exit(1) this stops and exits the program
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Incorrect number of input parameters.\n");
        return 0;
	}

	/*printf("%-12s %-11s %-42s %-34s %-3s %-15s %-16s Tower\n",
		"FAA Site", "Short Name", "Airport Name", "City", "ST",
		"Latitude", "Longitude");
	printf("%-12s %-11s %-42s %-34s %-3s %-15s %-16s =====\n",
		"========", "==========", "============", "====", "==",
		"========", "=========");
    */

	while(!feof(fp))
	{
  		if(buff[0] < '0' || buff[0] > '9')
  		{
  			lineSkipper++;
  			continue;
  		}
  		dataLength++;
  	}
  	rewind(fp); // This resets fp back to beginning. From stdio.h
  	
  	airPdata *air = malloc(sizeof(airPdata)* dataLength);
  	if(air == NULL)
  	{
  		printf("ERROR: Memory allocation for airPdata array failed. Aborting.\n");
  		return 0;
  	}

	// Read and parse each line of the input file.
	for(int i = 0; i<count; i++){
		fgets(buff, 1024, fp);
		
		if(buff[strlen(buff) - 1] == '\n') buff[strlen(buff)-1] = '\0';
		
		parseLine(buff, air+i);
	}

	
	fclose(fp);
    lListAirPdata *airHead = NULL;

    airHead = malloc(sizeof(lListAirPdata));
    if (airHead == NULL)
    {
        printf("ERROR: Memory allocation for lListAirPdata failed. Try again.\n");
        return 0;
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
                airHead->airportPtr = (air+k);
                airHead->nextAirport = NULL;
                header=1;
            }
        }

        lListAirPdata *currentAirport = airHead;

        for(k = (heliPad + 1); k < dataLength; k++)
        {
            while (currentAirport->nextAirport != NULL)
            {
                currentAirport = currentAirport->nextAirport;
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
                currentAirport->nextAirport = malloc(sizeof(lListAirPdata));
                currentAirport->nextAirport->airportPtr = (air+k);
                currentAirport->nextAirport->nextAirport = NULL;
            }
        }
    }


    dataLength -= heliPad;
    

    char sortArg;
    sortArg = argv[2][0];

    if (sortArg == 'N' || sortArg == 'n')
    {
    sortByLatitude(airHead, dataLength);
    }
    else if (sortArg == 'A' || sortArg == 'a')
    {
    sortByLocID(airHead, dataLength);
    }
    else
    {
        fprintf(stderr, "ERROR: Argument invalid.\nValid arguments are A/N/a/n.\n");
    }


    printf("code,name,city,lat,lon\n");

    lListAirPdata *current = airHead;
    
    while (current != NULL)
    {
        printf("%s,%s,%s,%.4f,%.4f\n", (current)->airportPtr->LocID,(current)->airportPtr->fieldName,
        (current)->airportPtr->city,(current)->airportPtr->latitude,(current)->airportPtr->longitude);
        current = current->nextAirport;
    }




    free(air);
    // Free the memory used for fields of the structs.
    for(i = 0; i < count; i++)
    {
        deleteStruct(air);
    }
    return 0;
}
		

void parseLine(char *line, airPdata *air1)
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
            air1->LocID = malloc(sizeof(char)*(i-j+1));
            if(air1->LocID==NULL)
            {
                printf("malloc failed to initialize airPdata.LocID.\n");
                exit(-1);
            }
            strncpy(air1->LocID, line+j, i-j+1);
            break;
        case 2:   //Grab the third "field" - Field Name
            air1->fieldName = malloc(sizeof(char)*(i-j+1));
            if(air1->fieldName==NULL)
            {
                printf("malloc failed to initialize airPdata.fieldName.\n");
                exit(-1);
            }
            strncpy(air1->fieldName, line+j, i-j+1);
            break;
        case 3:   //Grab the fourth "field" - City
            air1->city = malloc(sizeof(char)*(i-j+1));
            if(air1->city==NULL)
            {
                printf("malloc failed to initialize airPdata.city.\n");
                exit(-1);
            }
            strncpy(air1->city, line+j, i-j+1);
            break;
        case 8:   //Grab the ninth "field" - Latitude (sexagesimal string)

            latBuffer = malloc(sizeof(char)*(i-j+1));
            if(latBuffer==NULL){
                printf("malloc failed to initialize latBuffer.\n");
                exit(-1);
            }

            strncpy(latBuffer, line+j, i-j+1);
            air1->latitude = sexag2decimal(latBuffer);

            free(latBuffer);

            break;
        case 9:   //Grab the tenth "field" - Longitude (sexagesimal string)

            lonBuffer = malloc(sizeof(char)*(i-j+1));
            if(lonBuffer==NULL){
                printf("malloc failed to initialize lonBuffer.\n");
                exit(-1);
            }

            strncpy(lonBuffer, line+j, i-j+1);
            air1->longitude = sexag2decimal(lonBuffer);

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

            if ((airports->airportPtr->latitude) > (tmp->airportPtr->latitude))
            {
                air1 = (airports->airportPtr);
                (airports->airportPtr) = (tmp->airportPtr);
                (tmp->airportPtr) = air1;

            }
            tmp = (tmp->nextAirport);
        }
        airports = (airports->nextAirport);
    }
    free(tmp);

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
            if (strcmp((airports->airportPtr->LocID) , (tmp->airportPtr->LocID)) > 0)
            {
                air1 = (airports->airportPtr);
                (airports->airportPtr) = (tmp->airportPtr);
                (tmp->airportPtr) = air1;

            }
            tmp = (tmp->nextAirport);
        }
        airports = (airports->nextAirport);
    }
    free(tmp);
}
