/*
 * procC.c
 *
 *  Created on: Sep 22, 2011
 *      Author: mdesanti90
 */

#include "../../include/iolib.h"
#include "../../include/string.h"
#include "../../include/test.h"
#include "../../include/systemCalls.h"
#include "../../include/filesystem2.h"
#include "../../include/diskata.h"
#include "../../include/malloc.h"
#include "../../include/defs.h"
#include "../../include/AtaDiskAdapter.h"
#include "../../include/users.h"

int procC(int argc, char ** argv) {
	int fd;

	int k, pid;
	int size;
	char * wr = (char*) Malloc(21 * sizeof(char));

	if ((k = mkfifo("dinux/test2.txt", 511)) == -1)
		printf("ERROR CREAR FIFO\n");
	if ((fd = open("dinux/test2.txt", O_WRONLY)) == -1)
		printf("ERROR EN ABRIR ESCRITURA\n");

	for (k = 0; k < 20; k++) {
		wr[k] = 'A';
	}
	wr[k-1] = 'B';
	wr[k] = '\0';
	size = Write(fd, wr, 20);

	pid = createProcess(procD, 0, 0, "processD");
	waitProcess(pid);
	close(fd);
	return 1;
}
