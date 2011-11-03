#ifndef _FILESYSTEM_
#define _FILESYSTEM_

/* File system in memory, not used*/

#include "defs.h"

#define MAX_LENGHT 255
#define MAX_OWNER_LENGHT 30
#define MAX_GROUP_LENGHT 30
#define MAX_FILES_PER_DIR 20
#define MAX_FILES_IN_FILESYSTEM 300
#define MAX_HEIGHT 10
#define MAX_OPEN_FILES 50
#define MAX_SIZE_PER_FILE 1024
/*-------- PERMISSIONS------*/
#define IS_DIREC 1<<9
#define USR_RD_PERM 1
#define USR_WR_PERM 1<<1
#define USR_EX_PERM 1<<2
#define GRP_RD_PERM 1<<3
#define GRP_WR_PERM 1<<4
#define GRP_EX_PERM 1<<5
#define WRL_RD_PERM 1<<6
#define WRL_WR_PERM 1<<7
#define WRL_EX_PERM 1<<8
#define DIR_PERMS IS_DIREC|USR_RD_PERM|USR_WR_PERM|USR_EX_PERM|GRP_RD_PERM|GRP_EX_PERM|WRL_RD_PERM|WRL_EX_PERM
#define BLOCK 20
/*----------FLAGS------------*/
#define O_RDONLY 1
#define O_WRONLY 1<<1
#define O_RDWR 1<<2
#define O_CREATE 1<<3

/*----------WHENCE-----------*/
#define SEEK_CUR 1
#define SEEK_SET 0

typedef struct {
	char name[MAX_LENGHT];
	uint inodeNumber;
} fileInode;

typedef struct {
	int accPermission;
	char owner[MAX_OWNER_LENGHT];
	char group[MAX_GROUP_LENGHT];
	fileInode* files[MAX_FILES_PER_DIR];
	int filesQty;
	uint inodeNumber;

} Inode;

typedef struct {
	int inodeNumber;
	Inode* file;
	int counter;
	char* actPos;
	char* initPos;

} OpenFile;

typedef struct {
	uint inodeNumber;
	char file[MAX_SIZE_PER_FILE];
	struct File* next;
} File;

typedef struct {
	Inode* root;
	Inode* actualDir;
	Inode* inodes[MAX_FILES_IN_FILESYSTEM];
	char pwd[MAX_LENGHT * MAX_HEIGHT];
	uint inodesQty;
	OpenFile* openFiles[MAX_OPEN_FILES];
	File* firstFile;

} FileSystem;

FileSystem* sys;

void pwd();
int cd(char* argument);
void ls(char* arguments);
void initializeFilesystem();
Inode* createRoot();
void
initializeFiles(Inode* inode, uint selfInodeNumber, uint parentInodeNumber);
fileInode* createFileInode(char* name, uint inodeNumber);
fileInode* createSelfFileInode(uint inodeNumber);
fileInode* createParentFileInode(uint parentInodeNumber);
Inode* searchPath(char* path);
Inode* searchPathRec(Inode* start, char* path, int * counter);
Inode* getInode(Inode* from, char* name);
char* getNameFromPath(char* path, int* counter);
int mkdir(char* pathname);
int createDir(char*name, Inode* parent);
void printPermissions(Inode* inode);
void printFiles(Inode* inode);
void printInode(Inode* inode);
int createFile(char* name, int mode, Inode* parent);
void insertFile(int inodeNumber);
int openFile(int inode);
char* getDirection(uint inode);
int creat(char* pathname, int mode);
OpenFile* createOpenFile(int inode);
int open(char* pathname, int flags, ...);
void printOpenFiles();
void printExistentFiles();
void close(int fd);
void closeFile(int inode);
int readFromFile(OpenFile* file, char* buff, int size);
int Read(int fd, char* buff, int size);
int writeInFile(OpenFile* file, char* buff, int size);
OpenFile* getFile(int pos);
int Write(int fd, char* buff, int size);
OpenFile* getFileFromFD(int fd);
int lseek(int fd, int offset, int whence);
#endif
