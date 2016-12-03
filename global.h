#ifndef _GLOBAL_H_
#define _GLOBAL_H_

//----------------------------------------------------------------------------
#include "error.h"
#include "chatpad.h"

#include <stdio.h>

//----------------------------------------------------------------------------
typedef
	enum device_t {
		DEV_STDOUT = 0x01,
		DEV_TTY    = 0x02,
		DEV_KEYLOG = 0x04,
	}
device_t;

//----------------------------------------------------------------------------
typedef
	enum matchMode_t {
		MODE_PRE  = 0x01,
		MODE_UTF8 = 0x02,
		MODE_POST = 0x04,
		MODE_FUNC = 0x08,
		MODE_CTRL = 0x10,
		MODE_MOVE = 0x20,
		MODE_SYS  = 0x40,
		MODE_NPAD = 0x80,
	}
mmode_t;

#define MODE_STANDARD \
	(MODE_UTF8 | MODE_FUNC | MODE_CTRL | MODE_MOVE | MODE_SYS)

//----------------------------------------------------------------------------
typedef
	struct global_t {
		char*     cfg;          // Config filename
		FILE*     cfgfh;        // File handle for config file

		bool      alive;        // Are we receiveing status packets?

		char*     uart;         // Name of input tty "/dev/ttyS0"
		int       uartfd;       // File descriptor for open UART

		char*     term;         // Name of input tty "/dev/pts/1"
		int       termfd;       // File descriptor for open terminal
		FILE*     termfh;       // File handle for terminal

		int       init_retry;   // Number of retries to init the chatpas
		int       init_uSwait;  // Time to wait for an init reply (microseconds)

		bool      uk;           // UK mode puts GBP symbol on Shf+3
		bool      sysLocks;     // System handles mod locks (efficient)
		state_t   st;           // Decoded from Status packet
		mmode_t   matchMode;    // Match mode
		bool      wakeKey;      // Key which turns on the LEDs is ignored
		bool      modRepeat;    // Repeat modifiers?
		bool      repeat;       // Repeat enabled?
		device_t  device;       // Output device
	}
global_t;

//----------------------------------------------------------------------------
#ifdef  __GLOBAL__
#	define SCOPE
#else
#	define SCOPE extern
#endif

SCOPE  global_t g;

//----------------------------------------------------------------------------
error_t  init     (void) ;
error_t  usrCfg   (void) ;
error_t  parseCLI (int argc,  char** argv) ;
char*    getStr   (char* val,  int lcnt) ;

#endif // _GLOBAL_H_
