#include "../../include/diskata.h"
#include "../../include/kasm.h"
#include "../../include/iolib.h"

/* Driver taken from Cristian Pereyra, Maximo Videla and Agustin Marseillan */

#define BIT0 1
#define BIT1 1<<1
#define BIT2 1<<2
#define BIT3 1<<3
#define BIT4 1<<4
#define BIT5 1<<5
#define BIT6 1<<6
#define BIT7 1<<7
#define BIT8 1<<8
#define BIT9 1<<9
#define BIT10 1<<10
#define BIT11 1<<11
#define BIT12 1<<12
#define BIT13 1<<13
#define BIT14 1<<14
#define BIT15 1<<15

#define ERR				BIT0
#define	DRQ				BIT3
#define	SRV				BIT4
#define	DF				BIT5
#define	RDY				BIT6
#define	BSY				BIT7

/* This defines are to check the sepecifications of the disk.
 #TODO: check size!*/
#define IS_REMOVABLE(D) (D & BIT7) ? printf("Is removable\n") : printf("Is not removable\n")
#define IS_ATA_DRIVE(D) (D & BIT15) ? printf("Is not ATA\n") : printf("Is ATA\n")

#define DMA_SUP(D) (D & BIT8) ? printf("DMA is supported\n") : printf("DMA is not supported\n")
#define LBA_SUP(D) (D & BIT9) ? printf("LBA is supported\n") : printf("LBA is not supported\n")
#define DMA_QUEUED_SUP(D) (D & BIT1) ? printf("DMA QUEUED supported\n") : printf("DMA QUEUED is not supported\n")

void writeDataToRegister2(int ata, char upper, char lower);
void identifyDevice2(int ata);
unsigned short getStatusRegister2(int ata);
void sendComm2(int ata, int rdwr, unsigned int sector);
unsigned short getErrorRegister2(int ata);
unsigned short getDataRegister2(int ata);
void writeDataToRegister2(int ata, char upper, char lower);
void translateBytes2(char * ans, unsigned short databyte, int sector);

// To read N bytes from hard disk, must alloc N+1 bytes for ans, as N+1 byte is used to null-character
void _disk_read(int ata, char * ans, int count, unsigned short sector,
		int offset) {

	ata = ATA0;
	// Just a sector...
	if (count > 512 - offset)
		return;

	char tmp[512];
	int i = 0;
	for (i = 0; i < 512; ++i) {
		tmp[i] = 0;
	}

	sendComm(ata, LBA_READ, sector);

	_Halt(); // TODO: Make the 500ns wait

	int b;
	unsigned short data;
	int errors = 0;
	for (b = 0; b < 512 && errors < 10; b += 2) {
		data = getDataRegister(ata);
		if (data == 65535) {
			errors++;
			b -= 2;
		}
		translateBytes(tmp + b, data);
	}

	for (i = 0; i < count; i++) {
		ans[i] = tmp[offset + i];
	}
}

// Translate one word into two char
void translateBytes(char * ans, unsigned short databyte) {
	ans[0] = databyte & 0xFF;
	ans[1] = databyte >> 8;
}

// Writes to the ata chosen the msg received the ammount of bytes requested starting from the secto chose + the offset
void _disk_write(int ata, char * msg, int bytes, unsigned short sector,
		int offset) {

	ata = ATA0;
	int i = 0;

	char write_tmp[512];

	for (i = 0; i < 512; ++i) {
		write_tmp[i] = 0;
	}
	i = 0;

	// Prepare sectors with new data
	for (i = 0; i < bytes; i++) {
		write_tmp[offset + i] = msg[i];
	}

	// Send write command
	sendComm(ata, LBA_WRITE, sector);

	_Halt(); // TODO: Make the 500ns wait

	// Now write all the sector
	int b;
	for (b = 0; b < 512; b += 2) {
		writeDataToRegister(ata, write_tmp[b + 1], write_tmp[b]);
	}
	_Halt();
	_Halt();
}

void writeDataToRegister(int ata, char upper, char lower) {
	_Cli();
	unsigned short out = 0;

	// Wait for driver's ready signal.
	while (!(_inw(ata + WIN_REG7) & BIT3))
		;

	out = ((upper << 8) & 0xFF00) | (lower & 0xFF);
	_outw(ata + WIN_REG0, out);

	_Sti();
}

unsigned short getDataRegister(int ata) {
	_Cli();
	unsigned short ans;

	// Wait for driver's ready signal.
	while (!(_inw(ata + WIN_REG7) & BIT6))
		;

	ans = _inw(ata + WIN_REG0);

	_Sti();
	return ans;
}

