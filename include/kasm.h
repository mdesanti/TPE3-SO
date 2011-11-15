/*********************************************
kasm.h

************************************************/

#include "defs.h"


unsigned int    _read_msw();

void            _lidt (IDTR *idtr);

void		_mascaraPIC1 (byte mascara);  /* Escribe mascara de PIC1 */
void		_mascaraPIC2 (byte mascara);  /* Escribe mascara de PIC2 */

void		_Cli(void);        /* Deshabilita interrupciones  */
void		_Sti(void);	 /* Habilita interrupciones  */

void		_int_08_hand();      /* Timer tick */
void		_int_09_hand();      
void		_int_80_hand();
void		_int_21_hand();
params *		_wrapper_int_80();

void		_call_int_08();
void		_Halt();
void		_restart_PC();
void 		_out();
int			_inw();
int			_outw();
int 		_in();
unsigned char _inb();
void		_outb();

void		_debug (void);
