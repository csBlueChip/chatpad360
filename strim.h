#ifndef STRIM_H_
#define STRIM_H_

#include <ctype.h>

// trim whitespace .. head (start), tail (end), or both

// isblank() -> {' ', '\t'}
// isspace() -> {' '. '\t'. '\r'. '\n'. '\f'. '\v'}
#define  ISWHITE(ch)  isspace(ch)

char*  strimh (char* s) ;
char*  strimt (char* s) ;
char*  strim  (char* s) ;

#endif  //STRIM_H_
