#include "../../include/buffer.h"
#include "../../include/iolib.h"
#include "../../include/string.h"

/* Buffer of keyboard, it's implemented as a circular buffer, but it's
 * only used as a variable, because when the int 09 adds a key, the
 * shell removes it immediately */

static unsigned char buffer[MAX_BUFFER];
static int toRead = 0; // Position to Read
static int toInsert = 0; // Position to Insert
static int bufferRealSize;

int addKey(unsigned char c) {
	if (bufferIsEmpty() == 1) {
		buffer[toInsert++] = c;
		if (toInsert == MAX_BUFFER) {
			toInsert = 0;
		}
		bufferRealSize++;
		return 1;
	} else
		return 0;
}

int bufferIsFull() {
	return (bufferSize() == MAX_BUFFER);
}

int bufferIsEmpty() {
	return bufferRealSize == 0;
}

void clearBuffer() {
	toRead = 0;
	toInsert = 0;
	bufferRealSize = 0;
}

int bufferSize() {
	return bufferRealSize;
}
