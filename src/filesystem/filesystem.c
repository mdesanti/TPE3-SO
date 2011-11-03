#include "../../include/defs.h"
#include "../../include/filesystem.h"
#include "../../include/calls.h"
#include "../../include/scheduler.h"
#include "../../include/iolib.h"
#include "../../include/malloc.h"
#include "../../include/string.h"
#include <stdarg.h>

/* Implementation of file system in memory */

#define NULL 0

void pwd() {
	printf("%s \n", sys->pwd);

}

int cd(char* argument) {

	Inode* resp = searchPath(argument);
	if (resp == NULL
		)
		return -1;
	sys->actualDir = resp;

	return 1;
}

void ls(char* arguments) {

}

void initializeFilesystem() {
	int i = 0;
	sys = (FileSystem*) Malloc(sizeof(FileSystem));
	sys->root = createRoot();
	sys->firstFile = NULL;
	for (i = 0; i < MAX_OPEN_FILES; i++) {
		sys->openFiles[i] = NULL;
	}
	sys->actualDir = sys->root;
	sys->inodesQty = 0;
	sys->inodes[(sys->inodesQty)++] = sys->root;

	strcpy("\\", sys->pwd);

}

Inode* createRoot() {
	Inode* root = (Inode *) Malloc(sizeof(Inode));
	root->accPermission = DIR_PERMS;
	strcpy("root", root->owner);
	strcpy("root", root->group);
	initializeFiles(root, 0, 0);
	root->inodeNumber = 0;
	return root;

}

void initializeFiles(Inode* inode, uint selfInodeNumber, uint parentInodeNumber) {
	inode->files[0] = createSelfFileInode(selfInodeNumber);
	inode->files[1] = createParentFileInode(parentInodeNumber);
	inode->filesQty = 2;
}

fileInode* createFileInode(char* name, uint inodeNumber) {
	fileInode* resp = (fileInode *) Malloc(sizeof(fileInode));
	resp->inodeNumber = inodeNumber;
	strcpy(name, resp->name);
	return resp;
}

fileInode* createSelfFileInode(uint inodeNumber) {
	return createFileInode(".", inodeNumber);
}
fileInode* createParentFileInode(uint parentInodeNumber) {
	return createFileInode("..", parentInodeNumber);
}

int mkdir(char* pathname) {
	int i = 0;
	int flag = 1;
	Inode* actual, *prev;
	char*name;

	if (pathname[i] == '/') {
		pathname += 1;
		actual = prev = sys->root;
	} else {
		actual = prev = sys->actualDir;
	}
	while (pathname[i] != '\0' && flag) {
		name = getNameFromPath(pathname, &i);
		actual = getInode(prev, name);
		if (actual == NULL) {
			flag = 0;
		} else
			prev = actual;
	}
	if (!flag) {
		i -= strlen(name);
		while (pathname[i] != '\0')
			if (pathname[i++] == '/')
				return -1;
		return createDir(name, prev);
	}
	return -1;
}

int createDir(char*name, Inode* parent) {
	if (!(parent->accPermission & IS_DIREC)) {
		return -1;
	}
	char* ownerAndGroup = "JORGE";
	Inode* newDir = (Inode *) Malloc(sizeof(Inode));
	newDir->accPermission = DIR_PERMS;
	strcpy(ownerAndGroup, newDir->owner);
	strcpy(ownerAndGroup, newDir->group);
	newDir->inodeNumber = (sys->inodesQty);
	initializeFiles(newDir, newDir->inodeNumber, parent->inodeNumber);
	parent->files[(parent->filesQty)++] = createFileInode(name,
			newDir->inodeNumber);
	sys->inodes[(sys->inodesQty)++] = newDir;

	return 1;
}

Inode* searchPath(char* path) {
	Inode* resp;
	int counter = 0;
	if (path[0] == '/') {
		counter = 1;
		resp = searchPathRec(sys->root, path, &counter);
	} else if (path[0] == '\\' && path[1] == '/') {
		counter = 2;
		resp = searchPathRec(sys->root, path, &counter);
	} else if (path[0] == '.' && path[1] == '/') {
		counter = 2;
		resp = searchPathRec(sys->actualDir, path, &counter);
	} else
		resp = searchPathRec(sys->actualDir, path, &counter);

	return resp;
}

