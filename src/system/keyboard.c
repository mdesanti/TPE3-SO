#include "../../include/keyboard.h"
#include "../../include/buffer.h"
#include "../../include/scheduler.h"
#include "../../include/terminal.h"
#include "../../include/kasm.h"

static int SHIFT; //Shift state
static int CAPS_LOCK; //Caps Lock state
static int restart= NOTHING;
int UP_ARROW;
int DOWN_ARROW;
int INT_KEY=0;
int ENTER=1;

/* Process' ids blocked waiting for keyboard are stored here*/
static uint blocked[PROCESS_QTY]={-1};


/*Array with the key and the shifted key, in which each position 
 * corresponds to the make code of the non shifted key*/
unsigned char keyboard[MAX_KEYS][2]= {

	//MAKECODE							SPECIAL KEY
	/*00*/		{NON_PRINT,NON_PRINT},
	/*01*/		{NON_PRINT,NON_PRINT},	//Esc
	/*02*/		{'1','!'},	
	/*03*/		{'2','@'},	
	/*04*/		{'3','#'},	
	/*05*/		{'4','$'},	
	/*06*/		{'5','%'},	
	/*07*/		{'6','^'},	
	/*08*/		{'7','&'},	
	/*09*/		{'8','*'},	
	/*0A*/		{'9','('},	
	/*0B*/		{'0',')'},	
	/*0C*/		{'-','_'},	
	/*0D*/		{'=','+'},	
	/*0E*/		{'\b','\b'},			//Backspace
	/*0F*/		{NON_PRINT,NON_PRINT},	//Tab
	/*10*/		{'q','Q'},	
	/*11*/		{'w','W'},	
	/*12*/		{'e','E'},	
	/*13*/		{'r','R'},	
	/*14*/		{'t','T'},	
	/*15*/		{'y','Y'},	
	/*16*/		{'u','U'},	
	/*17*/		{'i','I'},	
	/*18*/		{'o','O'},	
	/*19*/		{'p','P'},	
	/*1A*/		{'[','{'},	
	/*1B*/		{']','}'},	
	/*1C*/		{'\n','\n'},			//Enter	
	/*1D*/		{0x1D,0x1D},	//Left Ctrl
	/*1E*/		{'a','A'},	
	/*1F*/		{'s','S'},	
	/*20*/		{'d','D'},	
	/*21*/		{'f','F'},	
	/*22*/		{'g','G'},	
	/*23*/		{'h','H'},	
	/*24*/		{'j','J'},	
	/*25*/		{'k','K'},	
	/*26*/		{'l','L'},	
	/*27*/		{';',':'},		
	/*28*/		{'\'','"'},	
	/*29*/		{'`','~'},	
	/*2A*/		{NON_PRINT,NON_PRINT},	//Left Shift
	/*2B*/		{'\\','|'},	
	/*2C*/		{'z','Z'},	
	/*2D*/		{'x','X'},	
	/*2E*/		{'c','C'},	
	/*2F*/		{'v','V'},	
	/*30*/		{'b','B'},	
	/*31*/		{'n','N'},	
	/*32*/		{'m','M'},	
	/*33*/		{',','<'},	
	/*34*/		{'.','>'},	
	/*35*/		{'/','?'},	
	/*36*/		{NON_PRINT,NON_PRINT},	//Right Shift
	/*37*/		{'*','*'},				//Keypad *
	/*38*/		{NON_PRINT,NON_PRINT},	//Left Alt
	/*39*/		{' ',' '},				//Space
	/*3A*/		{NON_PRINT,NON_PRINT},	//Caps Lock
	/*3B*/		{NON_PRINT,NON_PRINT},	//F1
	/*3C*/		{NON_PRINT,NON_PRINT},	//F2
	/*3D*/		{NON_PRINT,NON_PRINT},	//F3
	/*3E*/		{NON_PRINT,NON_PRINT},	//F4
	/*3F*/		{NON_PRINT,NON_PRINT},	//F5
	/*40*/		{NON_PRINT,NON_PRINT},	//F6
	/*41*/		{NON_PRINT,NON_PRINT},	//F7
	/*42*/		{NON_PRINT,NON_PRINT},	//F8
	/*43*/		{NON_PRINT,NON_PRINT},	//F9
	/*44*/		{NON_PRINT,NON_PRINT},	//F10
	/*45*/		{NON_PRINT,NON_PRINT},	//Num Lock
	/*46*/		{NON_PRINT,NON_PRINT},	//Scroll Lock
	/*47*/		{NON_PRINT,NON_PRINT},	//Keypad 7
	/*48*/		{NON_PRINT,NON_PRINT},	//Keypad 8 // Up- Arrow
	/*49*/		{NON_PRINT,NON_PRINT},	//Keypad 9
	/*4A*/		{NON_PRINT,NON_PRINT},	//Keypad -
	/*4B*/		{NON_PRINT,NON_PRINT},	//Keypad 4
	/*4C*/		{NON_PRINT,NON_PRINT},	//Keypad 5
	/*4D*/		{NON_PRINT,NON_PRINT},	//Keypad 6
	/*4E*/		{NON_PRINT,NON_PRINT},	//Keypad +
	/*4F*/		{NON_PRINT,NON_PRINT},	//Keypad 1
	/*50*/		{NON_PRINT,NON_PRINT},	//Keypad 2 // Down-Arrow
	/*51*/		{NON_PRINT,NON_PRINT},	//Keypad 3
	/*52*/		{NON_PRINT,NON_PRINT},	//Keypad 0
	/*53*/		{NON_PRINT,NON_PRINT},	//Keypad . // Supr
	/*54*/		{NON_PRINT,NON_PRINT},	//Sys Req (AT)
	/*55*/		{'+','*'},	
	/*56*/		{'+','*'},	
	/*57*/		{NON_PRINT,NON_PRINT},	//F11
	/*58*/		{NON_PRINT,NON_PRINT}};	//F12


