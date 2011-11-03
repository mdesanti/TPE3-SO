#include "../../include/iolib.h"
#include "../../include/buffer.h"
#include "../../include/prints.h"
#include "../../include/string.h"
#include "../../include/scheduler.h"
#include "../../include/terminal.h"
#include "../../include/malloc.h"
#include "../../include/kasm.h"
#include "../../include/systemCalls.h"
#include <stdarg.h>

void write(char *string, int length, int fileDes) {
	int i = 0;
	checkUserPerms(P_WRITE);
	for (; i < length; i++) {
		params p;
		p.param4 = string + i;
		p.operation = WRITE;
		p.param2 = fileDes;
		_wrapper_int_80(&p);
	}
}

char * read(int fileDesc) {
	checkUserPerms(P_READ);
	params p;
	p.operation = READ;
	p.param2 = fileDesc;
	_wrapper_int_80(&p);
	return *p.param3;
}

void putc(int fileDesc, char ascii) {
	write(&ascii, 1, fileDesc);
}

int printf(const char* fmt, ...) {

	/* Pointer to variable arguments list*/
	va_list list;

	/*Indicates list where does the variable arguments starts*/
	va_start(list,fmt);

	char c;
	int i = 0;
	int qty = 6;
	char qtyDigits[MAX_BUFFER];
	int qtySize = 0;
	for (; (c = fmt[i]) != '\0'; i++) {
		if (c == '%') {
			char next = fmt[++i];
			if (next == '.') {
				next = fmt[++i];
				if ((next - '0') >= 0 && (next - '0') <= 9) {
					qtyDigits[qtySize++] = next;
					while (((next = fmt[++i]) - '0') >= 0 && (next - '0') <= 9) {
						qtyDigits[qtySize++] = next;
					}
					qty = toInteger(qtyDigits, qtySize);
				} else {
					qty = 0;
					if (next != 'f')
						putc(STDOUT, '.');
				}

			}
			switch (next) {
			/* Integer with sign, decimal base*/
			case 'd':
				printInteger(va_arg(list,int));
				break;
				/* Integer with sign, decimal base*/
			case 'i':
				printInteger(va_arg(list,int));
				break;
				/* Integer in octal base without sign*/
			case 'o':
				printOct(va_arg(list,long));
				break;
				/* Integer in hexadecimal base with sign*/
			case 'x':
				printHex(va_arg(list,long));
				break;
				/* Integer in hexadecimal base with sign*/
			case 'X':
				printHex(va_arg(list,long));
				break;
				/* Integer without sign, decimal base*/
			case 'u':
				printUnsignedInteger(va_arg(list,unsigned int));
				break;
				/* Character*/
			case 'c':
				putc(STDOUT, va_arg(list,int));
				break;
				/* String*/
			case 's':
				printString(va_arg(list,char*));
				break;
				/* Float without exponent*/
			case 'f':
				printFloat(va_arg(list,double), qty);
				break;
				/*Float with exponent*/
			case 'e':
				printExponent(va_arg(list,double));
				break;
				/*Float with exponent*/
			case 'E':
				printExponent(va_arg(list,double));
				break;
				/* % symbol*/
			case '%':
				putc(STDOUT, '%');
				break;
			}
		} else {
			if (c == '\t') {
				putc(STDOUT, ' ');
				putc(STDOUT, ' ');
				putc(STDOUT, ' ');
				putc(STDOUT, ' ');
			} else {
				putc(STDOUT, c);
			}
		}

	}

	/* Frees list*/
	va_end(list);
	return i;
}

									/** Old function from Dinux 1.0 **/

									/*int scanf(const char* fmt, ...) {

									 Pointer to variable arguments list
									 va_list list;

									 Indicates list where does the variable arguments starts
									 va_start(list,fmt);

									 char c;
									 unsigned char type;
									 int i = 0;
									 int qtySize = 0;
									 int qty = 1;
									 unsigned char qtyDigits[MAX_BUFFER];
									 unsigned char inserted[MAX_BUFFER] = { '\0' };
									 char* auxS;
									 char* auxC;
									 int* auxD;
									 float *auxF;
									 readShellBuffer = 0;
									 for (i; (c = fmt[i]) != '\0'; i++) {
									 int withSize = 0;
									 qty = 0;
									 qtySize = 0;
									 if (c == '%') {
									 char next = fmt[++i];
									 if ((next - '0') >= 0 && (next - '0') <= 9) {
									 qtyDigits[qtySize++] = next;
									 while (((next = fmt[++i]) - '0') >= 0 && (next - '0') <= 9) {
									 qtyDigits[qtySize++] = next;
									 }
									 qty = toInteger(qtyDigits, qtySize);
									 type = fmt[i];
									 withSize = 1;
									 } else {
									 type = next;
									 }
									 cleanShellBuffer();
									 inserted[0] = getc(STDIN);
									 if (type == 'd' && withSize == 1 && inserted[0] == '-')
									 qty++;
									 int pos = 1;
									 if (withSize == 1) {
									 while ((--qty) > 0 && readShellBuffer < insertShellBuffer) {
									 if (shellBuffer[readShellBuffer] != '\n')
									 inserted[pos++] = shellBuffer[readShellBuffer++];
									 }
									 } else {
									 while (readShellBuffer < insertShellBuffer) {
									 inserted[pos++] = shellBuffer[readShellBuffer++];
									 }
									 }
									 switch (type) {
									 Character auxC = va_arg(list,char*);
									 *auxC = inserted[0];
									 case 'c':
									 *va_arg(list,char*) = inserted[0];
									 break;
									 Integer with sign, decimal base
									 case 'd':
									 if(inserted[0]=='\n')
									 inserted[0]='0';
									 auxD = va_arg(list,int*);
									 *auxD = toInteger(inserted, pos);
									 break;
									 Float without exponent
									 case 'f':
									 auxF=va_arg(list,float*);
									 *auxF = toFloat(inserted, pos);
									 break;
									 String
									 case 's':
									 auxS=va_arg(list,char*);
									 strcpy(inserted, auxS);
									 break;
									 }
									 cleanShellBuffer();
									 }
									 }
									 Frees list
									 va_end(list);
									 return 1;
									 }*/
