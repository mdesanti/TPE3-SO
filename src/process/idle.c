/*
 * idle.c
 *
 *  Created on: Sep 24, 2011
 *      Author: mdesanti90
 */

#include "../../include/iolib.h"
#include "../../include/kasm.h"

int idle(int argc, char ** argv) {
	while(1) {
//		printf(".");
		_Halt();
	}

	return 0;
}
