/*
 * malloc.h
 *
 *  Created on: Sep 22, 2011
 *      Author: mdesanti90
 */

#ifndef MALLOC_H_
#define MALLOC_H_

void* Malloc(int size);

void * realloc(void * ptr, int size, int oldSize);
#endif /* MALLOC_H_ */
