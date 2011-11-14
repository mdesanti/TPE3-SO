#include "../../include/diskata.h"
#include "../../include/sem.h"
#include "../../include/systemCalls.h"
#include "../../include/filesystem2.h"
#include "../../include/string.h"
#include "../../include/malloc.h"
#include "../../include/terminal.h"
#include "../../include/iolib.h"
#include "../../include/users.h"
#include "../../include/AtaDiskAdapter.h"
#include "../../include/lib.h"
#include <stdarg.h>

/* Implementation of file system on disk */

#define NULL 0

superBlock filesys;
generalHeader* root;

generalHeader* actDir;
openFilesTable table;
char clean[250 * 512] = {0};

static int wrFifoSem = -1;
static int rdFifoSem = -1;

void initializeFilesystem() {

	int i;
	printf("Checking if filesystem already exists...\n");
	//	for disk cleaning
	//cleanDisk();

	if (existsFilesystem()) {
		printf("Loading SuperBlock...\n");
		loadSuperBlock();
		printf("Loading Root...\n");
		loadRoot();
		initializeSTDIO();

		for (i = 3; i < MAX_OPEN_FILES; i++) {
			table.openFiles[i].inodeNumber = -1;
		}

	} else {

		printf("Initializing SuperBlock...\n");

		diskWrite(clean, 200 * 512, 3, 0);
		diskWrite(clean, 200 * 512, 203, 0);
		for (i = 0; i < MAX_BLOCK_QTY; i++) {
			filesys.bitMap[i] = 0;
		}
		for (i = 0; i < HEADERS; i++) {
			filesys.headers[i] = 0;
		}

		filesys.rootSect = START_HEADERS;
		filesys.actDirSect = START_HEADERS;
		filesys.inodesQty = 0;
		filesys.firstDataSect = START_FILES;
		filesys.freeBlockQty = MAX_BLOCK_QTY * 8;

		printf("Writing SuperBlock To Disk...\n");
		printf("Creating root directory ...\n");
		createRoot();
		refreshSuperBlock();

		printf("Initilizing Table of Open Files of the File System...\n");

		initializeSTDIO();
		for (i = 3; i < MAX_OPEN_FILES; i++) {
			table.openFiles[i].inodeNumber = -1;
		}
		printf("Initializing users...\n");
		createDefaultUsers();
	}
}

void cleanDisk() {
	diskWrite(clean, 200 * 512, 3, 0);
	diskWrite(clean, 200 * 512, 203, 0);
}

void initializeSTDIO() {
	int i = 0;
	for (i = 0; i < 3; i++) {
		table.openFiles[i].inodeNumber = -i + 1;
		table.openFiles[i].counter = 1;
	}
}
int existsFilesystem() {
	generalHeader test;

	diskRead((char*) &test, sizeof(generalHeader), START_HEADERS, 0);
	if (test.numSect == START_HEADERS && test.dataSect == START_FILES
			&& test.inodeNumber == 0)
		return 1;
	return 0;
}

void loadSuperBlock() {

	diskRead((char*) &filesys, sizeof(superBlock), 3, 0);
}

void loadRoot() {
	root = Malloc(sizeof(generalHeader));
	actDir = Malloc(sizeof(generalHeader));
	diskRead((char*) root, sizeof(generalHeader), START_HEADERS, 0);
	actDir = root;
}

void createRoot() {
	root = Malloc(sizeof(generalHeader));
	actDir = Malloc(sizeof(generalHeader));
	strcpy("root", root->owner);
	strcpy("root", root->group);
	root->data = 0;
	root->permissions = DIR_PERMS;
	root->realSize = sizeof(generalHeader) + sizeof(file) * 2;
	root->diskSize = sizeof(generalHeader) + sizeof(file) * 2;
	root->numSect = START_HEADERS;
	root->inodeNumber = getHeaderInode();
	root->dataSect = getFreeBlocks();
	root->cantFiles = 2;
	root->write = 0;
	root->read = 0;
	root->type = DIR;

	writeHeaderToDisk(root);

	createFirstFiles(root->dataSect, root->numSect, root->inodeNumber,
			root->numSect, root->inodeNumber);
	actDir = root;
	return;
}

void createFirstFiles(int sect, int mySect, int myInodeNum, int parentSect,
		int parentInodeNum) {
	file self, father;
	strcpy(".", self.name);
	self.numSect = mySect;
	self.numInode = myInodeNum;
	strcpy("..", father.name);
	father.numSect = parentSect;
	father.numInode = parentInodeNum;

	diskWrite((char*) (&self), sizeof(file), sect, sizeof(file));
	diskWrite((char*) (&father), sizeof(file), sect, 0);

	return;
}

