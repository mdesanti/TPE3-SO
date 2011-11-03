#include "../../include/sleepQueue.h"
#include "../../include/defs.h"
#include "../../include/malloc.h"

static node * addElemR(sleepQueue * sq, int id, int time, node * actual);

sleepQueue * createQueue() {
	sleepQueue * sq = (sleepQueue *) Malloc(sizeof(sleepQueue));

	sq->elemQty = 0;
	sq->first = NULL;
	return sq;
}

void addElem(sleepQueue * sq, int id, int time) {

	if (sq->first == NULL) {
		sq->first = (node *) Malloc(sizeof(node));
		sq->first->time = time;
		sq->first->id = id;
		sq->first->next = NULL;
	} else
		sq->first = (node*) addElemR(sq, id, time, sq->first);
	sq->elemQty++;
}

node * addElemR(sleepQueue * sq, int id, int time, node * actual) {

	if (actual == NULL || time < actual->time) {
		node * n = (node*) Malloc(sizeof(node));
		if (actual != NULL)
			actual->time -= time;
		n->time = time;
		n->id = id;
		n->next = (struct node*) actual;
		return n;
	} else {
		actual->next = (struct node*) addElemR(sq, id, time - actual->time,
				(node*) actual->next);
		return actual;
	}
}

void reduceUnitOfTime(sleepQueue * sq) {
	if (sq != NULL && sq->first != NULL) {
		sq->first->time -= 1;
	}
}

int readyElements(sleepQueue * sq) {
	if (sq->first != NULL && sq->first->time == 0)
		return 1;
	return 0;
}

int getReadyID(sleepQueue * sq) {
	if (sq->first->time != 0) {
		return -1;
	}
	int ret = sq->first->id;
	sq->first = (node*) sq->first->next;
	sq->elemQty--;
	return ret;
}
