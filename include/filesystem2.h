#ifndef _FILESYSTEM_
#define _FILESYSTEM_

/* File system on disk*/

#define MAX_NAME_LENGHT 56
#define MAX_OWNER_LENGHT 13
#define MAX_GROUP_LENGHT 13
#define MAX_OPEN_FILES 200
#define MAX_BLOCK_QTY 32704
#define HEADERS 18

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

/*---------BITS-------*/
#define BIT0 1
#define BIT1 1<<1
#define BIT2 1<<2
#define BIT3 1<<3
#define BIT4 1<<4
#define BIT5 1<<5
#define BIT6 1<<6
#define BIT7 1<<7
#define ALL_USED BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7
#define UNUSED 0

/*----------WHENCE-----------*/
#define SEEK_CUR 1
#define SEEK_SET 0
#define SEEK_END 2
#define SUPERBLOCK 64
#define START_HEADERS 72
#define START_FILES 216

#define DIR 0
#define FILE 1
#define FIFO 2
#define FILES_PER_SECTOR 8
#define HEADERS_PER_SECTOR 8

#define SECTOR_SIZE 512

typedef struct {
	char group[MAX_GROUP_LENGHT];
	char owner[MAX_OWNER_LENGHT];
	char type;
	char cantFiles;
	int permissions;
	char* data;
	int realSize;
	int diskSize;
	int dataSect;
	int numSect;
	int inodeNumber;
	char* write;
	char* read;

} generalHeader;
/* Size 64, can be put up to 8 per sector */

typedef struct {
	char name[MAX_NAME_LENGHT];
	int numSect;
	int numInode;
} file;
/* Size 64, can be put up to 8 per sector */

typedef struct {
	int sectQty;
	int nextSect;
	char info[504];
} fragSector;

typedef struct {
	char bitMap[MAX_BLOCK_QTY];
	char headers[HEADERS];
	char unused[26];
	int rootSect;
	int actDirSect;
	int inodesQty;
	int freeBlockQty;
	int firstDataSect;

} superBlock;

typedef struct {
	char sector[SECTOR_SIZE];
} sector;

typedef struct {
	int inodeNumber;
	int counter;
	generalHeader header;
	int pidReader;
	int pidWriter;
	int pos;
	int pos2;
	int sectorQty;
	int nextSector;
	int currSector;
	int firstSector;
} openFile;

typedef struct {
	openFile openFiles[MAX_OPEN_FILES];
} openFilesTable;

void createRoot();
void createFirstFiles(int sect, int mySect, int myInodeNum, int parentSect,
		int parentInode);
generalHeader* searchHeader(int numSect, int numInode);
int createDir(char* name, generalHeader* prev);
void createNewHeader(generalHeader* newHeader, char* name, generalHeader* prev,
		char type, int perms);
generalHeader* getHeader(generalHeader* parent, char* name);
int getHeaderInode();
void writeHeaderToDisk(generalHeader* header);
int getHeaderSect(int inode);
void createNewEntry(char* name, int inodeNumber, int numSect,
		generalHeader* dir);
void getNameFromPath(char* path, int* counter, char* name);
void pwd();
void refreshSuperBlock();
generalHeader* searchPathRec(generalHeader* start, char* path, int * counter);
int getFreeBlocks();
generalHeader* searchPath(char* path);
int createFile(char* name, int mode, generalHeader* parent);
int slotForFile();
int openTheFile(int inode, int slot, generalHeader* file, int flag);
void newOpenFile(int inode, generalHeader* file, int pos, int flag);
int cd(int argc, char** argument);
int writeInFile(int pos, char* buff, int size);
void writeBuffToDisk(int pos);
int getNewBuffer(int pos);
int getMoreSpace(int pos);
int readFromFile(int pos, char* buff, int size);
int ls(int argc, char** arguments);
int create(char* pathname, int mode, int type);
void initializeFilesystem();
int mkdir(char* pathname);
int creat(char* pathname, int mode);
int open(char* pathname, int flags, ...);
void close(int fd);
void closeFile(int fd);
void cleanDisk();
int Read(int fd, char* buff, int size);
int Write(int fd, char* buff, int size);
int lseek(int fd, int offset, int whence);
void copyHeader(int pos, generalHeader* file);
int existInstanceOpen(int fd);
void closeInstance(int fd);
void printHeader(generalHeader* header);
void printFile(file* file);
int mkfifo(char* pathname, int mode);
int createFifo(char* name, generalHeader* prev, int perms);
void initializeSTDIO();
void listDirFiles(generalHeader* dir, int showAll);
void printPermissions(generalHeader* inode);
generalHeader** startPoint(char* pathname);
int deleteEntry(generalHeader* father, int childInode);
void freeSectors(int from, int qty);
void deleteDataFromFile(generalHeader* toDelete);
void deleteHeader(generalHeader* toDelete);
void delet(generalHeader** start);
void initializeDataSect(int dataSect);
int rm(int unused, char** arguments);
void deleteChilds(generalHeader* deleteFrom);
void recursiveDelete(generalHeader** start);
int writeInFifo(int pos, char* buff, int size);
void loadSuperBlock();
int existsFilesystem();
void loadRoot();
int readFromFifo(int pos, char* buff, int size);
int getFilePos(generalHeader* dir);
#endif
