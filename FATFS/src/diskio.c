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

#define SD_CARD	 	1  			//SD��,���Ϊ0
#define EX_FLASH 	0			//�ⲿspi flash,���Ϊ1
#define EX_NAND  	2			//�ⲿnand flash,���Ϊ2

//����W25Q256
//ǰ16M�ֽڸ�fatfs��,25M�ֽں�,���ڴ���ֿ�,�ֿ�ռ��6.01M.	ʣ�ಿ��,���ͻ��Լ���	 
#define FLASH_SECTOR_SIZE 	512	
#define FLASH_SECTOR_COUNT 	1024*16*2	//W25Q256,ǰ25M�ֽڸ�FATFSռ��	
#define FLASH_BLOCK_SIZE   	8     		//ÿ��BLOCK��8������		
  
 
//��ô���״̬
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{ 
	return RES_OK;
}  
//��ʼ������
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	uint8_t res=1;	    
	switch(pdrv)
	{
		case SD_CARD:		//SD��
			//res=SD_Init();	//SD����ʼ��
			printf("Not support this drv:%d\r\n",pdrv);
  			break;
		case EX_FLASH:		//�ⲿflash
			W25QXX_Init();  //W25QXX��ʼ��
			res=0;
 			break;
		case EX_NAND:		//�ⲿNAND
			//res=FTL_Init();	//NAND��ʼ��
			printf("Not support this drv:%d\r\n",pdrv);
 			break;
		default:
			res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; //��ʼ���ɹ� 
} 
//������
//pdrv:���̱��0~9
//*buff:���ݽ��ջ����׵�ַ
//sector:������ַ
//count:��Ҫ��ȡ��������
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	uint8_t res=1; 
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 
	//�����ٽ���
    taskENTER_CRITICAL();
	switch(pdrv)
	{
		case SD_CARD://SD��
			#if 0
			res=SD_ReadDisk(buff,sector,count);	 
			while(res)//������
			{
				SD_Init();	//���³�ʼ��SD��
				res=SD_ReadDisk(buff,sector,count);	
				//printf("sd rd error:%d\r\n",res);
			}
			#endif
			printf("Not support this drv:%d\r\n",pdrv);
			break;
		case EX_FLASH://�ⲿflash
			for(;count>0;count--)
			{
				W25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		case EX_NAND:		//�ⲿNAND
			//res=FTL_ReadSectors(buff,sector,512,count);	//��ȡ����	
			printf("Not support this drv:%d\r\n",pdrv);
			break;
		default:
			res=1; 
	}
	//�˳��ٽ���
    taskEXIT_CRITICAL();
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}
//д����
//pdrv:���̱��0~9
//*buff:���������׵�ַ
//sector:������ַ
//count:��Ҫд��������� 
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	uint8_t res=1;  
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������		 	
	//�����ٽ���
    taskENTER_CRITICAL(); 
	switch(pdrv)
	{
		case SD_CARD://SD��
			#if 0
			res=SD_WriteDisk((u8*)buff,sector,count);
			while(res)//д����
			{
				SD_Init();	//���³�ʼ��SD��
				res=SD_WriteDisk((u8*)buff,sector,count);	
				//printf("sd wr error:%d\r\n",res);
			}
			#endif
			printf("Not support this drv:%d\r\n",pdrv);
			break;
		case EX_FLASH://�ⲿflash
			for(;count>0;count--)
			{										    
				W25QXX_Write((uint8_t*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
			res=0;
			break;
		case EX_NAND:		//�ⲿNAND
			//res=FTL_WriteSectors((u8*)buff,sector,512,count);//д������
			printf("Not support this drv:%d\r\n",pdrv);
			break;
		default:
			res=1; 
	}
	//�˳��ٽ���
    taskEXIT_CRITICAL();
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
} 
//����������Ļ��
//pdrv:���̱��0~9
//ctrl:���ƴ���
//*buff:����/���ջ�����ָ�� 
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;						
	//�����ٽ���
    taskENTER_CRITICAL(); 	  			     
	if(pdrv==SD_CARD)//SD��
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
	}else if(pdrv==EX_FLASH)	//�ⲿFLASH  
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
	}else if(pdrv==EX_NAND)	//�ⲿNAND FLASH
	{
		#if 0
	    switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;	//NAND FLASH����ǿ��Ϊ512�ֽڴ�С
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = nand_dev.page_mainsize/512;//block��С,�����һ��page�Ĵ�С
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = nand_dev.valid_blocknum*nand_dev.block_pagenum*nand_dev.page_mainsize/512;//NAND FLASH����������С
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
		#endif
		printf("Not support this drv:%d\r\n",pdrv);
		
	}else res=RES_ERROR;//�����Ĳ�֧��
	//�˳��ٽ���
    taskEXIT_CRITICAL();
    return res;
} 
//���ʱ��
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
//��̬�����ڴ�
void *ff_memalloc (UINT size)			
{
	return (void*)GWMalloc(size);
}
//�ͷ��ڴ�
void ff_memfree (void* mf)		 
{
	GWfree(mf);
}









