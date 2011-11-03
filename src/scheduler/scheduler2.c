#include "../../include/scheduler.h"
#include "../../include/calls.h"
#include "../../include/iolib.h"
#include "../../include/malloc.h"
#include "../../include/idle.h"
#include "../../include/terminal.h"
#include "../../include/defs.h"
#include "../../include/kasm.h"
#include "../../include/string.h"
#include "../../include/sleepQueue.h"
#include "../../include/priorityQueue.h"

/* Implementation of scheduler with priorities */

/* Process array */
static process p[PROCESS_QTY];

/* PID indicator*/
static uint pid = 1;

/* IDLE process */
static process idleP;

/* IDLE process created*/
static int idleFlag = 0;

/* Time Slot */
static uint time = EXEC_TIME;

/* Flag that tells when it is the first time a context is saved */
static int firstTime = 1;

/* Stores the actual process */
static process * actual = NULL;

/* Stores the processes ready to execute*/
static priorityQueue * ready;

/* Stores the processes ready to execute*/
static process ** blocked;

/* Stores all blocked processes*/
static int last100[100];

/* Points to the next process to leave the the last100 array */
static int tNext = 0;

/* Stores all finished processes
 * Might not be used */
static process ** finished;

/* Points to the next place to put a dead process*/
static int fNext = 0;

/* Sleeping process queue*/
static sleepQueue * sq;

/* Indicates whether a call to int08 was made by the PIC or called by the kernel */
static int artificial = 0;
/* Creates process idle */
void createIDLE(int(*process)(int, char**), int argc, char **argv);

/* Inserts a new process into the array of blocked processes */
static void insertIntoBlocked(process * p);

/* Wakes up processes that were blocked waiting for process identified by pid */
static void wakeProcessWaitingFor(int pid);

void _saveContext(uint esp) {

	//If firstTime no context must be saved
	if (firstTime) {
		if (!idleFlag) {
			createIDLE(idle, 0, 0);
			idleFlag = 1;
			time = 0;
		}
		return;
	}

	if (!artificial) {
		reduceUnitOfTime(sq);
		while (readyElements(sq)) {
			int pid = getReadyID(sq);
			wakePID(pid);
		}
	}
	artificial = 0;

	if (time != 0 && actual->st == RUNNING) {
		actual->esp = esp;
		return;
	}

	actual->esp = esp;
	time = 0;
	if (actual->pid == 0) {

	} else if (actual->st == RUNNING) {
		actual->st = READY;
		actual->waitOn = -1;
		insertElem(ready, actual, actual->priority);
	} else if (actual->st == BLOCKED) {
		insertIntoBlocked(actual);
	} else {
		finished[fNext] = actual;
		actual->waitOn = -1;
		fNext++;
	}
	actual = NULL;

}

uint _getNewContext() {

	if (time-- != 0 && !firstTime) {
		return actual->esp;
	}
	firstTime = 0;
	if (queueIsEmpty(ready)) {
		actual = &idleP;
	} else {
		actual = getNext(ready);
		actual->st = RUNNING;
	}

	time = EXEC_TIME;

	last100[tNext++] = actual->pid;
	if (tNext == 99)
		tNext = 0;
	return actual->esp;
}

void insertIntoBlocked(process * p) {
	int i = 0;
	for (i = 0; i < PROCESS_QTY; i++) {
		if (blocked[i] == NULL) {
			blocked[i] = p;
			return;
		}
	}
}

void block() {
	actual->st = BLOCKED;
	artificial = 1;
	_call_int_08();
	return;
}

void yield() {
	time = 0;
	artificial = 1;
	_call_int_08();
}

void waitPID(int PID) {
	int i = 0, flag = 1;
	//Checks if the process to wait for exists
	for (i = 0; i < PROCESS_QTY && flag; i++) {
		if (p[i].pid == PID && p[i].free == 0 && p[i].st != DEAD
				&& actual->waitOn == -1) {
			flag = 0;
			actual->waitOn = PID;
			block();
		}
	}
}

void wakePID(uint pid) {
	int i = 0, flag = 1;
	for (i = 0; i < PROCESS_QTY && flag; i++) {
		if (blocked[i] != NULL && blocked[i]->pid == pid && blocked[i]->st
				== BLOCKED) {
			blocked[i]->st = READY;
			insertElem(ready, blocked[i], blocked[i]->priority);
			blocked[i] = NULL;
			flag = 0;
		}
	}
}

process * getFreeProcess() {
	int i = 0;

	for (i = 0; i < PROCESS_QTY; i++) {
		if (p[i].free) {
			p[i].free = 0;
			return &p[i];
		}
	}
	return NULL;
}

int setPriority(int parentPriority, char * name) {
	if (strcmp(name, "Init"))
		return 0;
	if (strcmp(name, "Shell"))
		return 1;
	if (parentPriority == MAX_PRIORITY)
		return MAX_PRIORITY;
	else
		return parentPriority + 1;
}

