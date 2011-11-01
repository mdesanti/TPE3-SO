/*
 * userList.h
 *
 *  Created on: Oct 29, 2011
 *      Author: mdesanti90
 */

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
}userListHeader;

typedef struct {
	char * username;
	char * pass;
	int perms;
} userData;

userListHeader * createUserList();
void addUser(userListHeader * uh, char * name, char * pass, int perms);
userData * getUser(userListHeader * uh, char * name);

#endif /* USERLIST_H_ */