int create(char* pathname, int mode, int type) {

	int i = 0;
	char name[MAX_NAME_LENGHT];
	int flag = 1;
	generalHeader* actual, *prev;
	if (pathname[i] == '/') {
		pathname += 1;
		actual = root;
		prev = root;
	} else {
		actual = root;
		prev = actDir;
	}
	while (pathname[i] != '\0' && flag) {
		getNameFromPath(pathname, &i, name);

		actual = getHeader(prev, name);

		if (actual == NULL) {
			flag = 0;
		} else if (!(actual->permissions & IS_DIREC)) {
			return -1;
		} else {
			prev = actual;
		}
	}
	if (!flag) {
		if (!(prev->permissions & IS_DIREC))
			return -1;
		i -= strlen(name);
		while (pathname[i] != '\0')
			if (pathname[i++] == '/')
				return -1;
		switch (type) {
		case DIR:

			return createDir(name, prev);
		case FIFO:

			if (mode & IS_DIREC
			)
				return -1;
			return createFifo(name, prev, mode);
		case FILE:
			if (mode & IS_DIREC
			)
				return -1;

			return createFile(name, mode, prev);
		}

	}
	return -1;
}

int mkfifo(char* pathname, int mode) {

	return create(pathname, mode, FIFO);
}

int creat(char* pathname, int mode) {

	return create(pathname, mode, FILE);
}

int mkdir(char* pathname) {

	return create(pathname, DIR_PERMS, DIR);
}

generalHeader* getHeader(generalHeader* parent, char* name) {
	if (!(parent->permissions & IS_DIREC)) {

		return NULL;
	}
	file files[8];
	int i = 0;
	int cantSects = 0;
	int pos = 0;
	int k = 0;
	for (i = 0; k < parent->cantFiles && cantSects < 8; i++, pos++, k++) {
		if (i % FILES_PER_SECTOR == 0) {
			diskRead((char*) files, SECTOR_SIZE, (parent->dataSect) + cantSects,
					0);
			cantSects++;
			pos = 0;
		}
		if (files[pos].numInode == -1) {
			k--;
		} else if (strcmp(files[pos].name, name)) {

			return searchHeader(files[pos].numSect, files[pos].numInode);
		}
	}
	return NULL;
}

generalHeader* searchHeader(int numSect, int numInode) {

	generalHeader* header = Malloc(sizeof(generalHeader));
	diskRead((char*) header, sizeof(generalHeader), numSect,
			(numInode % 8) * sizeof(generalHeader));
	return header;

}

int createDir(char* name, generalHeader* prev) {
	generalHeader new;
	if (prev->type != DIR
	)
		return -1;
	createNewHeader(&new, name, prev, DIR, DIR_PERMS);

	if (new.dataSect == -1)
		return -1;

	createNewEntry(name, new.inodeNumber, new.numSect, prev);

	createFirstFiles(new.dataSect, new.numSect, new.inodeNumber, prev->numSect,
			prev->inodeNumber);

	refreshSuperBlock();

	return 1;
}

void refreshSuperBlock() {

	diskWrite((char*) (&filesys), sizeof(superBlock), 3, 0);

}
void createNewHeader(generalHeader* newHeader, char* name, generalHeader* prev,
		char type, int perms) {

	strcpy("PEPE", newHeader->owner);
	strcpy("TODO", newHeader->group);

	newHeader->data = NULL;
	newHeader->permissions = perms;
	newHeader->inodeNumber = getHeaderInode();
	newHeader->numSect = getHeaderSect(newHeader->inodeNumber);
	newHeader->cantFiles = type == DIR ? 2 : 0;
	newHeader->realSize = sizeof(generalHeader)
			+ newHeader->cantFiles * sizeof(file);
	newHeader->diskSize = sizeof(generalHeader)
			+ newHeader->cantFiles * sizeof(file);
	newHeader->write = NULL;
	newHeader->read = NULL;
	newHeader->type = type;
	newHeader->dataSect = getFreeBlocks();
	if (newHeader->dataSect == -1) {
		return;
	}
	if (type == FILE
	)
		initializeDataSect(newHeader->dataSect);
	writeHeaderToDisk(newHeader);
	return;
}

void initializeDataSect(int dataSect) {
	fragSector sect;
	sect.nextSect = -1;
	sect.sectQty = 7;
	strcpy("", sect.info);
	diskWrite((char*) &sect, SECTOR_SIZE, dataSect, 0);
}

