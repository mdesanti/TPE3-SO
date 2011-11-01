/*
 * syscalls.h
 *
 *  Created on: Oct 14, 2011
 *      Author: mdesanti90
 */

#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include "defs.h"

/*The logged user must have enough permissions in order to execute any of these calls.
 * Each of these calls must call at the very beginning a function that validates if
 * the user has the permissions to execute.*/

/* Creates a process and inserts it into the ready array *///ready
int createProcess(int(*process)(int, char**), int argc, char **argv,
		char * name);

/* Writes a string array of "length" in the fileDesc, if
 * fileDesc=STDOUT, writes it in the screen*///ready
void write(char * string, int length, int fileDes);

/* Returns the value in the keyboard buffer *///ready
char * read(int fileDesc);

/* Inserts a process in the blocked array *///ready
void block();

/* Returns the actual process' PID*///ready
uint getPID();

/* Puts the process identified by PID back into the READY list*///ready
void wakePID(uint pid);

void waitPID(int pid);

/* Voluntarily leaves processor*///ready
void yield();

/* Sends signal to process identified with pid
 * If "-" is specified in option, the signal is sent to its group*///ready
void kill(char option, int signal, int pid);

//ready
char * getProcessName(int PID);

//ready
void switchToPasswordMode();

//ready
void switchToCannonicalMode();

int * getLast100();

void sleep(int secs);

void clear_screen();

int getFD(int inode, int flags);
int getPos(int fd);
void deleteFDFromProc(int fd);
int openForWrite(int pos);

#endif /* SYSCALLS_H_ */
