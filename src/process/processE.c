#include "../../include/iolib.h"
#include "../../include/systemCalls.h"
#include "../../include/process.h"

int procE(int argc, char ** argv) {
	int i = 1;
	while (i++ < 200) {
		printf("Ejecuta proceso E - - %d\n", getMyPID());
		leaveProcessor();
	}

	return 0;
}
