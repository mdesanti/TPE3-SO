/*
 * init.c
 *
 *  Created on: Oct 21, 2011
 *      Author: mdesanti90
 */
#include "../../include/filesystem.h"
#include "../../include/terminal.h"
#include "../../include/kasm.h"
#include "../../include/users.h"
#include "../../include/scheduler.h"

int init(int argc, char ** argv) {

	initializeFilesystem();
	initTerms();
	readDefaultUsers();

	return 0;
}
