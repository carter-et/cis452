#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

bool running = true;

void sigHandler(int number) {
	printf ("Signal handler of process %d\n", getpid());
	running = false;
}

int main() {
	//signal(SIGUSR1, sigHandler);
	pid_t who = fork();
	if (who == 0) {
		signal(SIGUSR1, sigHandler);
		while (running) {
			printf ("I'm the child PID:%d\n", getpid());
			sleep(2);
		}
		return 73;
	}
	else {
		int code;
		while (running) {
			printf ("I'm the parent PID:%d\n", getpid());
			sleep(1);
		}

	
		wait(&code);
		printf ("Child exit code is %d\n", WEXITSTATUS(code));
	}
	printf ("Out of main()\n");
}
