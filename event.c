#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "io.h"
#include "macros.h"
#include "global.h"
#include "error.h"
#include "xlat.h"

//------------------------------------------------------------------------------
static
void  sendCode (char* kc)
{
	//INFOF("[%02X:%02X:%02X:%02X:%02X:%02X]", kc[0], kc[1], kc[2], kc[3], kc[4], kc[5]);

	if (g.device & DEV_TTY) {
		send2termfd(kc);
	}

	// stdio
	if (g.device & DEV_STDOUT) {
		// Handle escape codes
		if (kc[0] == '\x1b') {
			printf("^[%s", (kc[1] == '\x1b') ? "^[" : &kc[1]);

		// Handle "control keys" (EG. ^C)
		} else if ((kc[0] == '\xff') && (kc[1] != 0) && (kc[2] == '\0')) {
			if      (kc[1] == '\xff')        printf("^@") ;
			else if (INRANGE(kc[1], 1, 26))  printf("^%c", kc[1] + '@') ;

		// Handle everything else
		} else printf("%s", kc) ;
		fflush(stdout);
	}

	// Keyboard logger
	if (g.device & DEV_KEYLOG) {
	}
}

//------------------------------------------------------------------------------
error_t  id2event (char* id)
{
	char*  code = NULL;

	// Wake the terminal
	// If there is some unknown-to-me and EFFICIENT method to
	// work out if this is required, then I'd like to know about it
	// In lieu of that knowledge - we make sure the terminal is awake
	// with EVERY keystroke
	send2termfh(ESC"[13]");

	// Perform user "pre" translation
	if (g.matchMode & MODE_PRE)  code = xlat_pre(id) ;

	// Perform Standard translations
	if ((!code) && (id[0] == '+')) {  // Key-Down Event
		// Caveat: Call xlat_sys() before xlat_curs() or you will lose Alt-Return
		if ( (g.matchMode & MODE_UTF8) && (code = xlat_utf8(id[7])) )  goto stdOK ;
		if ( (g.matchMode & MODE_FUNC) && (code = xlat_func(id[7])) )  goto stdOK ;
		if ( (g.matchMode & MODE_CTRL) && (code = xlat_ctrl(id[7])) )  goto stdOK ;
		if ( (g.matchMode & MODE_SYS ) && (code = xlat_sys (id[7])) )  goto stdOK ;
		if ( (g.matchMode & MODE_MOVE) && (code = xlat_curs(id[7])) )  goto stdOK ;
		if ( (g.matchMode & MODE_NPAD) && (code = xlat_npad(id[7])) )  goto stdOK ;
	} stdOK:

	// Perform user "post" translation
	if ((!code) && (g.matchMode & MODE_POST))  code = xlat_post(id) ;

	// Ignore untranslated Key-Up Events
	if ((!code) && (id[0] == '-'))  code = "" ;

	// Strip untranslated modifiers
	if ((!code) && strchr("*^#@&-", id[7]))  code = "" ;

	// A bit of error checking, and we're ready to send the code
	if (!code)  code = "\a" ;
	if (*code)  sendCode(code) ;

	return ERR_OK;
}
