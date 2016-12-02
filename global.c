#define __GLOBAL__
#include "global.h"
#undef  __GLOBAL__

#include "macros.h"
#include "error.h"
#include "xlat.h"
#include "file.h"
#include "str2n.h"
#include "strim.h"
#include "strdup.h"

#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//----------------------------------------------------------------------------
void  cleanup (void)
{
	FREE(g.cfg);
	FREE(g.uart);
	FREE(g.term);

	CLOSE(g.uartfd);
	CLOSE(g.termfd);
	FCLOSE(g.cfgfh);

	return;
}

//----------------------------------------------------------------------------
bool  isTrue (char* val,  int lcnt)
{
	if ( STRIEQ(val, "on") || STRIEQ(val, "true") || 
	     STRIEQ(val, "1")  || STRIEQ(val, "yes" ) )  return true ;

	if ( STRIEQ(val, "off") || STRIEQ(val, "false") || 
	     STRIEQ(val, "0")   || STRIEQ(val, "no"   ) )  return false ;

	printf("! Config error: Line %d - use true/false, on/off, yes/no or 1/0\n", lcnt);
	exit(error(ERR_BADCFG));
}

//----------------------------------------------------------
int  getN (char* val,  int lcnt)
{
	uint32_t  d;

	if (str2n(val, &d))  return (int)d ;

	printf("! Config error: Line %d - not a valid number\n", lcnt);
	exit(error(ERR_BADCFG));
}

//----------------------------------------------------------
char*  getStr (char* val,  int lcnt)
{
	char*  cp;

	if (val && *val) {
		if ( (cp = strdup(val)) )  
			return cp;
		else {
			printf("! Config error: Line %d - malloc() fail\n", lcnt);
			exit(255);
		}
	}

	if (lcnt == -1)
		printf("! Config error: Line %d - bad string in exe\n", lcnt);
	else
		printf("! Config error: Line %d - bad string\n", lcnt);
	exit(error(ERR_BADCFG));
}

//----------------------------------------------------------
void  getMode (char* val,  int lcnt)
{
(void)val;
(void)lcnt;
	return;
}

//----------------------------------------------------------
void  getOut (char* val,  int lcnt)
{
(void)val;
(void)lcnt;
	return;
}
//----------------------------------------------------------
bool  split (char* l,  int lcnt,  char** key,  char** val)
{
	char*  cp = strchr(l, '=');

	if (cp) {
		*cp  = '\0';
		*key = getStr(l, lcnt);
		*val = getStr(cp+1, lcnt);
		return true;
	}

	return false;
}