int getHeaderInode() {
	int i = 0;
	int inode;
	int found = 0;
	for (i = 0; i < HEADERS && !found; i++) {
		if (!(filesys.headers[i] & BIT0)) {
			inode = i * 8;
			filesys.headers[i] |= BIT0;
			found = 1;
		} else if (!(filesys.headers[i] & BIT1)) {
			inode = (i * 8) + 1;
			filesys.headers[i] |= BIT1;
			found = 1;
		} else if (!(filesys.headers[i] & BIT2)) {
			inode = (i * 8) + 2;
			filesys.headers[i] |= BIT2;
			found = 1;
		} else if (!(filesys.headers[i] & BIT3)) {
			inode = (i * 8) + 3;
			filesys.headers[i] |= BIT3;
			found = 1;
		} else if (!(filesys.headers[i] & BIT4)) {
			inode = (i * 8) + 4;
			filesys.headers[i] |= BIT4;
			found = 1;
		} else if (!(filesys.headers[i] & BIT5)) {
			inode = (i * 8) + 5;
			filesys.headers[i] |= BIT5;
			found = 1;
		} else if (!(filesys.headers[i] & BIT6)) {
			inode = (i * 8) + 6;
			filesys.headers[i] |= BIT6;
			found = 1;
		} else if (!(filesys.headers[i] & BIT7)) {
			inode = (i * 8) + 7;
			filesys.headers[i] |= BIT7;
			found = 1;
		}
	}
	(filesys.inodesQty)++;

	return inode;
}

int getHeaderSect(int inodeNumber) {
	int sect = START_HEADERS + (inodeNumber) / 8;
	return sect;
}
void createNewEntry(char* name, int inodeNumber, int numSect,
		generalHeader* dir) {

	file newEntry;
	strcpy(name, newEntry.name);
	newEntry.numSect = numSect;
	newEntry.numInode = inodeNumber;
	int pos = getFilePos(dir);
	diskWrite((char*) (&newEntry), sizeof(file), dir->dataSect + (pos / 8),
			(pos % 8) * sizeof(file));

	dir->cantFiles++;
	writeHeaderToDisk(dir);

}

int getFilePos(generalHeader* dir) {

	file files[8];
	int i = 0;
	int cantSects = 0;
	int pos = 0;
	int k = 0;
	for (i = 0; k < dir->cantFiles && cantSects < 7; i++, pos++, k++) {
		if (i % FILES_PER_SECTOR == 0) {
			diskRead((char*) files, SECTOR_SIZE, (dir->dataSect) + cantSects,
					0);
			cantSects++;
			pos = 0;
		}
		if (files[pos].numInode == -1) {
			return i;
		}
	}
	return k;

}

void writeHeaderToDisk(generalHeader* header) {

	diskWrite((char*) header, sizeof(generalHeader), header->numSect,
			(header->inodeNumber % 8) * sizeof(generalHeader));
}
void getNameFromPath(char* path, int* counter, char* name) {
	int i = 0;
	while (path[*counter] != '\0' && path[*counter] != '/') {
		name[i++] = path[(*counter)++];
	}
	name[i++] = '\0';
	if (path[*counter] == '/')
		(*counter)++;
	return;
}

generalHeader* searchPath(char* path) {
	generalHeader* resp;
	int counter = 0;
	if (path[0] == '/') {
		counter = 1;
		resp = searchPathRec(root, path, &counter);
	} else if (path[0] == '\\' && path[1] == '/') {
		counter = 2;
		resp = searchPathRec(root, path, &counter);
	} else if (path[0] == '.' && path[1] == '/') {
		counter = 2;
		resp = searchPathRec(actDir, path, &counter);
	} else
		resp = searchPathRec(actDir, path, &counter);

	return resp;
}

generalHeader* searchPathRec(generalHeader* start, char* path, int * counter) {
	char name[MAX_NAME_LENGHT];
	getNameFromPath(path, counter, name);

	generalHeader* resp = NULL;

	if (strcmp(name, ".")) {
		if (path[(*counter)] == '\0')
			return start;

		return searchPathRec(start, path, counter);

	}
	if (strcmp(name, "..")) {
		resp = getHeader(start, "..");
		if (path[(*counter)] == '\0')
			return resp;

		return searchPathRec(resp, path, counter);
	}

	resp = getHeader(start, name);

	if (resp == NULL
	)
		return NULL;
	if (path[*counter] == '\0')
		return resp;

	return searchPathRec(resp, path, counter);

}

int ls(int argc, char** arguments) {
	int showHidden = 0;
	if (arguments[0][2] != ' ' && arguments[0][2] != '\0') {
		printf("Wrong ls arguments1\n");
		return 0;
	}
	if (arguments[0][2] != '\0'
			&& (arguments[0][3] != '-' || arguments[0][4] != 'a')) {
		printf("Wrong ls arguments2\n");
		return 0;
	}
	if (arguments[0][3] == '-') {
		showHidden = 1;
	}
	listDirFiles(actDir, showHidden);
	return 1;
}

