#ifndef _SHELL_
#define _SHELL_
#include "../include/buffer.h"
#include "../include/defs.h"

#define UP_DISPLAY 1
#define DOWN_DISPLAY 0
#define NONE 2
#define TERM_QTY 4

/* Prints what is in the shell buffer at the moment */
void printShellBuffer();

/* If INT_KEY = 1 then analyzes what's in the keyboard buffer */
void sendToTerm(char c);

/* Puts insertShellBuffer and readShellBuffer in 0 */
void cleanShellBuffer();

/* Reads what's in the shell buffer and leaves a string in
 * stringBuffer */
char* getStringInBuffer();

/*Analyzes the content of the buffer in search for known commands.
 * If a command is recognized, calls the function in charge of executing
 * that command. If not, an error message is printed. Also adds the
 *  command to the commands buffer, if a command is not repetead*/
int shell(int argc, char ** argv);

/* Adds what is in the shell to te commands buffer */
void addCommand(char *stringBuffer);

char * read(int fileDesc);

/* Sets the process identified with PID in the actual terminal to run in foreground */
void setForegroundProcess(int PID);

/* Creates shells and initialize terminals */
void initTerms();

/* Returns the actual terminal */
int getActualTTY();

/* Switchs to another terminal */
void switchTerminal();

/* Prints data into the terminal */
void terminalPrint(char *data);

/* Returns data in the terminal buffer */
char * getKey(void);

/* Returns the name of the user logged in the terminal */
char * getTTYUser();

/* Logs out user from terminal */
void logout();

/* Sets new user to terminal */
void setUserToTerm(char * userName);

enum termType {
	CANNONICAL = 0, PASSWORD = 1
};

typedef struct {
	char shellBuffer[MAX_BUFFER];
	char stringBuffer[MAX_BUFFER];
	/* When enter is pressed, the blocked process is unblocked*/
	enum termType cannonical;
	int readShellBuffer;
	int insertShellBuffer;
	uint blockedPID;
	int foregroundProcess;
	int cursorPos;
	unsigned char video[80 * 25 * 2];char * user;
	int permissions;
} term;

#endif
