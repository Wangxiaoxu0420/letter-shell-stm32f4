/**
 * @file shell_fs.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell file system support
 * @version 0.1
 * @date 2020-07-22
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#ifndef __SHELL_FS_H__
#define __SHELL_FS_H__

#include "stddef.h"
#include "shell.h"
#include "fattester.h"
#include "ff.h"

#define     SHELL_FS_VERSION                "1.0.0"

#define     SHELL_COMPANION_ID_FS           -1

#define     SHELL_FS_LIST_FILE_BUFFER_MAX   4096

/**
 * @brief shell文件系统支持结构体
 * 
 */
typedef struct shell_fs
{
    FRESULT (*getcwd)(char *, size_t);
    FRESULT (*chdir)(char *);
    size_t (*listdir)(char *dir);
    FRESULT (*mkdir)(char *dir);
    FRESULT (*remove)(char *file);
    FRESULT (*rename)(char *oldName, char *newName);
    FRESULT (*stat)( const TCHAR* path,	/* Pointer to the file path */
	                FILINFO* fno);		/* Pointer to file information to return */

    FRESULT (*open)( FIL* fp,			/* Pointer to the blank file object */
                    TCHAR* path,	/* Pointer to the file name */
                    BYTE mode);			/* Access mode and file open mode flags */

    FRESULT (*close)( FIL* fp );

    FRESULT (*read)( FIL* fp, 	/* Pointer to the file object */
                    void* buff,	/* Pointer to data buffer */
                    UINT btr,	/* Number of bytes to read */
                    UINT* br	/* Pointer to number of bytes read */);

    FRESULT (*write)( FIL* fp,			/* Pointer to the file object */
                     const void* buff,	/* Pointer to the data to be written */
                     UINT btw,			/* Number of bytes to write */
                     UINT* bw);			/* Pointer to number of bytes written */
    size_t (*showfree)(void);
    void * (*malloc)(u32 size);
    size_t (*free)(void *ptr);
    struct {
        char *path;
        size_t pathLen;
    } info;
} ShellFs;

void shellFsInit(ShellFs *shellFs, char *pathBuffer, size_t pathLen);

#endif
