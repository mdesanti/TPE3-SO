#ifndef _BUFFER_
#define _BUFFER_

#define MAX_BUFFER 255
#define EOF 0

/*Adds a key to the buffer if it's empty*/
int addKey(unsigned char c);

/*Returns 1 if the buffer is full, else 0*/
int bufferIsFull();

/*Returns 1 if the buffer is empty, else 0*/
int bufferIsEmpty();

/*Returns the buffer's size*/
int bufferSize();

/*Clears the buffer*/
void clearBuffer();

#endif