int cd(int argc, char** argument) {

	char * arg = argument[0];
	int i = 2;
	while (arg[i] == ' ') {
		i++;
	}
	generalHeader* resp = searchPath(&(arg[i]));
	if (resp == NULL || resp->type != DIR
	)
		return -1;
	actDir = resp;

	return 1;
}

int open(char* path, int flags, ...) {
	int mode;
	int perms;
	int fd;

	generalHeader* file = searchPath(path);
	if (file != NULL && (file->permissions & IS_DIREC)) {
		return -1;
	}

	if (file == NULL) {

		if (!(flags & O_CREATE))
		return -1;
		else {
			va_list list;
			va_start(list, flags);
			perms=va_arg(list,int);

			fd = creat(path, perms);
			return fd;
		}
	}
	mode = flags & O_RDONLY ? O_RDONLY : flags & O_WRONLY ? O_WRONLY : O_RDWR;
	fd = getFD(openTheFile(file->inodeNumber, -1, file, mode), mode);
	return fd;

}

int Read(int fd, char* buff, int size) {

	int pos = getPos(fd);

	if (pos >= 0) {
		if (table.openFiles[pos].header.type == FILE) {

			return readFromFile(pos, buff, size);
		} else {

			return readFromFifo(pos, buff, size);
		}
	}

	return 0;
}

int readFromFile(int pos, char* buff, int size) {

	int i;
	for (i = 0; i < size; i++) {
		if (table.openFiles[pos].pos == SECTOR_SIZE)
		{
//			printf("Reading sector %d\n",
//					table.openFiles[pos].currSector);
			if (!getNewBuffer(pos)) {
				buff[i] = '\0';
				return i;
			}
		}
		if (table.openFiles[pos].header.data[table.openFiles[pos].pos]
				== '\0') {
			buff[i] = '\0';
			return i;
		} else {
			buff[i] =
					table.openFiles[pos].header.data[table.openFiles[pos].pos++];
		}
	}
	buff[i] = '\0';
	return i;
}
int Write(int fd, char* buff, int size) {

	int writtenChars = -1;
	int pos = getPos(fd);
	if (!openForWrite(fd)) {

		return -1;
	}

	if (pos == 1 || pos == 2) {
		write(buff, size, 1);

		return size;
	}

	if (pos != -1 && table.openFiles[pos].header.type == FILE) {

		writtenChars = writeInFile(pos, buff, size);

	} else {
		writtenChars = writeInFifo(pos, buff, size);
	}

	return writtenChars;

}
int writeInFile(int pos, char* buff, int size) {

	int i = 0;

	for (i = 0; i < size; i++) {
		if (table.openFiles[pos].pos == SECTOR_SIZE) {
			writeBuffToDisk(pos);
			if (!getNewBuffer(pos)) {
//				printf("Writing sector %d\n",
//						table.openFiles[pos].currSector);
				getMoreSpace(pos);
			}
		}

		if (buff[i] != '\0')
			table.openFiles[pos].header.data[table.openFiles[pos].pos++] =
					buff[i];

	}
	buff[i] = '\0';
	writeBuffToDisk(pos);
	return size;
}

void writeBuffToDisk(int pos) {
	diskWrite(table.openFiles[pos].header.data, SECTOR_SIZE,
			table.openFiles[pos].currSector, 0);
}

int getMoreSpace(int pos) {
	fragSector sect;
	int i = 0;
//	int slot = (table.openFiles[pos].firstSector
//			+ table.openFiles[pos].sectorQty + 1 - START_FILES) / 8;
//
//	if (filesys.bitMap[slot + 1] == UNUSED) {
//		sleep(2);
//		printf("EL SLOT LIBRE ES %d QUE CORRESPONDE A EL SECTOR %d\n", slot + 1,
//				(slot + 1) * 8 + START_FILES);
//		filesys.bitMap[slot + 1] = ALL_USED;
//		diskRead((char*) &sect, SECTOR_SIZE, table.openFiles[pos].firstSector,
//				0);
//		sect.sectQty += 8;
//		diskWrite((char *) &sect, SECTOR_SIZE, table.openFiles[pos].firstSector,
//				0);
//		filesys.freeBlockQty -= 8;
//		refreshSuperBlock();
//
//		table.openFiles[pos].currSector++;
//		table.openFiles[pos].pos = 0;
//		table.openFiles[pos].sectorQty += 8;
//
//		return 1;
//	}

	diskRead((char*) &sect, SECTOR_SIZE, table.openFiles[pos].firstSector, 0);

	sect.nextSect = getFreeBlocks();

	diskWrite((char *) &sect, SECTOR_SIZE, table.openFiles[pos].firstSector, 0);

	table.openFiles[pos].currSector = sect.nextSect;
	table.openFiles[pos].firstSector = table.openFiles[pos].currSector;
	diskRead((char*) &sect, SECTOR_SIZE, table.openFiles[pos].currSector, 0);
	sect.sectQty = 7;
	sect.nextSect = -1;
	for (i = 0; i < 8; i++) {
		table.openFiles[pos].header.data[i] = ((char*) &sect)[i];
	}
	table.openFiles[pos].pos = 8;
	table.openFiles[pos].sectorQty = 7;

	return 1;
}

