#ifndef USERS_H_
#define USERS_H_

/* Creates /dinux/users/users.txt with default users to test */
void createDefaultUsers();

/* Reads default users and adds them to the users list */
void readDefaultUsers();

/* Returns permission of user with name "user" */
int getUserPerm(char * user);

/* Returns 1 if the username and passwords are valid, else returns 0 */
int validUserAndPass(char * user, char * pass);

/* Creates new user, adds it to users.txt and the users list*/
int createNewUser(int argc, char ** argv);

#endif
