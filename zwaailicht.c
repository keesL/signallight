/***************************************************************************
 *
 * control program for the KISS signal light. Makes RTS high (+3-15V) for n
 * seconds. This was originally developed to activate a rotating light via
 * a serial port on a Linux box. 
 *
 * (C) 2000 Kees Leune <kees@leune.org>
 ***************************************************************************/

#include <unistd.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#define PORT "/dev/zwaailicht"


// It happens here
int main (int argc, char* argv[]) {
	int     fd;
	int     status;
	struct termios attr;
	int     delay;

	if (argc <= 1) {
		fprintf(stderr, "Usage: %s <seconds>\n", argv[0]);
		exit(-1);
	}
	delay = atoi(argv[1]);
	if (delay == 0) {
		fprintf(stderr, "Usage: %s <seconds>\n", argv[0]);
		exit(-1);
	}


	/* open serial port in ReadWrite mode, without waiting for a DCD */
	fd = open(PORT, O_RDWR | O_NDELAY | O_NOCTTY) ;
	if (fd == -1) {
		fprintf(stderr, "Unable to open %s: %s\n", 
			PORT, strerror(errno));
		exit(-1);
	}

    /* set baud rate to 0 so DTR is dropped */
	if ( tcgetattr(fd, &attr) == -1 ) {
		close(fd);
		perror( "tcgetattr" );
		exit(-1);
	}
	cfsetospeed(&attr, B0);
	cfsetispeed(&attr, B0);
	if ( tcsetattr(fd, TCSANOW, &attr) == -1) {
		close(fd);
		perror( "tcsetattr" );
		exit(-1);
	}

	/* get status of PIN */
	status = ioctl(fd, TIOCMGET, &status);
	if ( status == -1 ) {
		close(fd);
		perror( "ioctl TIOCMGET" );
		exit(-1);
	}

    // switch RTS off and DTR on
	status |= TIOCM_RTS;
	status &= ~TIOCM_DTR;

	if ( ioctl(fd, TIOCMSET, &status) == -1 ) {
		close(fd);
		perror ("ioctl TIOCMSET");
		exit(-1);
	}

	sleep(delay);
	close(fd);

	return 0;
}
