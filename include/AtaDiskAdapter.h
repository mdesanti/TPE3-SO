/*
 * fileSystemDiskAtaAdapter.h
 *
 *  Created on: Oct 26, 2011
 *      Author: mdesanti90
 */

#ifndef FILESYSTEMDISKATAADAPTER_H_
#define FILESYSTEMDISKATAADAPTER_H_

/*
 * This interface is used to read from disk "bytes" that are in consecutive sectors
 */

void diskWrite(char * msg, int bytes, int sector, int offset);

void diskRead(char * ans, int bytes, int sector, int offset);

#endif /* FILESYSTEMDISKATAADAPTER_H_ */
