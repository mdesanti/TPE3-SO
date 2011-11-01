/*
 * users.h
 *
 *  Created on: Oct 28, 2011
 *      Author: mdesanti90
 */

#ifndef USERS_H_
#define USERS_H_

void createDefaultUsers();
void readDefaultUsers();
int getUserPerm(char * user);
int validUserAndPass(char * user, char * pass);
int createNewUser(int argc, char ** argv);

#endif /* USERS_H_ */
