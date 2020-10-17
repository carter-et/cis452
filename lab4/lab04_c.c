#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void *do_greeting3(void *arg);
int sharedData = 5;
char val[2] = {'a', 'b'};

int main()
{
	pthread_t thread1, thread2;
        void *result1, *result2;
	int status;
	
	if ((status = pthread_create(&thread1, NULL, do_greeting3, &val[0])) != 0) {
		fprintf(stderr, "thread create error %d: %s\n", status, strerror(status));
		exit(1);
	}
	if ((status = pthread_create(&thread2, NULL, do_greeting3, &val[1])) != 0) {
		fprintf(stderr, "thread create error %d: %s\n", status, strerror(status));
		exit(1);
	}

	printf("Parent sees %d\n", sharedData);
	sharedData++;

	if ((status = pthread_join(thread1, &result1)) != 0) {
		fprintf(stderr, "join error %d:%s\n", status, strerror(status));
		exit(1);
	}
	if ((status = pthread_join (thread2, &result2)) != 0) {
		fprintf(stderr, "join error %d: %s\n", status, strerror(status));
		exit(1);
	}
	printf("Parent sees %d\n", sharedData);
	return 0;
}
void *do_greeting3(void *arg)
{
	char *val_ptr = (char *)arg;
	
	printf("Child receiving %c initially sees %d\n", *val_ptr, sharedData);
	sleep(1);
	sharedData++;
	printf("Child receiving %c now sees %d\n", *val_ptr, sharedData);
	return NULL;
}
