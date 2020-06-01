#ifndef SD_H
#define SD_H

#include "hal_types.h"


//��غ궨��
//-------------------------------------------------------------
#define SD_VER_ERR     0X00		 // �޷�ʶ��İ汾
#define SD_VER_MMC     0X01		 // �汾�ţ�MMC
#define SD_VER_V1      0X02		 // �汾�ţ�SD1.0
#define SD_VER_V2      0X03	         //�汾�ţ�SD2.0(��ͨSD)
#define SD_VER_V2HC    0X04		 //�汾�ţ�SD2.0(������SDHC)

#define INIT_ERROR                  0x01 //��ʼ������
#define INIT_CMD0_ERROR             0x02 //CMD0����
#define INIT_CMD1_ERROR             0x03 //CMD1����
#define INIT_SDV2_ACMD41_ERROR	    0x04 //ACMD41����
#define INIT_SDV1_ACMD41_ERROR	    0x05 //ACMD41����

#define WRITE_CMD24_ERROR           0x06 //д��ʱ����CMD24����
#define WRITE_BLOCK_ERROR           0x07 //д�����

#define READ_BLOCK_ERROR            0x08 //�������

#define WRITE_CMD25_ERROR           0x09 //���������дʱ����CMD25����
#define WRITE_NBLOCK_ERROR          0x0A //�������дʧ��

#define READ_CMD18_ERROR            0x0B //����������ʱ����CMD18����
 
#define GET_CSD_ERROR               0x0C //��CSDʧ��



static void SD_Select(void);
static void SD_Deselect(void);
extern uint8 SD_Init(void);
static uint8 SD_Write_Cmd(uint8 *cmd);
static void SD_add_sclk(void);
extern uint8 SD_Read_Sector(uint8 *data, uint32 addr);
extern uint8 SD_Read_nSector(uint8 *data, uint32 addr, uint32 nsec);
extern uint8 SD_Write_Sector(const uint8 *data, uint32 addr);
extern uint8 SD_Write_nSector(const uint8 *data, uint32 addr, uint32 nsec);
extern uint32 SD_GetTotalSec(void);
#endif