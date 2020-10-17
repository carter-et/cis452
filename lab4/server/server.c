#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define MAXBUF 255          // max buffer size
#define MAXTHREADS 9001     // max number of threads

int num_files_found = 0;    // number of files
int total_time = 0;         // total time
int num_threads = 0;
pthread_mutex_t mutex_time; // access to time
char* input_buffer;

//methods go here
void* worker_thread(void* arg);
void display_stats(int signal_num);



int main(){
	srand(time(NULL));
	pthread_mutex_init(&mutex_time, NULL);
	signal(SIGINT, display_stats);

	//get it going
	while(1){
		if(num_threads == MAXTHREADS){
			perror("The number of threads is over 9000!\n");
			exit(1);
		}

		//trying something new
		input_buffer = malloc(MAXBUF * sizeof(char));
		printf("\nEnter file name: ");
		if(fgets(input_buffer, MAXBUF, stdin) == NULL){
			perror("Problem entering file name\n");
			exit(1);
		}

		//reallocate buffer based on the actual size needed, +1
		input_buffer = realloc(input_buffer, strlen(input_buffer) + 1 * sizeof(char));
		//awesome, now we are reay to go!

		pthread_t worker;
		int status;

		if((status = pthread_create(&worker, NULL, worker_thread, (void*) input_buffer)) != 0){
			fprintf(stderr, "\nCan't create %d: %s\n", status, strerror(status));
			exit(1);
		}
	
		if((status = pthread_join(worker, NULL)) != 0){
			fprintf(stderr, "\nCan't create %d: %s\n", status, strerror(status));
			exit(1);
		}	

		//these are the same thing essentially
		++num_files_found;
		++num_threads;

		//stand against slavery! free it!
		free(input_buffer);
		
	}
	pthread_mutex_destroy(&mutex_time); //gotta free it
	return 0;
}

void* worker_thread(void* arg){

	signal(SIGINT, display_stats);

	int worker_time = 0;

	//20% chance to hit
	if((rand()%19) < 4){
		worker_time = rand()%4+7;
		sleep(worker_time);
		printf("\n%s retrieved: %d\n", (char*) arg, worker_time);	
	}else {
		worker_time = 1;
		sleep(worker_time);
		printf("\n%s retrieved: %d\n", (char*) arg, worker_time);
	}	

	//the tricky stuff. make sure only a single thread modifies total time at single instance
	pthread_mutex_lock(&mutex_time);
	total_time += worker_time;
	pthread_mutex_unlock(&mutex_time);

	return NULL;
}

void display_stats(int signal_num){

	free(input_buffer);

	printf("\n---------------STATS---------------\n");
	printf("Number of files requested: \t%d\n", num_threads);
	printf("Number of files found:\t%d\n", num_files_found);
	printf("Access success rate: \t%.3f\n", ((float)num_files_found/(float)num_threads));
	printf("Average time of access:\t%.3f\n)", ((float)total_time/(float)num_files_found));
	exit(0);
}
