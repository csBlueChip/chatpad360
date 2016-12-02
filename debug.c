#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "debug.h"
#include "error.h"
#include "chatpad.h"
#include "global.h"

//----------------------------------------------------------------------------
void  spin (void)
{
	static int i = 0;

	printf("%c\b", "|/-\\"[++i&3]);
	fflush(stdout);

	return;
}

//----------------------------------------------------------------------------
void  dumpBuf (uint8_t* buf)
{
	int  i;

	printf("{");
	if (*buf) {
		for (i = 1;  i < *buf;  i++)  printf("%02X,", buf[i]) ;
		printf("%02X}[%d]", buf[i], i);
	} else {
		printf("}[]");
	}

	return;
}

//----------------------------------------------------------------------------
// code is non-reentrant
//
char*  timestamp (void)
{
	static char     buf[32];

	int             len;
	int             mSec;
	struct tm*      tm;
	struct timeval  tv;

	gettimeofday(&tv, NULL);

	mSec = (tv.tv_usec +500) /1000; // Round to nearest millisec
	if (mSec >= 1000) {  // In case we round up to a second
		mSec -= 1000;
		tv.tv_sec++;
	}

	tm = localtime(&tv.tv_sec);

	len = strftime(buf, sizeof(buf), /*"%Y:%m:%d "*/"%H:%M:%S", tm);

	if ( ((sizeof(buf) - len) - 1) >= (1 + 3))
		sprintf(buf + len, ".%03d\n", mSec);

	return buf;
}

//----------------------------------------------------------------------------
void  showStatus (void)
{
	int  i = 0;

	printf("# Status {");
	for (i = 0;  i < STATUSLEN -1;  i++)  printf("%02X,", g.st.status[i]) ;
	printf("%02X}[%d]\n", g.st.status[i], i);

	printf("  LEDs : Main   : %s\n", g.st.ledMain ? "on" : "off");
	printf("       : Shift  : %s\n", g.st.ledShf  ? "on" : "off");
	printf("       : Green  : %s\n", g.st.ledGrn  ? "on" : "off");
	printf("       : People : %s\n", g.st.ledPpl  ? "on" : "off");
	printf("       : Orange : %s\n", g.st.ledOrn  ? "on" : "off");

	return;
}

