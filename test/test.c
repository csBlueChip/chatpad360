#include <fcntl.h>  // open/close
#include <sys/ioctl.h> // ioctl
#include <unistd.h> // sleep
#include <string.h> // strcmp
#include <stdio.h>  // printf, fopen

#define ESC "\x1b"

int main (int argc, char** argv)
{
	if (argv[1] && *argv[1] == 'w') {
		FILE*  fh = fopen("/dev/tty1", "wb");
		printf("Wake terminal\n");
		if (fh == NULL) return 1 ;
		fprintf(fh, ESC"[13]");
		fclose(fh);

	} else {
		int    fd = open("/dev/tty1", O_WRONLY | O_NDELAY | O_NOCTTY);
		if (fd == -1)  return 1 ;
		while (1) {
			char*  cp = (argc > 1) ? argv[1] : "ls\n" ;
			printf("\nSend |%s|...", cp);
			while (*cp)  ioctl(fd, TIOCSTI, cp++) ;
			printf(" Wait...", cp);
			sleep(3);
		}
	close(fd);
	}


	return 0;
}

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