Inode* searchPathRec(Inode* start, char* path, int * counter) {
	char* name = getNameFromPath(path, counter);
	char * dot = NULL;
	char * bar = NULL;
	Inode* resp;

	*dot = '.';
	*bar = '/';

	if (strcmp(name, dot)) {
		if (path[(*counter) + 1] == *bar) {
			(*counter) += 2;
			return searchPathRec(start, path, counter);
		} else
			return start;
	}
	if (strcmp(name, "..")) {
		if (path[(*counter) + 2] == *bar) {
			(*counter) += 3;
			return searchPathRec(sys->inodes[start->files[0]->inodeNumber],
					path, counter);
		} else
			return sys->inodes[start->files[0]->inodeNumber];
	}

	resp = getInode(start, name);

	if (resp == NULL
		)
		return NULL;
	if (path[*counter] == '\0')
		return resp;
	(*counter)++;
	return searchPathRec(resp, path, counter);

}

Inode* getInode(Inode* from, char* name) {
	int i = 0;
	for (i = 0; i < from->filesQty; i++) {
		if (strcmp(from->files[i]->name, name)) {
			return sys->inodes[from->files[i]->inodeNumber];
		}
	}
	return NULL;
}

char* getNameFromPath(char* path, int* counter) {
	char* name = (char *) Malloc(MAX_LENGHT);
	int i = 0;
	while (path[*counter] != '\0' && path[*counter] != '/') {
		name[i++] = path[(*counter)++];
	}
	name[i++] = '\0';
	if (path[*counter] == '/')
		(*counter)++;
	return name;
}

void printInode(Inode* inode) {
	printPermissions(inode);
	printf("%s\n", inode->owner);
	printf("%s\n", inode->group);
	printFiles(inode);

}

void printFiles(Inode* inode) {
	int i = 0;
	printf("El directorio tiene %d archivos, y son\n", inode->filesQty);
	for (; i < inode->filesQty; i++) {
		printf("%d | %s \n", inode->files[i]->inodeNumber,
				inode->files[i]->name);
	}
}

void printPermissions(Inode* inode) {
	printf("Los permisos de acceso son: \n");
	if (inode->accPermission & IS_DIREC
		)
		printf("d");
	else
		printf("-");
	if (inode->accPermission & USR_RD_PERM
		)
		printf("r");
	else
		printf("-");
	if (inode->accPermission & USR_WR_PERM
		)
		printf("w");
	else
		printf("-");
	if (inode->accPermission & USR_EX_PERM
		)
		printf("x");
	else
		printf("-");
	if (inode->accPermission & GRP_RD_PERM
		)
		printf("r");
	else
		printf("-");
	if (inode->accPermission & GRP_WR_PERM
		)
		printf("w");
	else
		printf("-");
	if (inode->accPermission & GRP_EX_PERM
		)
		printf("x");
	else
		printf("-");
	if (inode->accPermission & WRL_RD_PERM
		)
		printf("r");
	else
		printf("-");
	if (inode->accPermission & WRL_WR_PERM
		)
		printf("w");
	else
		printf("-");
	if (inode->accPermission & WRL_EX_PERM
		)
		printf("x");
	else
		printf("-");
	printf("\n");
	return;
}

int creat(char* pathname, int mode) {

	int i = 0;
	int flag = 1;
	Inode* actual, *prev;
	char*name;

	if (pathname[i] == '/') {
		pathname += 1;
		actual = prev = sys->root;
	} else {
		actual = prev = sys->actualDir;
	}
	while (pathname[i] != '\0' && flag) {
		name = getNameFromPath(pathname, &i);
		actual = getInode(prev, name);
		if (actual == NULL) {
			flag = 0;
		} else
			prev = actual;
	}
	if (!flag) {
		i -= strlen(name);
		while (pathname[i] != '\0')
			if (pathname[i++] == '/')
				return -1;
		return createFile(name, mode, prev);
	}
	return -1;
}

int createFile(char* name, int mode, Inode* parent) {
	if (!(parent->accPermission & IS_DIREC)) {
		return -1;
	}
	Inode* newFile = (Inode *) Malloc(sizeof(Inode));
	newFile->inodeNumber = (sys->inodesQty);
	int fd = getFD(openFile(newFile->inodeNumber));
	if (fd == -1)
		return fd;
	char* ownerAndGroup = "JORGE";
	newFile->accPermission = mode;
	strcpy(ownerAndGroup, newFile->owner);
	strcpy(ownerAndGroup, newFile->group);
	newFile->filesQty = 0;
	parent->files[(parent->filesQty)++] = createFileInode(name,
			newFile->inodeNumber);
	sys->inodes[(sys->inodesQty)++] = newFile;

	insertFile(newFile->inodeNumber);

	return fd;

}
// Simula ser el disco , se "guarda "el archivo en diso
void insertFile(int inodeNumber) {
	File* act, *prev;
	File* toInsert = (File*) Malloc(sizeof(File));
	toInsert->inodeNumber = inodeNumber;
	if (sys->firstFile == NULL) {
		sys->firstFile = toInsert;
		toInsert->next = NULL;
		return;
	}
	act = prev = sys->firstFile;
	while (act != NULL) {
		if (act->inodeNumber > inodeNumber) {
			prev->next = (struct File*) toInsert;
			toInsert->next = (struct File*) act;
			return;
		}
		prev = act;
		act = (File*) act->next;
	}
	prev->next = (struct File*) toInsert;
	toInsert->next = NULL;
	return;
}

