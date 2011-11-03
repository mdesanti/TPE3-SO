#include "../../include/defs.h"
#include "../../include/iolib.h"
#include "../../include/terminal.h"
#include "../../include/calls.h"
#include "../../include/malloc.h"
#include "../../include/kasm.h"
#include "../../include/users.h"
#include "../../include/shell.h"
#include "../../include/filesystem2.h"
#include "../../include/handlers.h"
#include "../../include/systemCalls.h"
#include "../../include/sem.h"

static int log = 0;

void int_80(params * p) {

	if (p->operation == WRITE) {
		service_Write(p->param4, p->param2);
	} else if (p->operation == READ) {
		service_Read(p->param3, p->param2);
	} else if (p->operation == FORMAT_PROC) {
		handleFormat();
	} else if (p->operation == CREATE_PROCESS) {
		handleCreateProcess(p);
	} else if (p->operation == BLOCK_PROC) {
		handleBlockProcess(p);
	} else if (p->operation == GET_PID) {
		handleGetPid(p);
	} else if (p->operation == WAKE_PID) {
		handleWakeProcess(p);
	} else if (p->operation == WAIT_PID) {
		handleWaitProcess(p);
	} else if (p->operation == YIELD) {
		handleYield(p);
	} else if (p->operation == KILL) {
		handleKill(p);
	} else if (p->operation == GET_PROCESS_NAME) {
		handleGetProcessName(p);
	} else if (p->operation == PASS_MODE) {
		handlePasswordMode();
	} else if (p->operation == CANNONICAL_MODE) {
		handleCannonicalMode();
	} else if (p->operation == LAST100) {
		last100Handler(p);
	} else if (p->operation == SLEEP) {
		sleepPHandler(p);
	} else if (p->operation == CLEAR_SCREEN) {
		clearScreenHandler(p);
	} else if (p->operation == UP_SEM) {
		handleUpSem(p);
	} else if (p->operation == DOWN_SEM) {
		handleDownSem(p);
	} else if (p->operation == GET_SEM) {
		handleGetSem(p);
	} else {
		//		It must never get here!
	}
}

void service_Write(char *data, int fileDesc) {
	if (fileDesc == STDOUT) {
		terminalPrint(data);
	}
}

void service_Read(char **data, int fileDesc) {
	if (fileDesc == STDIN) {
		(*data) = (char*) getKey();
	}
}

int checkUserPerms(int perm) {

//	to skip log in
//	return 1;

	char * actualUser = getTTYUser();
	int userPerm = -1;
	int firstTime = 1;

	if (actualUser != NULL) {
		userPerm = getUserPerm(actualUser);
	}

	if (P_WRITE == perm)
		return 1;

	if (userPerm != -1 && userPerm <= perm)
		return 1;

	while (userPerm == -1 || userPerm > perm) {
		if (!firstTime)
			printf(
					"Authentication Failed. It is possible that the user you enterd doesn't have enough permissions to perform the requested operation. Please try again...\n");
		int pid = createProcess(logIn, perm, NULL, "LogIn");
		waitPID(pid);
		actualUser = getTTYUser();
		userPerm = getUserPerm(actualUser);
		setForegroundProcess(getPID());
		firstTime = 0;
	}
	clear_screen();
	printShell();

	return 1;
}

int logIn(int perm, char ** argv) {

	char * user;
	char * pass;
	int userPerm;
	log = 1;
	setForegroundProcess(getPID());
	printf("Username:");
	service_Read(&user, STDIN);

	printf("Password:");
	switchToPasswordMode();
	service_Read(&pass, STDIN);
	switchToCannonicalMode();

	if (validUserAndPass(user, pass)) {
		printf("Valid Username and Password\n");
		setUserToTerm(user);
		userPerm = getUserPerm(user);
		if (userPerm != -1 && userPerm <= perm) {
			log = 0;
			return 1;
		}
	} else {
		printf("Invalid username and/or password\n");
	}

	log = 0;
	return 0;
}

/*
 * All System Calls must call "callInt80" sending as a parameter a pointer to
 * params struct. This struct contains 4 different data types (the most common
 * ones), a void pointer and an integer representing the operation that wants
 * to be done.
 *
 * The define for each operation can be found in defs.h
 */

void callInt80(params * p) {

	_wrapper_int_80(p);

}

/****************************************************************
 ***********From here onwards, System Calls**********************
 ****************************************************************/
int newProcess(int(*processFunct)(int, char**), int argc, char **argv,
		char * name) {

	if (!checkUserPerms(CREATE_PROCESS)) {
		printf("Authentication failed. Process was not created\n");
		return -1;
	}
	params * ans = Malloc(sizeof(params));

	ans->operation = CREATE_PROCESS;
	ans->param1 = (int*) processFunct;
	ans->param2 = argc;
	ans->param3 = argv;
	ans->param4 = name;
	callInt80(ans);

	int ret = (int) ans->ret;
	return ret;

}

int getSem(int initVal) {
	params * ans = Malloc(sizeof(params));

	ans->operation = GET_SEM;
	ans->param2 = initVal;

	callInt80(ans);
	return ans->param2;
}

