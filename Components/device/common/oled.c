#include "oled.h"
#include "spi_hard.h"
#include "ioSTC8.h"
#include "delay.h"
#include <math.h>
#include "uart.h"
#include "font.h"

#define  OLED_DC     P1_6    //OLED����/����ѡ������
#define  OLED_RES    P1_7    //OLED��λ����
#define  OLED_CS     P1_5    //OLEDƬѡ����

#define  OLED_CMD    0
#define  OLED_DATA   1

#define PI 3.14159

uint8 OLED_DISPLAY[8][128]; //��ʾ�������飨�ܹ����Ա�ʾ8192�����ص㣩

#define     OLED_GET_PAGE_BIT(PAGE, Bit)	((((PAGE) & (0x80 >> (Bit)))>0)?1:0) //˳�򣨸�λ��ǰ��
//#define     OLED_GET_PAGE_BIT(PAGE, Bit)	((((PAGE) & (0x01 << (Bit)))>0)?1:0) //���򣨵�λ��ǰ��

/**************************************************************************************************
 *@����            OLED_WR_Byte
 *
 *@����            OLEDдһ���ֽ�
 *
 *@�������        dat - ����
 *                cmd - ָ�����ͣ�����/���ݣ�
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
void OLED_WR_Byte(uint8 dat, uint8 cmd)
{
    uint8 value = dat;
    if(cmd)
    {
        OLED_DC = 1; //����ģʽ
    }   
    else
    {
        OLED_DC = 0; //����ģʽ
    }    
    OLED_CS = 0; //ʹ��Ƭѡ
    hard_spi_transaction(&value, 1, NULL, 0); //���ݴ���
    OLED_CS = 1; //ʧ��Ƭѡ
    OLED_DC = 1; //ʧ������
}
/**************************************************************************************************
 *@����            OLED_WR_Bytes
 *
 *@����            OLEDд����ֽ�
 *
 *@�������        dat - ���ݣ��������
 *                len - ���ݳ���
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
void OLED_WR_Bytes(uint8 *dat, uint32 len)
{
    OLED_DC = 1; //����ģʽ 
    OLED_CS = 0; //ʹ��Ƭѡ
    hard_spi_transaction(dat, len, NULL, 0); //���ݴ���
    OLED_CS = 1; //ʧ��Ƭѡ
    OLED_DC = 1; //ʧ������
}

/**************************************************************************************************
 *@����            OLED_Init
 *
 *@����            ��ʼ��OLED
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
void OLED_Init(void)
{ 	
    set_hard_spi_speed(0x03);
    OLED_DC = 1;
    OLED_CS = 1;
    
    OLED_RES=0;
    Delay_ms(100);
    OLED_RES=1;
    Delay_ms(100);
					  
    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
    OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
    OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
    OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_CMD);//-not offset
    OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
    OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)  //ҳѰַģʽ
    OLED_WR_Byte(0x00,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
    OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
	
    OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
    OLED_Clear();
    set_hard_spi_speed(0x00);
} 
/**************************************************************************************************
 *@����            OLED_Set_Pos
 *
 *@����            ����OLED��ʼҳ����ʼ��
 *
 *@�������         x:0-127
 *                  y:0-7
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
void OLED_Set_Pos(uint8 x, uint8 y) 
{ 
    OLED_WR_Byte(0xb0+y,OLED_CMD);//��ʼҳ��ַ
    OLED_WR_Byte((x&0x0f),OLED_CMD); //��ʼ�е͵�ַ
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD); //��ʼ�иߵ�ַ 
}
/**************************************************************************************************
 *@����            OLED_Clear
 *
 *@����            OLED����
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
void OLED_Clear(void)  
{  	    
    for(uint8 i = 0; i < 8; i++)  
    {  
        OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
	OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ���е͵�ַ
	OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ���иߵ�ַ   
	for(uint8 n = 0; n < 128; n++)
        {
            OLED_WR_Byte(0,OLED_DATA); 
        }
    } 
}

/**************************************************************************************************
 *@����            OLED_Display_On
 *
 *@����            ����OLED��ʾ
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
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
/**************************************************************************************************
 *@����            OLED_Display_Off
 *
 *@����            �ر�OLED��ʾ
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
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	
/**************************************************************************************************
 *@����            OLED_SetPixel
 *
 *@����            OLED�����������ص�״̬
 *
 *@�������        x - x����
 *                y - y����
 *                PixelValue - ���ص�״̬��1����䣬0����գ�
 *
 *@����            ��
 *
 *�������
 *
 *��
 * 
 *@����     ��
 *˵�����涨OLED��ʾ�������ϽǶ��㴦Ϊ����ԭ��(0,0),
 *      x���������������ҡ�
 *      y���������������¡�
 *      ����ԭ��(0,0)��ӦOLED_DISPLAY[0][0],������ҳ��һ�����ص�
 *      ����(127,63)ΪOLED��Ļ���½Ƕ���
 **************************************************************************************************
 */