//
int openFile(int inode) {
	int nullPos;
	int i = 0;
	for (i = 0; i < MAX_OPEN_FILES; i++) {
		if ((!sys->openFiles[i] == NULL)) {
			if (sys->openFiles[i]->inodeNumber == inode) {
				(sys->openFiles[i]->counter)++;
				return i;
			}
		} else
			nullPos = i;
	}

	sys->openFiles[nullPos] = createOpenFile(inode);
	return nullPos;
}

OpenFile* createOpenFile(int inode) {
	OpenFile* file = (OpenFile*) Malloc(sizeof(OpenFile));
	file->inodeNumber = inode;
	file->file = sys->inodes[inode];
	file->counter = 1;
	file->actPos = file->initPos = getDirection(inode);
	return file;
}

char* getDirection(uint inode) {
	File* act;
	act = sys->firstFile;
	while (act != NULL) {
		if (act->inodeNumber == inode)
			return act->file;
		act = (File*) act->next;
	}
	return NULL;
}

int open(char* pathname, int flags, ...) {
	int perms;
	int fd;
	//CHECKEAR PERMISOS
		Inode* file = searchPath(pathname);
		if (file == NULL) {
			if (!(flags & O_CREATE))
			return -1;
			else {
				va_list list;
				va_start(list, flags);
				perms=va_arg(list,int);

				if ((fd = creat(pathname, perms)) == -1)
				return -1;
				file = sys->inodes[sys->inodesQty - 1];
				return fd;
			}
		}
		fd = getFD(openFile(file->inodeNumber));
		return fd;
	}

void printOpenFiles() {
	printf("PRINTING OPEN FILES\n");
	int i = 0;
	for (i = 0; i < MAX_OPEN_FILES; i++) {
		if (sys->openFiles[i] != NULL) {
			printf("Inode: %d   ", sys->openFiles[i]->inodeNumber);
			printf("Counter: %d \n", sys->openFiles[i]->counter);
		}
	}
}

void printExistentFiles() {
	printf("PRINTING FILES\n");
	File* file = sys->firstFile;
	while (file != NULL) {
		printf("Inode :%d\n", file->inodeNumber);
		file = (File*) file->next;
	}
}

int Write(int fd, char* buff, int size) {
	if (fd == 1 || fd == 2) {
		write(buff, size, 1);
		return size;
	}
	OpenFile* file = getFileFromFD(fd);
	return writeInFile(file, buff, size);

}

OpenFile* getFile(int pos) {
	return sys->openFiles[pos];
}

int writeInFile(OpenFile* file, char* buff, int size) {
	int i;
	for (i = 0; i < size; i++) {
		file->actPos[i] = buff[i];
	}
	file->actPos[i] = '\0';
	file->actPos += i;
	return size;

}

int Read(int fd, char* buff, int size) {
	OpenFile* file = getFileFromFD(fd);

	return readFromFile(file, buff, size);
}
int readFromFile(OpenFile* file, char* buff, int size) {
	int i;

	for (i = 0; i < size; i++) {
		if (file->actPos[i] == '\0') {
			buff[i] = '\0';
			file->actPos += i;
			return i;
		} else {
			buff[i] = file->actPos[i];
		}
	}
	buff[i] = '\0';
	file->actPos += i;
	return i;
}
OpenFile* getFileFromFD(int fd) {
	OpenFile* file;
	int pos = getPos(fd);
	if (pos != -1)
		file = getFile(pos);
	return file;
}

int lseek(int fd, int offset, int whence) {
	OpenFile* file = getFileFromFD(fd);

	switch (whence) {
	case SEEK_CUR:
		file->actPos += offset;
		if (file->actPos < file->initPos)
			file->actPos = file->initPos;
		return offset;
	case SEEK_SET:
		if (offset >= 0)
			file->actPos = file->initPos + offset;
		return offset;
	default:
		return -1;
	}

}

void close(int fd) {
	OpenFile* file = getFileFromFD(fd);

	(file->counter)--;
	if (!file->counter)
		closeFile(fd);

	deleteFDFromProc(fd);

	return;

}

void closeFile(int fd) {
	int pos = getPos(fd);
	sys->openFiles[pos] = NULL;
}
