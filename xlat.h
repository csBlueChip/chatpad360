#ifndef _XLAT_H_
#define _XLAT_H_

#include <stdint.h>

#include "xlat_utf8.h"

void   setUK     (void) ;

char*  xlat_pre  (char* id) ;
char*  xlat_post (char* id) ;

char*  xlat_utf8 (uint8_t ch) ;

char*  xlat_func (uint8_t ch) ;
char*  xlat_ctrl (uint8_t ch) ;
char*  xlat_curs (uint8_t ch) ;
char*  xlat_sys  (uint8_t ch) ;
char*  xlat_npad (uint8_t ch) ;

#endif // _XLAT_H_

