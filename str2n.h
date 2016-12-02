#ifndef STR2N_H_
#define STR2N_H_

#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
#define DEFAULT_BASE  (10)
#define TERMINATORS   "\t\" +-*/%|&^=,:;_[]{}()"
#define MAXVAL        (0xFFFFFFFF)
#define S2D_TYPE      uint32_t
#define S2D_MAX       UINT32_MAX

//-----------------------------------------------------------------------------
bool  str2n (char* s,  S2D_TYPE* num) ;
int   str2ne(char* s,  S2D_TYPE* num) ;
int   str2nn(char* s,  S2D_TYPE* num) ;

//-----------------------------------------------------------------------------
typedef
	enum s2dErr {
		ERR_S2D_OK       = 0,
		ERR_S2D_EMPTY    = 1,
		ERR_S2D_BASE     = 2,
		ERR_S2D_DIGIT    = 3,
		ERR_S2D_OVERFLOW = 4,
	}
s2dErr_t;

#endif  // STR2N_H_
