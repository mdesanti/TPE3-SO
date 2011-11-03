#define SIZE 1024 * 1024 * 5
#include "../../include/filesystem2.h"
#include "../../include/iolib.h"
#include "../../include/process.h"

static char wr[SIZE];
static char rd[SIZE];

int procG(int argc, char ** argv) {
	int i = 0;
	int fd, aux;
	for (; i < SIZE; i++) {
		wr[i] = 'A';
		rd[SIZE] = 'B';
	}

	if ((fd = open("test1.txt", O_CREATE)) == -1)
		printf("Error\n");

	aux = Write(fd, wr, SIZE);
	close(fd);

	if ((fd = open("test1.txt", O_RDWR)) == -1)
		printf("Error\n");

	aux = Read(fd, rd, SIZE);

	for(i = 0; i < SIZE; i++) {
		if(wr[i] != rd[i])
			printf("Mal\n");
	}

	close(fd);
	return 1;
}
