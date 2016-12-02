#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "error.h"

//----------------------------------------------------------------------------
error_t  error (error_t err)
{
	int disp = 1;

	printf("! ");
	switch (err) {
		case ERR_OK:
			printf("No error\n");
			disp = 0;
			break;
		case ERR_NULL:
			printf("NULL pointer\n");
			disp = 0;
			break;
		case ERR_NOTTY:
			printf("Unable to open device. Ensure it is not in use by another application\n");
			break;
		case ERR_RDATTR:
			printf("\n");
			break;
		case ERR_TXFAIL:
			printf("Write Fail");
			break;
		case ERR_RXFAIL:
			printf("Read Fail");
			break;
		case ERR_INITFAIL:
			printf("Failed to initialise the chatpadB");
			break;
		default:
			printf("Error #%d", err);
			break;
	}
	if (disp)  printf(" (%d - %s)\n", errno, strerror(errno)) ;
	
	return err;
}