void up_Sem(int semKey, int qty) {

	params * ans = Malloc(sizeof(params));

	ans->operation = UP_SEM;
	ans->param2 = semKey;
	ans->param1 = &qty;

	callInt80(ans);
}

void down_Sem(int semKey, int qty) {
	params * ans = Malloc(sizeof(params));

	ans->operation = DOWN_SEM;
	ans->param2 = semKey;
	ans->param1 = &qty;

	callInt80(ans);
}

void format() {
	checkUserPerms(P_FORMAT);
	params * ans = Malloc(sizeof(params));

	ans->operation = FORMAT_PROC;
	callInt80(ans);
	return;
}

void blockMe() {

	checkUserPerms(P_BLOCK);

	params * ans = Malloc(sizeof(params));

	ans->operation = BLOCK_PROC;
	callInt80(ans);
	return;

}

int getMyPID() {

	checkUserPerms(P_GET_PID);

	params * ans = Malloc(sizeof(params));

	ans->operation = GET_PID;
	callInt80(ans);

	return (int) ans->ret;
}

void wakeProcess(int pid) {

	checkUserPerms(P_WAKE_PID);

	params * ans = Malloc(sizeof(params));

	ans->operation = WAKE_PID;

	callInt80(ans);

	return;
}

void waitProcess(int pid) {

	checkUserPerms(P_WAIT_PID);

	params * ans = Malloc(sizeof(params));

	ans->operation = WAIT_PID;
	ans->param2 = pid;
	callInt80(ans);

	return;
}

void leaveProcessor() {
	checkUserPerms(P_YIELD);

	params * ans = Malloc(sizeof(params));

	ans->operation = YIELD;
	callInt80(ans);

	return;
}

void signalP(char option, int signal, int pid) {

	checkUserPerms(P_KILL);

	params * ans = Malloc(sizeof(params));

	ans->operation = KILL;
	ans->param1 = &signal;
	ans->param2 = pid;
	ans->param4 = &option;
	callInt80(ans);

	return;
}

char * processName(int pid) {

	checkUserPerms(P_GET_PROCESS_NAME);

	params * ans = Malloc(sizeof(params));

	ans->operation = GET_PROCESS_NAME;
	ans->param2 = pid;
	callInt80(ans);

	return (char *) ans->ret;
}

void passwordMode() {

	checkUserPerms(P_PASS_MODE);

	params * ans = Malloc(sizeof(params));

	ans->operation = PASS_MODE;
	callInt80(ans);

}

void cannonicalMode() {

	checkUserPerms(P_CANNONICAL_MODE);

	params * ans = Malloc(sizeof(params));

	ans->operation = CANNONICAL_MODE;
	callInt80(ans);

}

int * last100() {
	checkUserPerms(P_LAST100);

	params * ans = Malloc(sizeof(params));

	ans->operation = LAST100;
	callInt80(ans);

	return (int *) ans->ret;
}

void sleepP(int secs) {

	checkUserPerms(P_SLEEP);

	params * ans = Malloc(sizeof(params));

	ans->operation = SLEEP;
	ans->param2 = secs;

	callInt80(ans);
}

void clearThisScreen() {

	checkUserPerms(P_CLEAR_SCREEN);

	params * ans = Malloc(sizeof(params));

	ans->operation = CLEAR_SCREEN;
	callInt80(ans);
}

/*******************************************************************
 ********From here onwards, handlers for each system call***********
 *******************************************************************/
void handleCreateProcess(params * in) {
	int pid = createProcess((int(*)(int, char**)) in->param1, in->param2,
			in->param3, in->param4);

	in->ret = (void *) pid;
}

void handleFormat() {
	printf("Cleaning disc, please wait...\n");
	cleanDisk();
//	printf("Dinux will restart in a moment");
//	sleep(2);
//	_restart_PC();

}

void handleBlockProcess(params * p) {
	block();
}

void handleGetPid(params * p) {
	p->ret = (void *) getPID();
//	int i = 0;
//	while(i++<100000000){
//		printf("PID %d\n", (int)p->ret);
//	}

}

void handleWakeProcess(params * p) {
	wakePID((uint) p->param2);
}

void handleWaitProcess(params * p) {
	waitPID((uint) p->param2);
}

void handleYield(params * p) {
	yield();
}

void handleKill(params * p) {
	kill(*(p->param4), *(p->param1), p->param2);
}

void handleGetProcessName(params * p) {
	p->ret = (void *) getProcessName(p->param2);

}

void handlePasswordMode() {
	switchToPasswordMode();
}

void handleCannonicalMode() {
	switchToCannonicalMode();
}

void last100Handler(params * p) {
	p->ret = (void *) getLast100();

}

void sleepPHandler(params * p) {
	sleep(p->param2);

}

void clearScreenHandler(params * p) {
	clear_screen();
}

void handleUpSem(params * p) {
	_up_sem(p->param2, *(p->param1));
}

void handleDownSem(params * p) {
	_down_sem(p->param2, *(p->param1));
}

void handleGetSem(params * p) {
	p->param2 = _getSem(p->param2);
}
