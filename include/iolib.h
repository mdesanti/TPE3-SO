#ifndef _IOLIB_
#define _IOLIB_

#define WRITE 4
#define READ 3
#define STDOUT 1
#define STDIN 0

#include "calls.h"

/* Puts an ascii in the fileDesc, if fileDesc=STDIOUT puts it in the
 * screen*/
void putc(int fileDesc, char ascii);

/* Gets a character from the fileDesc, if fileDesc=STDIN, gets a
 * character, from the keyboard's buffer*/
unsigned char getc(int fileDesc);

/* Similar to printf of stdio.h */
int printf(const char* fmt, ...);

/* Similar to scanf of stdio.h */
int scanf(const char* fmt, ...);

#endif