unsigned short getErrorRegister(int ata) {
	_Cli();
	unsigned short rta = _in(ata + WIN_REG1) & 0x00000FFFF;
	_Sti();
	return rta;
}

// Send a command to the disk in order to read or write
void sendComm(int ata, int rdwr, unsigned short sector) {
	_Cli();

	_out(ata + WIN_REG1, 0);
	_out(ata + WIN_REG2, 1); // Set count register sector in 1

	_out(ata + WIN_REG3, (unsigned char) sector); // LBA low
	_out(ata + WIN_REG4, (unsigned char) (sector >> 8)); // LBA mid
	_out(ata + WIN_REG5, (unsigned char) (sector >> 16)); // LBA high
	_out(ata + WIN_REG6, 0);

	// Set command
	_out(ata + WIN_REG7, rdwr);

	_Sti();
}

unsigned short getStatusRegister(int ata) {
	unsigned short rta;
	_Cli();
	rta = _in(ata + WIN_REG7) & 0x00000FFFF;
	_Sti();
	return rta;
}

void identifyDevice(int ata) {
	_Cli();
	_out(ata + WIN_REG6, 0);
	_out(ata + WIN_REG7, WIN_IDENTIFY);
	_Sti();
}

// Check disk features
void check_drive(int ata) {
	printf("-----------------------\n");
	printf("Identifying device ");
	switch (ata) {
	case ATA0:
		printf("ATA0...");
		break;
	case ATA1:
		printf("ATA1...");
		break;
	}
	printf("\n");

	identifyDevice(ata);

	unsigned short data = 0;

	int i;
	for (i = 0; i < 255; i++) {
		data = getDataRegister(ata);
		switch (i) {
		case 0:
			//printf("Data returned (%d): %d\n", i,data);
			IS_REMOVABLE(data);
			IS_ATA_DRIVE(data);
			break;
		case 49:
			DMA_SUP(data);
			LBA_SUP(data);
			break;
		case 83:
			DMA_QUEUED_SUP(data);
			break;
		}
	}
}

//#define BIT0 1
//#define BIT1 1<<1
//#define BIT2 1<<2
//#define BIT3 1<<3
//#define BIT4 1<<4
//#define BIT5 1<<5
//#define BIT6 1<<6
//#define BIT7 1<<7
//#define BIT8 1<<8
//#define BIT9 1<<9
//#define BIT10 1<<10
//#define BIT11 1<<11
//#define BIT12 1<<12
//#define BIT13 1<<13
//#define BIT14 1<<14
//#define BIT15 1<<15
//
///* This defines are to check the sepecifications of the disk.
//	#TODO: check size!*/
//#define IS_REMOVABLE(D) (D & BIT7) ? printf("Is removable\n") : printf("Is not removable\n")
//#define IS_ATA_DRIVE(D) (D & BIT15) ? printf("Is not ATA\n") : printf("Is ATA\n")
//
//#define DMA_SUP(D) (D & BIT8) ? printf("DMA is supported\n") : printf("DMA is not supported\n")
//#define LBA_SUP(D) (D & BIT9) ? printf("LBA is supported\n") : printf("LBA is not supported\n")
//#define DMA_QUEUED_SUP(D) (D & BIT1) ? printf("DMA QUEUED supported\n") : printf("DMA QUEUED is not supported\n")
//
//#define SECTOR_SIZE 512
//
//

void _400ns() {
	_inb(0x3F6);
	_inb(0x3F6);
	_inb(0x3F6);
	_inb(0x3F6);
	_inb(0x3F6);
	_inb(0x3F6);
	_inb(0x3F6);
	_inb(0x3F6);
	_inb(0x3F6);
	_inb(0x3F6);
}

int _drq_wait() {

	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();

	int test;
	while ((test = _inb(0x1F7)) && 1) {
		if (!(test & BSY)) {
			if (test & DRQ) {
				break;
			} else if (test & (ERR | DF)) {
				if (test & ERR) {
					printf("OUCH ERROR! %d %d %d\n",
							!!(test & DF), !!(test & ERR));while (1)
;				}

				return 1;
				break;
			}
		}
	}
	return 0;
}

int _next_io() {
	int test;
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();
	_400ns();

	while ((test = _inb(0x1F7)) && 1) {
		if (!(test & BSY)) {
			if (test & (ERR | DF)) {
				if (test & ERR) {
					printf("IO Error %d %d\n", !!(test & DF), !!(test & ERR) )
							;while (1)
;				}
				return -1;
				break;
			} else {
				break;
			}
		}
	}
	return 0;
}