int getFreeBlocks() {
	int i = 0;
	for (; i < MAX_BLOCK_QTY; i += 8) {
		if (filesys.bitMap[(i / 8)] == UNUSED) {
			filesys.bitMap[(i / 8)] = ALL_USED;
			filesys.freeBlockQty -= 8;
			return START_FILES + i;
		}
	}
	return -1;
}

int getNewBuffer(int pos) {

	if (table.openFiles[pos].sectorQty > 0) {

		//hay sectores libres, no hay que levantar nada
		diskRead((char*) table.openFiles[pos].header.data, SECTOR_SIZE,
				table.openFiles[pos].currSector + 1, 0);
		table.openFiles[pos].currSector++;
		table.openFiles[pos].sectorQty--;
		table.openFiles[pos].pos = 0;
		return 1;
	}
	if (table.openFiles[pos].nextSector != -1) {

		diskRead(table.openFiles[pos].header.data, SECTOR_SIZE,
				table.openFiles[pos].nextSector, 0);
		table.openFiles[pos].currSector = table.openFiles[pos].nextSector;
		table.openFiles[pos].nextSector =
				((fragSector*) table.openFiles[pos].header.data)->nextSect;
		table.openFiles[pos].sectorQty =
				((fragSector*) table.openFiles[pos].header.data)->sectQty;
		table.openFiles[pos].pos = 8;

		table.openFiles[pos].firstSector = table.openFiles[pos].currSector;
		return 1;
	}

	return 0;
}

int lseek(int fd, int offset, int whence) {
	return 1;
}

void unlink(char* pathname) {
}

int createFile(char* name, int mode, generalHeader* parent) {
	if (!(parent->permissions & IS_DIREC)) {
		return -1;
	}
	int slot;
	if ((slot = slotForFile()) == -1)
		return -1;

	generalHeader newFile;
	createNewHeader(&newFile, name, parent, FILE, mode);

	createNewEntry(name, newFile.inodeNumber, newFile.numSect, parent);
	int fd = getFD(openTheFile(newFile.inodeNumber, slot, &newFile, O_RDWR),
	O_RDWR);

	return fd;

}

int openTheFile(int inode, int slot, generalHeader* file, int flag) {

	if (file->type == DIR
	)
		return -1;
	int found = 0;
	if (slot != -1) {
		newOpenFile(inode, file, slot, flag);
		return slot;
	}
	int nullPos = -1;
	int i = 3;
	for (; i < MAX_OPEN_FILES; i++) {
		if (table.openFiles[i].inodeNumber == inode) {
			(table.openFiles[i].counter)++;
			if (file->type == FIFO) {

				table.openFiles[i].pidReader =
						flag & O_RDONLY ? getPID() :
						flag & O_RDWR ? getPID() : table.openFiles[i].pidReader;

				table.openFiles[i].pidWriter =
						((flag & O_WRONLY) || (flag & O_RDWR)) ?
								getPID() : table.openFiles[i].pidWriter;

				if (table.openFiles[i].pidWriter != -1 && wrFifoSem == -1) {
					wrFifoSem = getSem(0);
				}
				if (table.openFiles[i].pidReader != -1 && rdFifoSem == -1) {
					rdFifoSem = getSem(0);
				}

			}

			return i;
		} else if (table.openFiles[i].inodeNumber == -1 && !found) {
			nullPos = i;
			found = 1;
		}
	}
	if (nullPos == -1)
		return -1;
	newOpenFile(inode, file, nullPos, flag);

	return nullPos;
}

int slotForFile() {
	int i = 3;
	for (; i < MAX_OPEN_FILES; i++) {
		if (table.openFiles[i].inodeNumber == -1) {
			return i;
		}
	}
	return -1;
}

