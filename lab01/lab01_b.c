/** Ethan Carter **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 16

int main() {
	char *data1, *data2;
	int k;
	do {
		data1 = malloc(SIZE);
		printf ("Please input your EOS username: ");
		scanf ("%s", data1);
		if (! strcmp (data1, "quit")) {
			/*exit is here, but nothing freed :( */
			free(data1);
			break;
		}
		data2 = malloc(SIZE);
		for (k = 0; k < SIZE; k++)
			data2[k] = data1[k];
				free (data1);
		printf ("data2 :%s:\n", data2);
		free(data2);
	} while(1);
	return 0;
}
