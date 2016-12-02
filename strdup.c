#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//------------------------------------------------------------------------------
char*  strdup (char* s)
{
	char* cp;

	return (cp = malloc(strlen(s) +1)) ? strcpy(cp, s) : NULL ;
}

//------------------------------------------------------------------------------
char*  xstrdup (char* s)
{
	char* cp;

	if (!(cp = strdup(s))) {
		fprintf(stderr, "! Malloc failed.\n");
		exit(255);
	}

	return cp;
}
