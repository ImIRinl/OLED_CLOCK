/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "SD.h"

/******************************ʹ��ʾ��**********************************/

/************************��ȡ��������������С*****************************
  uint32 SumSec;
  uint32 SecSiZe;
  disk_initialize(0);//��ʼ��SD��
  disk_ioctl(0, GET_SECTOR_COUNT, &SumSec);//��ȡSD��ȫ��������
  disk_ioctl(0, GET_SECTOR_SIZE, &SecSiZe);//��ȡSD��������С
  printfs("SumSec=%ld\r\n",SumSec);
  printfs("SecSiZe=%ld\r\n",SecSiZe);

**************************���� �����ļ�.txt*******************************
 FIL  filescr1;
 UINT bw; 
 FATFS  fatfs;
 disk_initialize(0);//��ʼ��SD��
 f_mount(&fatfs,"0:",1);//����SD����Ϊ����
 f_open(&filescr1, "0:/�����ļ�.txt", FA_CREATE_NEW|FA_WRITE|FA_READ);//�����򿪲����ļ�.txt
 f_write(&filescr1, "Hello,World!", 12, &bw);//�������ļ�.txtд��Hello,World!
 f_close(&filescr1);//�ر��ļ�
**************************************************************************/
/**************************************************************************************************
 *@����            disk_status
 *
 *@����            ��ȡ����״̬
 *
 *@�������  
 *
 *@����            pdrv - �������
 *                       
 *�������
 *
 *��
 * 
 *@����     ��ʼ��״̬
 **************************************************************************************************
 */
DSTATUS disk_status (BYTE pdrv)
{
   return 0;
}
/**************************************************************************************************
 *@����            disk_initialize
 *
 *@����            ��ʼ������������
 *
 *@�������  
 *
 *@����            pdrv - �������
 *                       
 *�������
 *
 *��
 * 
 *@����     ��ʼ��״̬
 **************************************************************************************************
 */
DSTATUS disk_initialize (BYTE pdrv)
{
  DSTATUS stat;
  stat = SD_Init();
  if(stat == 0)
     return RES_OK;
  else
     return RES_NOTRDY;
}
/**************************************************************************************************
 *@����            disk_read
 *
 *@����            ���̶�ȡ
 *
 *@�������  
 *
 *@����            pdrv - �������
 *                 buff - ����ָ��
 *                 sector - ��ʼ����
 *                 count - ��ȡ����
 *                     
 *�������
 *
 *��
 * 
 *@����     ״̬
 **************************************************************************************************
 */
DRESULT disk_read (BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{	
        uint8 result;
        if(!count)
          return RES_PARERR;
        if(count == 1)
        {
         result = SD_Read_Sector(buff, sector);
        }
        else
        {
          //for(uint32 i = 0; i < count; i++)
          //{
          //  SD_Read_Sector(buff, sector+i);
          //  buff += 512;
          //}
          //result = 0;
           result = SD_Read_nSector(buff, sector, count);
        }
        if(result == 0)
          return RES_OK;
        else
          return RES_ERROR;
}
/**************************************************************************************************
 *@����            disk_write
 *
 *@����            ����д��
 *
 *@�������  
 *
 *@����            pdrv - �������
 *                 buff - ����ָ��
 *                 sector - ��ʼ����
 *                 count - ��ȡ����
 *                     
 *�������
 *
 *��
 * 
 *@����     ״̬
 **************************************************************************************************
 */
#if FF_FS_READONLY == 0  //ֻ���꿪�� 0����/д��1��ֻ��
DRESULT disk_write (BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
	uint8 result;
        if(count == 1)
        {
          result = SD_Write_Sector(buff, sector);
        }
        else
        {
           result = SD_Write_nSector(buff, sector, count);
           //for(uint32 i = 0; i < count; i++)
           //{
           //   SD_Write_Sector(buff+i, sector);
            //  buff += 512;
           //}
           //result = 0;
        }
        if(result == 0)
          return RES_OK;
        else
          return RES_ERROR;
}

#endif
/**************************************************************************************************
 *@����            disk_ioctl
 *
 *@����            ������ؿ�������
 *
 *@�������  
 *
 *@����            pdrv - �������
 *                 cmd - ����
 *                 buff - ���������
 *                     
 *�������
 *
 *��
 * 
 *@����     ״̬
 **************************************************************************************************
 */
DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
        DRESULT res;
        switch(cmd)
        { 
          case CTRL_SYNC:
		res = RES_OK;
             break;          
          case GET_SECTOR_SIZE://��ȡ������С,һ����512��1024��2048��4096
              *(WORD*)buff = 512;
              res = RES_OK;
            break;
            
          case GET_BLOCK_SIZE://��ȡ�ܴ�С
            // *(WORD*)buff = 512;
             *(DWORD*)buff = SD_GetTotalSec();
             res = RES_OK;
            break;
          case GET_SECTOR_COUNT:
             *(DWORD*)buff = SD_GetTotalSec();
             res = RES_OK;
            break;
          default:
            res = RES_PARERR;
            break;
        }
        return res;
}