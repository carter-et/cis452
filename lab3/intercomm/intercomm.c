#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigHandler(int sigID);
void exitNicely(int sigID);

int main() {
	//pid_t pid; //pid of parent process

	signal(SIGUSR1, sigHandler);
	signal(SIGUSR2, sigHandler);

	int random = (rand()%5)+1;//random num 1-5

	pid_t cid = fork();
	if(cid < 0){
		perror("fork failure\n");
		exit(1);
	}
	else if(cid > 0){
		printf("spawn child PID #%d\n\n", cid);
		printf("waiting...    ");
		fflush(stdout);
		pause();

		while(1){
			printf("waiting...   ");
			fflush(stdout);
			pause();
			signal(SIGINT, sigHandler);
			signal(SIGINT, exitNicely);
		}
	}
	else{
		while(1){
			sleep(random);
			int signal = rand()%2;
			if(signal == 0){
				kill(getppid(), SIGUSR1);
			}
			else{
				kill(getppid(), SIGUSR2);
			}
		}
	}
}

void sigHandler(int sigID){
	if(sigID == SIGUSR1){
		printf("Recieved a SIGUSR1 signal\n");
	}
	if(sigID == SIGUSR2){
		printf("Recieved a SIGUSR2 signal\n");
	}
}


void exitNicely(int sigID){
	printf("\nThat's it, I'm shutting you down...\n");
	exit(0);
}


