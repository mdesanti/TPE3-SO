#include "../../include/iolib.h"
#include "../../include/commands.h"
#include "../../include/buffer.h"
#include "../../include/defs.h"
#include "../../include/scheduler.h"
#include "../../include/idle.h"
#include "../../include/terminal.h"
#include "../../include/calls.h"
#include "../../include/lib.h"
#include "../../include/shell.h"
#include "../../include/prints.h"
#include "../../include/kasm.h"
#include "../../include/malloc.h"
#include "../../include/kc.h"
#include "../../include/systemCalls.h"

#define NON_PRINT -1

static term terminals[TERM_QTY];
static int tty = 0;
static int interrupt = 0;
static int ctrl = 0;

extern int UP_ARROW;
extern int DOWN_ARROW;

void switchToPasswordMode() {
	terminals[tty].cannonical = PASSWORD;
}

void switchToCannonicalMode() {
	terminals[tty].cannonical = CANNONICAL;
}

void initTerms() {
	int i = 0, pid = 1;

	for (i = 0; i < TERM_QTY; i++) {
		int j = 0;
		terminals[i].readShellBuffer = 0;
		terminals[i].insertShellBuffer = 0;
		terminals[i].cannonical = CANNONICAL;
		terminals[i].cursorPos = 0;
		for (j = 0; j < 80 * 25 * 2;) {
			terminals[i].video[j++] = ' ';
			terminals[i].video[j++] = 0x07;
		}
		pid = createShell(shell, 0, 0, i);
		terminals[i].foregroundProcess = pid;
		terminals[i].blockedPID = pid;
		terminals[i].user = NULL;
	}
	tty = 0;
}

void sendToTerm(char c) {

	interrupt = 1;
	if (UP_ARROW == 1) {
		UP_ARROW = 0;
	} else if (DOWN_ARROW == 1) {
		DOWN_ARROW = 0;
	} else if ((terminals[tty].cannonical == CANNONICAL
			|| terminals[tty].cannonical == PASSWORD) && c == '\n') {
		putc(STDOUT, '\n');
		if (terminals[tty].foregroundProcess == terminals[tty].blockedPID) {
			wakePID(terminals[tty].blockedPID);
		}
		interrupt = 0;
		ctrl = 0;
		return;
	} else {
		if (c == '\n' || terminals[tty].insertShellBuffer == MAX_BUFFER) {
			putc(STDOUT, '\n');
			ctrl = 0;
		} else if (c == '\b') {
			if (terminals[tty].insertShellBuffer > 0) {
				if (terminals[tty].cannonical != PASSWORD)
					write(&c, 1, STDOUT); // Erases
				terminals[tty].insertShellBuffer--;
			}
			ctrl = 0;
		} else if (c == 0x1D || c == -1) { // make for CTRL
			if (c == 0x1D)
				ctrl = 1;
		} else if (ctrl && c == 't') {
			switchTerminal();
			ctrl = 0;
		} else if (ctrl && c == 'c') {
			signalP(0, 0, terminals[tty].foregroundProcess);
			ctrl = 0;
			c = 0;
		} else {
			ctrl = 0;
			if (terminals[tty].insertShellBuffer < MAX_BUFFER) {
				terminals[tty].shellBuffer[terminals[tty].insertShellBuffer++] =
						c;
				if (terminals[tty].cannonical != PASSWORD) {
					putc(STDOUT, c);
				}
			}
		}
	}

	if (!(terminals[tty].cannonical == CANNONICAL)
			&& !(terminals[tty].cannonical == PASSWORD)) {
		wakePID(terminals[tty].blockedPID);
	}

	interrupt = 0;
}

int checkForNonPrint(char c) {
	if (c == 0x1D || c == 0x01 || c == 0x0F || c == 0x2A || c == 0x36
			|| (c >= 0x3A && c <= 0x54) || c == 0x9D) {
		return 1;
	}
	return 0;
}

