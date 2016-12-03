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
#if INF_DEBUG == 1
	static int i = 0;

	printf("%c\b", "|/-\\"[++i&3]);
	fflush(stdout);
#endif

	return;
}

//----------------------------------------------------------------------------
void  dumpBuf (uint8_t* buf)
{
	int  i;

	INFOF("{");
	if (*buf) {
		for (i = 1;  i < *buf;  i++)  INFOF("%02X,", buf[i]) ;
		INFOF("%02X}[%d]", buf[i], i);
	} else {
		INFOF("}[]");
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

	INFOF("# Status {");
	for (i = 0;  i < STATUSLEN -1;  i++)  INFOF("%02X,", g.st.status[i]) ;
	INFOF("%02X}[%d]\n", g.st.status[i], i);

	INFOF("  LEDs : Main   : %s\n", g.st.ledMain ? "on" : "off");
	INFOF("       : Shift  : %s\n", g.st.ledShf  ? "on" : "off");
	INFOF("       : Green  : %s\n", g.st.ledGrn  ? "on" : "off");
	INFOF("       : People : %s\n", g.st.ledPpl  ? "on" : "off");
	INFOF("       : Orange : %s\n", g.st.ledOrn  ? "on" : "off");

	return;
}

