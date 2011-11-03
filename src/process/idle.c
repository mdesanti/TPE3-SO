#include "../../include/iolib.h"
#include "../../include/kasm.h"

int idle(int argc, char ** argv) {
	while (1) {
		_Halt();
	}

	return 0;
}
