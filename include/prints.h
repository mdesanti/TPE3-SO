#ifndef _PRINTS_
#define _PRINTS_

/* Prints "ascii" in the position "where" of the screen*/
int print(char *ascii, int where, unsigned char * dirVideo);

/* Prints a string*/
void printString(char *s);

/* Prints an integer*/
void printInteger(int integer);

/* Print an hexadecimal number*/
void printHex(long hexa);

/* Print an octal number*/
void printOct(long oct);

/* Prints a float with exponent*/
void printExponent(float number);

/*Prints an unsigned int number*/
void printUnsignedInteger(unsigned int integer);

/* Prints a float number */
void printFloat(float number, int precision);

#endif
