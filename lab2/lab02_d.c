#include <unistd.h>
#include <stdio.h>

int main() {
	     
	//The list of args must end with a NULL
	
	//execl("/usr/bin/calculus", "some random string", "-3", "5", "2045", NULL);
	//execlp("cal", "some random string", "-3", "5", "2045", NULL);
	
	char* run_args[] = {"some random string", "-3", "5", "2045", NULL};
	execvp("cal", run_args);
	perror("After exec()");
	printf ("Just checking\n");
	return 0;
}
