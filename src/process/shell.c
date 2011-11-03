#include "../../include/defs.h"
#include "../../include/calls.h"
#include "../../include/process.h"
#include "../../include/top.h"
#include "../../include/commands.h"
#include "../../include/iolib.h"
#include "../../include/terminal.h"
#include "../../include/string.h"
#include "../../include/systemCalls.h"
#include "../../include/filesystem2.h"
#include "../../include/users.h"
#include "../../include/malloc.h"
#include "../../include/cat.h"

static void getKillParams(char * from, char * option, int * signal, int * pid);
void getCDParameters(char * source, char * params);
char * getPath(char * source);

void printShell() {
	char * user = getTTYUser();
	if (user != NULL
		)
		printf("%s@", getTTYUser());
	printf("Dinux:$ ");
}

int shell(int argc, char ** argv) {
	setForegroundProcess(getPID());
	while (1) {
		int back = 0, pid = -1;
		char * c;
		printShell();
		c = read(STDIN);
		if (c[0] == '&') {
			back = 1;
		}

		if (strcmp(c, "info")) {
			info();
		} else if (strcmp(c, "clear")) {
			clearThisScreen();
		} else if (strcmp(c + back, "commands")) {
			commands();
		} else if (startsWith("kill", 4, c + back)) {
			int kpid = 0;
			int signal = 0;
			char option = '\0';
			getKillParams(c + back, &option, &signal, &kpid);
			printf("Killing %d\n", kpid);
			signalP(option, signal, kpid);
		} else if (strcmp(c + back, "help")) {
			help();
		} else if (strcmp(c + back, "format")) {
			format();
		} else if (strcmp(c + back, "processA")) {
			pid = newProcess(procA, 0, 0, "ProcessA");
		} else if (strcmp(c + back, "processC")) {
			pid = newProcess(procC, 0, 0, "ProcessC");
		} else if (strcmp(c + back, "processB")) {
			pid = newProcess(procB, 0, 0, "ProcessB");
		} else if (strcmp(c + back, "processD")) {
			pid = newProcess(procD, 0, 0, "ProcessD");
		} else if (strcmp(c + back, "processF")) {
			pid = newProcess(procF, 0, 0, "ProcessF");
		} else if (strcmp(c + back, "processG")) {
			pid = newProcess(procG, 0, 0, "ProcessG");
		} else if (strcmp(c + back, "top")) {
			pid = newProcess(top, 0, 0, "TOP");
		} else if (strcmp(c + back, "logout")) {
			clearThisScreen();
			logout();
		} else if (strcmp(c + back, "newUser")) {
			pid = createNewUser(0, NULL);
		} else if (startsWith(c + back, 2, "ls")) {
			if ((c + back)[2] != ' ' && (c + back)[2] != '\0')
				printf("Wrong ls arguments1\n");
			if ((c + back)[2] != '\0'
					&& ((c + back)[3] != '-' || (c + back)[4] != 'a'))
				printf("Wrong ls arguments2\n");
			if ((c + back)[2] == '\0') {

			}
			pid = newProcess(ls, 0, &c, "ls");

		} else if (startsWith(c + back, 2, "cd")) {
			char ** params = (char **) Malloc(1 * (sizeof(char*)));
			params[0] = c;
			pid = newProcess(cd, 0, params, "cd");

		} else if (startsWith(c + back, 2, "rm")) {
			char ** args = (char **) Malloc(1 * sizeof(char*));
			args[0] = c;
			pid = newProcess(rm, 0, args, "rm");
		} else if (startsWith(&((c + back)[0]), 3, "cat")) {
			char ** send = (char **) Malloc(1 * sizeof(char*));
			send[0] = c;
			pid = newProcess(cat, 0, send, "cat");
		} else if (!strcmp(c, "")) {
			printf("%s%s", c, ": comando invalido\n");
		}
		if (!back && pid != -1) {
			setForegroundProcess(pid);
			waitProcess(pid);
			setForegroundProcess(getMyPID());
		}
	}
	return 0;
}

void getKillParams(char * from, char * option, int * signal, int * pid) {
	int opFlag = 0;
	int i = 0;

	while (from[i] != '\0') {
		if (from[i] == '-' && !opFlag) {
			opFlag = 1;
			*option = '-';
			i++;
		} else if (from[i] == '-') {
			*option = '\0';
			*signal = -1;
			*pid = -1;
			return;
		} else if (from[i] >= '0' && from[i] <= '9') {
			if (!opFlag) {
				*option = '\0';
				opFlag = 1;
			}
			while (from[i] >= '0' && from[i] <= '9') {
				(*signal) = (*signal) * 10;
				(*signal) += (from[i] - '0');
				i++;
			}
			while (from[i] == ' ') {
				i++;
			}
			while (from[i] >= '0' && from[i] <= '9') {
				*pid *= 10;
				*pid += from[i] - '0';
				i++;
			}
		} else {
			i++;
		}
	}
}

void getCDParameters(char * source, char * params) {
	int index = 3;
	int i = 0;

	while (source[index] != '\0') {
		params[i++] = source[index++];
	}
	params[i] = '\0';
	printf("%s\n", params);

	return;
}

char * getPath(char * source) {
	char * ret = (char *) Malloc(MAX_NAME_LENGHT * 2);
	int i = 0;
	int j = 0;
	while (source[i] == ' ')
		i++;
	while (source[i] != '\0')
		ret[j++] = source[i++];
	ret[j++] = '\0';

	return ret;
}
