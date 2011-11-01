/**********************
 kc.h
**********************/
#include "defs.h"

#ifndef _kc_
#define _kc_

#define WHITE_TXT 0x07 // Atributo de video. Letras blancas, fondo negro

/* Erases cursor from screen */
void cursorOFF();

void _screenCursorPos();

/* Scrolls all the lines up one position leaving the last empty to
 * continue writing*/
int scrollOneUp(unsigned char * dirVideo);

/* Muestra la imagen de inicio */
void showSplashScreen();

/* Tiempo de espera */
void wait(int time);

/* Limpia la pantalla */
void k_clear_screen(unsigned char * vidmem);

/* Inicializa la entrada del IDT */
void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset,
												byte access, byte cero);

#endif