void newOpenFile(int inode, generalHeader* file, int pos, int flag) {

	table.openFiles[pos].inodeNumber = inode;
	table.openFiles[pos].counter = 1;
	copyHeader(pos, file);

	if (file->type == FILE) {
		table.openFiles[pos].header.data = Malloc(SECTOR_SIZE);
		diskRead(table.openFiles[pos].header.data, SECTOR_SIZE,
				table.openFiles[pos].header.dataSect, 0);

	} else {
		table.openFiles[pos].header.write = Malloc(4 * 1024);
		table.openFiles[pos].header.read = table.openFiles[pos].header.write;
	}

	if (file->type == FIFO) {
		table.openFiles[pos].pidReader = flag & O_RDONLY ? getPID() :
											flag & O_RDWR ? getPID() : -1;

	}

	if (file->type == FIFO) {
		table.openFiles[pos].pidWriter =
				((flag & O_WRONLY) || (flag & O_RDWR)) ? getPID() : -1;

	}
	table.openFiles[pos].pos = file->type == FIFO ? 0 : 8;
	table.openFiles[pos].pos2 = 0;
	table.openFiles[pos].sectorQty =
			((fragSector*) table.openFiles[pos].header.data)->sectQty;
	table.openFiles[pos].nextSector =
			((fragSector*) table.openFiles[pos].header.data)->nextSect;
	table.openFiles[pos].currSector = table.openFiles[pos].header.dataSect;
	table.openFiles[pos].firstSector = table.openFiles[pos].header.dataSect;

	return;
}

void copyHeader(int pos, generalHeader* file) {
	strcpy(file->owner, table.openFiles[pos].header.owner);
	strcpy(file->group, table.openFiles[pos].header.group);
	table.openFiles[pos].header.permissions = file->permissions;
	table.openFiles[pos].header.data = NULL;
	table.openFiles[pos].header.realSize = file->realSize;
	table.openFiles[pos].header.diskSize = file->realSize;
	table.openFiles[pos].header.numSect = file->numSect;
	table.openFiles[pos].header.dataSect = file->dataSect;
	table.openFiles[pos].header.inodeNumber = file->inodeNumber;
	table.openFiles[pos].header.cantFiles = file->cantFiles;
	table.openFiles[pos].header.write = NULL;
	table.openFiles[pos].header.read = NULL;
	table.openFiles[pos].header.type = file->type;
}

void closeInstance(int pos) {
	table.openFiles[pos].counter--;

	return;
}

void close(int fd) {
	int pos = getPos(fd);
	if (pos == -1)
		return;
	closeInstance(pos);

	if (!existInstanceOpen(pos)) {
		closeFile(pos);
	}

	deleteFDFromProc(fd);

	return;

}

void closeFile(int pos) {

	table.openFiles[pos].inodeNumber = -1;
}
int existInstanceOpen(int pos) {
	return table.openFiles[pos].counter;
}

void printHeader(generalHeader* header) {
	printf("Group:%s  ", header->group);
	printf("Owner:%s\n", header->owner);
	printf("P:%d ", header->permissions);
	printf("NSect:%d ", header->numSect);
	printf("DSect:%d  ", header->dataSect);
	printf("#Files:%d  ", header->cantFiles);
	printf("#Inode: %d ", header->inodeNumber);
	printf("Type:%d\n", header->type);
}

void printFile(file* file) {
	printf("%s  \n", file->name);
}

int createFifo(char* name, generalHeader* prev, int perms) {
	generalHeader new;
	if (prev->type != DIR
	)
		return -1;
	createNewHeader(&new, name, prev, FIFO, perms);
	if (new.dataSect == -1)
		return -1;

	createNewEntry(name, new.inodeNumber, new.numSect, prev);

	refreshSuperBlock();

	return 1;

}

void listDirFiles(generalHeader* dir, int showAll) {

	if (!(dir->permissions & IS_DIREC))
		return;
	file files[8];
	int i = 0;
	int cantSects = 0;
	int pos = 0;
	int k = 0;
	for (i = 0; k < dir->cantFiles && cantSects < 8; i++, pos++, k++) {
		if (i % FILES_PER_SECTOR == 0) {
			diskRead((char*) &files, SECTOR_SIZE, (dir->dataSect) + cantSects,
					0);
			cantSects++;
			pos = 0;
		}
		if (files[pos].numInode == -1) {
			k--;
		} else {
			if (showAll) {
				printPermissions(
						searchHeader(files[pos].numSect, files[pos].numInode));
				printFile(&files[pos]);
			} else {
				if (!startsWith(files[pos].name, 1, "."))
					printFile(&files[pos]);
			}
		}
	}

	return;

}

