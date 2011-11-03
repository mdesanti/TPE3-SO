#include "../../include/iolib.h"
#include "../../include/string.h"
#include "../../include/process.h"
#include "../../include/systemCalls.h"
#include "../../include/filesystem2.h"
#include "../../include/diskata.h"
#include "../../include/malloc.h"
#include "../../include/defs.h"
#include "../../include/AtaDiskAdapter.h"
#include "../../include/users.h"

int procA(int argc, char ** argv) {

	printf("Se quiere crear el directorio hola3\n");
	if (mkdir("hola3") == -1) {
		printf("Esto no deberia pasar \n");
	} else {
		printf("Creado satisfactoriamente\n\n");
	}

	printf("Se quiere crear el archivo myar.txt en el directorio actual\n");
	if (creat("myar.txt", 511) == -1) {
		printf("Error en myar.txt\n");
	} else {
		printf("Creado satisfactoriamente\n\n");
	}

	printf("Se crea el directorio jorge, hijo del directorio actual\n");
	if (mkdir("jorge") == -1) {
		printf("Error en Jorge\n");
	} else {
		printf("Creado satisfactoriamente\n\n");
	}

	printf("Se crea el directorio jorgito, hijo del directorio actual\n");
	if (mkdir("jorgito") == -1) {
		printf("Error en Jorgito\n");
	} else {
		printf("Creado satisfactoriamente\n\n");
	}

	printf("Se crea el directorio chau dentro de jorgito\n");
	if (mkdir("jorgito/chau") == -1) {
		printf("Errot en Chau\n");
	} else {
		printf("Creado satisfactoriamente\n\n");
	}

	printf("Se crea el directorio nestor, hijo del directorio jorgito/chau\n");
	if (mkdir("jorgito/chau/nestor") == -1) {
		printf("Error en Jorge\n");
	} else {
		printf("Creado satisfactoriamente\n\n");
	}

	printf("Se crea el directorio jorgito1, hijo del directorio actual\n");
	if (mkdir("jorgito1") == -1) {
		printf("Error en Jorge\n");
	} else {
		printf("Creado satisfactoriamente\n\n");
	}

	printf("Se crea el directorio jorgito2, hijo del directorio actual\n");
	if (mkdir("jorgito2") == -1) {
		printf("Error en Jorge\n");
	} else {
		printf("Creado satisfactoriamente\n\n");
	}

	printf("Se crea el archivo myar.txt en jorgito/chau/nestor\n");
	if (creat("jorgito/chau/nestor/myar.txt", 511) == -1) {
		printf(" Esto no deberia pasar\n");
	} else {
		printf("Creado satisfactoriamente\n\n");
	}

	printf(
			"Se abre el archivo myarchivo.txt para lectura solamente en jorgito/chau/nestor\n");
	if (open("jorgito/chau/nestor/myarchivo.txt", O_RDONLY) == -1) {
		printf("Error esperado. OK\n");
	} else {
		printf("Creado satisfactoriamente\n\n");
	}
	return 0;

}

