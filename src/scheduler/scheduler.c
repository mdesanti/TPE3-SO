#include "../../include/scheduler.h"
#include "../../include/calls.h"
#include "../../include/iolib.h"
#include "../../include/malloc.h"
#include "../../include/idle.h"
#include "../../include/terminal.h"
#include "../../include/sleepQueue.h"
#include "../../include/defs.h"
#include "../../include/kasm.h"
#include "../../include/string.h"
#include "../../include/top.h"

/* Implementation of scheduler with round robin */

/* PID indicator*/
static uint pid = 1;

/* IDLE process created*/
static int idleFlag = 0;

/* Time Slot */
static uint time = EXEC_TIME;

/* Flag that tells when it is the first time a context is saved */
static int firstTime = 1;

/* Stores the actual process */
static process * actual;

/* Stores the processes ready to execute*/
static process all[PROCESS_QTY];

/* Points to the next process to execute*/
static int rNext = 0;

/* Points to the next free space to insert a ready process*/
static int rInsert = 0;

/* Stores all blocked processes*/
static int last100[100] = { -1 };

/* Points to the next process to leave the the last100 array */
static int tNext = 0;

/* Stores all finished processes
 * Might not be used */
//static process finished[PROCESS_QTY];
/* Indicates whether a call to int08 was made by the PIC or called by the kernel */
static int artificial = 0;

/* Sleeping process queue*/
static sleepQueue * sq;

/* Creates process idle */
static void createIDLE(int(*process)(int, char**), int argc, char **argv);

/* Returns process with identified by pid */
static process * getProcessByPID(int pid);

/* Wakes up processes that were blocked waiting for process identified by pid */
static void wakeProcessWaitingFor(int pid);

/* Initialize false context */
static void createFalseContext(context * cxt, int(*process)(int, char**),
		int argc, char **argv);

void _saveContext(uint esp) {

	//If first no context must be saved
	if (firstTime) {
		sq = (sleepQueue*) createQueue();
		if (!idleFlag) {
			idleFlag = 0;
			createIDLE(idle, 0, 0);
			time = 0;
		}
		return;
	}
	process * save = actual;

	if (!artificial) {
		reduceUnitOfTime(sq);
		while (readyElements(sq)) {
			int pid = getReadyID(sq);
			wakePID(pid);
		}
	}
	artificial = 0;

	if (time != 0 && actual->st == RUNNING) {
		save->esp = esp;
		return;
	}
	time = 0;
	save->esp = esp;
	if (actual->st == RUNNING) {
		save->st = READY;
		save->waitOn = -1;
	} else if (actual->st == BLOCKED) {
		save->st = BLOCKED;
	} else {
		save->st = DEAD;
		actual->waitOn = -1;
	}
}

uint _getNewContext() {
	int aux = rNext;
	//this should never happen
	if (rNext == rInsert)
		return all[0].esp;

	if (time-- != 0 && !firstTime) {
		return actual->esp;
	}
	firstTime = 0;
	//looks for the first unblocked process. If there are no process available, IDLE is chosen
	int flag = 1;
	while (flag) {
		if (all[rNext].st == READY && rNext != 0 && rNext < rInsert) {
			flag = 0;
		} else {
			rNext++;
			if (rNext == rInsert) {
				rNext = 0;
			}
			if (rNext == aux) {
				rNext = 0;
				flag = 0;
			}
		}
	}
	time = EXEC_TIME;
	actual = &(all[rNext]);
	actual->st = RUNNING;

	rNext++;
	if (rNext == rInsert)
		rNext = 0;

	last100[tNext++] = actual->pid;
	if (tNext == 99)
		tNext = 0;
	return actual->esp;
}

void block() {
	all[getPID()].st = BLOCKED;
	_call_int_08();
	artificial = 1;
	return;
}

void yield() {
	time = 0;
	artificial = 1;
	_call_int_08();
}

void wakePID(uint pid) {
	if (all[pid].st != DEAD)
		all[pid].st = READY;
}

process * getProcessByPID(int pid) {
	int i = 0;
	for (i = 0; i < rInsert; i++) {
		if (all[i].pid == pid)
			return &all[i];
	}
	return 0;
}

int createProcess(int(*process)(int, char**), int argc, char **argv,
		char * name) {
	int i = 0;

	if (!idleFlag) {
		idleFlag = 1;
		createIDLE(idle, 0, 0);
	}
	for (i = 0; i < 3; i++) {
		all[rInsert].files[i].fd = -i - 2;
	}
	for (; i < MAX_FILES; i++) {
		all[rInsert].files[i].fd = -1;
	}
	context * cxt;
	all[rInsert].pid = pid;
	all[rInsert].name = name;
	all[rInsert].waitOn = -1;
	all[rInsert].st = READY;
	all[rInsert].stack = Malloc(STACK_SIZE);
	all[rInsert].esp = (int) all[rInsert].stack + STACK_SIZE - 1;
	all[rInsert].esp -= sizeof(context);
	all[rInsert].parentPID = getPID();
	all[rInsert].terminal = actual->terminal;
	cxt = (context*) all[rInsert].esp;
	rInsert++;
	createFalseContext(cxt, process, argc, argv);

	pid++;
	return all[rInsert - 1].pid;
}

