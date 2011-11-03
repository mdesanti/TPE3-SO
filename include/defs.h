/***************************************************
 Defs.h

 ****************************************************/

#ifndef _defs_
#define _defs_

typedef struct {
	int operation;
	int * param1;
	int param2;
	char ** param3;
	char * param4;
	void * ret;
} params;

/*************Defines para system calls*******************/
#define BLOCK_PROC 1
#define CREATE_PROCESS 2
#define GET_PID 5
#define WAKE_PID 6
#define WAIT_PID 7
#define YIELD 8
#define KILL 9
#define GET_PROCESS_NAME 10
#define PASS_MODE 11
#define CANNONICAL_MODE 12
#define LAST100 13
#define SLEEP 14
#define CLEAR_SCREEN 15
#define FORMAT_PROC 16
#define UP_SEM 17
#define DOWN_SEM 18
#define GET_SEM 19

/*********Defines for system calls permissions***********/
#define P_READ 4
#define P_WRITE 8
#define P_CREATE_PROCESS 3
#define P_BLOCK 3
#define P_GET_PID 4
#define P_WAKE_PID 3
#define P_WAIT_PID 3
#define P_YIELD 2
#define P_KILL 3
#define P_GET_PROCESS_NAME 4
#define P_PASS_MODE 4
#define P_CANNONICAL_MODE 4
#define P_LAST100 2
#define P_SLEEP 3
#define P_CLEAR_SCREEN 2
#define P_FORMAT 3

#define byte unsigned char
#define word short int
#define dword int
#define NULL 0

/* Flags para derechos de acceso de los segmentos */
#define ACS_PRESENT     0x80           /* segmento presente en memoria*/
#define ACS_CSEG        0x18           /* segmento de codigo */
#define ACS_DSEG        0x10           /* segmento de datos */
#define ACS_READ        0x02           /* segmento de lectura */
#define ACS_WRITE       0x02           /* segmento de escritura */
#define ACS_IDT         ACS_DSEG
#define ACS_INT_386 	0x0E		/* Interrupt GATE 32 bits */
#define ACS_INT         ( ACS_PRESENT | ACS_INT_386 )

#define ACS_CODE        (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA        (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK       (ACS_PRESENT | ACS_DSEG | ACS_WRITE)

#define TICKS_PER_SEC 18
#define WRITE 4
#define READ 3
#define STDOUT 1
#define STDIN 0
#define PROCESS_QTY 1000

#pragma pack (1) 		/* Alinear las siguiente estructuras a 1 byte */

/* Descriptor de segmento */
typedef struct {
	word limit, base_l;
	byte base_m, access, attribs, base_h;
} DESCR_SEG;

/* Descriptor de interrupcion */
typedef struct {
	word offset_l, selector;
	byte cero, access;
	word offset_h;
} DESCR_INT;

/* IDTR  */
typedef struct {
	word limit;
	dword base;
} IDTR;

typedef unsigned int uint;

#endif