void printPermissions(generalHeader* inode) {
	if (inode->permissions & IS_DIREC
	)
		printf("d");
	else
		printf("-");
	if (inode->permissions & USR_RD_PERM
	)
		printf("r");
	else
		printf("-");
	if (inode->permissions & USR_WR_PERM
	)
		printf("w");
	else
		printf("-");
	if (inode->permissions & USR_EX_PERM
	)
		printf("x");
	else
		printf("-");
	if (inode->permissions & GRP_RD_PERM
	)
		printf("r");
	else
		printf("-");
	if (inode->permissions & GRP_WR_PERM
	)
		printf("w");
	else
		printf("-");
	if (inode->permissions & GRP_EX_PERM
	)
		printf("x");
	else
		printf("-");
	if (inode->permissions & WRL_RD_PERM
	)
		printf("r");
	else
		printf("-");
	if (inode->permissions & WRL_WR_PERM
	)
		printf("w");
	else
		printf("-");
	if (inode->permissions & WRL_EX_PERM
	)
		printf("x");
	else
		printf("-");
	printf("\t");
	return;
}

generalHeader * getParentHeader(generalHeader * dir) {
	if (!(dir->permissions & IS_DIREC))
		return NULL;
	file files[8];
	int i = 0;
	int cantSects = 0;
	int pos = 0;
	int k = 0;
	for (i = 0; k < dir->cantFiles && cantSects < 8; i++, pos++, k++) {
		if (i % FILES_PER_SECTOR == 0) {
			diskRead((char*) &files, SECTOR_SIZE, (dir->dataSect) + cantSects,
					0);
			cantSects++;
			pos = 0;
		}
		if (files[pos].numInode == -1) {
			k--;
		} else if (strcmp(files[pos].name, "..")) {
			return searchHeader(files[pos].numSect, files[pos].numInode);
		}
	}

	return NULL;
}

void printChildName(int childINode, generalHeader * dir) {
	if (!(dir->permissions & IS_DIREC))
		return;

	printHeader(dir);
	file files[8];
	int i = 0;
	int cantSects = 0;
	int pos = 0;
	int k = 0;
	for (i = 0; k < dir->cantFiles && cantSects < 8; i++, pos++, k++) {
		printf("For\n");
		if (i % FILES_PER_SECTOR == 0) {
			diskRead((char*) &files, SECTOR_SIZE, (dir->dataSect) + cantSects,
					0);
			cantSects++;
			pos = 0;
		}
		if (files[pos].numInode == -1) {
			k--;
		} else if (files[pos].numInode == childINode) {
			printf("/%s", files[pos].name);
			return;
		}
	}

	return;
}

void pwdRec(int childINode, generalHeader * myHeader) {

	printHeader(myHeader);

	printf("En la recursiva\n");
	if (myHeader->inodeNumber == root->inodeNumber) {
		return;
	}

	generalHeader * parent = getParentHeader(myHeader);
	pwdRec(myHeader->inodeNumber, parent);
	if (childINode != -1) {
		printChildName(childINode, myHeader);
	} else
		printf("\n");
}
void pwd() {

	pwdRec(-1, actDir);

}

int rm(int unused, char** arguments) {

	int recursive = 0;
	char * pathname = arguments[0];
	int i = 2;

	//move to parameters
	while (pathname[i] != ' ')
		i++;
	i++;

	if (pathname[i] != '-') {
		recursive = 0;
	} else {
		recursive = 1;
		if (pathname[i + 1] == '-'
				&& startsWith(&(pathname[i + 2]), strlen("recursive"),
						"recursive")) {
			//move to path
			while (pathname[i] != ' ')
				i++;
			while (pathname[i] == ' ')
				i++;
		} else {
			if (pathname[i + 1] == 'r' || pathname[i + 1] == 'R') {
				i += 2;
				while (pathname[i] == ' ')
					i++;
			} else {
				return -1;
			}
		}
	}
	generalHeader** start = startPoint(&(pathname[i]));
	if (start == NULL) {
		return 0;
	}

	if (recursive) {
		recursiveDelete(start);
	} else {

		delet(start);
	}

	return 0;
}

void recursiveDelete(generalHeader** start) {

	if (start[0]->type != DIR) {
		delet(start);
		return;
	}

	deleteEntry(start[1], start[0]->inodeNumber);
	deleteChilds(start[0]);
	deleteHeader(start[0]);

}

void deleteChilds(generalHeader* deleteFrom) {
	generalHeader header;
	file files[8];
	generalHeader* newStart[2];
	newStart[1] = deleteFrom;

	int i = 0;
	int cantSects = 0;
	int pos = 0;
	int k = 0;
	for (i = 0; k < deleteFrom->cantFiles && cantSects < 8; i++, pos++, k++) {
		if (i % FILES_PER_SECTOR == 0) {
			diskRead((char*) &files, SECTOR_SIZE,
					(deleteFrom->dataSect) + cantSects, 0);
			cantSects++;
			pos = 0;
		}
		if (files[pos].numInode == -1) {
			k--;
		} else if (!strcmp(".", files[pos].name)
				&& !strcmp("..", files[pos].name)) {
			diskRead((char*) &header, sizeof(generalHeader), files[pos].numSect,
					sizeof(generalHeader) * (files[pos].numInode % 8));
			newStart[0] = &header;
			recursiveDelete(newStart);

		}
	}

	return;

}
void delet(generalHeader** start) {
	if (start[0]->permissions & IS_DIREC) {
		return;
	}
	if (deleteEntry(start[1], start[0]->inodeNumber)) {

		deleteHeader(start[0]);
	}
}

