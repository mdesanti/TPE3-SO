#include "../../include/filesystem2.h"
#include "../../include/string.h"
#include "../../include/iolib.h"
#include "../../include/userList.h"
#include "../../include/malloc.h"
#include "../../include/string.h"
#include "../../include/users.h"
#include "../../include/malloc.h"

userListHeader * userList;

#define INITIAL 15

void analyzeUser(char * str, int top);
void analyzeStr(char * str, int *where, char * insert);
int getPerms(char * str, int index, int top);

void createDefaultUsers() {
	int fd2 = 0;
	int size = 0;
	char root[12] = "root#root#1";
	root[11] = '\n';
	char user1[14] = "user1#user1#2";
	user1[13] = '\n';
	char user2[14] = "user2#user2#4";
	user2[13] = '\n';

	if (mkdir("/dinux") == -1)
		printf("ERROR 1\n");

	if (mkdir("/dinux/users") == -1)
		printf("ERROR 5\n");

	if ((fd2 = open("/dinux/users/users.txt", O_RDWR | O_CREATE, 511)) == -1)
		printf("ERROR en open\n");

	size = Write(fd2, root, sizeof(root));
	Write(fd2, user1, sizeof(user1));
	Write(fd2, user2, sizeof(user2));

	close(fd2);

}

void readDefaultUsers() {

	int fd2 = 0, aux;
	char read;
	int i = 0;
	char user[MAX_NAME_LENGHT];

	userList = createUserList();

	if ((fd2 = open("/dinux/users/users.txt", O_RDWR, 511)) == -1)
		printf("ERROR en open\n");

	while ((aux = Read(fd2, &read, 1)) != 0) {
		if (read != '\n') {
			user[i++] = read;
		} else {
			user[i] = '\0';
			analyzeUser(user, i);
			i = 0;
		}
	}
	close(fd2);

}

void analyzeUser(char * str, int top) {
	int i = 0;
	char user[MAX_NAME_LENGHT];
	analyzeStr(str, &i, user);
	char pass[MAX_NAME_LENGHT];
	analyzeStr(str, &i, pass);
	int perms = getPerms(str, i, top);

	addUser(userList, user, pass, perms);

}

void analyzeStr(char * str, int *where, char * insert) {
	int i = 0;
	while (str[*where] != '#') {
		insert[i++] = str[*where];
		(*where)++;
	}
	(*where)++;
	insert[i] = '\0';

}

int getPerms(char * str, int index, int top) {
	int ret = 0;

	while (index < top) {
		ret *= 10;
		ret += str[index++] - '0';
	}

	return ret;
}

int getUserPerm(char * user) {
	userData * ud = getUser(userList, user);
	if (ud == NULL)
		return -1;
	return ud->perms;
}

int validUserAndPass(char * user, char * pass) {
	userData * ud = getUser(userList, user);

	if (ud == NULL)
		return 0;

	return strcmp(pass, ud->pass);
}

int createNewUser(int argc, char ** argv) {
	char * username;
	char * password;
	char * perms;
	printf("Username: ");
	username = read(STDIN);
	printf("Password:");
	password = read(STDIN);
	do {
		printf("Enter permissions for user: ");
		perms = read(STDIN);
	} while (perms[0] <= '0' || perms[0] >= '5');
	char aux, read;
	int fd, wr = 0, other = 0;
	int size;
	char write[MAX_NAME_LENGHT * 2];
	while (username[other] != '\0')
		write[wr++] = username[other++];
	write[wr++] = '#';
	other = 0;
	while (password[other] != '\0')
		write[wr++] = password[other++];
	write[wr++] = '#';
	write[wr++] = perms[0];
	write[wr++] = '\n';
	write[wr] = '\0';

	if ((fd = open("/dinux/users/users.txt", O_RDWR | O_CREATE, 511)) == -1)
		printf("ERROR en open\n");

	//move to the end of file
	while ((aux = Read(fd, &read, 1)) != 0)
		;

	size = Write(fd, write, strlen(write));
	close(fd);

	printf("%s successfully created!\nWelcome!\n", username);

	readDefaultUsers();

	return 1;
}
