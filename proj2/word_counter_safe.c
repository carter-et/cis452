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
#include <fcntl.h>

/*define*/

/*vars*/
int total_lines = 0;
int total_words = 0;
size_t total_bytes = 0;

/*methods*/
int countWords(char* file_name, int* lines_addr);
size_t countBytes(char* file_name);
void openFile(char* file_name, int* words_addr, int* lines_addr, size_t* bytes_addr);


/*Main Method*/
int main(int argc, char** argv){
	
	if(argc == 0 || argc == 1){
		printf("");
	}

	for (int i = 1; i < argc; ++i){
		int words = 0;
		int lines = 0;
		size_t bytes = 0;

		openFile(argv[i], &words, &lines, &bytes);
		printf("%d \t%d \t%ld \t%s\n", lines, words, bytes, argv[i]);

		total_lines += lines;
		total_words += words;
		total_bytes += bytes;
	}

	printf("%d \t%d \t%ld \t%s\n", total_lines, total_words, total_bytes, "Total");

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
