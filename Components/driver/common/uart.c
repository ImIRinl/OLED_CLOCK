
/********************include******************/
#include "uart.h"
#include "stdarg.h"  //����ʹ������⣬�����ռ1820�ֽڵ�RAM
#include "stdio.h"
#include "stdlib.h"
#include "ioSTC8.h"
#include "heap_manage.h"
/**********************************************/

/**********************************************
���ƣ�uart_init
���ܣ����ڳ�ʼ�����ڲ�24MHz��������9600
������void
���أ�void
**********************************************/
void uart_init(void)
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
	TL1 = 0xCC;		//�趨��ʱ��ֵ
	TH1 = 0xFF;		//�趨��ʱ��ֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
}

/**********************************************
���ƣ�Uart_write
���ܣ����ڷ��ͺ���
������*pBuf--> ����������ָ��  	len-->���ݳ���
���أ�void
**********************************************/
void Uart_write(uint8 *pBuf,int ulLen)
{
 	int i;
    for(i = 0; i < ulLen; i++)
    {
       SBUF=*pBuf;  //���ݼ��ص����ͻ�����
       while(!TI);  //�ȴ��������         
	   TI=0;		//���ͱ�־����
	   pBuf++;     //����ָ��ָ����һ������ 
    }
}

/*********************************************
���ƣ�printfs
���ܣ�����printf���ڴ�ӡ����(�������黺������)
������fmt-->����������־λ����%d��%s��
���أ��������ݳ���
ע�⣺��IARƽ̨����ʹ�ã�
����keilƽ̨��������Ϊ�������ͣ���Ҫ��Ϊ�����������ݲ�������
��ʾlong���� %d ===> %ld
��ʾchar���� %d ===> %bd

*********************************************/

int printfs(char*fmt,...)
{  
    char  pBuf[50];
    int ulen;
    va_list ap;
    va_start(ap, fmt);
    ulen = vsprintf(pBuf, fmt, ap);
	va_end(ap);
	Uart_write((uint8*)pBuf, ulen);
    return ulen;
}	 


/*********************************************
���ƣ�printfd
���ܣ�����printf���ڴ�ӡ����(���ö�̬����ռ仺������)
������fmt-->����������־λ����%d��%s��
���أ��������ݳ���
ע�⣺��IARƽ̨����ʹ�ã�
����keilƽ̨��������Ϊ�������ͣ���Ҫ��Ϊ�����������ݲ�������
��ʾlong���� %d ===> %ld
��ʾchar���� %d ===> %bd
*********************************************/ 

int printfd(char*fmt,...)
{  
    int ulen;
    va_list  ap;
    char *pBuf;
    pBuf = (char*)heap_alloc(60);
    if(pBuf==NULL)
      return 0;
    va_start(ap, fmt);
    ulen = vsprintf(pBuf, fmt, ap);
    va_end(ap);
    Uart_write((uint8*)pBuf, ulen);
    heap_free(pBuf);
    pBuf = NULL;
    return ulen;

}  	  
