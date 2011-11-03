#include "../../include/kc.h"

/***************************************************************
*k_clear_screen
*
* Borra la pantalla en modo texto color.
****************************************************************/

void k_clear_screen(unsigned char * vidmem)
{
	unsigned int i=0;
	while(i < (80*25*2))
	{
		vidmem[i]=' ';
		i++;
		vidmem[i]=WHITE_TXT;
		i++;
	};
}

int scrollOneUp(unsigned char * dirVideo){
	int i=0;
	int j=80*2; // Beggining of second line
	while(i<80*24*2){
		dirVideo[i++]=dirVideo[j++];
		dirVideo[i++]=dirVideo[j++];
	}
	while(i<80*25*2){ // Cleans the last row
		dirVideo[i++]=' ';
		dirVideo[i++]=WHITE_TXT;
	}
	return 80*24*2;	// Leaves the cursor in the last line
}

void cursorOFF(){
	// 0x3D4 y 0x3D5 refers to video VGA in IN/OUT
	_screenCursorPos(0x3D4, 14); //high
	_screenCursorPos(0x3D5, -1>>8); // Out of screen
	_screenCursorPos(0x3D4, 15); //low
	_screenCursorPos(0x3D5, -1);
}

/***************************************************************
*setup_IDT_entry
* Inicializa un descriptor de la IDT
*
*Recibe: Puntero a elemento de la IDT
*	 Selector a cargar en el descriptor de interrupcion
*	 Puntero a rutina de atencion de interrupcion
*	 Derechos de acceso del segmento
*	 Cero
****************************************************************/

void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset,
											  byte access, byte cero) {
  item->selector = selector;
  item->offset_l = offset & 0xFFFF;
  item->offset_h = offset >> 16;
  item->access = access;
  item->cero = cero;
}