void terminalPrint(char *data) {

	//Returns the terminal in which the actual process is running
	int where = getTTY();
	int * pos = &(terminals[where].cursorPos);
	unsigned char * dirVideo = (unsigned char *) 0xB8000;
	if (interrupt) {
		dirVideo = (unsigned char *) 0xB8000;
		pos = &(terminals[tty].cursorPos);
	} else if (where == tty) {
		dirVideo = (unsigned char *) 0xB8000;
	} else {
		dirVideo = terminals[where].video;
	}
	if ((*data) == 0x08) { // Backspace
		char c = ' ';
		print(&c, (*pos) - 2, dirVideo);
		(*pos) -= 2;
		(*pos) = ((*pos) < 0 ? 0 : (*pos));
	} else if ((*data) == '\n') { // Enter
		char c = ' ';
		int scroll;
		(*pos) = ((*pos) + (160 - ((*pos) % 160)));
		scroll = print(&c, (*pos), dirVideo);
		if (scroll) {
			(*pos) = 80 * 24 * 2;
		}
	} else if ((*data) != 0x09) { //Tab
		int scroll;
		scroll = print(data, (*pos), dirVideo);
		(*pos) += 2;
		if (scroll) {
			(*pos) = 80 * 24 * 2;
		}
	}
}

void switchTerminal() {
	term * actual = &terminals[tty++];
	if (tty == TERM_QTY
		)
		tty = 0;
	term * next = &terminals[tty];
	char * vid = (char *) 0xB8000;
	int i = 0;
	for (i = 0; i < 80 * 25 * 2; i++) {
		//saves actual video
		actual->video[i] = vid[i];
		//prints new terminal
		vid[i] = next->video[i];
	}
}

void printShellBuffer() {
	int i = 0;
	for (; i < terminals[tty].insertShellBuffer; i++) {
		putc(STDOUT, terminals[tty].shellBuffer[i]);
	}

}

char * getKey(void) {
	int i = 0;
	char * ret = (char *) Malloc(MAX_BUFFER);
	if (terminals[getTTY()].insertShellBuffer == 0) {
		if (getPID() == terminals[getTTY()].foregroundProcess) {
			terminals[getTTY()].blockedPID = getPID();
		}
		block();
	}
	for (; i < terminals[tty].insertShellBuffer; i++) {
		ret[i] = terminals[tty].shellBuffer[i];
	}
	ret[i] = '\0';
	cleanShellBuffer();
	return ret;
}

char* getStringInBuffer() {
	if (terminals[tty].insertShellBuffer == 0) {
		terminals[tty].stringBuffer[0] = '\0';
		return terminals[tty].stringBuffer;
	}
	int i = 0;
	for (; i < terminals[tty].insertShellBuffer; i++) {
		terminals[tty].stringBuffer[i] = terminals[tty].shellBuffer[i];
	}
	terminals[tty].stringBuffer[i] = '\0';

	return terminals[tty].stringBuffer;
}

void cleanShellBuffer() {
	terminals[tty].insertShellBuffer = 0;
	terminals[tty].readShellBuffer = 0;
}

void setForegroundProcess(int PID) {
	terminals[getTTY()].foregroundProcess = PID;
}

int getActualTTY() {
	return tty;
}

void clear_screen() {
	int procTerm = getTTY();
	if (procTerm == tty)
		k_clear_screen((unsigned char *) 0xB8000);
	else
		k_clear_screen((unsigned char *) terminals[procTerm].video);
	terminals[procTerm].cursorPos = 0;
}

char * getTTYUser() {
	return terminals[getTTY()].user;
}

void setUserToTerm(char * userName) {
	terminals[getTTY()].user = userName;
}

void logout() {
	terminals[getTTY()].user = NULL;
}

/* Old function from Dinux 1.0 */
/*void addCommand(char* stringBuffer) {
 int i = 0;
 commandBufferActive = 1; // Actives the command display
 for (i; stringBuffer[i] != '\0' && i < MAX_BUFFER; i++) {
 commandsBuffer[commandToInsert][i] = stringBuffer[i];
 }
 if (i <= (MAX_BUFFER - 1))
 commandsBuffer[commandToInsert][i] = '\0';

 commandToInsert++;
 commandToRead = commandToInsert - 1; // Leaves the cursor to read before
 // the last added

 if (commandToInsert == MAX_COMMAND_BUFFER) {
 commandToInsert = 0; // Starts inserting from the beginning again
 commandsOverflow = 1; // This flag let's the cursorToRead move
 // without problem when the buffer is full
 }
 prev = NONE;
 }*/
