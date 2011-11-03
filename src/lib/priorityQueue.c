#include "../../include/priorityQueue.h"
#include "../../include/defs.h"
#include "../../include/malloc.h"

static priorityNode * insertElemR(priorityQueue * pq, process * proc,
		int priority, priorityNode * actual);

priorityQueue * createPriorityQueue() {

	priorityQueue * pq = (priorityQueue *) Malloc(sizeof(priorityQueue));
	pq->elemQty = 0;
	pq->first = NULL;

	return pq;
}

void insertElem(priorityQueue * pq, process * proc, int priority) {
	if (pq->first == NULL) {
		priorityNode * pn = (priorityNode*) Malloc(sizeof(priorityNode));
		pn->p = proc;
		pn->priority = priority;
		pn->next = NULL;
		pq->first = pn;
	} else {
		pq->first = (priorityNode*) insertElemR(pq, proc, priority, pq->first);
	}
	pq->elemQty++;
}

priorityNode * insertElemR(priorityQueue * pq, process * proc, int priority,
		priorityNode * actual) {

	//Process are inserted before the next process with less priority
	if (actual == NULL || actual->priority > priority) {
		priorityNode * pn = (priorityNode*) Malloc(sizeof(priorityNode));
		pn->p = proc;
		pn->priority = priority;
		pn->next = (struct priorityNode*) actual;
		return pn;
	} else {
		actual->next = (struct priorityNode*) insertElemR(pq, proc, priority,
				(priorityNode*) actual->next);
		return actual;
	}
}

process * getNext(priorityQueue * pq) {
	if (pq->first == NULL) {
		return NULL;
	}
	process * p = pq->first->p;
	pq->first = (priorityNode*) pq->first->next;
	return p;
}

process * removePID(priorityQueue * pq, int pid) {
	priorityNode * actual = pq->first;
	priorityNode * previous = NULL;

	if (pq->first->p->pid == pid) {
		actual = pq->first;
		pq->first = (priorityNode *) pq->first->next;
		return actual->p;
	}

	while (actual != NULL && actual->p->pid != pid) {
		previous = actual;
		actual = (priorityNode *) actual->next;
	}

	if (actual == NULL
		)
		return NULL;

	previous->next = actual->next;
	return actual->p;
}

int queueIsEmpty(priorityQueue * pq) {
	return pq->first == NULL;
}
