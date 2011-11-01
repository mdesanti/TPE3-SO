/*
 * processD.c
 *
 *  Created on: Oct 21, 2011
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

int procD(int argc, char ** argv) {

	int fd = 1;

	int size;
	char rd[21];

	if ((fd = open("dinux/test2.txt", O_RDONLY)) == -1)
		printf("ERROR EN ABRIR ESCRITURA\n");

	size = Read(fd, rd, 20);
	printf("%s\n", rd);
	close(fd);

	return 1;
}
