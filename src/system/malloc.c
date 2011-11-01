char heap_space[1024 * 1024 * 100];
int offset;

// Roughly allocs some memory
void * Malloc(int size) {
	void* ret = 0;
	ret = heap_space + offset;
	offset += size;
	return ret;
}

// Roughly callocs some memory
void * calloc(int size, int cols) {
	char* ret = (char*) Malloc(size * cols);
	int i = 0;
	for (; i < size; i++)
		ret[i] = 0;
	return ret;
}

// Roughly reallocs some memory
void * realloc(void * ptr, int size, int oldSize) {

	int i = 0;
	char* ret = (char*) Malloc(size);

	for(i = 0; i < oldSize; i++) {
		ret[i] = ((char *) ptr)[i];
	}
	return ret;
}

// The cake is a lie, but the cake follows the standard implementation.
void free(void* ptr) {
	return;
}
