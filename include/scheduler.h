#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "filesystem.h"
#include "defs.h"
#include "calls.h"
#define STACK_SIZE 32*1024
#define PROCESS_QTY 1000
#define EXEC_TIME 3
#define MAX_FILES 20
#define MAX_PRIORITY 8
enum state {
	READY = 0, BLOCKED = 1, RUNNING = 2, DEAD = 3
};
typedef struct {
	int fd;
	int apertureMode;
} fileDescriptor;

typedef struct {
	fileDescriptor files[MAX_FILES];
	int pid;
	int waitOn;
	uint esp;
	int parentPID;
	int terminal;
	int free;
	enum state st;
	int priority;
	char * stack;
	char * name;

} process;

typedef struct {
	int edi __attribute__((packed));
	int esi __attribute__((packed));
	int ebp __attribute__((packed));
	int esp __attribute__((packed));
	int ebx __attribute__((packed));
	int edx __attribute__((packed));
	int ecx __attribute__((packed));
	int eax __attribute__((packed));
	int eip __attribute__((packed));
	int cs __attribute__((packed));
	int eflags __attribute__((packed));
	void * retaddr __attribute__((packed));
	int argc __attribute__((packed));
	char** argv __attribute__((packed));
} context;

/* Saves the actual context and puts the process in either the ready array or the blocked array */
void _saveContext(uint esp);

/* Chooses a new process to run and returns the esp of that process */
uint _getNewContext();

/* This function must be called when the main function of a process returns */
void endProcess();

/* Returns the terminal in which the process is running */
int getTTY();

/* Creates shell and returns it's pid*/
int createShell(int(*proc)(int, char**), int argc, char **argv, int tty);

/* Sleeps process a quantity of seconds "secs" */
void sleep(int secs);

#endif