void OLED_SetPixel(int32 x, int32 y, int32 PixelValue)
{
    int32 pos,bx,temp=0;
    
    pos = y/8;//����y��������ҳ
    bx = y%8;//����λƫ��
    temp = 1<<bx;
    if(PixelValue)
    {
        OLED_DISPLAY[pos][x]|=temp;
    }
    else 
    {
        OLED_DISPLAY[pos][x]&=~temp;	  
    }
}

/**************************************************************************************************
 *@����            OLED_GetPixel
 *
 *@����            ��ȡ�������ص�״̬
 *
 *@�������        x - x����
 *                y - y����
 *
 *@����            ��
 *
 *�������
 *
 *��
 * 
 *@����          PixelValue - ���ص�״̬
 **************************************************************************************************
 */
int32 OLED_GetPixel(int32 x, int32 y)
{
    int32 pos,bx,temp=0;

    pos = y/8;//����y��������ҳ
    bx = y%8;//����λƫ��
    temp = 1<<bx;

    if(OLED_DISPLAY[pos][x] & temp)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**************************************************************************************************
 *@����            OLED_ReverseAllPixel
 *
 *@����            OLED�������ص�ȡ��
 *
 *@�������        
 *
 *@����            ��
 *
 *�������
 *
 *��
 * 
 *@����         
 **************************************************************************************************
 */
void OLED_ReverseAllPixel(void)
{
    uint8 *value;   
    value = (uint8*)OLED_DISPLAY;//����ָ��תΪһ��ָ��
    for(uint16 i = 0; i < 1024; i++,value++)
    {
        *value = ~*value;      
    }
}
/**************************************************************************************************
 *@����            OLED_Refresh_Display
 *
 *@����            OLED������ʾ
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
void OLED_Refresh_Display(void)
{
    uint8 *value;
    value = (uint8*)OLED_DISPLAY;//����ָ��תΪһ��ָ��
    OLED_WR_Byte (0xb0,OLED_CMD);//��ʼҳ��0
    OLED_WR_Byte (0x00,OLED_CMD); //��ʼ�е͵�ַΪ0
    OLED_WR_Byte (0x10,OLED_CMD); //��ʼ�иߵ�ַΪ0
    OLED_WR_Bytes(value,1024);
}
/**************************************************************************************************
 *@����            OLED_Clear_Display
 *
 *@����            OLED����
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
void OLED_Clear_Display(void)  
{  
    uint8 *value;
    value = (uint8*)OLED_DISPLAY;//����ָ��תΪһ��ָ��
    
    for(uint32 i = 0; i < 1024; i++)
    {
      value[i] = 0x00;
    } 
    OLED_Refresh_Display();
}
/**************************************************************************************************
 *@����            OLED_Clear_Ram
 *
 *@����            OLED���Դ�
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
void OLED_Clear_Ram(void)  
{  
    uint8 *value;
    value = (uint8*)OLED_DISPLAY;//����ָ��תΪһ��ָ��
    
    for(uint32 i = 0; i < 1024; i++)
    {
      value[i] = 0x00;
    } 
}
/**************************************************************************************************
 *@����            OLED_ShowChinese
 *
 *@����            OLED��ʾ���֣�����ȡģ��ʽ����
 *
 *@�������        iStartX - ���x����
 *                iStartY - ���y����
 *                code - GBK��������
 *                size - �����񣨵ȿ�ȸߣ�
 *
 *@����            ��
 *
 *�������
 *
 *��
 * 
 *@����     ��
˵���� ����ȡģ��ʽ��
                 ��iStartY �� ��iStartY+1  ��  .......
��iStartX      �� H(�ֽ�1)      �� H          .
              �� L(�ֽ�1)      �� L          .
��iStartX+1  �� H(�ֽ�2)       �� H         .
   .        �� L(�ֽ�2)       �� L         .
   .        .                 .           .
   .       .                 .           .
   .      .                 .           .
 **************************************************************************************************
 */
void OLED_ShowChinese(uint8 iStartX, uint8 iStartY, uint8 *code,uint16 size)
{
    uint8 uiPixIndex = 0;
    uint8 y0 = iStartY;
    for(uint8 j = 0; j < size; j++,code++,uiPixIndex = 0)
    {
        for(uint8 k = 0; k < size; k++)
        {
            if(uiPixIndex == 8)
            {
                uiPixIndex = 0;
                code++;
            }
            OLED_SetPixel(iStartX,iStartY, OLED_GET_PAGE_BIT(*code, uiPixIndex));            
            uiPixIndex++;
            iStartY++;
        }        
        iStartY = y0;
        iStartX++;
    }
}
/**************************************************************************************************
 *@����            OLED_ShowChar
 *
 *@����            OLED��ʾһ���ַ�
 *
*@�������        iStartX - ���x����
 *                iStartY - ���y����
 *                ascii_code - �ַ���ASCII��
 *                size - ��������С1206��
 *
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
void OLED_ShowChar(uint8 iStartX, uint8 iStartY,uint8 ascii_code,uint16 size)
{
    uint8 uiPixIndex = 0;
    uint8 y0 = iStartY;
    uint8 __code const *code; //����һ��ָ�룬ָ��uint8������const��

    if(size == 12) //���Ϊ12������
    {
        code = asc2_1206[ascii_code - ' '];
    }
    else if(size == 16) //�����16������
    {
        code = asc2_1608[ascii_code - ' '];
    }
   
    
    for(uint8 j = 0; j < size/2; j++,code++,uiPixIndex = 0)//��ѭ�����ֿ���size/2��ʾ��ǣ�
    {
        for(uint8 k = 0; k < size; k++) //��ѭ�����ָߣ�
        {
            if(uiPixIndex == 8)//�ж��Ƿ�д��һ���ֽ�
            {
                uiPixIndex = 0;
                code++;
            }
            OLED_SetPixel(iStartX,iStartY, OLED_GET_PAGE_BIT(*code, uiPixIndex));//�����ֽ�λ�ж�������仹�����            
            uiPixIndex++;//�ֽ�λ����
            iStartY++;//��һ��
        }        
        iStartY = y0;//��ʼ��
        iStartX++;//��һ��
    }
}
/**************************************************************************************************
 *@����            OLED_ShowNum
 *
 *@����            OLED��ʾһ������
 *
*@�������        iStartX - ���x����
 *                iStartY - ���y����
 *                num - ����
 *                size - ��������С1206��
 *
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
void OLED_ShowNum(uint8 iStartX, uint8 iStartY,uint32 num ,uint16 size)
{
    uint16 len_str = 0;
    uint16 x;
    uint8 len = 0;
    uint8 temp = 0;
    uint32 num_len = num;
    do
    {
        num_len /= 10;
        len++;
    }while(num_len!=0);//�������ֵ�λ��
      
    for(uint8 i = 0; i <len ; i++)
    {
        temp = (num/OLED_Pow(10,len-i-1))%10; //ȡ����λ
        
        x = i*size/2 - len_str ;
        if(x+iStartX+size >= 127) //����޷�������ʾһ���ַ����Ƶ���һ��
        {
            len_str = i*size/2;
            iStartY += size; //��һ��
            iStartX = 0; //��ʼƫ������
            x = 0;//�ӵ����п�ʼ
            if(iStartY > 63) //�����һ���޷�������ʾһ���ַ���break
            {
                break;
            }
        }
        
        OLED_ShowChar(x+iStartX,iStartY,temp+'0',size);//���ַ�λ��    
    }
}
/**************************************************************************************************
 *@����            OLED_Pow
 *
 *@����            OLED�ݺ���
 *
*@�������        base - ����
 *                index - ָ��
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
uint32 OLED_Pow(uint8 base,uint8 index)
{
    uint32 num = 1;
    while(index--)
    {
        num *= base;
    }
    return num;
}
/**************************************************************************************************
 *@����            OLED_ShowString
 *
 *@����            OLED��ʾ�ַ���
 *
*@�������        iStartX - ���x����
 *                iStartY - ���y����
 *                str - �ַ���
 *                size - ��������С1206��
 *
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
void OLED_ShowString(uint8 iStartX, uint8 iStartY,uint8 *str ,uint16 size)
{
  uint8 i = 0;;
  uint16 len = 0;
  uint16 x = 0;
  while(*str!= '\0')//��������ַ���ĩβ���˳�ѭ��
  {    
       x = i*size/2 - len ;
        if(x+iStartX+size >= 127) //����޷�������ʾһ���ַ����Ƶ���һ��
        {
            len = i*size/2;
            iStartY += size;
            iStartX = 0;
            x = 0;
            if(iStartY > 63) //�����һ���޷�������ʾһ���ַ���break
            {
                break;
            }
        }
        OLED_ShowChar(x+iStartX,iStartY,*str,size);//���ַ�λ��
        str++;//��һ���ַ���
        i++;
  }
}
/**************************************************************************************************
 *@����            OLED_DrawLine
 *
 *@����            OLED��һ����
 *
 *@�������        iStartX - ���x����
 *                iStartY - ���y����
 *                iEndX - �յ�x����
 *                iEndY - �յ�y����
 *                fill - ���(0������䣬1�����)
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
void OLED_DrawLine(int16 iStartX, int16 iStartY, int16 iEndX, int16 iEndY, int16 fill)
{
    /*----------------------------------*/
    /* Variable Declaration				*/
    /*----------------------------------*/
    int16  iDx, iDy;
    int16  iIncX, iIncY;
    int16  iErrX = 0, iErrY = 0;
    int16  iDs;
    int16  iCurrentPosX, iCurrentPosY;

    /*----------------------------------*/
    /* Initialize						*/
    /*----------------------------------*/
    iErrX = 0;
    iErrY = 0;
    iDx = iEndX - iStartX; //X���ֵ
    iDy = iEndY - iStartY; //Y���ֵ
    iCurrentPosX = iStartX;
    iCurrentPosY = iStartY;

    if(iDx > 0) //X���ֵ����0
    {
        iIncX = 1;
    }
    else
    {
        if(iDx == 0) //X���ֵ����0
        {
            iIncX = 0;
        }
        else   //X���ֵС��0
        {
            iIncX = -1;
            iDx = -iDx; //iDxȡ��
        }
    }

    if(iDy > 0) //Y���ֵ����0
    {
        iIncY = 1;
    }
    else
    {
        if(iDy == 0) //Y���ֵ����0
        {
            iIncY = 0;
        }
        else    //Y���ֵС��0
        {
            iIncY = -1;
            iDy = -iDy;
        }
    }

    if(iDx > iDy) //б��С��45��
    {
        iDs = iDx;
    }
    else     //б�ʴ��ڵ���45��
    {
        iDs = iDy;
    }

    /*----------------------------------*/
    /* Process							*/
    /*----------------------------------*/
    for(uint8 i = 0; i <= iDs+1; i++)
    {
        OLED_SetPixel(iCurrentPosX,iCurrentPosY, fill);//��ǰλ�û���
        iErrX += iDx; //X��ƫ��
        if(iErrX > iDs)
        {
            iErrX -= iDs;
            iCurrentPosX += iIncX;
        }
        iErrY += iDy; //Y��ƫ��
        if(iErrY > iDs)
        {
            iErrY -= iDs;
            iCurrentPosY += iIncY;
        }
    }
}
/**************************************************************************************************
 *@����            OLED_DrawDottedLine
 *
 *@����            OLED������
 *
 *@�������        iStartX - ���x����
 *                iStartY - ���y����
 *                iEndX - �յ�x����
 *                iEndY - �յ�y����
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
void OLED_DrawDottedLine(int16 iStartX, int16 iStartY, int16 iEndX, int16 iEndY)
{
    /*----------------------------------*/
    /* Variable Declaration				*/
    /*----------------------------------*/
    int16  iDx, iDy;
    int16  iIncX, iIncY;
    int16  iErrX = 0, iErrY = 0;
    int16  iDs;
    int16  iCurrentPosX, iCurrentPosY;
    uint16 Dotted_piont = 1;
    bool point = 1;
    /*----------------------------------*/
    /* Initialize						*/
    /*----------------------------------*/
    iErrX = 0;
    iErrY = 0;
    iDx = iEndX - iStartX; //X���ֵ
    iDy = iEndY - iStartY; //Y���ֵ
    iCurrentPosX = iStartX;
    iCurrentPosY = iStartY;

    if(iDx > 0) //X���ֵ����0
    {
        iIncX = 1;
    }
    else
    {
        if(iDx == 0) //X���ֵ����0
        {
            iIncX = 0;
        }
        else   //X���ֵС��0
        {
            iIncX = -1;
            iDx = -iDx; //iDxȡ��
        }
    }

    if(iDy > 0) //Y���ֵ����0
    {
        iIncY = 1;
    }
    else
    {
        if(iDy == 0) //Y���ֵ����0
        {
            iIncY = 0;
        }
        else    //Y���ֵС��0
        {
            iIncY = -1;
            iDy = -iDy;
        }
    }

    if(iDx > iDy) //б��С��45��
    {
        iDs = iDx;
    }
    else     //б�ʴ��ڵ���45��
    {
        iDs = iDy;
    }

    /*----------------------------------*/
    /* Process							*/
    /*----------------------------------*/
    for(uint8 i = 0; i <= iDs+1; i++)
    {
        OLED_SetPixel(iCurrentPosX,iCurrentPosY, point);//��ǰλ�û���
        iErrX += iDx; //X��ƫ��
        if(iErrX > iDs)
        {
            iErrX -= iDs;
            iCurrentPosX += iIncX;
        }
        iErrY += iDy; //Y��ƫ��
        if(iErrY > iDs)
        {
            iErrY -= iDs;
            iCurrentPosY += iIncY;
        }
        
        if(++Dotted_piont && Dotted_piont%3 == 0)//�������߼��
        {
            point = !point;
        }
    }
}
/**************************************************************************************************
 *@����            OLED_DrawCircle
 *
 *@����            OLED��Բ
 *
 *@�������        uiCx - Բ��x����
 *                uiCy - Բ��y����
 *                uiRadius - �뾶
 *                eEdgeColor - ��Ե���(0������䣬1�����)
 *                eFillColor - Բ���������(0������䣬1�����)
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
void OLED_DrawCircle(uint8 uiCx, uint8 uiCy, uint8 uiRadius, uint8 eEdgeColor, uint8 eFillColor)
{
     /*----------------------------------*/
    /* Variable Declaration				*/
    /*----------------------------------*/
    uint8 uiPosXOffset, uiPosYOffset;
    int16 uiPosXOffset_Old, uiPosYOffset_Old;
    int16 iXChange, iYChange, iRadiusError;

    /*----------------------------------*/
    /* Initialize						*/
    /*----------------------------------*/
    uiPosXOffset				= uiRadius;
    uiPosYOffset				= 0;
    uiPosXOffset_Old			= 0xFFFF;
    uiPosYOffset_Old			= 0xFFFF;
    iXChange					= 1 - 2 * uiRadius;
    iYChange					= 1;
    iRadiusError				= 0;

    /*----------------------------------*/
    /* Process							*/
    /*----------------------------------*/
    if(uiRadius < 1) //�뾶С��1
    {
        OLED_SetPixel(uiCx, uiCy, eEdgeColor);
    }
    else
    {
        while(uiPosXOffset >= uiPosYOffset)
        {
            if((uiPosXOffset_Old != uiPosXOffset) || (uiPosYOffset_Old != uiPosYOffset) )
            {
                // Fill the circle ���ȦȦ
                if((uiRadius > 1) && (eFillColor != 2) && (uiPosXOffset_Old != uiPosXOffset))
                {

                    OLED_DrawLine(uiCx-uiPosXOffset, uiCy-uiPosYOffset+1, uiCx-uiPosXOffset, uiCy+uiPosYOffset-1, eFillColor);
                    OLED_DrawLine(uiCx+uiPosXOffset, uiCy-uiPosYOffset+1, uiCx+uiPosXOffset, uiCy+uiPosYOffset-1, eFillColor);
                    uiPosXOffset_Old = uiPosXOffset;
                }
                OLED_DrawLine(uiCx-uiPosYOffset, uiCy-uiPosXOffset+1, uiCx-uiPosYOffset, uiCy+uiPosXOffset-1, eFillColor);
                OLED_DrawLine(uiCx+uiPosYOffset, uiCy-uiPosXOffset+1, uiCx+uiPosYOffset, uiCy+uiPosXOffset-1, eFillColor);
                uiPosYOffset_Old = uiPosYOffset;

                // Draw edge.
                OLED_SetPixel(uiCx+uiPosXOffset, uiCy+uiPosYOffset, eEdgeColor);
                OLED_SetPixel(uiCx-uiPosXOffset, uiCy+uiPosYOffset, eEdgeColor);
                OLED_SetPixel(uiCx-uiPosXOffset, uiCy-uiPosYOffset, eEdgeColor);
                OLED_SetPixel(uiCx+uiPosXOffset, uiCy-uiPosYOffset, eEdgeColor);
                OLED_SetPixel(uiCx+uiPosYOffset, uiCy+uiPosXOffset, eEdgeColor);
                OLED_SetPixel(uiCx-uiPosYOffset, uiCy+uiPosXOffset, eEdgeColor);
                OLED_SetPixel(uiCx-uiPosYOffset, uiCy-uiPosXOffset, eEdgeColor);
                OLED_SetPixel(uiCx+uiPosYOffset, uiCy-uiPosXOffset, eEdgeColor);
            }
            uiPosYOffset++;
            iRadiusError += iYChange;
            iYChange += 2;
            if ((2 * iRadiusError + iXChange) > 0)
            {
                uiPosXOffset--;
                iRadiusError += iXChange;
                iXChange += 2;
            }
        }
    }
}
/**************************************************************************************************
 *@����            OLED_DrawRectangle
 *
 *@����            OLED������
 *
 *@�������        uiStartX - ���x����
 *                uiStartY - ���y����
 *                uiWidth - ���
 *                uiHeight - �߶�
 *                eEdgeColor - ��Ե���(0������䣬1�����)
 *                eFillColor - Բ���������(0������䣬1�����)
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
void OLED_DrawRectangle(uint16 uiStartX, uint16 uiStartY, uint16 uiWidth, uint16 uiHeight, uint8 eEdgeColor, uint8 eFillColor)
{
    uint16 uiColumnIndex;

    if((uiWidth > 0) && (uiHeight > 0))
    {
        if((uiWidth == 1) && (uiHeight == 1))
        {
            OLED_SetPixel(uiStartX, uiStartY, eEdgeColor);
        }
        else if(uiWidth == 1)
        {
            OLED_DrawLine(uiStartX, uiStartY, uiStartX, uiStartY+uiHeight-1, eEdgeColor);
        }
        else if(uiHeight == 1)
        {
            OLED_DrawLine(uiStartX, uiStartY, uiStartX+uiWidth-1, uiStartY, eEdgeColor);
        }
        else
        {
            // Draw edge.
            // Check and set changed page and column index is in edge display action.�������߹��ɾ���
            OLED_DrawLine(uiStartX, uiStartY, uiStartX, uiStartY+uiHeight-1, eEdgeColor);
            OLED_DrawLine(uiStartX+uiWidth-1, uiStartY, uiStartX+uiWidth-1, uiStartY+uiHeight-1, eEdgeColor);
            OLED_DrawLine(uiStartX, uiStartY, uiStartX+uiWidth-1, uiStartY, eEdgeColor);
            OLED_DrawLine(uiStartX, uiStartY+uiHeight-1, uiStartX+uiWidth-1, uiStartY+uiHeight-1, eEdgeColor);
            // Fill area.
            if((eFillColor != 2) && (uiWidth > 2) && (uiHeight > 2))
            {
                for(uiColumnIndex=(uiStartX+1); uiColumnIndex<(uiStartX+uiWidth-1); uiColumnIndex++)
                {
                    OLED_DrawLine(uiColumnIndex, uiStartY+1, uiColumnIndex, uiStartY+uiHeight-2, eFillColor);
                }
            }
        }
    }
}
/**************************************************************************************************
 *@����            OLED_DrawEllipse
 *
 *@����            OLED����Բ
 *
 *@�������        uiCx - Բ��x����
 *                uiCy - Բ��y����
 *                a - �᳤
 *                b - �ݳ�
 *                fill - ��Ե���(0������䣬1�����)
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
void OLED_DrawEllipse(uint8 uiCx,uint8 uiCy,int a,int b,uint8 fill)    //�е�Bresenham�㷨����Բ
{
    int x,y;
    float d1,d2;
    x = 0;y = b;
    d1=b*b+a*a*(-b+0.25);

    OLED_SetPixel(uiCx+x,uiCy+y,fill);
    OLED_SetPixel(uiCx-x,uiCy-y,fill);
    OLED_SetPixel(uiCx-x,uiCy+y,fill);
    OLED_SetPixel(uiCx+x,uiCy-y,fill);
 
    while(b*b*(x+1) < a*a*(y-0.5)){
        if(d1<=0){
            d1+=b*b*(2*x+3);
            x++;
        }
        else{
            d1+=b*b*(2*x+3)+a*a*(-2*y+2);
            x++;
            y--;
        }
        OLED_SetPixel(uiCx+x,uiCy+y,fill);
        OLED_SetPixel(uiCx-x,uiCy-y,fill);
        OLED_SetPixel(uiCx-x,uiCy+y,fill);
        OLED_SetPixel(uiCx+x,uiCy-y,fill);
    }
    d2=b*b*(x+0.5)*(x+0.5)+a*a*(y-1)*(y-1)-a*a*b*b;
    while(y>0){
        if(d2<=0){
            d2+=b*b*(2*x+2)+a*a*(-2*y+3);
            x++,y--;
        }
        else{
            d2+=a*a*(-2*y+3);
            y--;
        }
        OLED_SetPixel(uiCx+x,uiCy+y,fill);
        OLED_SetPixel(uiCx-x,uiCy-y,fill);
        OLED_SetPixel(uiCx-x,uiCy+y,fill);
        OLED_SetPixel(uiCx+x,uiCy-y,fill);
    }
}
/**************************************************************************************************
 *@����            OLED_Drawcos
 *
 *@����            OLED��cosͼ(һ������)
 *
 *@�������        uiStartX - ���x����
 *                uiStartY - ���y����
 *                EnlargeX - x�Ŵ�ϵ��
 *                EnlargeY - y�Ŵ�ϵ��
 *                fill - ��Ե���(0������䣬1�����)
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
void OLED_Drawcos(uint8 uiStartX,uint8 uiStartY,uint8 EnlargeX,uint8 EnlargeY,uint8 fill)
{
    double y;                   
    int x;                   
    for(y = 1;y >= -1;y -= 0.05) 
    {
        x=(int)(acos(y)*EnlargeX);       
        OLED_SetPixel(x+uiStartX,(int)((1-y)*EnlargeY)+uiStartY,fill);  //0-�е�ֵ
        OLED_SetPixel((int)(2*PI*EnlargeX)-x+uiStartX,(int)((1-y)*EnlargeY)+uiStartY,fill);  
    }
    OLED_SetPixel((int)(PI*EnlargeX)+uiStartX,2*EnlargeY+uiStartY,1);
}
/**************************************************************************************************
 *@����            OLED_DrawSin
 *
 *@����            OLED��sinͼ(һ������)
 *
 *@�������        uiStartX - ���x����
 *                uiStartY - ���y����
 *                EnlargeX - x�Ŵ�ϵ��
 *                EnlargeY - y�Ŵ�ϵ��
 *                fill - ��Ե���(0������䣬1�����)
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
void OLED_DrawSin(uint8 uiStartX,uint8 uiStartY,uint8 EnlargeX,uint8 EnlargeY,uint8 fill)
{   
    double y;                  
    int x;                    
    for(y = 1;y >= 0;y -= 0.05)  
    {
        x=(int)(asin(y)*EnlargeX);    
        OLED_SetPixel(x+uiStartX,(int)((1-y)*EnlargeY)+uiStartY,fill);
        OLED_SetPixel((int)(PI*EnlargeX-x)+uiStartX,(int)((1-y)*EnlargeY)+uiStartY,fill);
    }
    for(y = 0;y >= -1;y -= 0.05)
    {  
        x=(int)(asin(-y)*EnlargeX);
        OLED_SetPixel((int)(PI*EnlargeX+x)+uiStartX,(int)((-y+1)*EnlargeY)+uiStartY,fill);
        OLED_SetPixel((int)(2*PI*EnlargeX-x)+uiStartX,(int)((-y+1)*EnlargeY)+uiStartY,fill);
    }
    OLED_SetPixel((int)(PI*EnlargeX+(double)1/2*PI*EnlargeX)+uiStartX,2*EnlargeY+uiStartY,fill);
}

//��������
void drawCuboid(uint8 x,uint8 y,uint8 width,uint8 height,uint8 depth)
{
    OLED_DrawRectangle(x,y,width,height,1,0);//ǰ�����
    OLED_DrawRectangle(x-depth/2,y+depth/2,width,height,1,0);//�������
  
    OLED_DrawDottedLine(x,y,x,y+height-1);//����(��)
    OLED_DrawDottedLine(x,y+height-1,x-depth/2,y+depth/2+height-1);//����(��)
    OLED_DrawDottedLine(x,y+height-1,x+width-1,y+height-1);//����(��)
    
    OLED_DrawLine(x,y,x-depth/2,y+depth/2,1);//ʵ��(��)
    OLED_DrawLine(x+width-1,y,x-depth/2+width-1,y+depth/2,1);//ʵ��(��)
    OLED_DrawLine(x+width-1,y+height-1,x-depth/2+width-1,y+depth/2+height-1,1);//ʵ��(��)
}