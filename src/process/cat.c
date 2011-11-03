#include "../../include/filesystem2.h"
#include "../../include/iolib.h"

int cat(int argc, char ** argv) {
	char * filename = argv[0];
	char read;
	int i = 3;
	while (filename[i] == ' ')
		i++;
	int fd = open(&(filename[i]), O_RDONLY);
	int aux;

	if (fd == -1)
		printf("Archivo desconocido\n");

	while ((aux = Read(fd, &read, 1)) != 0)
		printf("%c", read);
	close(fd);

	return 1;
}
