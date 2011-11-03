#include "../../include/userList.h"
#include "../../include/string.h"
#include "../../include/defs.h"
#include "../../include/malloc.h"
#include "../../include/iolib.h"

userListHeader * createUserList() {
	userListHeader * ret = (userListHeader *) Malloc(sizeof(userListHeader));

	ret->first = NULL;
	return ret;
}

void addUser(userListHeader * uh, char * name, char * pass, int perms) {
	userNode * new = (userNode *) Malloc(sizeof(userNode));
	strcpy(name, new->username);
	strcpy(pass, new->password);
	new->perms = perms;
	new->next = uh->first;
	uh->first = (struct userNode *) new;
}

userData * getUser(userListHeader * uh, char * name) {
	userNode * actual = (userNode *) uh->first;

	while (actual != NULL && !strcmp(name, actual->username)) {
		actual = (userNode *) actual->next;
	}

	if (actual == NULL)
		return NULL;
	userData * ret = (userData *) Malloc(sizeof(userData));
	ret->username = actual->username;
	ret->pass = actual->password;
	ret->perms = actual->perms;

	return ret;
}
