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

/* Creates an returns an sleep queue */
sleepQueue * createQueue();

/* Adds process with pid "id" to the sleep queue */
void addElem(sleepQueue * sq, int id, int time);

/* Reduces one unit of time to the queue*/
void reduceUnitOfTime(sleepQueue * sq);

/* Returns 1 if there are ready processes in the queue*/
int readyElements(sleepQueue * sq);

/* Returns -1 if there are no ready processes, else returns the first process ready */
int getReadyID(sleepQueue * sq);

#endif
