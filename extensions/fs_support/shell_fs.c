/**
 * @file shell_fs.c
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell file system support
 * @version 0.1
 * @date 2020-07-22
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#include "shell_fs.h"
#include "shell.h"
#include "stdio.h"

/**
 * @brief 改变当前路径(shell调用)
 * 
 * @param dir 路径
 */
void shellCD(char *dir)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);
    if (shellFs->chdir(dir) != 0)
    {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, dir);
        shellWriteString(shell, " is not a directory\r\n");
    }
    shellFs->getcwd(shellFs->info.path, shellFs->info.pathLen);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
cd, shellCD, change dir);

/**
 * @brief 列出文件(shell调用)
 * 
 */
void shellLS(void)
{
    //size_t count;
    //char *buffer;

    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    //buffer = SHELL_MALLOC(SHELL_FS_LIST_FILE_BUFFER_MAX);
    //SHELL_ASSERT(buffer, return);
    //count = shellFs->listdir(shellGetPath(shell), buffer, SHELL_FS_LIST_FILE_BUFFER_MAX);
    
	  shellFs->listdir(shellGetPath(shell));
    
	  //shellWriteString(shell, buffer);

    //SHELL_FREE(buffer);
}
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
ls, shellLS, list all files);

/**
 * @brief 初始化shell文件系统支持
 * 
 * @param shellFs shell文件系统对象
 * @param pathBuffer shell路径缓冲
 * @param pathLen 路径缓冲区大小
 */
void shellFsInit(ShellFs *shellFs, char *pathBuffer, size_t pathLen)
{
    shellFs->info.path = pathBuffer;
    shellFs->info.pathLen = pathLen;
    shellFs->getcwd(shellFs->info.path, pathLen);
}



/**
 * @brief 创建目录
 * 
 * @param dir 路径
 */
void shellMKDIR(char *dir)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);


    if (shellFs->mkdir(dir) != 0)
    {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, dir);
        shellWriteString(shell, "directory make failed\r\n");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
mkdir, shellMKDIR, make dir);

/**
 * @brief 删除目录
 * 
 * @param filename 路径
 */
void shellREMOVE(char *filename)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);


    if (shellFs->remove(filename) != 0)
    {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, filename);
        shellWriteString(shell, "remove failed\r\n");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
rm, shellREMOVE, remove file or directory);

/**
 * @brief 重命名
 * 
 * @param dir 路径
 */
void shellRENAME( char *oldname, char *newname)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);


    if (shellFs->rename(oldname, newname) != 0)
    {
        shellWriteString(shell, "error: ");
        shellWriteString(shell, oldname);
        shellWriteString(shell, "rename failed\r\n");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
rename, shellRENAME, rename file or directory);


/**
 * @brief 读取文件
 * 
 * @param filename 路径
 */
void shellREAD(char *filename, char *mode)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

	
    FIL *fp = shellFs->malloc(sizeof(FIL));//申请内存;
    UINT br,bw;			//读写变量
    u16 i,t;
	u8 res=0;
	u16 len=0;

    FRESULT state = shellFs->open(fp,filename,FA_READ);//只读方式打开，根据文件是否存在，如果存在则打印文件内容，如果不存在，则返回错误
	len = fp->obj.objsize;
	printf("\r\nfilesize is: %d B\r\n",len);
    u8 *fatbuf = shellFs->malloc(512);	//分配内存，用于读写文件
	printf("\r\nRead file data is:\r\n");
	for(i=0;i<len/512;i++)
	{
		res=shellFs->read(fp,fatbuf,512,&br);
		if(res)
		{
			printf("Read Error:%d\r\n",res);
			break;
		}else
		{
			//len+=br;
			for(t=0;t<br;t++)
            {
                if(*mode=='b')
                {
                    printf("%x",fatbuf[t]); 
                }
                else{
                    printf("%c",fatbuf[t]); 
                }
            }
		}
	}
	if(len%512)
	{
		res=shellFs->read(fp,fatbuf,len%512,&br);
		if(res)	//读数据出错了
		{
			printf("\r\nRead Error:%d\r\n",res);   
		}else
		{
			//len+=br;
			for(t=0;t<br;t++)
            {
                if(*mode=='b')
                {
                    printf("%x",fatbuf[t]); 
                }
                else{
                    printf("%c",fatbuf[t]); 
                }
            }
		}	 
	}
	if(len) printf("\r\nReaded data len:%d\r\n",len);//读到的数据长度
	printf("Read data over\r\n");	
    shellFs->free(fatbuf);	//分配内存，用于读写文件
     
    shellFs->close(fp);
    
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
read, shellREAD, read file);


/**
 * @brief 写文件，如果文件不存在则创建后，写入内容
 * 
 * @param filename 路径
 */
void shellWRITE(char *filename, char *content, char *mode)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    u8 res;
    u16 t,len=strlen(content);
    UINT br,bw;			//读写变量
    u8 *buffer = shellFs->malloc(512);//申请内存;
	u16 i;
    if( (*mode=='b') && (len%2!=0))
    {
        printf("Error: The length of the data to be written is not an even number!\r\n");
        return;
    }
    FIL *fp = shellFs->malloc(sizeof(FIL));//申请内存;
    FRESULT state = shellFs->open(fp,filename,FA_WRITE|FA_OPEN_ALWAYS);//先打开文件，判断文件是否存在，如果不存在则创建，如果存在则清空文件内容并写入
	
    //二进制写入文件
    if(*mode=='b')
    {
        for(i=0;i<len/2;i++)
        {
            buffer[i] = (content[2*i]-'0') << 4 | (content[2*i+1]-'0') << 0;
        }
        len/=2;
    }
	printf("\r\nBegin Write file...\r\n");
	printf("Write data len:%d\r\n",len);	 
	res=shellFs->write(fp,buffer,len,&bw);   
	if(res)
	{
		printf("Write Error:%d\r\n",res);
	}else printf("Writed data len:%d\r\n",bw);
	printf("Write data over.\r\n");

    shellFs->close(fp);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
write, shellWRITE, write or create file);


/**
 * @brief 写文件，如果文件不存在则创建后，写入内容
 * 
 * @param filename 路径
 */
void shellSHOWFREE(char *filename, char *content)
{
    Shell *shell = shellGetCurrent();
    ShellFs *shellFs = shellCompanionGet(shell, SHELL_COMPANION_ID_FS);
    SHELL_ASSERT(shellFs, return);

    shellFs->showfree();//查看剩余容量
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
showfree, shellSHOWFREE, show free space of file system);
