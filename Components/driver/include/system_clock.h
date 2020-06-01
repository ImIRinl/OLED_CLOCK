#ifndef  SYSTEM_CLOCK_H
#define SYSTEM_CLOCK_H

#include "hal_types.h"


#define  IRC24M_CLOCK          0x00   //�ڲ���ƵRC����
#define  HXCO_CLOCK            0x01   //�ⲿ��Ƶ����
#define  EXTERLAN_INPUT_CLOCK  0x10   //�ⲿʱ������
#define  IRC32K_CLOCK          0x11   //�ڲ���ƵRC����

#define  CLOCK_OUTPUT_P5_4     0x54  //ϵͳʱ�����P5.4
#define  CLOCK_OUTPUT_P1_6     0x16  //ϵͳʱ�����P1.6

#define ORIGINAL_ICR_FREQ_ADJUST    0xaa   //ԭ��ICRƵ��

typedef struct 
{
    uint8 lirtrim;   //IRCƵ��΢������
    uint8 irtrim;    //IRCƵ�ʵ�������
    uint8 get_status;    //״̬
}icr_freq;

typedef struct 
{
    uint8 clock_source;  //ʱ��Դ
    uint8 clock_div;     //��Ƶϵ��
}clock_info;

extern void system_clock_init(void);
static void get_original_irc_freq_adjust(void);
extern void set_irc_freq_adjust(uint8 coarse_tuning, uint8 fine_tuning,uint8 mode);
extern clock_info get_system_clock_source(void);
extern void set_system_clock_source(uint8 clock_source, uint8 clock_div);
extern void set_system_clock_output(uint8 clock_div,uint8 clock_pin,uint8 output_enable);



#endif