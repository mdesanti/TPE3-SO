#ifndef SYSTEMCALLS_H_
#define SYSTEMCALLS_H_

#include "calls.h"
#include "kasm.h"
#include "defs.h"

/*The logged user must have enough permissions in order to execute any of these calls.
 * Each of these calls must call at the very beginning a function that validates if
 * the user has the permissions to execute.*/

/* Identifies the operation and calls the respective service */
void int_80(params * p);

/* Log in a user */
int logIn(int perm, char ** argv);

/* Analizes which is the the data, and according to it, calls print*/
void service_Write(char *data, int fileDesc);

/* Modifies data with the value in the buffer */
void service_Read(char **data, int fileDesc);

/* Checks if the actual user can make something that needs "perm" of permission */
int checkUserPerms(int perm);

/* Creates a process and inserts it into the ready array */
int newProcess(int(*process)(int, char**), int argc, char **argv, char * name);

/* Inserts a process in the blocked array */
void blockMe();

/* Returns the actual process' PID*/
int getMyPID();

/* Puts the process identified by PID back into the READY list*/
void wakeProcess(int pid);

/* Waits process with pid */
void waitProcess(int pid);

/* Voluntarily leaves processor*/
void leaveProcessor();

/* Sends signal to process identified with pid
 * If "-" is specified in option, the signal is sent to its group*/
void signalP(char option, int signal, int pid);

/* Returns the name of the process identified with pid */
char * processName(int pid);

/* Changes terminal to password mode*/
void passwordMode();
/* Changes terminal to cannonical mode*/
void cannonicalMode();

/* Cleans the disk */
void format();

/* Returns the array with the last processes that use the CPU */
int * last100();

/* Sleeps process a quantity of seconds "secs" */
void sleepP(int secs);

/* Clears the actual terminal screen*/
void clearThisScreen();

/* Increase semaphore with key "semKey" and quantity "qty" */
void up_Sem(int semKey, int qty);

/* Decrease semaphore with key "semKey" and quantity "qty" */
void down_Sem(int semKey, int qty);

/* Creates a sem with value "initVal" */
int getSem(int initVal);

#endif
