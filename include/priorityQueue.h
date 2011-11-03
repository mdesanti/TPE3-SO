#ifndef PRIORITYQUEUE_H_
#define PRIORITYQUEUE_H_

#include "scheduler.h"

typedef struct {

	process * p;
	struct priorityNode * next;
	int priority;
} priorityNode;

typedef struct {

	int elemQty;
	priorityNode * first;
} priorityQueue;

/* Creates priority queue */
priorityQueue * createPriorityQueue();

/* Adds element to the priority queue*/
void insertElem(priorityQueue * pq, process * proc, int priority);

/* Gets the element with higher priority. Returns NULL if he queue is empty */
process * getNext(priorityQueue * pq);

/* Returns 1 if the queue is empty */
int queueIsEmpty(priorityQueue * pq);

/* Removes from the queue the proces identified by pid */
process * removePID(priorityQueue * pq, int pid);

#endif
