#ifndef MALLOC_H_
#define MALLOC_H_
/* Allocs memory */
void* Malloc(int size);

/* Allocs more memory */
void * realloc(void * ptr, int size, int oldSize);

#endif
