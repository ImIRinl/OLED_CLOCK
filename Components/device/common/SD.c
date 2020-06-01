#include "SD.h"
#include "spi_hard.h"
#include "ioSTC8.h"
#include "diskio.h"
#include "uart.h"
#include "system_clock.h"

#define SD_CS  P2_1 //SD��Ƭѡ
 

uint8 Addr_Mode=0;        // Ѱַ��ʽ��1Ϊ��Ѱַ��0Ϊ�ֽ�Ѱַ��ʹ��ʱ�轫��ַ����9λ��
uint8 SD_Ver=SD_VER_ERR;  // SD���İ汾

/***********************SD����д����********************************
uint8 badapple[1024];
uint8 *point_data;
point_data = badapple;
disk_initialize(0);//��ʼ��SD��
//512�ֽ� ���д
SD_Write_nSector(point_data,1136976,2);
//512�ֽڵ���д
SD_Write_Sector(point_data, 1136976);
point_data += 512;
SD_Write_Sector(point_data, 1136977);
//512�ֽ� ����
SD_Read_nSector(point_data,1136976,2);
//512�ֽڵ����
SD_Read_Sector(point_data,1136976);
point_data += 512;
SD_Read_Sector(point_data,1136977);
******************************************************************/
/**************************************************************************************************
 *@����            SD_Select
 *
 *@����            ʹ��SD��Ƭѡ
 *
 *@�������  
 *
 *@����            ��
 *                       
 *�������
 *
 *��
 * 
 *@����     ��
 **************************************************************************************************
 */
static void SD_Select(void)
{
  SD_CS = 0;
}
/**************************************************************************************************
 *@����            SD_Deselect
 *
 *@����            ʧ��SD��Ƭѡ
 *
 *@�������  
 *
 *@����            ��
 *                       
 *�������
 *
 *��
 * 
 *@����     ��
 **************************************************************************************************
 */
static void SD_Deselect(void)
{
  SD_CS = 1;
  SD_add_sclk();//8����ʱ��
}
/**************************************************************************************************
 *@����            SD_add_sclk
 *
 *@����            Ӳ��SPI����8����ʱ��
 *
 *@�������  
 *
 *@����            ��
 *                       
 *�������
 *
 *��
 * 
 *@����     ��
 **************************************************************************************************
 */
static void SD_add_sclk(void)
{
  uint8 temp = 0xff;
  hard_spi_transaction(&temp, 1, NULL, 0);
}
/**************************************************************************************************
 *@����            SD_Init
 *
 *@����            SD����ʼ��
 *
 *@�������  
 *
 *@����            ��
 *                       
 *�������
 *
 *��
 * 
 *@����     ��
 **************************************************************************************************
 */

