#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "error.h"
#include "debug.h"

//----------------------------------------------------------------------------
error_t  error (error_t err)
{
	int disp = 1;

	INFOF("! ");
	switch (err) {
		case ERR_OK:
			INFOF("No error\n");
			disp = 0;
			break;
		case ERR_NULL:
			INFOF("NULL pointer\n");
			disp = 0;
			break;
		case ERR_NOTTY:
			INFOF("Unable to open device. Ensure it is not in use by another application\n");
			break;
		case ERR_RDATTR:
			INFOF("\n");
			break;
		case ERR_TXFAIL:
			INFOF("Write Fail");
			break;
		case ERR_RXFAIL:
			INFOF("Read Fail");
			break;
		case ERR_INITFAIL:
			INFOF("Failed to initialise the chatpadB");
			break;
		default:
			INFOF("Error #%d", err);
			break;
	}
	if (disp)  INFOF(" (%d - %s)\n", errno, strerror(errno)) ;
	
	return err;
}

