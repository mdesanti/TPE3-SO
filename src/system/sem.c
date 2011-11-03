#include "../../include/sem.h"
#include "../../include/scheduler.h"

static semaphore sems[50];
static int firstTime = 1;
static int key = 1;

static void initSems();

static void initSems() {
	int i = 0;
	for (i = 0; i < 50; i++) {
		sems[i].status = -1;
		sems[i].value = 0;
		sems[i].key = -1;
	}
}

int _getSem(int initVal) {
	if (firstTime) {
		initSems();
	}
	int i = 0;
	for (; i < 50; i++) {
		if (sems[i].status == -1) {
			sems[i].value = initVal;
			sems[i].status = 1;
			sems[i].key = key++;
			return sems[i].key;
		}
	}
	return -1;
}

void _up_sem(int semKey, int qty) {
	int i = 0;
	for (; i < 50; i++) {
		if (sems[i].key == semKey && sems[i].key != -1) {
			if (sems[i].status == 1)
				wakePID(sems[i].blocked_pid);
			sems[i].value += qty;
		}
	}
}

void _down_sem(int semKey, int qty) {
	int i = 0;
	for (; i < 50; i++) {
		if (sems[i].key == semKey && sems[i].key != -1) {
			while (qty-- > 0) {
				if (sems[i].value == 0) {
					sems[i].blocked_pid = getPID();
					block();
				}
				sems[i].value--;
			}
		}
	}
}

