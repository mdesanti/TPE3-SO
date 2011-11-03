#include "../../include/diskata.h"
#include "../../include/AtaDiskAdapter.h"
#include "../../include/iolib.h"

/*
 * This is an implementatio of the interface proposed by
 * AtamDiskAdapter.h. Its aim is to give the user
 * the possiblity of writing into disk with an offset and a size
 * smaller thank 512 bytes.
 */

void diskWrite(char * msg, int bytes, int sector, int offset) {
	if (bytes == 0)
		return;
	if (offset == 0 && bytes == 512) {
		_disk_write2(0, msg, 1, (unsigned int) sector);
		return;
	}

	//qty of sectors to use
	int sectQty = (bytes + offset) / 512 + 1;
	int j = 0;
	int i = offset;
	char read[512];

	_disk_read2(0, read, 1, sector);

	for (; i < SECTOR_SIZE && j < bytes; i++, j++) {
		read[i] = msg[j];
	}
	_disk_write2(0, read, 1, sector);
	if (j == bytes)
		return;
	sector++;
	sectQty--;
	_disk_write2(0, &msg[j], sectQty - 1, sector);

	j += SECTOR_SIZE * (sectQty - 1);
	sector += sectQty;
	if (j == bytes)
		return;
	_disk_read2(0, read, 1, sector);
	i = 0;
	for (; i < SECTOR_SIZE && j < bytes; i++, j++) {
		read[i] = msg[j];
	}
	_disk_write2(0, read, 1, sector);
}

void diskRead(char * ans, int bytes, int sector, int offset) {

	if (bytes == 0)
		return;

	//qty of sectors to use
	int sectQty = (bytes + offset) / 512 + 1;
	int j = 0;
	int i = offset;
	char read[512];

	_disk_read2(0, read, 1, sector);

	for (; i < SECTOR_SIZE && j < bytes; i++, j++) {
		ans[j] = read[i];
	}
	if (j == bytes)
		return;
	sector++;
	sectQty--;
	_disk_read2(0, &ans[j], sectQty - 1, sector);

	j += SECTOR_SIZE * (sectQty - 1);
	sector += sectQty;
	if (j == bytes)
		return;

	_disk_read2(0, read, 1, sector);
	i = 0;
	for (; i < SECTOR_SIZE && j < bytes; i++, j++) {
		ans[j] = read[i];
	}
}
