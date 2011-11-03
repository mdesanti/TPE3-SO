#include "../../include/string.h"
#include "../../include/prints.h"
#include "../../include/iolib.h"
#include "../../include/kc.h"

int print(char *ascii, int where, unsigned char *dirVideo) {
	int ret = 0;
	if( where>=(80*25*2)) {	// Next to the last position
		where=scrollOneUp(dirVideo);
		ret = 1;
	}
	dirVideo[where]=*ascii;
	dirVideo[where+1]=WHITE_TXT;
	return ret;
}

void printFloat(float number, int precision){
   int i=0;
   int integer;
   float decimal;
   int pot=1;
   for(;i<precision;i++)
      pot*=10;
    if(number<0){
		putc(STDOUT,'-');
		number*=-1;
	}
   integer=number;
   decimal=(number-(int)number);
   printf("%d",integer);
   if(precision!=0){
	   putc(STDOUT,'.');
	   int i=10;
	  while(i<=pot){
		  int n=decimal*i;
		  printf("%d",n%10);
		  i*=10;
	  }
  }
}

void printExponent(float number){
   int i=0;
   int integer;
   float decimal;
   int pot=1000;
   int array[INT_MAX_VALUE]={0};
   int arraySize=0;
   if( number<0){
	  putc(STDOUT,'-');
	  number*=-1;
   }

   integer=number;
   decimal=(number-(int)number);

	if(integer>0){
		while(integer >9){
		   array[arraySize++]=integer%10;
		   integer/=10;
	   }
	   printf("%d.",integer);
	   int e=arraySize;
	   int aux=arraySize;
	   while((--arraySize)>=0){
		   printf("%d",array[arraySize]);
	   }
	   if(aux<3){
			aux=3-aux;
			i=10;
			while((aux--)>0 && i<=pot){
				  int n=decimal*i;
				  printf("%d",n%10);
				  i*=10;
			}
	   }
	   printf("e+%d",e);
   }else if(integer == 0){
	   int e=0;
	   i=10;
	   int n=0;
	   while(n==0 && i<=1000000){
		   n=decimal*i;
		   if(n!=0){
			   printf("%d.",n);
		   }
			e++;
			i*=10;
	   }
	   pot=1;
	   int aux=e+1;
	   while(aux-->0)
			pot*=10;
		i=0;
	   while(i<=3){
		   int n=decimal*pot;
		   printf("%d",n%10);
		   pot*=10;
		   i++;
	   }
	   printf("e-%d",e);
   }
}

void printString(char *s){
	int i=0;
	while(s[i]!='\0'){
		putc(STDOUT,s[i++]);
	}
}

void printInteger(int integer){
	int reverse[INT_MAX_VALUE];
	int arrayPos=INT_MAX_VALUE-1;
	int realSize=0;
	if( integer<0){
		putc(STDOUT,'-');
		integer*=-1;
	}else if(integer==0){
		putc(STDOUT,'0');
		return;
	}
	while(integer>0){
		int inAscii='0'+ integer%10;
		reverse[arrayPos--]=inAscii;
		++realSize;
		integer/=10;
	}
	arrayPos++;
	while(realSize-->0){
		putc(STDOUT,reverse[arrayPos++]);
	}
}

void printUnsignedInteger(unsigned int integer) {
	int reverse[INT_MAX_VALUE];
	int arrayPos=INT_MAX_VALUE-1;
	int realSize=0;
	if(integer==0){
		putc(STDOUT,'0');
		return;
	}
	while(integer>0){
		int inAscii='0'+ integer%10;
		reverse[arrayPos--]=inAscii;
		++realSize;
		integer/=10;
	}
	arrayPos++;
	while(realSize-->0){
		putc(STDOUT,reverse[arrayPos++]);
	}
}

void printHex(long hexa){
	int reverse[INT_MAX_VALUE/4];
	int arrayPos=(INT_MAX_VALUE/4)-1;
	int realSize=0;
	while(hexa>0){
		int mod=hexa%16;
		int inAscii;
		if( mod>=10)
			inAscii='A'+mod-10; // A to F
		else
			inAscii='0'+mod;
		reverse[arrayPos--]=inAscii;
		++realSize;
		hexa/=16;
	}
	arrayPos++;
	while(realSize-->0){
		putc(STDOUT,reverse[arrayPos++]);
	}
}
void printOct(long oct){
	int reverse[INT_MAX_VALUE];
	int arrayPos=(INT_MAX_VALUE)-1;
	int realSize=0;
	while(oct>0){
		int inAscii='0'+oct%8;
		reverse[arrayPos--]=inAscii;
		++realSize;
		oct/=8;
	}
	arrayPos++;
	while(realSize-->0){
		putc(STDOUT,reverse[arrayPos++]);
	}
}
