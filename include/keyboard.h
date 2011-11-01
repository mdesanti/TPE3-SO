#ifndef _KEYBOARD_
#define _KEYBOARD_

#define NON_PRINT -1 /* Refers to a non printeable character*/
#define MAX_KEYS  89 /* Keys quantity */
#define MAKE_CTRL 0x1D
#define MAKE_ALT 0x38
#define MAKE_SUPR 0x53
#define BREAK_CTRL 0x9D
#define BREAK_ALT 0xB8
#define BREAK_SUPR 0xD3
#define RESTART 3
#define NOTHING 0
#define CTRL 1
#define ALT 2
#define SUPR 3
#define MAKE_UP 0x48
#define MAKE_DOWN 0x50
#define BREAK_UP 0xC8
#define BREAK_DOWN 0xD0

/* Interruption 09, which corresponds to the interruption of the
 * keyboard */
void int_09(int scanCode);

/* Refresh the up and down arrow status*/
void refreshArrows(int scanCode);

/* Refresh the Shift status*/
void refreshShift(int scanCode);

/*Refresh the Caps Lock status*/
void refreshCapsLock(int scanCode);

/* Returns 1 if the scanCode corresponds to a letter*/
int isLetter(int scanCode);

/* Refresh the status of restart variable*/
void checkStatus(int scanCode);
	
#endif
