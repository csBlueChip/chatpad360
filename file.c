#include <stdio.h>

//+============================================================================
// Handle all line endings
// Always terminate string \0
// Return blank lines ""
// Return strlen(l)
//
int  freadl(char* l,  const int max,  FILE* fin)
{
	int   i;
	
	if (!l || !fin)  return -1 ;

	*l = '\0';

	for (i = 0;  i + 1 < max;  i++) {
		char  ch = fgetc(fin);
		if (feof(fin))  break ;

		// handle all forms of line termination
		if ( (ch=='\r') || (ch=='\n') ) {
			char  ch2 = fgetc(fin);
			if (feof(fin))  break ;

			if ( (ch2 == ch) || ((ch2 != '\r') && (ch2 != '\n')) )
				fseek(fin, -1, SEEK_CUR);

			break;
		}

		*(l + i)     = ch;
		*(l + i + 1) = '\0';
	}

	return i;
}