void createIDLE(int(*process)(int, char**), int argc, char **argv) {
	context * cxt;
	all[0].pid = 0;
	all[0].name = "IDLE";
	all[0].st = READY;
	all[0].stack = Malloc(STACK_SIZE);
	all[0].esp = (int) all[rInsert].stack + STACK_SIZE - 1;
	all[0].esp -= sizeof(context);
	all[rInsert].parentPID = -1;
	all[rInsert].terminal = 0;
	cxt = (context*) all[rInsert].esp;
	rInsert++;
	createFalseContext(cxt, process, argc, argv);
}

void createFalseContext(context * cxt, int(*process)(int, char**), int argc,
		char **argv) {
	cxt->edi = 0;
	cxt->esi = 0;
	cxt->ebp = 0;
	cxt->esp = 0;
	cxt->ebx = 0;
	cxt->edx = 0;
	cxt->ecx = 0;
	cxt->eax = 0;
	cxt->eip = (int) process;
	cxt->cs = 0x08;
	cxt->eflags = 0x200;
	cxt->retaddr = endProcess;
	cxt->argc = 0;
	cxt->argv = 0;

	return;
}

void endProcess() {

	all[getPID()].st = DEAD;
	time = 0;
	wakeProcessWaitingFor(getPID());
	_call_int_08();
}

void kill(char option, int signal, int pid) {
	int i = 0;
	for (i = rNext; i < rInsert; i++) {
		if (all[i].pid == pid || all[i].parentPID == pid) {
			if (strcmp(all[i].name, "Shell"))
				return;
			all[i].st = DEAD;
			time = 0;
			wakeProcessWaitingFor(all[i].pid);
		}
	}
}

void wakeProcessWaitingFor(int pid) {
	int i = 0;
	for (i = 1; i < rInsert; i++) {
		if (all[i].waitOn == pid) {
			all[i].st = READY;
			all[i].waitOn = -1;
		}
	}
}

uint getPID() {
	return actual->pid;
}

void waitPID(int PID) {
	int i = 0, flag = 1;
	for (i = 0; i < rInsert && flag; i++) {
		if (all[i].pid == PID && all[i].st != DEAD) {
			flag = 0;
			all[getPID()].waitOn = PID;
			block();
		}
	}
}

int createShell(int(*proc)(int, char**), int argc, char **argv, int tty) {
	int pid = createProcess(proc, argc, argv, "Shell");
	all[pid].terminal = tty;
	return pid;
}

void sleep(int secs) {
	int ticks = secs * TICKS_PER_SEC;

	addElem(sq, getPID(), ticks);
	block();
}

int getTTY() {
	return actual->terminal;
}

int * getLast100() {
	return last100;
}

int getFD(int inode, int flags) {
	int i = 3;
	for (i = 3; i < MAX_FILES; i++) {
		if (actual->files[i].fd == -1) {
			actual->files[i].fd = inode;
			if (flags & O_RDWR) {
				actual->files[i].apertureMode = O_RDWR;
			}
			if (flags & O_RDONLY) {
				actual->files[i].apertureMode = O_RDONLY;
			}
			if (flags & O_WRONLY) {
				actual->files[i].apertureMode = O_WRONLY;
			}
			return i;
		}
	}

	return -1;
}

int getPos(int fd) {
	int ret = -1;
	if (actual->files[fd].fd != -1 && fd < MAX_FILES
		)
		ret = actual->files[fd].fd;
	return ret;
}
void deleteFDFromProc(int fd) {
	actual->files[fd].fd = -1;
	actual->files[fd].apertureMode = 0;
}

int openForWrite(int pos) {
	if ((actual->files[pos].apertureMode & O_WRONLY)
			|| (actual->files[pos].apertureMode & O_RDWR))
		return 1;
	return 0;
}

char * getProcessName(int PID) {
	return (getProcessByPID(PID))->name;
}

int top(int argc, char ** argv) {
	while (1) {

		clear_screen();
		int percentage[PROCESS_QTY] = { 0 };
		int i = 0;

		for (i = 0; i < 100; i++) {
			if (last100[i] != -1)
				percentage[last100[i]]++;
		}

		printf("TOP V1.0: PID and percentage of use of the CPU\n");
		printf("Percentage\t \t  PID \t \t Name\n");

		for (i = 0; i < PROCESS_QTY; i++) {
			if (percentage[i] != 0) {
				printf("%d\t \t \t \t %d \t \t \t %s\n", percentage[i], i,
						getProcessName(i));
			}
		}
		sleep(1);
	}
	return 0;
}
