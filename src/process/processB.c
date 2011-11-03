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

int procB(int argc, char ** argv) {

	printf("Este test crea un proceso que imprime en pantalla.\n");
	printf(
			"Llegado un momento, crea un proceso hijo que ejecuta durante un cierto tiempo y lo espera\n");
	printf("Luego este proceso sigue ejecutando hasta que termina\n");
	sleep(5);
	int i = 1;
	int pid = 0;
	while (i++ < 300) {
		printf("Ejecutando proceso B, %d\n", i);
		yield();
		if (i == 100) {
			pid = newProcess(procE, 0, 0, "processB");
			waitProcess(pid);
		}

	}

	return 0;
}
