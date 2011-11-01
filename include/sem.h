/*
 * sem.h
 *
 *  Created on: Oct 30, 2011
 *      Author: mdesanti90
 */

#ifndef SEM_H_
#define SEM_H_

typedef struct {
	int key;
	int value;
	int blocked_pid;
	int status;
} semaphore;

int _getSem(int initVal);

void _up_sem(int semKey, int qty);

void _down_sem(int semKey, int qty);


#endif /* SEM_H_ */
