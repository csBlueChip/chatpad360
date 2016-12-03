#include "io.h"
#include "debug.h"
#include "global.h"

#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "macros.h"

//----------------------------------------------------------------------------
error_t  openTerm (char* term)
{
	// Open the terminal for key-stuffing
	if ((g.termfd  = open(term, O_WRONLY | O_NOCTTY | O_NDELAY)) == -1)
		return ERR_NOTTY;

	// Open the terminal for terminal commands
	if ((g.termfh = fopen(term, "wb")) == NULL)  return ERR_NOTERM ;

	return ERR_OK;
}

//----------------------------------------------------------------------------
error_t  send2termfd (char* s)
{
	if ((s[0] == '\xff') && (s[1] != 0) && (s[2] == '\0')) {
		if      (s[1] == '\xff')        ioctl(g.termfd, TIOCSTI, &s[2]) ;
		else if (INRANGE(s[1], 1, 26))  ioctl(g.termfd, TIOCSTI, &s[1]) ;
	} else {
		while (*s)
//!echk
			ioctl(g.termfd, TIOCSTI, s++) ;
	}

	return ERR_OK;
}

//------------------------------------------------------------------------------
void  send2termfh (char* s)
{
	fprintf(g.termfh, s);
	fflush(g.termfh);

	return;
}

//----------------------------------------------------------------------------
// At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD 
// (ie the alt0 function) respectively.
//
//
error_t  openUART (char* uart)
{
	struct termios  tty;
	// OPEN THE UART
	// The flags (defined in fcntl.h):
	//   Access modes:  O_RDONLY - Open for reading only.
	//                  O_WRONLY - Open for writing only.
	//                  O_RDWR   - Open for reading and writing.
	//
	// O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. 
	//   When set read requests on the file can return immediately with the 
	//   failure status of 'EAGAIN' if there is no input immediately available 
	//   (instead of blocking/waiting). 
	//   Likewise, write requests can also return immediately with a failure 
	//   status of '???' if the output can't be written immediately.
	//
	// O_NOCTTY - When set and path identifies a terminal device, 
	//            open() shall NOT cause the terminal device to become 
	//            the controlling terminal for the process.
	//
	if ((g.uartfd = open(uart, O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
		return ERR_NOTTY ;

	// Grab the current configuration options
//!echk
	tcgetattr(g.uartfd, &tty);

	//CONFIGURE THE UART
	// The flags (defined in /usr/include/termios.h - 
	// http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//   CSIZE  - CS5, CS6, CS7, CS8 (#bits) [CSZIE is the mask]
	//   CLOCAL - Ignore modem status lines (DTR/DCD)
	//   CREAD  - Enable receiver (Rx)
	//   PARENB - Parity enable
	//   PARODD - Odd parity (else even) [MARK & SPACE are also available]
	//   IGNPAR - Ignore characters with parity errors
	//   ICRNL  - Map CR to NL on input (Use for ASCII comms to autocorrect EOL
	//                                   chars. Do NOT use for bianry comms)
	//

	tty.c_cflag = CS8 | CLOCAL | CREAD;
	tty.c_iflag = IGNPAR;
	tty.c_oflag = 0;
	tty.c_lflag = 0;

	// Set In/Out (Rx/Tx) speeds
	// Baud rate - B1200, B2400, B4800, B9600, B19200, B38400, B57600, 
	//             B115200, B230400, B460800, B500000, B576000, B921600, 
	//             B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, 
	//             B3500000, B4000000
//!echk
	cfsetispeed(&tty, B19200);
	cfsetospeed(&tty, B19200);

	// Flush any data currently in the UART buffer
//!echk
	tcflush(g.uartfd, TCIFLUSH);

	// Write the settings to the port
//!echk
	tcsetattr(g.uartfd, TCSANOW, &tty);

	return ERR_OK;
}

//----------------------------------------------------------------------------
// Akin to a Pascal String, 
// the first byte is the number of subsequent bytes to be written
//
error_t  tx (uint8_t* buf)
{
	int  cnt, bytes;

	if (!buf)  return ERR_NULL ;

	PKTDBGF("? Checksum=");
	switch (checksum(buf, true)) {
		case ERR_OK      :  PKTDBGF("OK : ");         break ;
		default:
		case ERR_CKSM    :  PKTDBGF("FAIL : ");       break ;
		case ERR_CKSM_OK :  PKTDBGF("CORRECTED : ");  break ;
	}

	PKTDBGF("->{");

	// We may need to issue multiple write()s to send all the data
	for (cnt = 0;  cnt < *buf;  cnt += bytes) {
		bytes = write(g.uartfd, buf +1 +cnt, *buf -cnt);
		if (bytes < 0)  return ERR_TXFAIL ;
//!echk
		tcdrain(g.uartfd);

#if PKT_DEBUG == 1
		{  // Hex dump time...
			int  i;
			for (i = 0;  i < bytes;  i++) {
				printf("%02X", *(buf +1 +cnt +i));
				if ( !((i >= bytes -1) && (cnt +bytes >= *buf)) )  printf(",");
			}
		}
#endif
	}
	PKTDBGF("}[%d]\n", *buf);

	return ERR_OK;
}

//----------------------------------------------------------------------------
// Note the first byte of the buffer is used to store the length,
// EG. A buffer of length 10 can hold 1 length byte and 9 data bytes
//
error_t  rx (uint8_t* buf,  int len)
{
	int  n;

	if (!buf)     return ERR_NULL ;
	if (len < 2)  return ERR_BADLEN ;

	// Set 0 bytes read
	*buf = 0;
	//memset(buf, 0, len);

	// Perform a read
	n = read(g.uartfd, buf +1, len -1);
	PKTDBGF("? [%d]", n);
	// Error check
	// We ignore EAGAIN, it means : a non-blocking read returned 0 bytes
	if (n < 0)  return (errno == EAGAIN) ? ERR_RXNONE : ERR_RXFAIL ;

	*buf = n;

#if PKT_DEBUG == 1
	PKTDBGF("<-");
	dumpBuf(buf);
	PKTDBGF("\n");
#endif

	return ERR_OK;
}

//----------------------------------------------------------------------------
// Set 'fix' to true if you want bad checksums to be fixed
//           or false if you just want to verify it
//
error_t  checksum (uint8_t* buf,  bool fix)
{
	int      i;
	uint8_t  cksm = 0;

	if (!buf)  return ERR_NULL ;

	for (i = 0;  i < (*buf) -1;  i++)  cksm += buf[1+i] ;
	cksm = (~cksm) +1;  // 2's complement

	if (buf[1+i] != cksm) {
		if (fix) {
			buf[1+i] = cksm;
			return ERR_CKSM_OK;
		} else {
			return ERR_CKSM;
		}
	}

	return ERR_OK;
}

