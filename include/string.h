#ifndef _STRING_
#define _STRING_

#define INT_MAX_VALUE 10

/*Compares s with t, if they are equals returns 1*/
int strcmp(char *s, char *t);

/*Copies "from" to "to" and returns "to" */
char *strcpy(char *from, char *to);

/* Generates a float with an array of char*/
float toFloat(char* array, int arraySize);

/* Generates an integer with an array of char*/
int toInteger(char* array, int arraySize);

/* Calculates the lenght of a String*/
int strlen(char* s);

/* Returns 1 if source stars with comp in the first "length" places */
int startsWith(char * source, int length, char * comp);

#endif
