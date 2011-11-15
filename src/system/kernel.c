#include "../../include/iolib.h"
#include "../../include/kasm.h"
#include "../../include/defs.h"
#include "../../include/terminal.h"
#include "../../include/kc.h"
#include "../../include/scheduler.h"
#include "../../include/diskata.h"
#include "../../include/init.h"

DESCR_INT idt[0x81]; /* IDT with 129 entries*/
IDTR idtr; /* IDTR */
unsigned long long int tickpos = 0;

//page directory inserted at de beginning of the second mega
static pageDirectory *page_directory = (pageDirectory*)0x100000;
static pageDirectory * lastAsignedPageTable = (pageDirectory*)0x100000;
//static unsigned int * lastAsignedPage = (unsigned int *)(5 * 1024 * 1024);
static unsigned int * asignPageMemDir(unsigned int * lastPage);

void setUpPaging(void);

void int_08() {
	tickpos++;
}

/**********************************************
 kmain()
 Punto de entrada de cóo C.
 *************************************************/

int kmain() {

	/* Borra la pantalla y cursor */

	k_clear_screen((unsigned char *) 0xB8000);
	cursorOFF();

	/* CARGA DE IDT CON LA RUTINA DE ATENCION DE IRQ0    */

	setup_IDT_entry(&idt[0x08], 0x08, (dword) &_int_08_hand, ACS_INT, 0);

	/* CARGA DE IDT CON LA RUTINA DE ATENCION DE IRQ1    */

	setup_IDT_entry(&idt[0x09], 0x08, (dword) &_int_09_hand, ACS_INT, 0);

	/*	CARGA LA INT 80H AL IDT */
	setup_IDT_entry(&idt[0x80], 0x08, (dword) &_int_80_hand, ACS_INT, 0);

	/* Loads IDTR */

	idtr.base = 0;
	idtr.base += (dword) &idt;
	idtr.limit = sizeof(idt) - 1;

	_lidt(&idtr);


	setUpPaging();

	/* Enables timer tick and keyboard interruption */
	_mascaraPIC1(0xFC);
	_mascaraPIC2(0xFF);

	_Cli();
//	k_clear_screen((unsigned char*) 0xB8000);

	createProcess(init, 0, 0, "Init");
	_Sti();

	while (1)
		;
	return 0;
}

/*
 * Paging will work the following way:
 * 		CR3 register will point to the page directory.
 * 		Page directory contains 1024 references to page tables.
 * 		Page tables contain 1024 references to pages.
 * **********************************************************************
 * The 32 bits of the incoming address will be used in the following way:
 * 		Lowest 12 bits (from 0 to 11) will be the offset in the page.
 * 		Mid 10 bits (12 to 21) will be the offset in the page table.
 * 		High 10 bits (22 to 31) will be the offset in the page directory.
 * **********************************************************************
 * The 32 bits (int) entry in the page directory or page table will be
 * organized in the following way:
 * 		Bits 11 to 31 will be the Page Frame Address
 * 		Bit 0 -> Present
 * 		Bit 1 -> Read/Write
 * 		Bit 2 -> User/Supervisor
 * 		Bit 6 -> Dirty
 * **********************************************************************
 * First mega is assigned to kernel
 * From the beggining of the second till the fifth will be reserved for
 * the page directory and the page tables
 * From fifth mega owards will be for mallocs
 */

void setUpPaging(void) {
	int i = 0;

	//attribute: supervisor level, read/write, not present
	for(i = 0; i < 1024; i++) {
		page_directory->entries[i] = 0 | 2;
	}

	//our first page table comes right after the page directory
	unsigned int *first_page_table = asignPageMemDir((unsigned int *)lastAsignedPageTable);
	pageDirectory * page = (pageDirectory *) first_page_table;
	unsigned int start = 0x0;

	//First 256 entries of the first page are where the kernel is. Must be mapped
	//in the first mega
	for(i = 0; i < 256; i++) {
		page->entries[i] = start;
		start += FOURKB;
	}

	//Then map over the 5th mega
	start = 5 * 1024 * 1024 + FOURKB;
	for( ; i < 1024; i++) {
		page->entries[i] = start;
		start += FOURKB;
	}

	page_directory->entries[0] = (unsigned int)first_page_table;

	//moves page_directory (which is a pointer) into the cr3 register.
	asm volatile("mov %0, %%cr3":: "b"(page_directory));

	//reads cr0, switches the "paging enable" bit, and writes it back.
	unsigned int cr0;
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
	k_clear_screen((unsigned char*) 0xB8000);
	return;
}

unsigned int * asignPageMemDir(unsigned int * lastPage) {
	unsigned int * ret = lastPage + FOURKB;

	return ret;
}
