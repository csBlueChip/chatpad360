#ifndef _DEBUG_H_
#define _DEBUG_H_

//----------------------------------------------------------------------------
#define PKT_DEBUG 0
#define KEY_DEBUG 0
#define RUN_DEBUG 0
#define INF_DEBUG 1

//----------------------------------------------------------------------------
#include <stdio.h>

//----------------------------------------------------------------------------
#if PKT_DEBUG == 1
#	define PKTDBGF(...)  do{ printf(__VA_ARGS__);  fflush(stdout); }while(0)
#else
#	define PKTDBGF(...)  (void)0
#endif

#if RUN_DEBUG == 1
#	define RUNDBGF(...)  do{ printf(__VA_ARGS__);  fflush(stdout); }while(0)
#else
#	define RUNDBGF(...)  (void)0
#endif

#if KEY_DEBUG == 1
#	define KEYDBGF(...)  do{ printf(__VA_ARGS__);  fflush(stdout); }while(0)
#else
#	define KEYDBGF(...)  (void)0
#endif

#if INF_DEBUG == 1
#	define INFOF(...)  do{ printf(__VA_ARGS__);  fflush(stdout); }while(0)
#else
#	define INFOF(...)  (void)0
#endif

//----------------------------------------------------------------------------
#include <stdint.h>

//----------------------------------------------------------------------------
void   spin (void) ;
void   dumpBuf (uint8_t* buf) ;
char*  timestamp (void) ;
void   showStatus (void) ;

#endif // _DEBUG_H_
