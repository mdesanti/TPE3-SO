/*
 * processE.c
 *
 *  Created on: Oct 31, 2011
 *      Author: mdesanti90
 */

#include "../../include/iolib.h"
#include "../../include/systemCalls.h"
#include "../../include/test.h"

int procE(int argc, char ** argv) {
	int i = 1;
	while (i++ < 200) {
		printf("Ejecuta proceso E - - %d\n", getMyPID());
		leaveProcessor();
	}

	return 0;
}