//----------------------------------------------------------
static
error_t  usrCfg (void)
{
	enum {
		STATE_NONE,
		STATE_OPTIONS,
		STATE_PRE,
		STATE_POST,
	} state = STATE_NONE ;

	char      l[200];
	int       lcnt = 0;
	int       len  = 0;
	char*     key  = NULL;
	char*     val  = NULL;

	printf("# Load config file %s\n", g.cfg);

	if ( !(g.cfgfh = fopen(g.cfg, "rb")) )  {
		printf("! Failed to open file |%s|\n", g.cfg);
		return ERR_NOFILE;
	}

	FOREVER {
		freadl(l, sizeof(l), g.cfgfh);
		if (feof(g.cfgfh))  break ;
		lcnt++;

		strim(l);
		if ((!*l) || (*l == '#'))  continue ;

		len = strlen(l);
		if (l[len -1] == '\n')  l[len -1] = '\0' ;

		if (*l == '[') {
			printf("  %s\n", l);
			if (STRIEQ(l, "[options]")  ) { state = STATE_OPTIONS;  continue; }
			if (STRIEQ(l, "[xlat_pre]") ) { state = STATE_PRE;      continue; }
			if (STRIEQ(l, "[xlat_post]")) { state = STATE_POST;     continue; }
			printf("! Config error: Line %d - valid headers are\n"
			       "    [options] [xlat_pre] and [xlat_post]?\n", lcnt);
			return ERR_BADCFG;
		}

		switch (state) {
			case STATE_OPTIONS:
				if (!split(l, lcnt, &key, &val)) {
					printf("! Config error: Line %d - what are you trying to say?\n", lcnt);
					return ERR_BADCFG;
				}
				printf("    |%s=%s|\n", key, val);

				if      (STRIEQ(key, "uart"      ))  g.uart = getStr(val, lcnt) ;
				else if (STRIEQ(key, "term"      ))  g.term = getStr(val, lcnt) ;

				else if (STRIEQ(key, "initRetry" ))  g.init_retry  = getN(val, lcnt) ;
				else if (STRIEQ(key, "initReply" ))  g.init_uSwait = getN(val, lcnt) ;
				else if (STRIEQ(key, "dcDelay"   ))  g.st.dcDelay  = getN(val, lcnt) ;
				else if (STRIEQ(key, "rptDelay"  ))  g.st.rptSt    = getN(val, lcnt) ;
				else if (STRIEQ(key, "rptRate"   ))  g.st.rptRpt   = getN(val, lcnt) ;

				else if (STRIEQ(key, "repeat"    ))  g.repeat    = isTrue(val, lcnt) ;
				else if (STRIEQ(key, "modRepeat" ))  g.modRepeat = isTrue(val, lcnt) ;
				else if (STRIEQ(key, "sysLocks"  ))  g.sysLocks  = isTrue(val, lcnt) ;
				else if (STRIEQ(key, "wakeKey"   ))  g.wakeKey   = isTrue(val, lcnt) ;
				else if (STRIEQ(key, "uk"        ))  g.uk        = isTrue(val, lcnt) ;

				else if (STRIEQ(key, "matchMode" ))  getMode(val, lcnt) ;
				else if (STRIEQ(key, "output"    ))  getOut(val, lcnt) ;
				else {
					printf("! Config error: Line %d - unknown option\n", lcnt);
					return ERR_BADCFG;
				}
				break; 

			case STATE_PRE:
			case STATE_POST:
			case STATE_NONE:
			default:
				break;
		}// Switch state

	}// FOREVER
//printf("total=%d lines\n", lcnt);

	return ERR_OK;
}

//----------------------------------------------------------------------------
// The following are expected to be read from the config file
//   Init retry count
//   Init reply wait
//   double-click delay
//
error_t  init (void)
{
	error_t  err = ERR_OK;

	g.cfg         = NULL;
	g.cfgfh       = NULL;

	g.alive       = false;

	g.uart        = NULL;  // Pi0..3 compatible
	g.uartfd      = -1;

	g.term        = NULL;
	g.termfd      = -1;

	g.device      = DEV_STDOUT;

	g.init_retry  = 10;
	g.init_uSwait = 50 *1000;  // 50mS

	g.sysLocks    = true;
	g.matchMode   = MODE_STANDARD;
	g.wakeKey     = false;
	g.uk          = false;

	memset(g.st.status, 0, STATUSLEN);
	g.st.ledMain = false;
	g.st.ledShf  = false;
	g.st.ledGrn  = false;
	g.st.ledOrn  = false;
	g.st.ledPpl  = false;
	memset(g.st.key, 0, K_MAX +1);
	memset(g.st.id, 0, IDLEN);

	gettimeofday(&g.st.dcTime, NULL);
	g.st.dcKey    = 0;
	g.st.dcMods   = 0xFF;
	g.st.dcCnt    = 0;
	g.st.dcDelay  = 350 *1000; // 350mS

	g.modRepeat   = false;
	gettimeofday(&g.st.rptTime, NULL);
	g.st.rptVirt  = false;
	g.st.rptSt    = 350 *1000;  // 350mS
	g.st.rptRpt   = 150 *1000;  // 150mS

	// on init: g.st.rptId != g.st.id
	memset(g.st.rptId, '-', IDLEN);  // '-' hints to treat this as a key-down event

//	checkCLI();

	if (!g.cfg)   g.cfg  = getStr("default.cfg", -1);
	if ((err = usrCfg()) != ERR_OK)  return err ;

	// We just hard-tweak the keyboard map for the GBP symbol
	if (g.uk)  setUK() ;

	if (!g.uart)  g.uart = getStr("/dev/serial0", -2);  // Pi0..3 compatible
	if (!g.term)  g.term = getStr("/dev/pts/1", -3);

	atexit(cleanup);

	return ERR_OK;
}

