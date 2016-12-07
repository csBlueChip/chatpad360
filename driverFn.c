#include "io.h"
#include "global.h"
#include "error.h"
#include "macros.h"

#include <stdio.h>
#include <syslog.h>

char*  driverFn (char* id)
{
	char*  handled = "";

	if (STREQ(id, "+;12232k&k")) // all locks AND ppl+k == kill
		exit(ERR_OK);

	if (STREQ(id, "+;000105&5")) {  // ppl+5 == close+reopen devices
		error_t  err;
		if ((err = reopenAll()) != ERR_OK)  syslog(LOG_NOTICE, "Device reopen failed") ;
		return handled;
	}

	return NULL;  // not handled
}