void int_09(int scanCode){

	int state;

	// Refresh the Shift, Caps Lock ands up/down arrows state
	refreshShift(scanCode);
	refreshCapsLock(scanCode);
	refreshArrows(scanCode);
	
	// Checks if it has to restart
	if(scanCode == BREAK_ALT || scanCode== BREAK_CTRL ||
												scanCode== BREAK_SUPR){
		restart=NOTHING;
		//sendToTerm(keyboard[scanCode][0]);
	}
	else if(scanCode == MAKE_CTRL || scanCode== MAKE_ALT ||
												scanCode==MAKE_SUPR ){
		checkStatus(scanCode);
		if(restart == RESTART){
			_restart_PC();
			restart=NOTHING;		
		}
		//sendToTerm(keyboard[scanCode][0]);
	}
	
	// Checks if it is a valid scanCode and adds to buffer and sets
	// INT_KEY in 1
	if( (scanCode>= 0x01 && scanCode <= 0x58 ) &&
											keyboard[scanCode][0]!=0
												&& ENTER==1){	
		state=SHIFT&&CAPS_LOCK;
		if(SHIFT){
			if( state ==1){
				sendToTerm(keyboard[scanCode][0]);
			}
			else if( state ==0){
				sendToTerm(keyboard[scanCode][1]);
			}
		
		}else{
			if(isLetter(scanCode)){
				if(CAPS_LOCK){
					sendToTerm(keyboard[scanCode][1]);
				}
				else{
					sendToTerm(keyboard[scanCode][0]);
				}
			}else{
				sendToTerm(keyboard[scanCode][0]);
			}
		}
		INT_KEY=1;
	}
	if ( (UP_ARROW==1 || DOWN_ARROW==1) && ENTER==1)	
		INT_KEY=1;
	
}
	
void refreshShift(int scanCode){
	if( scanCode == 0x2A || scanCode == 0x36){	// Make Code
		SHIFT=1;
	// Break Code
	}else if( scanCode == (0x2A +0x80) || scanCode == (0x36 +0x80)){ 
		SHIFT=0;
	}
}

void refreshCapsLock(int scanCode){
	if( scanCode == 0x3A){	// Make Code
		CAPS_LOCK= !CAPS_LOCK;
	}
}

void refreshArrows(int scanCode){
	if ( scanCode == MAKE_UP){
		UP_ARROW=1;
	}
	else if(scanCode == MAKE_DOWN)
		DOWN_ARROW=1;
	else if( scanCode == BREAK_UP)
		UP_ARROW=0;
	else if( scanCode == BREAK_DOWN)
		DOWN_ARROW=0;
}

int isLetter(int scanCode){
	if( (scanCode>=0x10 && scanCode<=0x19) ||
							(scanCode>=0x1E && scanCode<=0x26) ||
									(scanCode>=0x2C && scanCode<=0x32)){
		return 1;
	}
	return 0;
}

void checkStatus(int scanCode){

	switch(scanCode){

	case MAKE_CTRL: restart= CTRL;
				break;
	case MAKE_ALT: restart=(restart==CTRL? ALT: NOTHING);
				break;
	case MAKE_SUPR: restart=(restart==ALT? SUPR:NOTHING);
				break;
	return;
	}
}

void blockOnKeyboard() {
	int i = 0;

	for(i = 0; i < PROCESS_QTY; i++) {
		if(blocked[i] == -1)
			break;
	}
	blocked[i] = getPID();
	block();
}
