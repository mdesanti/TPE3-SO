/*
 * priorityQueue.h
 *
 *  Created on: Oct 14, 2011
 *      Author: mdesanti90
 */

#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include "scheduler.h"

typedef struct {

	process * p;
	struct priorityNode * next;
	int priority;
}priorityNode;

typedef struct {

	int elemQty;
	priorityNode * first;
}priorityQueue;

priorityQueue * createPriorityQueue();

void insertElem(priorityQueue * pq, process * proc, int priority);

process * getNext(priorityQueue * pq);

int queueIsEmpty(priorityQueue * pq);

process * removePID(priorityQueue * pq, int pid);

#endif /* PRIORITYQUEUE_H_ */
