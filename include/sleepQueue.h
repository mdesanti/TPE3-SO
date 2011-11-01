/*
 * sleepQueue.h
 *
 *  Created on: Oct 12, 2011
 *      Author: mdesanti90
 */

#ifndef SLEEPQUEUE_H_
#define SLEEPQUEUE_H_

typedef struct {
	int id;
	int time;
	struct node * next;
} node;

typedef struct {
	int elemQty;
	node * first;
} sleepQueue;


sleepQueue * createQueue();
void addElem(sleepQueue * sq, int id, int time);
void reduceUnitOfTime(sleepQueue * sq);
int readyElements(sleepQueue * sq);
int getReadyID(sleepQueue * sq);

#endif /* SLEEPQUEUE_H_ */
