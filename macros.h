#ifndef MACROS_H
#define MACROS_H

#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>

// Check value is in a given range (inclusive)
#define INRANGE(x,lo,hi)  ( ( ((x) >= (lo)) && ((x) <= (hi)) ) ? true : false )

// Friendly loop
#define FOREVER  while(1)

// close() the file AND reset the file descriptor to "no file"
#define CLOSE(fd)   do{ if ((fd) != -1) close(fd);  (fd) = -1; }while(0)
#define FCLOSE(fh)  do{ if (fh) fclose(fh);  (fh) = NULL; }while(0)

// Friendly memcmp()
#define MEMEQ(p1,p2,len)  (memcmp((p1), (p2), (len)) == 0)

// Friendly strcmp()
#define STREQ(s1,s2)        (strcmp     (s1, s2)      == 0)
#define STRNEQ(s1,s2,len)   (strncmp    (s1, s2, len) == 0)

#define STRIEQ(s1,s2)       (strcasecmp (s1, s2)      == 0)
#define STRNIEQ(s1,s2,len)  (strncasecmp(s1, s2, len) == 0)

// Free the memory AND clear the pointer
#define FREE(p)  do{  free(p);  p = NULL;  }while(0)

// Supplemental to ctype:isdigit()
#define isbdigit(ch)  (((ch >= '0') && (ch <= '1')) ? 1 : 0)
#define isodigit(ch)  (((ch >= '0') && (ch <= '7')) ? 1 : 0)

#endif  // MACROS_H
