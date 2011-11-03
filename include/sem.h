#ifndef SEM_H_
#define SEM_H_

typedef struct {
	int key;
	int value;
	int blocked_pid;
	int status;
} semaphore;

/* Creates a sem with value "initVal" */
int _getSem(int initVal);

/* Increase semaphore with key "semKey" and quantity "qty" */
void _up_sem(int semKey, int qty);

/* Decrease semaphore with key "semKey" and quantity "qty" */
void _down_sem(int semKey, int qty);

#endif
