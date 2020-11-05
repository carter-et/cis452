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
struct data {int li; int wo; size_t by; char* ar;} wc_data;

/*vars*/
int total_lines = 0;
int total_words = 0;
size_t total_bytes = 0;

int countWords(char* file_name, int* lines_addr);
size_t countBytes(char* file_name);
void openFile(char* file_name, int* words_addr, int* lines_addr, size_t* bytes_addr);
void processTotal(struct data wc_data);
void signalHandler(int signum);

/*Main Method*/
int main(int argc, char** argv){
	signal(SIGUSR1, signalHandler);
	
	if(argc == 0 || argc == 1){
		printf("");
	}

	int size = 2 * argc;

	//set up pipes
	int p_to_c[size]; //parent to child
	int c_to_p[size]; //child to parent

	//forking stuff
	int status = 0;

	for (int i = 1; i < argc; ++i){

		pid_t pid;

		//validate pipes
		if(pipe(p_to_c + (2*i)) == -1){
			fprintf(stderr, "Plumbing problem");
			return 1;
		}
		if(pipe(c_to_p + (2*i)) == -1){
			fprintf(stderr, "Plumbing problem");
			return 1;
		}
	
		
		pid = fork(); //split off to children now
		if(pid < 0){
			perror("Fork ran off with the spoon");
			return 1;
		}
		else if(pid > 0){ //parent process
			//close reading end of p_to_c pipe
			close(p_to_c[i*2]);
			
			//write to p_to_c pipe
			write(p_to_c[i*2+1], argv[i], strlen(argv[i])+1);

			//close write to p_to_c pipe
			close(p_to_c[i*2+1]);
			
			//wait on child
			//wait(&status);

			//close writing end of c_to_p pipe
			close(c_to_p[i*2+1]);
		}
		else{ //child process
				
			//close writing end of p_to_c pipe
			close(p_to_c[i*2+1]);

			//wait for SIGUSR1 signal
			//pause();

			//read buffer for child
			char* read_buffer = (char*) malloc(255 * sizeof(char));

			//read into buffer from p_to_c pipe
			read(p_to_c[i*2], read_buffer, 255);

			//close both reading ends
			close(p_to_c[i*2]);
			close(c_to_p[i*2]);

			int words = 0;
			int lines = 0;
			size_t bytes = 0;
	
			openFile(read_buffer, &words, &lines, &bytes);

			//some logic to store it as a string
			size_t write_size;
			write_size = snprintf(NULL, 0, "%3d %3d %5ld \t%s\n", lines, words, bytes, argv[i]);

			wc_data.li = lines; 
			wc_data.wo = words;
			wc_data.by = bytes;
			wc_data.ar = argv[i];

			//write result to parent
			write(c_to_p[i*2+1], &wc_data, sizeof(wc_data));
			close(c_to_p[i*2+1]);			

			//free read buffer
			//free write buffer
			free(read_buffer);
			
			return 0;
		}
	}
	pause();

	for (int i = 1; i < argc; ++i){
		// char* read_buffer = (char*) malloc(255 * sizeof(char));
		struct data read_buffer;
		// int status = 0;
		wait(&status);
		//read child's input
		read(c_to_p[i*2], &read_buffer, sizeof(read_buffer));

		//process and output read buffer
		//printf("%s",read_buffer);
		printf("%3d %3d %5ld \t%s\n", read_buffer.li, read_buffer.wo, read_buffer.by, read_buffer.ar);
		processTotal(read_buffer);

		//close reading end of c_to_p pipe
		close(c_to_p[i*2]);
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

void processTotal(struct data wc_data){
	total_lines += wc_data.li;
	total_words += wc_data.wo;
	total_bytes += wc_data.by;
}

void signalHandler(int signum){

}
