#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>

/*define*/

/*vars*/
int total_lines = 0;
int total_words = 0;
size_t total_bytes = 0;

int countWords(char* file_name, int* lines_addr);
size_t countBytes(char* file_name);
void openFile(char* file_name, int* words_addr, int* lines_addr, size_t* bytes_addr);
void processTotal(char* return_buffer);
void signalHandler(int signum);

/*Main Method*/
int main(int argc, char** argv){

	signal(SIGUSR1, signalHandler);
	
	if(argc == 0 || argc == 1){
		printf("");
	}

	for (int i = 1; i < argc; ++i){

		//set up pipes
		int p_to_c[2]; //parent to child
		int c_to_p[2]; //child to parent

		//forking stuff
		int status = 0;
		pid_t pid;

		//validate pipes
		if(pipe(p_to_c) == -1){
			fprintf(stderr, "Plumbing problem");
			return 1;
		}
		if(pipe(c_to_p) == -1){
			fprintf(stderr, "Plumbing problem");
			return 1;
		}
	
		
		pid = fork(); //split off to children now
		if(pid < 0){
			perror("Fork ran off with the spoon");
			return 1;
		}
		else if(pid > 0){ //parent process
			//make a return buffer
			char* read_buffer = (char*) malloc(255 * sizeof(char));

			//close reading end of p_to_c pipe
			close(p_to_c[0]);
			
			//write to p_to_c pipe
			write(p_to_c[1], argv[i], strlen(argv[i])+1);

			//close write to p_to_c pipe
			close(p_to_c[1]);
			
			//tell child to continue
			kill(pid, SIGUSR1);

			//wait on child
			wait(&status);

			//close writing end of c_to_p pipe
			close(c_to_p[1]);
			
			//read child's input
			read(c_to_p[0], read_buffer, 255);

			//process and output read buffer
			printf("%s",read_buffer);
			processTotal(read_buffer);

			//close reading end of c_to_p pipe
			close(c_to_p[0]);

			//free buffer
			free(read_buffer);
		}
		else{ //child process
				
			//close writing end of p_to_c pipe
			close(p_to_c[1]);

			//wait for SIGUSR1 signal
			pause();

			//read buffer for child
			char* read_buffer = (char*) malloc(255 * sizeof(char));

			//read into buffer from p_to_c pipe
			read(p_to_c[0], read_buffer, 255);

			//close both reading ends
			close(p_to_c[0]);
			close(c_to_p[0]);

			int words = 0;
			int lines = 0;
			size_t bytes = 0;
	
			openFile(read_buffer, &words, &lines, &bytes);

			//some logic to store it as a string
			char* write_buffer;
			size_t write_size;
			write_size = snprintf(NULL, 0, "%3.3d %3.3d %5.5ld \t%s\n", lines, words, bytes, argv[i]);
			write_buffer = (char*) malloc(write_size + 1);
			snprintf(write_buffer, write_size + 1, "%3.3d %3.3d %5.5ld \t%s\n", lines, words, bytes, argv[i]);
	
			//Because everything in the child gets reset, this has no real impact
			//total_lines += lines;
			//total_words += words;
			//total_bytes += bytes;

			//write result to parent
			write(c_to_p[1], write_buffer, strlen(write_buffer)+1);
			close(c_to_p[1]);			

			//free read buffer
			//free write buffer
			free(read_buffer);
			free(write_buffer);

			return 0;
		}
	}

	printf("%3d %3d %5ld \t%s\n", total_lines, total_words, total_bytes, "Total");

	return 0;
}

int countWords(char* file_name, int* lines_addr){
	//we must assume that every file that we can open has at least one word
	int word_count = 0;
	int line_count = 0;
	char buffer[1];
	enum states { WHITESPACE, WORD }; //recomended from stack overflow
	int state = WHITESPACE;
	int file = open(file_name, O_RDONLY);
	char last = ' ';
	while(read(file, buffer, 1) == 1){
		if (buffer[0] == ' ' || buffer[0] == '\t'){
			state = WHITESPACE;
		}
		else if (buffer[0] == '\n'){
			++line_count;
			state = WHITESPACE;
		}
		else{
			//increment only if its the first letter of the word
			if (state == WHITESPACE){
				++word_count;
			}
			state = WORD;
		}
		last = buffer[0];
	}
	*lines_addr = line_count;
	return word_count;
}

size_t countBytes(char* file_name){
	struct stat magic;
	if(stat(file_name, &magic) == -1){
		perror("stat error");
		exit(1);
	}
	else{
		return (int) magic.st_size;	
	}
}

void openFile(char* file_name, int* words_addr, int* lines_addr, size_t* bytes_addr){
	FILE* file = fopen(file_name, "r");

	if(file == NULL){
		printf("File Not Found\n");
	}
	else{
		*words_addr = countWords(file_name, lines_addr);
		*bytes_addr = countBytes(file_name);
	}
	fclose(file);
}

void processTotal(char* return_buffer){
	char* token;
	int i = 0;
	while(token = strsep(&return_buffer, " "), token != NULL && i < 8){
		switch(i){
			case 0:
				total_lines += atoi(token);
				break;
			case 1:
				total_words += atoi(token);
				break;
			case 2:
				total_bytes += atoi(token);
				break;
			default:
				break;
		}
		++i;
	}
}

void signalHandler(int signum){
	//some handling code here maybe?
	//signal(SIGUSR1, signalHandler); //rset signal?
}
