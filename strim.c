#include <stddef.h>
#include <string.h>
#include "strim.h" // ISWHITE()

//+============================================================================
// strimt ... trim suffixing whitespace (string trim tail)
// strimh ... trim prefixing whitespace (string trim head)
// strim  ... trim line from both ends  (string trim head+tail)
//

//+====================================
char*  strimh (char* s)
{
	if (s && *s) {
		char  *src, *dst;

		for (src = s;  *src && ISWHITE(*src);  src++) ;
		for (dst = s;  *src;  *dst++ = *src++) ;
		*dst = '\0';
	}
	return s;
}

//+====================================
char*  strimt (char* s)
{
	char*  cp = NULL;

	if (s && *s)
		for (cp = s + strlen(s) - 1;  
		     (cp >= s) && ISWHITE(*cp);  
		     *cp-- = '\0') ;
	return s;
}

//+====================================
char*  strim (char* s)
{
	if (s) {
		(void)strimt(s);
		(void)strimh(s);
	}
	return s;
}