uint8 SD_Init(void)
{
  uint8 CMD0[6] = {0x40,0x00,0x00,0x00,0x00,0x95};//CMD0,��λSD���������л�ΪSPIģʽ(0x95ΪCRCУ�飩
  uint8 CMD1[6] ={0x41,0x00,0x00,0x00,0x00,0x01}; //CMD1��MMC��ʹ��CMD1������г�ʼ��
  uint8 CMD8[6] ={0x48,0x00,0x00,0x01,0xAA,0x87}; //CMD8�����ڼ���SD���İ汾�����ɴ�Ӧ���֪SD���Ĺ�����ѹ
  uint8 CMD16[6]={0x50,0x00,0x00,0x02,0x00,0x01}; //CMD16������������СΪ512�ֽڣ������������ڳ�ʼ�����֮�������̽�ԵĲ�����
  uint8 CMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55�����ڸ�֪SD��������ACMD����Ӧ�ò����� CMD55+ACMD41���ʹ��
  uint8 ACMD41H[6]={0x69,0x40,0x00,0x00,0x00,0x01}; //ACMD41,���������ڼ��SD���Ƿ��ʼ����ɣ�MMC���������ô�������2.0��SD��
  uint8 ACMD41S[6]={0x69,0x00,0x00,0x00,0x00,0x01}; //ACMD41,���������ڼ��SD���Ƿ��ʼ����ɣ�MMC���������ô�������1.0��SD��
  uint8 CMD58[6]={0x7A,0x00,0x00,0x00,0x00,0x01}; //CMD58�����ڼ���SD2.0������SDHC��������ͨ��SD�������߶�������ַ��Ѱַ��ʽ��ͬ

  uint8 value[4],temp;
  uint16 times = 5000;//����ʹ���

  clock_info pre_clock;
  pre_clock = get_system_clock_source();//��¼��ǰ��ʱ��Դ�ͷ�Ƶϵ��
  set_system_clock_source(IRC24M_CLOCK,6);//ѡ���ڲ�RC��Ƶ������Ϊ��ʱ��Դ��6��Ƶ
  set_irc_freq_adjust(0,0,0);//��ʱ��Ƶ����Ϊ16M


  SPCTL |= 0x03;//Ӳ��SPIʱ��Ϊ��ʱ��32��Ƶ
  SD_Select();//ʹ��SD��Ƭѡ
/************************��λ***********************************/  
  for(uint8 i = 0; i < 10; i++) // ����Ҫ��������74��ʱ���źţ����Ǳ���ģ�����SD��
  {
    SD_add_sclk();
  }
  
  do
  {
    temp = SD_Write_Cmd(CMD0);//����CMD0ָ���SD���л�ΪSPIģʽ
    if(--times == 0)//���ʹ����ﵽ���ֵ
    {
      SD_Deselect();//ʧ��SD��Ƭѡ
      return INIT_CMD0_ERROR;//����CMD0ָ���޻�Ӧ
    }
  }while(temp != 0x01);//�ж�SD����Ӧ�Ƿ�Ϊ0x01�������ظ�����
/*************************************************************/  
  
  temp = SD_Write_Cmd(CMD8);//д��CMD8����ȡSD����Ӧ

  if(temp ==0x01) //�ж�CMD8��Ӧ�Ƿ�Ϊ0x01��SD���汾Ϊ2.0
  {
    hard_spi_transaction(NULL, 0, value, 4);//��ȡ4���ֽڵ�R7��Ӧ��ͨ������֪��SD���Ƿ�֧��2.7~3.6V�Ĺ�����ѹ
    if(value[2] == 0x01 && value[3] == 0xaa)//SD���Ƿ�֧��2.7~3.6V,����������֧��-------------------
    {
          times = 39000;
      do
      {
        //������Ҫ��������CMD55��ADCMD41�����0.12S-0.3s��SD���Ż�Ӧ0x00
          SD_Write_Cmd(CMD55);// д��CMD55
          temp = SD_Write_Cmd(ACMD41H);// д��ACMD41�����SD2.0
          if(--times == 0)//���ʹ����ﵽ���ֵ
          {
            SD_Deselect();//ʧ��SD��Ƭѡ 
            return(INIT_SDV2_ACMD41_ERROR);//��SD2.0ʹ��ACMD41���г�ʼ��ʱ��������
          }
      }while(temp!=0);//�ж�SD����Ӧ�Ƿ�Ϊ0x00�������ظ�����
     
        
      if(0==SD_Write_Cmd(CMD58))//д��CMD58����ʼ����SD2.0
      {  
        //SDHC�洢������Ϊ��4GB-32GB����
        hard_spi_transaction(NULL, 0, value, 4);//��ȡ4���ֽڵ�OCR������CCSָ������SDHC������ͨ��SD
        if(value[0] & 0x40)
        {
           SD_Ver=SD_VER_V2HC; //SDHC��	
	   Addr_Mode=1; //SDHC��Ϊ����Ѱַ��ʽ
        }
        else
        {
          SD_Ver=SD_VER_V2; //��ͨ��SD����2.0�Ŀ�����SDHC��һЩ��ͨ�Ŀ����ֽ�Ѱַ��ʽ	
        }
      }
    }
  }
    else // SD V1.0��MMC
    {
      // SD��ʹ��ACMD41���г�ʼ������MMCʹ��CMD1�����г�ʼ������������һ���ж���SD����MMC
       SD_Write_Cmd(CMD55);            // д��CMD55
       temp=SD_Write_Cmd(ACMD41S);     // д��ACMD41�����SD1.0
       if(temp <= 1)//��鷵��ֵ�Ƿ���ȷ�������ȷ��˵��ACMD41������ܣ���ΪSD��
       {
         SD_Ver=SD_VER_V1; //��ͨ��SD1.0����һ����˵�������ᳬ��2G
         times = 500;
         do
         {
           SD_Write_Cmd(CMD55);//д��CMD55
           temp=SD_Write_Cmd(ACMD41S);//д��ACMD41�����SD1.0
           if(--times == 0)
           {
             SD_Deselect();//ʧ��SD��Ƭѡ 
             return(INIT_SDV1_ACMD41_ERROR);//��SD1.0ʹ��ACMD41���г�ʼ��ʱ��������
           }
         }while(temp!=0);
       }
       else//����ΪMMC
       {
         SD_Ver=SD_VER_MMC; //MMC��������֧��ACMD41�������ʹ��CMD1���г�ʼ��
         times=500;
         do
         {
           temp=SD_Write_Cmd(CMD1);//д��CMD1
           if(--times == 0)
           {
             SD_Deselect();//ʧ��SD��Ƭѡ
             return(INIT_CMD1_ERROR);//MMC��ʹ��CMD1������г�ʼ���в�������
           }
         }while(temp!=0);
       }
    }
    if(0 != SD_Write_Cmd(CMD16))//SD���Ŀ��С����Ϊ512�ֽ�
    {
      SD_Ver=SD_VER_ERR; //������ɹ�����˿�Ϊ�޷�ʶ��Ŀ�
      SD_Deselect();//ʧ��SD��Ƭѡ
      return INIT_ERROR;
    }
       
  SD_Deselect();//ʧ��SD��Ƭѡ
  SPCTL &= 0xfc;//Ӳ��SPIʱ��Ϊ��ʱ��4��Ƶ
  
  set_system_clock_source(pre_clock.clock_source,pre_clock.clock_div);//�ָ�֮ǰ��ʱ��Դ�ͷ�Ƶϵ��
  set_irc_freq_adjust(0,0,ORIGINAL_ICR_FREQ_ADJUST);//�ָ�֮ǰ��Ƶ��
  return 0;
}
/**************************************************************************************************
 *@����            SD_Write_Cmd
 *
 *@����            SD��д����
 *
 *@�������  
 *
 *@����            *cmd - ����
 *                       
 *�������
 *
 *��
 * 
 *@����     0xff - ���ɹ���������Ϊ�ɹ�
 **************************************************************************************************
 */

