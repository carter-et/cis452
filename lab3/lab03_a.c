#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>

void sigHandler (int);
bool i_can_run = true;

int main () {
	signal (SIGINT, sigHandler);
	int extra_life = 5;
	while (i_can_run || extra_life > 0) {
		printf ("waiting...\n");
		usleep(300000); // microseconds
		if (errno == EINTR) {
			perror ("Oops...");
		}
		if (!i_can_run)
			extra_life--;
	}

	pause ();
	perror("After pause()");
	
	printf ("Out of the main loop\n");
	return 0;
}

void sigHandler (int sigNum) {
	printf ("Inside signal handler.\n");
	sleep (5);
	i_can_run = false;
	printf ("outta handler.\n");
}
