#include "system_clock.h"
#include "ioSTC8.h"


icr_freq original_icr_freq; //��ʼIRCƵ�ʵ�������

/**************************************************************************************************
 *@����            system_clock_init
 *
 *@����            ϵͳʱ�ӳ�ʼ��
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
void system_clock_init(void)
{
    get_original_irc_freq_adjust();//��ȡ��ʼIRCƵ�ʵ�������
    set_system_clock_source(IRC24M_CLOCK, 1);//�����ڲ���ƵRC����������Ƶ
}
/**************************************************************************************************
 *@����            get_original_irc_freq_adjust
 *
 *@����            ��ȡ��ʼIRCƵ�ʵ�������
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
void get_original_irc_freq_adjust(void)
{
    original_icr_freq.irtrim = IRTRIM;
    original_icr_freq.lirtrim = LIRTRIM;
    original_icr_freq.get_status = 0xaa;//����Ѿ���ȡ����
}
/**************************************************************************************************
 *@����            set_irc_freq_adjust
 *
 *@����            ����IRCƵ�ʵ�������
 *
 *@�������       coarse_tuning - IRCƵ�ʵ�������
 *                fine_tuning - IRCƵ��΢������
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
void set_irc_freq_adjust(uint8 coarse_tuning, uint8 fine_tuning,uint8 mode)
{
    //�ж��Ƿ���س�ʼ������
    if((mode == ORIGINAL_ICR_FREQ_ADJUST) && (original_icr_freq.get_status == 0xaa))
    {
        IRTRIM = original_icr_freq.irtrim;
        LIRTRIM = original_icr_freq.lirtrim;
    }
    else
    {
        IRTRIM = coarse_tuning;
        LIRTRIM = fine_tuning;
    }
}
/**************************************************************************************************
 *@����            get_system_clock_source
 *
 *@����            ��ȡ��ǰϵͳʱ��Դ
 *
 *@�������       
 *
 *@����            ��
 *
 *�������
 *
 *��
 * 
 *@����     source - ʱ��Դ
 **************************************************************************************************
 */
clock_info get_system_clock_source(void)
{
    clock_info source;
    P_SW2 |= 0x80;
    
    switch(CKSEL&0x03)
    {
      case IRC24M_CLOCK:
        source.clock_source = IRC24M_CLOCK;
        break;
      case HXCO_CLOCK:
        source.clock_source = IRC24M_CLOCK;
        break;
      case IRC32K_CLOCK:
        source.clock_source = IRC32K_CLOCK;
        break;
      default:
        source.clock_source = EXTERLAN_INPUT_CLOCK;
        break;
    }
    source.clock_div = CLKDIV;
    P_SW2 &= 0x7f;
    return source;      
}
/**************************************************************************************************
 *@����            set_system_clock_source
 *
 *@����            ϵͳʱ��Դѡ��
 *
 *@�������       clock_source - ʱ��Դ
 *                clock_div - ��Ƶϵ����1-255��
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
void set_system_clock_source(uint8 clock_source, uint8 clock_div)
{
    clock_info old_clock_source;
    old_clock_source = get_system_clock_source();//��ȡ֮ǰϵͳʱ��Դ
    P_SW2 |= 0x80;
    if(clock_source != old_clock_source.clock_source)
    {   
        switch(clock_source) //�ж�ϵͳʱ��Դ������
        {
            case IRC24M_CLOCK:
                IRC24MCR |= 0x01;//ʹ���ڲ���ƵRC����
                while(IRC24MCR&0x8f == 0);//�ȴ���ƵRC�����ȶ�
                break;
            case HXCO_CLOCK:
                XOSCCR |= 0x01;//ʹ���ⲿ����
                while(XOSCCR&0x8f == 0);//�ȴ������ȶ�
                break;
            case IRC32K_CLOCK:
                IRC32KCR |= 0x01;//ʹ���ڲ���ƵRC����
                while(IRC32KCR&0x8f == 0);//�ȴ���ƵRC�����ȶ�
                break;
            default:
                break;
        }
        CKSEL &= 0xfc; //���ʱ��Դ
        CKSEL |= clock_source; //������ϵͳʱ��Դ
        switch(old_clock_source.clock_source)//�ж�֮ǰʱ��Դ������
        {
            case IRC24M_CLOCK:
                IRC24MCR &= 0xfe;//ʧ���ڲ���ƵRC����
                break;
            case HXCO_CLOCK:
                XOSCCR &= 0xfe;//ʧ���ⲿ����
                break;
            case IRC32K_CLOCK:
                IRC32KCR &= 0xfe;//ʧ���ڲ���ƵRC����
                break;
            default:
                break;
        }
    }
    
    CLKDIV = clock_div;
    P_SW2 &= 0x7f;
}
/**************************************************************************************************
 *@����            set_system_clock_output
 *
 *@����            ����ϵͳʱ�Ӷ������
 *
 *@�������       clock_div - �����Ƶ(0-7),��Ӧ2��(0-7�ݴη�)
 *                clock_pin - ʱ������ܽ�
 *                output_enable - ʹ�������0��ʧ��, !0:ʹ�ܣ�
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
void set_system_clock_output(uint8 clock_div,uint8 clock_pin,uint8 output_enable)
{
    P_SW2 |= 0x80;
    
    CKSEL &= 0xf0;
    if(output_enable != 0)//ʱ�����ʹ��
    {
        if(clock_div == 0)
        {
            CKSEL |= 0x01;
        }
        CKSEL |= clock_div<<1;
        CKSEL&= 0xf7;
        if(clock_pin == CLOCK_OUTPUT_P5_4)
        {
            CKSEL|= 0x08;
        }
    }
    
    P_SW2 &= 0x7f;
}