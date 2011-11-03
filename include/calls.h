#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include "defs.h"

/*The logged user must have enough permissions in order to execute any of these calls.
 * Each of these calls must call at the very beginning a function that validates if
 * the user has the permissions to execute.*/

/* Creates a process and inserts it into the ready array */
int createProcess(int(*process)(int, char**), int argc, char **argv,
		char * name);

/* Writes a string array of "length" in the fileDesc, if
 * fileDesc=STDOUT, writes it in the screen*///ready
void write(char * string, int length, int fileDes);

/* Returns the value in the keyboard buffer */
char * read(int fileDesc);

/* Inserts a process in the blocked array */
void block();

/* Returns the actual process' PID*/
uint getPID();

/* Puts the process identified by PID back into the READY list*/
void wakePID(uint pid);

/*Blocks a process until the process with PID finishes */
void waitPID(int pid);

/* Voluntarily leaves processor*/
void yield();

/* Sends signal to process identified with pid */
void kill(char option, int signal, int pid);

/* Returns the name of the process identified with PID */
char * getProcessName(int PID);

/* Switchs the terminal to password mode */
void switchToPasswordMode();

/* Switchs the terminal to cannonical mode */
void switchToCannonicalMode();

/* Returns the array containing the last 100 processes that use the CPU*/
int * getLast100();

/* Sleeps a process a quantity "secs" of seconds */
void sleep(int secs);

/* Clears all the text in the screen */
void clear_screen();

/* Returns an index in the files table */
int getFD(int inode, int flags);

/* Returns the position of a file descriptor in the array of files in a process */
int getPos(int fd);

/* Deletes file descriptor from process */
void deleteFDFromProc(int fd);

/* Returns 1 if the file aperture mode is for write */
int openForWrite(int pos);

#endif
