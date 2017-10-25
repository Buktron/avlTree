#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
  float digitalDegree, degrees, minutes, seconds_milliarc;  
	char direction;   
  char *temp = malloc(sizeof(char)* 20);
  char *degreeString = "29-22-58.8920S";
  
  if(degreeString == NULL)
  {
    return 0.0;
  }

  if(strlen(degreeString) == 14)
  {
    direction = degreeString[13];
  }
  else if(strlen(degreeString) == 15)
  {
    direction = degreeString[14];
  }

  strcpy(temp, degreeString);
  degrees = atof(strsep(&temp, "-"));
	minutes = atof(strsep(&temp, "-"));
	seconds_milliarc = atof(strsep(&temp, "-")); 
  digitalDegree = degrees + (minutes/60.0) + (seconds_milliarc/3600.0);
  if(direction == 'S' || direction == 'W')
  {
    digitalDegree = (0.0 - digitalDegree);
  }
  printf("%.4f\n",digitalDegree);

  return 0;
}
