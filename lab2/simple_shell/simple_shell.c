#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void cleanUp(char* input, char** args);
void processInput(char* input, char** finalArgs);
void removeNewline(char* input);

int main(int argc, char** argv){
	char*  inputBuffer;
    	char** argsBuffer;
	struct rusage* usageStruct = (struct rusage*) malloc(sizeof(struct rusage));
	long cumulativeInvContextSwitchesOffset = 0;
	suseconds_t cumulativeUsageOffset = 0;

	while(1){
		inputBuffer = (char*) malloc(9001 * sizeof(char));
		argsBuffer = (char**) malloc(10 * sizeof(char**));
		
		
		printf("(shell) @user: ");
		fgets(inputBuffer, 9001, stdin); //over ninethousand
		
		//remove new line
		removeNewline(inputBuffer);

		//process user args
		if(strcmp(inputBuffer, "quit") == 0) { //if given exit command
			//go clean up
			cleanUp(inputBuffer, argsBuffer);
			free(usageStruct);
			break;
		}
		else { 
			//break down input into args
			processInput(inputBuffer, argsBuffer);

			// Fork the process torun the the cmd.			       
			pid_t pid = fork();

			if(pid < 0) {
				perror("Fork failed");
				return 1;
			}
			else if (!pid) { //
				execvp(argsBuffer[0], argsBuffer);//bring in a new binary
				cleanUp(inputBuffer, argsBuffer);
				exit(1);
			}
			else { //magic
				int status;
				wait(&status);
				int stat = getrusage(RUSAGE_CHILDREN, usageStruct); 
				if(stat == -1) {
					perror("Error with getrusage");
				}
			}
		}
		
		//signed long		
		suseconds_t userCpuTime = usageStruct->ru_utime.tv_usec - cumulativeUsageOffset;
		long invContextSwitches = usageStruct->ru_nivcsw - cumulativeInvContextSwitchesOffset;
		
		cumulativeUsageOffset = usageStruct->ru_utime.tv_usec;
		cumulativeInvContextSwitchesOffset = usageStruct->ru_nivcsw;
		printf("Usage Statistics - User CPU time: %ld Involuntary Context Switches: %ld\n", userCpuTime, invContextSwitches);
		
		//clean up
		cleanUp(inputBuffer, argsBuffer);

	}
	return 0;
}

void cleanUp(char* input, char** args){
	free(input);
	for(int k = 0; k < (sizeof args / sizeof *args) + 1; ++k){
		free(args[k]);
	}
	free(args);
}

void processInput(char* input, char** finalArgs){
	char* token;
	int i = 0;
	while(token = strsep(&input, " "), token != NULL && i < 8){
		finalArgs[i] = (char*) malloc(10 * sizeof(char));
		strcpy(finalArgs[i], token);
		++i;
	}	
	finalArgs[i] = NULL;
}

void removeNewline(char* input){
	int end = strlen(input);
	input[end-1] = '\0';
}