int createProcess(int(*funct)(int, char**), int argc, char **argv, char * name) {

	if (!idleFlag) {
		createIDLE(idle, 0, 0);
		idleFlag = 1;
	}

	context * cxt;
	process * insert = getFreeProcess();
	int i = 0;
	for (i = 0; i < 3; i++) {
		insert->files[i].fd = -i - 2;
	}
	for (; i < MAX_FILES; i++) {
		insert->files[i].fd = -1;
	}
	insert->name = name;
	insert->pid = pid++;
	insert->waitOn = -1;
	insert->st = READY;
	insert->stack = Malloc(STACK_SIZE);
	insert->esp = (int) (insert->stack + STACK_SIZE - 1);
	insert->esp -= sizeof(context);
	insert->parentPID = getPID();
	insert->terminal = actual->terminal;
	insert->priority = setPriority(actual->priority, name);
	cxt = (context*) insert->esp;
	insertElem(ready, insert, 3);
	cxt->edi = 0;
	cxt->esi = 0;
	cxt->ebp = 0;
	cxt->esp = 0;
	cxt->ebx = 0;
	cxt->edx = 0;
	cxt->ecx = 0;
	cxt->eax = 0;
	cxt->eip = (int) funct;
	cxt->cs = 0x08;
	cxt->eflags = 0x202;
	cxt->retaddr = endProcess;
	cxt->argc = argc;
	cxt->argv = argv;

	return insert->pid;
}

void initArrays() {
	int i = 0;
	//	ready = (process**) Malloc(PROCESS_QTY * sizeof(process*));
	ready = createPriorityQueue();
	sq = createQueue();
	blocked = (process**) Malloc(PROCESS_QTY * sizeof(process*));
	finished = (process**) Malloc(PROCESS_QTY * sizeof(process*));

	for (i = 0; i < PROCESS_QTY; i++) {
		p[i].free = 1;
		blocked[i] = NULL;
		finished[i] = NULL;
		if (i < 100)
			last100[i] = -1;
		;
	}
}

void createIDLE(int(*process)(int, char**), int argc, char **argv) {

	initArrays();
	context * cxt;
	//	idleP = &p[0];
	idleP.pid = 0;
	idleP.name = "IDLE";
	idleP.st = READY;
	idleP.stack = Malloc(STACK_SIZE);
	idleP.esp = (int) (idleP.stack + STACK_SIZE - 1);
	idleP.esp -= sizeof(context);
	idleP.parentPID = -1;
	idleP.priority = MAX_PRIORITY;
	cxt = (context*) idleP.esp;
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
	cxt->eflags = 0x202;
	cxt->retaddr = endProcess;
	cxt->argc = 0;
	cxt->argv = 0;
	actual = NULL;
}

void endProcess() {

	_Cli();
	int i = 0;
	actual->st = DEAD;
	time = 0;
	for (i = 0; i < PROCESS_QTY; i++) {
		if (blocked[i] != NULL && blocked[i]->waitOn == actual->pid) {
			process * p = blocked[i];
			//			ready[rInsert]->esp = blocked[i]->esp;
			p->st = READY;
			p->waitOn = -1;
			insertElem(ready, p, p->priority);
			blocked[i] = NULL;
		}
	}
	_call_int_08();
}

uint getPID() {
	if (actual != NULL)
		return actual->pid;
	return -1;
}

int getTTY() {
	if (actual != NULL)
		return actual->terminal;
	return getActualTTY();
}

int createShell(int(*proc)(int, char**), int argc, char **argv, int tty) {
	int pid = createProcess(proc, argc, argv, "Shell");
	int i = 0, flag = 1;
	for (i = 0; i < PROCESS_QTY && flag; i++) {
		if (!p[i].free && p[i].pid == pid) {
			p[i].terminal = tty;
			flag = 0;
		}
	}
	return pid;
}

int getFD(int inode, int flags) {
	int i = 3;
	for (i = 3; i < MAX_FILES; i++) {
		if (actual->files[i].fd == -1) {
			actual->files[i].fd = inode;
			if (flags & O_RDWR)
				actual->files[i].apertureMode = O_RDWR;
			if (flags & O_RDONLY)
				actual->files[i].apertureMode = O_RDONLY;
			if (flags & O_WRONLY)
				actual->files[i].apertureMode = O_WRONLY;
			return i;
		}
	}

	return -1;
}

int getPos(int fd) {
	int ret = -1;
	if (actual->files[fd].fd != -1 && fd < MAX_FILES)
		ret = actual->files[fd].fd;
	return ret;
}

void kill(char option, int signal, int pid) {
	int i = 0;

	for (i = 0; i < PROCESS_QTY; i++) {
		if (p[i].pid == pid && !strcmp(p[i].name, "Shell")) {
			p[i].st = DEAD;
			time = 0;
			wakeProcessWaitingFor(p[i].pid);
			removePID(ready, pid);
		}
		if (p[i].parentPID == pid && !p[i].free) {
			kill(option, signal, p[i].pid);
		}
	}
}

void wakeProcessWaitingFor(int pid) {
	int i = 0;
	for (i = 0; i < PROCESS_QTY; i++) {
		if (blocked[i]->waitOn == pid) {
			blocked[i]->st = READY;
			blocked[i]->waitOn = -1;
			insertElem(ready, blocked[i], blocked[i]->priority);
		}
	}
}

int * getLast100() {
	return last100;
}

void sleep(int secs) {
	int ticks = secs * TICKS_PER_SEC;

	addElem(sq, getPID(), ticks);
	block();
}

void deleteFDFromProc(int fd) {
	actual->files[fd].fd = -1;
}

int openForWrite(int pos) {
	if ((actual->files[pos].apertureMode & O_WRONLY)
			|| (actual->files[pos].apertureMode & O_RDWR))
		return 1;
	return 0;
}

process * getProcessByPID(int pid) {
	int i = 0;
	if (pid == 0)
		return &idleP;
	for (i = 0; i < PROCESS_QTY; i++) {
		if (p[i].pid == pid && !p[i].free)
			return &p[i];
	}
	return NULL;
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

