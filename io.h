#ifndef _IO_H_
#define _IO_H_

//----------------------------------------------------------------------------
#include "error.h"
#include <stdint.h>
#include <stdbool.h>

//----------------------------------------------------------------------------
error_t  openTerm (char* term) ;
error_t  send2term (char* s) ;
error_t  openUART (char* uart) ;
error_t  tx (uint8_t* buf) ;
error_t  rx (uint8_t* buf,  int len) ;
error_t  checksum (uint8_t* buf,  bool fix) ;

#endif // _IO_H_
