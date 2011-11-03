#include "../../include/process.h"
#include "../../include/iolib.h"
#include "../../include/systemCalls.h"

int procF(int argc, char ** argv) {
	printf(
			"Este test crea dos processos y los espera. Una vez que ambos finalizan, termina\n");
	sleepP(4);

	int pid1;
	printf("Creo al proceso B y al C y los espero\n");
	pid1 = newProcess(procB, 0, 0, "processB");
	waitProcess(pid1);
	printf("Terminaron ambos procesos\n");
	return 0;
}