static uint8 SD_Write_Cmd(uint8 *cmd)
{
  uint8 temp;
  uint16 times;
  temp = 0xff;
  times = 500;
  
 // SD_Deselect();//��߼�����
  SD_Select();
 do
 {
   hard_spi_transaction(NULL, 0, &temp, 1);
 }while(temp != 0xff);//�ȴ�SD��׼���ã������䷢������
 
   hard_spi_transaction(cmd, 6, NULL, 0);//����6���ֽڵ���������

  do
  {
    hard_spi_transaction(NULL, 0, &temp, 1);
    times--;
  }while(temp == 0xff && times > 0);

  return temp;
}

/**************************************************************************************************
 *@����            SD_Read_Sector
 *
 *@����            SD����ȡһ������
 *
 *@�������  
 *
 *@����            *data - ��������
 *                 addr - ��ַ
 *                        
 *�������
 *
 *��
 * 
 *@����     ��
 **************************************************************************************************
*/ 
uint8 SD_Read_Sector(uint8 *data, uint32 addr)
{
  uint8 temp;
  uint16 times = 500;
  uint8 crc[2];
  uint8 cmd17[] = {0x51,0x00,0x00,0x00,0x00,0xff};

 if(!Addr_Mode)
 {
    addr <<= 9;//�����ַ��������ַ��תΪ�ֽڵ�ַ������512
 }
 cmd17[1]=addr>>24; //���ֽڵ�ַд�뵽CMD17�ֽ�������
 cmd17[2]=addr>>16;
 cmd17[3]=addr>>8;
 cmd17[4]=addr;	
  SD_Select();
  do
  {
    temp = SD_Write_Cmd(cmd17);    
      if(--times == 0)
      { 
         SD_Deselect();//ʧ��SD��Ƭѡ
        return(READ_BLOCK_ERROR);//����ʧ��
      }
  }while(temp != 0);
  do
  {
    hard_spi_transaction(NULL, 0, &temp, 1);
  }while(temp != 0xfe);//������0xfeʱ����ʾ��������������
  hard_spi_transaction(NULL, 0, data, 512);//��ȡ��������
  hard_spi_transaction(NULL, 0, crc, 2);//��ȡ2λCRCУ����
  SD_Deselect();
  return 0;
}
/**************************************************************************************************
 *@����            SD_Read_nSector
 *
 *@����            SD����ȡ�������
 *
 *@�������  
 *
 *@����            *data - ��������
 *                 addr - ��ַ
 *                        
 *�������
 *
 *��
 * 
 *@����     ��
 **************************************************************************************************
*/ 
uint8 SD_Read_nSector(uint8 *data, uint32 addr, uint32 nsec)
{
  uint8 temp;	
  uint8 crc[2];
  uint16 times = 5000;
  uint8 CMD18[6]={0x52,0x00,0x00,0x00,0x00,0x01}; //CMD18���ֽ�����
  uint8 CMD12[6]={0x4C,0x00,0x00,0x00,0x00,0x01}; //CMD12��ǿ��ֹͣ����	  
  if(!Addr_Mode) 
  {
    addr<<=9;
  }
  
  CMD18[1]=addr>>24; //���ֽڵ�ַд�뵽CMD17�ֽ�������
  CMD18[2]=addr>>16;
  CMD18[3]=addr>>8;
  CMD18[4]=addr;
  
  SD_Select();
  do
 {  
    temp=SD_Write_Cmd(CMD18);
    if(--times == 0) 
    { 
       SD_Deselect();//ʧ��SD��Ƭѡ
	return(READ_CMD18_ERROR); //����д��ʧ��
    }
  }while(temp!=0);
  for(uint32 i = 0; i < nsec; i++)
  {
     do
    {
      hard_spi_transaction(NULL, 0, &temp, 1);
    }while(temp != 0xfe);//������0xfeʱ����ʾ��������������
    hard_spi_transaction(NULL, 0, data, 512);//��ȡ��������
    hard_spi_transaction(NULL, 0, crc, 2);//��ȡ2λCRCУ����
    data += 512;
  }
  SD_Write_Cmd(CMD12);//д��CMD12���ֹͣ���ݶ�ȡ
 
  SD_Deselect();//ʧ��SD�� 
  return 0;
}
/**************************************************************************************************
 *@����            SD_Write_Sector
 *
 *@����            SD��д��һ������
 *
 *@�������  
 *
 *@����            *data - ��������
 *                 addr - ��ַ
 *                        
 *�������
 *
 *��
 * 
 *@����     0 - �ɹ�    WRITE_BLOCK_ERROR - ʧ��
 **************************************************************************************************
*/
uint8 SD_Write_Sector(const uint8 *data, uint32 addr)
{
  uint8 temp = 0xfe;
  uint8 crc[2]={0xff,0xff};
  uint16 times = 500;
  uint8 cmd24[] = {0x58,0x00,0x00,0x00,0x00,0xff};
  if(!Addr_Mode)
    addr <<= 9;//�����ַ��������ַ��תΪ�ֽڵ�ַ������512
  cmd24[1]=addr>>24; //���ֽڵ�ַд�뵽CMD17�ֽ�������
  cmd24[2]=addr>>16;
  cmd24[3]=addr>>8;
  cmd24[4]=addr;	
  SD_Select();
  do
  {
    temp = SD_Write_Cmd(cmd24);    
      if(--times == 0)
      {
         SD_Deselect();//ʧ��SD��Ƭѡ
        return(WRITE_BLOCK_ERROR);//д��ʧ��
      }
  }while(temp != 0); 
 
  temp = 0xfe;
  hard_spi_transaction(&temp, 1, NULL, 0);//д0xfe����ʾ��������������
  hard_spi_transaction(data, 512, NULL, 0);////д����������
  hard_spi_transaction(crc, 2, NULL, 0);////д��crcУ��
  hard_spi_transaction(NULL, 0, &temp, 1);//��ȡ����
  if((temp & 0x1F)!=0x05) //�ж�SD���Ƿ��������
  {
    return(WRITE_BLOCK_ERROR); //д������ʧ��
  }
  do
  {
    hard_spi_transaction(NULL, 0, &temp, 1);//��ȡ����
  }while(temp != 0xff);//�ȴ�SD���洢����
  SD_Deselect();
  return 0;
}
/**************************************************************************************************
 *@����            SD_Write_nSector
 *
 *@����            ����д��������
 *
 *@�������  
 *
 *@����            ��
 *                        
 *�������
 *
 *��
 * 
 *@����     Capacity - �������� 
 **************************************************************************************************
*/
uint8 SD_Write_nSector(const uint8 *data, uint32 addr, uint32 nsec)
{
  uint8 temp;
  uint8 crc[2] = {0xff,0xff};
  uint16 times = 500;
  uint8 CMD25[6]={0x59,0x00,0x00,0x00,0x00,0x01}; //CMD25������ɶ������д
  uint8 CMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55�����ڸ�֪SD��������ACMD,CMD55+ACMD23
  uint8 ACMD23[6]={0x57,0x00,0x00,0x00,0x00,0x01};//CMD23���������Ԥ����
  if(!Addr_Mode) 
  {
    addr<<=9;
  }
  CMD25[1]=addr>>24;
  CMD25[2]=addr>>16;
  CMD25[3]=addr>>8;
  CMD25[4]=addr;
  
  ACMD23[1]=nsec>>24;
  ACMD23[2]=nsec>>16;
  ACMD23[3]=nsec>>8;
  ACMD23[4]=nsec;
  
  SD_Select();
  if(SD_Ver!=SD_VER_MMC) //�������MMC����������д��Ԥ�������CMD55+ACMD23�����������������д���ٶȻ����
  {
     SD_Write_Cmd(CMD55);
     SD_Write_Cmd(ACMD23);
  }
  do
 {  
    temp=SD_Write_Cmd(CMD25);
    if(--times == 0) 
    { 
       SD_Deselect();//ʧ��SD��Ƭѡ
       return(WRITE_CMD25_ERROR); //����д��ʧ��
    }
  }while(temp!=0);
   do
  {
    hard_spi_transaction(NULL, 0, &temp, 1);//��ȡ����
  }while(temp != 0xff);//�ȴ�SD���洢����
  for(uint32 i = 0; i < nsec; i++)
  {
     temp = 0xfc;
     hard_spi_transaction(&temp, 1, NULL, 0);//д0xfc����ʾ��������������
     hard_spi_transaction(data, 512, NULL, 0);////д����������
     hard_spi_transaction(crc, 2, NULL, 0);////д��crcУ��
     hard_spi_transaction(NULL, 0, &temp, 1);//��ȡ����
     if((temp & 0x1F)!=0x05) //�������ֵ�� XXX00DELAY_TIME1 ˵�������Ѿ���SD��������
    {
       return(WRITE_NBLOCK_ERROR); //д������ʧ��
    }
    data += 512;
    do
    {
       hard_spi_transaction(NULL, 0, &temp, 1);//��ȡ����
    }while(temp != 0xff);//�ȴ�SD���洢����   
  }
  temp = 0xfd;
  hard_spi_transaction(&temp, 1, NULL, 0);//д0xfd,��ʾֹͣ���ֽ�д
  do
    {
       hard_spi_transaction(NULL, 0, &temp, 1);//��ȡ����
    }while(temp != 0xff);//�ȴ�SD���洢����  
  SD_Deselect();//ʧ��SD�� 
  return 0;
}
/**************************************************************************************************
 *@����            SD_GetTotalSec
 *
 *@����            ��ȡȫ����������
 *
 *@�������  
 *
 *@����            ��
 *                        
 *�������
 *
 *��
 * 
 *@����     Capacity - �������� 
 **************************************************************************************************
*/
uint32 SD_GetTotalSec(void)
{
  uint8 n,temp;
  uint8 CSD[16],crc[2];
  uint32 Capacity;  
  uint16 csize; 	
  uint8 CMD9[6]={0x49,0x00,0x00,0x00,0x00,0x01}; //CMD9
  SD_Select();
  if(SD_Write_Cmd(CMD9)!=0) //д��CMD9����
	{
		return GET_CSD_ERROR; //��ȡCSDʱ��������
	} 
  do
  {
     hard_spi_transaction(NULL, 0, &temp, 1);//��ȡ����
  }while(temp != 0xFE); //������0xfeʱ��˵���������16�ֽڵ�CSD����
  hard_spi_transaction(NULL, 0, CSD, 16);//��ȡ���� 
  hard_spi_transaction(NULL, 0, crc, 2);//��ȡ2��CRCУ��
  SD_Deselect();//ʧ��SD��
  //���ΪSDHC�����������������������淽ʽ����
	if((CSD[0]&0xC0)==0x40)	 //SD2.0�Ŀ�
	{	
		csize=CSD[9]+(((uint16)(CSD[8]))<<8)+1;
		Capacity=((uint32)csize)<<10;          // �õ�������	
                		   
	}
	else //SD1.0�Ŀ�
	{	
		n=(CSD[5]&0x0F)+((CSD[10]&0x80)>>7)+((CSD[9]&0x03)<<1)+2;
		csize=(CSD[8]>>6)+((uint16)CSD[7]<<2)+((uint16)(CSD[6]&0x03)<<0x0A)+1;
		Capacity=(uint32)csize<<(n-9);         // �õ�������   
	}
	return Capacity;
	
}