//// To read N bytes from hard disk, must alloc N+1 bytes for ans, as N+1 byte is used to null-character
void _disk_read2(int ata, char * ans, int numreads, unsigned int sector) {

	// We need this to make it work, I just don't know why

	ata = ATA0;
	//_Sti();
	_outw(0x3F6, BIT2);

	int i = 0;
	for (i = 0; i < SECTOR_SIZE * numreads; ++i) {
		ans[i] = 0;
	}

	// _next_io();
	int retry = 1;
	while (retry) {
		unsigned long long addr = ((unsigned long long) sector)
				& 0x00000000ffffffff;
		// printf("error: %d\n", getErrorRegister(ATA0));

		_outb(0x1F1, 0x00);
		_outb(0x1F1, 0x00);
		_outb(0x1F2, 0x00);
		_outb(0x1F2, numreads);
		_outb(0x1F3, (unsigned char) (addr >> 24));
		_outb(0x1F3, (unsigned char) addr);
		_outb(0x1F4, 0);
		_outb(0x1F4, (unsigned char) (addr >> 8));
		_outb(0x1F5, 0);
		_outb(0x1F5, (unsigned char) (addr >> 16));
		_outb(0x1F6, 0);
		_outb(0x1F7, 0x24);

		// printf("r1 %d\n", sector);
		retry = _drq_wait() == -1;
	}

	// printf("r2\n");

	int b;
	unsigned short data;
	int c = 0;
	for (i = 0; i < numreads;) {
		// printf("r3\n");
		for (b = 0; b < SECTOR_SIZE; b += 2, c += 2) {
			data = getDataRegister(ata);
			translateBytes2(ans + c, data, sector);
		}
		++i;
		if (i == numreads) {
			break;
		}
		retry = 1;
		while (retry) {
			retry = _next_io() == -1;
		}
	}

	//_Cli();
}

// Translate one word into two char
void translateBytes2(char * ans, unsigned short databyte, int sector) {
	ans[0] = databyte & 0x000000FF;
	ans[1] = (databyte >> 8) & 0x000000FF;
}

// Writes to the ata chosen the msg received the ammount of bytes requested starting from the secto chose + the offset
void _disk_write2(int ata, char * msg, int numreads, unsigned int sector) {

//	_Sti();
	//_Halt();
	_outw(0x3F6, BIT2);

	// _next_io();

	ata = ATA0;
	int i = 0;

	int retry = 1;
	while (retry) {
		unsigned long long addr = ((unsigned long long) sector)
				& 0x00000000ffffffff;

		_outb(0x1F1, 0x00);
		_outb(0x1F1, 0x00);
		_outb(0x1F2, 0x00);
		_outb(0x1F2, numreads);
		_outb(0x1F3, (unsigned char) (addr >> 24));
		_outb(0x1F3, (unsigned char) addr);
		_outb(0x1F4, (unsigned char) (addr >> 32));
		_outb(0x1F4, (unsigned char) (addr >> 8));
		_outb(0x1F5, (unsigned char) (addr >> 40));
		_outb(0x1F5, (unsigned char) (addr >> 16));
		_outb(0x1F6, 0);
		_outb(0x1F7, 0x34);

		// printf("r1 %d\n", sector);
		retry = _drq_wait() == -1;
	}

	// printf("Write wait...\n");

	// printf("w1\n");
	_drq_wait();
	// printf("w2\n");

	// while (!(_inb(0x1F7) & DRQ)) {}

	// Now write all the sector
	int b;
	int c = 0;
	for (i = 0; i < numreads;) {
		// printf("w3\n");
		for (b = 0; b < SECTOR_SIZE; b += 2, c += 2) {
			writeDataToRegister(ata, msg[c + 1], msg[c]);
		}
		if (i == numreads) {
			break;
		}
		++i;
		retry = 1;
		while (retry) {
			retry = _next_io() == -1;
		}
	}
	_Cli();
}

void writeDataToRegister2(int ata, char upper, char lower) {

	unsigned short out = 0;
	out = ((upper << 8) & 0xFF00) | (lower & 0xFF);

	_outw(ata + WIN_REG0, out);

}

unsigned short getDataRegister2(int ata) {

	unsigned short ans;
	ans = _inw(ata + WIN_REG0);
	return ans;
}

void identifyDevice2(int ata) {

	_out(ata + WIN_REG6, 0xA0);
	_out(ata + WIN_REG7, WIN_IDENTIFY);

}

// Check disk features
void check_drive2(int ata) {

	ata = ATA0;
	identifyDevice(ata);

	unsigned short data = 0;

	_Halt();

	int i;
	for (i = 0; i < 255; i++) {
		data = getDataRegister(ata);

		switch (i) {
		case 47:
		case 59:
			printf("%d\n", data);
		}
	}
}
