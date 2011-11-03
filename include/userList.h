#ifndef USERLIST_H_
#define USERLIST_H_

#include "filesystem2.h"

typedef struct {
	char username[MAX_NAME_LENGHT];
	char password[MAX_NAME_LENGHT];
	int perms;
	struct userNode * next;
} userNode;

typedef struct {

	int elemQty;
	struct userNode * first;
} userListHeader;

typedef struct {
	char * username;
	char * pass;
	int perms;
} userData;

/* Creates list of users*/
userListHeader * createUserList();

/* Adds user to the list */
void addUser(userListHeader * uh, char * name, char * pass, int perms);

/* Returns username, password and permissions of user with identified by "name" */
userData * getUser(userListHeader * uh, char * name);

#endif