void deleteHeader(generalHeader* toDelete) {

	deleteDataFromFile(toDelete);

	filesys.headers[toDelete->inodeNumber / 8] -= pow(2,
			(toDelete->inodeNumber % 8));
	filesys.inodesQty--;
	refreshSuperBlock();
}

void deleteDataFromFile(generalHeader* toDelete) {

	int sectQty = 0;
	int nextSect = 0;
	fragSector sect;
	if (toDelete->type == FILE) {
		while (nextSect != -1) {
			diskRead((char*) &sect, sizeof(fragSector), toDelete->dataSect, 0);
			sectQty = sect.sectQty;
			nextSect = sect.nextSect;
			freeSectors(toDelete->dataSect, sectQty);
		}
	} else
		freeSectors(toDelete->dataSect, 7);
	return;

}

void freeSectors(int from, int qty) {
	int pos = from - START_FILES;
	int blocks = (qty + 1) / 8;
	int i = 0;
	for (i = 0; i < blocks; i++) {

		filesys.bitMap[(pos / 8) + i] = UNUSED;
	}
	filesys.freeBlockQty += qty;
}

int deleteEntry(generalHeader* dir, int childInode) {

	file files[8];

	int i = 0;
	int cantSects = 0;
	int pos = 0;
	int k = 0;
	for (i = 0; k < dir->cantFiles && cantSects < 8; i++, pos++, k++) {
		if (i % FILES_PER_SECTOR == 0) {
			diskRead((char*) &files, SECTOR_SIZE, (dir->dataSect) + cantSects,
					0);
			cantSects++;
			pos = 0;
		}
		if (files[pos].numInode == -1) {
			k--;
		} else if (files[pos].numInode == childInode) {
			files[pos].numInode = -1;
			dir->cantFiles--;
			writeHeaderToDisk(dir);
			diskWrite((char*) &files, SECTOR_SIZE,
					(dir->dataSect) + cantSects - 1, 0);
			if (files[pos].numSect == -1)
				return 0;
			return 1;

		}
	}

	return -2;
}

generalHeader** startPoint(char* pathname) {
	int i = 0;
	char name[MAX_NAME_LENGHT];
	int flag = 1;
	generalHeader** resp = Malloc(sizeof(generalHeader) * 2);
	if (pathname[i] == '/') {
		pathname += 1;
		resp[0] = resp[1] = root;
	} else {
		resp[0] = resp[1] = actDir;
	}
	while (pathname[i] != '\0' && flag) {

		getNameFromPath(pathname, &i, name);
		resp[0] = getHeader(resp[0], name);
		if (resp[0] == NULL) {
			flag = 0;
		} else if (pathname[i] != '\0')
			resp[1] = resp[0];
	}
	if (flag) {

		return resp;
	}

	return NULL;
}

int writeInFifo(int pos, char* buff, int size) {
	up_Sem(wrFifoSem, 1);

	int i;
	for (i = 0; i < size; i++) {

		if (table.openFiles[pos].pos == table.openFiles[pos].pos2) {
			if (i != 0) {
				return i;
			}
		}
		table.openFiles[pos].header.write[table.openFiles[pos].pos2] = buff[i];
		table.openFiles[pos].pos2++;

		if (table.openFiles[pos].pos2 == 4 * SECTOR_SIZE
		)
			table.openFiles[pos].pos2 = 0;
		up_Sem(wrFifoSem, 1);
	}

	return size;
}
int readFromFifo(int pos, char* buff, int size) {
	down_Sem(wrFifoSem, 1);
	down_Sem(wrFifoSem, 1);

	int i;
	char c;
	for (i = 0; i < size; i++) {
		c = table.openFiles[pos].header.read[table.openFiles[pos].pos];
		if (table.openFiles[pos].pos == table.openFiles[pos].pos2) {
			if (i != 0) {
				buff[i] = '\0';
				return i;
			}
		}
		buff[i] = c;
		table.openFiles[pos].pos++;
		if (table.openFiles[pos].pos == 4 * SECTOR_SIZE
		)
			table.openFiles[pos].pos = 0;
		down_Sem(wrFifoSem, 1);

	}
	buff[i] = '\0';
	return size;
}

