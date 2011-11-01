/*
 * systemCalls.h
 *
 *  Created on: Oct 25, 2011
 *      Author: mdesanti90
 */

#ifndef SYSTEMCALLS_H_
#define SYSTEMCALLS_H_

/*The logged user must have enough permissions in order to execute any of these calls.
 * Each of these calls must call at the very beginning a function that validates if
 * the user has the permissions to execute.*/

int checkUserPerms(int perm);

int logging();

/* Creates a process and inserts it into the ready array */
int newProcess(int(*process)(int, char**), int argc, char **argv, char * name);

/* Inserts a process in the blocked array */
void blockMe();

/* Returns the actual process' PID*/
int getMyPID();

/* Puts the process identified by PID back into the READY list*/
void wakeProcess(int pid) ;

void waitProcess(int pid);

/* Voluntarily leaves processor*/
void leaveProcessor();

/* Sends signal to process identified with pid
 * If "-" is specified in option, the signal is sent to its group*/
void signalP(char option, int signal, int pid);

char * processName(int pid);

void passwordMode();

void cannonicalMode();

void format();

int * last100();

void sleepP(int secs);

void clearThisScreen();

void up_Sem(int semKey, int qty);

void down_Sem(int semKey, int qty);

int getSem();

#endif /* SYSTEMCALLS_H_ */
