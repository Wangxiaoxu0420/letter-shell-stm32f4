/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "diskio.h"			/* FatFs lower layer API */
//#include "sdio_sdcard.h"
#include "w25qxx.h"
#include "malloc.h"	 
#include "rtc.h"
#include <stdio.h>
//#include "nand.h"	 
//#include "ftl.h"	 
#include "FreeRTOS.h"
#include "task.h"

#define SD_CARD	 	1  			//SD卡,卷标为0
#define EX_FLASH 	0			//外部spi flash,卷标为1
#define EX_NAND  	2			//外部nand flash,卷标为2

//对于W25Q256
//前16M字节给fatfs用,25M字节后,用于存放字库,字库占用6.01M.	剩余部分,给客户自己用	 
#define FLASH_SECTOR_SIZE 	512	
#define FLASH_SECTOR_COUNT 	1024*16*2	//W25Q256,前25M字节给FATFS占用	
#define FLASH_BLOCK_SIZE   	8     		//每个BLOCK有8个扇区		
  
 
//获得磁盘状态
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{ 
	return RES_OK;
}  
//初始化磁盘
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	uint8_t res=1;	    
	switch(pdrv)
	{
		case SD_CARD:		//SD卡
			//res=SD_Init();	//SD卡初始化
			printf("Not support this drv:%d\r\n",pdrv);
  			break;
		case EX_FLASH:		//外部flash
			W25QXX_Init();  //W25QXX初始化
			res=0;
 			break;
		case EX_NAND:		//外部NAND
			//res=FTL_Init();	//NAND初始化
			printf("Not support this drv:%d\r\n",pdrv);
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //初始化成功 
} 
//读扇区
//pdrv:磁盘编号0~9
//*buff:数据接收缓冲首地址
//sector:扇区地址
//count:需要读取的扇区数
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	uint8_t res=1; 
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 
	//进入临界区
    taskENTER_CRITICAL();
	switch(pdrv)
	{
		case SD_CARD://SD卡
			#if 0
			res=SD_ReadDisk(buff,sector,count);	 
			while(res)//读出错
			{
				SD_Init();	//重新初始化SD卡
				res=SD_ReadDisk(buff,sector,count);	
				//printf("sd rd error:%d\r\n",res);
			}
			#endif
			printf("Not support this drv:%d\r\n",pdrv);
			break;
		case EX_FLASH://外部flash
			for(;count>0;count--)
			{
				W25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		case EX_NAND:		//外部NAND
			//res=FTL_ReadSectors(buff,sector,512,count);	//读取数据	
			printf("Not support this drv:%d\r\n",pdrv);
			break;
		default:
			res=1; 
	}
	//退出临界区
    taskEXIT_CRITICAL();
   //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}
//写扇区
//pdrv:磁盘编号0~9
//*buff:发送数据首地址
//sector:扇区地址
//count:需要写入的扇区数 
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	uint8_t res=1;  
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误		 	
	//进入临界区
    taskENTER_CRITICAL(); 
	switch(pdrv)
	{
		case SD_CARD://SD卡
			#if 0
			res=SD_WriteDisk((u8*)buff,sector,count);
			while(res)//写出错
			{
				SD_Init();	//重新初始化SD卡
				res=SD_WriteDisk((u8*)buff,sector,count);	
				//printf("sd wr error:%d\r\n",res);
			}
			#endif
			printf("Not support this drv:%d\r\n",pdrv);
			break;
		case EX_FLASH://外部flash
			for(;count>0;count--)
			{										    
				W25QXX_Write((uint8_t*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		case EX_NAND:		//外部NAND
			//res=FTL_WriteSectors((u8*)buff,sector,512,count);//写入数据
			printf("Not support this drv:%d\r\n",pdrv);
			break;
		default:
			res=1; 
	}
	//退出临界区
    taskEXIT_CRITICAL();
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
} 
//其他表参数的获得
//pdrv:磁盘编号0~9
//ctrl:控制代码
//*buff:发送/接收缓冲区指针 
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;						
	//进入临界区
    taskENTER_CRITICAL(); 	  			     
	if(pdrv==SD_CARD)//SD卡
	{
		#if 0
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
				*(WORD*)buff = SDCardInfo.CardBlockSize;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SDCardInfo.CardCapacity/512;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
		#endif
		printf("Not support this drv:%d\r\n",pdrv);
	}else if(pdrv==EX_FLASH)	//外部FLASH  
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}else if(pdrv==EX_NAND)	//外部NAND FLASH
	{
		#if 0
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;	//NAND FLASH扇区强制为512字节大小
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = nand_dev.page_mainsize/512;//block大小,定义成一个page的大小
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = nand_dev.valid_blocknum*nand_dev.block_pagenum*nand_dev.page_mainsize/512;//NAND FLASH的总扇区大小
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
		#endif
		printf("Not support this drv:%d\r\n",pdrv);
		
	}else res=RES_ERROR;//其他的不支持
	//退出临界区
    taskEXIT_CRITICAL();
    return res;
} 
//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{
	DWORD fat_time = 0;
	
	uint8_t hour, min, sec, ampm;
    uint8_t year, month, date, week;
	rtc_get_time(&hour, &min, &sec, &ampm);
    rtc_get_date(&year, &month, &date, &week);
	
//	printf("Get rtc time:%d-%d-%d %d:%d:%d\r\n",year,month,date,hour,min,sec);
	year+=20;
	sec/=2;
	fat_time = ((DWORD)year<<25)+((DWORD)month<<21)+((DWORD)date<<16)+((DWORD)hour<<11)+((DWORD)min<<5)+sec;
	return fat_time;
}			 
//动态分配内存
void *ff_memalloc (UINT size)			
{
	return (void*)GWMalloc(size);
}
//释放内存
void ff_memfree (void* mf)		 
{
	GWfree(mf);
}









