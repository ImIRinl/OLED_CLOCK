#include "ds3231.h"
#include "i2c_hard.h"

#define BCD_TO_DEC(bcd)   (bcd/16*10 + bcd % 16)  //BCDתDEC
#define DEC_TO_BAC(dec)   (dec/10*16 + dec%10)    //DECתBCD

#define DS3231_ADDR  0xd0  //DS3231�ӻ���ַ

/***********************DS3231��ؼĴ���***************************/
#define SECOND_REGISTER      0x00  //��Ĵ���
//#define MINUTE_REGISTER      0x01  //���ӼĴ���
//#define HOUR_REGISTER        0x02  //Сʱ�Ĵ���
//#define WEEK_REGISTER        0x03  //���ڼĴ���
//#define DAY_REGISTER         0x04  //���ڼĴ���
//#define MONTH_REGISTER       0x05  //���ڼĴ���
//#define YEAR_REGISTER        0x06  //��ݼĴ���
#define CONTROL_REGISTER     0x0e  //���ƼĴ���
#define STATUS_REGISTER      0x0f  //״̬�Ĵ���
#define TMEP_H_REGISTER      0x11  //�¶ȸ�λ�Ĵ���
#define TMEP_L_REGISTER      0x12  //�¶ȵͼĴ���

/*******************************************ʹ��ʾ��***********************************************
ds3231_time set_time,get_time;
uint8 temp,sign;
set_time.seconds = 10
set_time.minutes = 10
set_time.hours = 10
set_time.day =   1
set_time.week =   1
set_time.month =  1
set_time.year =  19
ds3231_init();//��ʼ��ds3231
ds3231_set_time(&set_time);//����ʱ��
ds3231_get_time(&get_time);//��ȡʱ��
ds3231_start_temp_convert();//�����¶�ת��
ds3231_get_temp(&temp,&sign);//��ȡ�¶�����
**************************************************************************************************/
/**************************************************************************************************
 *@����            ds3231_init
 *
 *@����            ds3231��ʼ��
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
void ds3231_init(void)
{ 
    uint8 value[2] ={CONTROL_REGISTER,0x00};
    hard_i2c_transaction(DS3231_ADDR, value, 2, NULL, 0);
}
/**************************************************************************************************
 *@����            ds3231_get_time
 *
 *@����            ��ȡDS3231ʱ��(24Сʱ��)
 *
 *@�������  
 *
 *@����            time - ʱ��ṹ�� ʮ���ƣ����ʱ�������꣩
 *
 *�������
 *
 *��
 * 
 *@����     0���ɹ�   1��ʧ��
 **************************************************************************************************
 */
uint8 ds3231_get_time(ds3231_time *time)
{
    uint8 *get_time;
    uint8 value,reg = SECOND_REGISTER; 
    get_time = (uint8*)time;
    
    for(uint8 i = 0; i < 7; i++,reg++)
    {
        if(hard_i2c_transaction(DS3231_ADDR, &reg, 1, &value, 1))
        {
            return 1;
        }       
        get_time[i] = BCD_TO_DEC(value);//BCD��תΪʮ����
    }
    
    return 0;
}
/**************************************************************************************************
 *@����            ds3231_set_time
 *
 *@����            ����DS3231ʱ�䣨24Сʱ�ƣ�
 *
 *@�������  
 *
 *@����            time - ʱ��ṹ�� ʮ���ƣ����ʱ�������꣩
 *
 *�������
 *
 *��
 * 
 *@����     0���ɹ�   1��ʧ��
 **************************************************************************************************
 */
uint8 ds3231_set_time(ds3231_time *time)
{ 
    uint8 set_time[8] = {SECOND_REGISTER};
    uint8 *data;
    data = (uint8*)time;
    
    if(data[2] >= 64)//��ֹ�趨12Сʱ��
    {
        return 1;
    }
    
    for(uint8 i = 0; i < 8; i++)
    {
        set_time[i+1] = DEC_TO_BAC(data[i]);//ʮ����תΪBCD��
    }
    
    return(hard_i2c_transaction(DS3231_ADDR, set_time, 8, NULL, 0));
}
/**************************************************************************************************
 *@����            ds3231_start_temp_convert
 *
 *@����            ����DS3231�¶Ȳ���ת�����¶�׼ȷ�ȣ���3�棩
 *
 *@�������  
 *
 *@����            ��
 *
 *�������
 *
 *��
 * 
 *@����     0���ɹ�   1��ʧ��
 *
 *˵���������Ŧ�۵�ع�������£�DS3231�¶Ȳ���ת������һ����64�롣
 **************************************************************************************************
 */
uint8 ds3231_start_temp_convert(void)
{
    uint8 value[2] = {CONTROL_REGISTER};
    //��ȡcontrol�Ĵ���ԭ�ȵ�����
    if(hard_i2c_transaction(DS3231_ADDR, value, 1, value+1, 1))
    {
        return 1;
    }
    value[1] |= 0x20;//�����¶�ת������λ
    //д��control�Ĵ�������
    if(hard_i2c_transaction(DS3231_ADDR, value, 2, NULL, 0))
    {
        return 1;
    }
    return 0;
}
/**************************************************************************************************
 *@����            ds3231_get_temp
 *
 *@����            ��ȡDS3231�¶�
 *
 *@�������  
 *
 *@����            *temp - �¶�����ָ�루�¶ȷŴ���100����
 *                 *sign - ��������ָ�루0 - 0������  1 - 0�����£�
 *
 *�������
 *
 *��
 * 
 *@����     0����ȡ�ɹ�   1����ȡʧ��
 **************************************************************************************************
 */
uint8 ds3231_get_temp(uint16 *temp, uint8 *sign)
{
    uint8 temp_h,temp_l,reg_temp_h,reg_temp_l,reg_status,status;
    
    reg_temp_h = TMEP_H_REGISTER;
    reg_temp_l = TMEP_L_REGISTER;
    reg_status = STATUS_REGISTER;
    
    if(hard_i2c_transaction(DS3231_ADDR, &reg_status, 1, &status, 1))
    {
        return 1;
    }
    
    if(status & 0x04 == 1)//�ж��¶��Ƿ�ת�����
    {
        return 1;
    }
    
    if(hard_i2c_transaction(DS3231_ADDR, &reg_temp_h, 1, &temp_h, 1))
    {
        return 1;
    }
    
    if(hard_i2c_transaction(DS3231_ADDR, &reg_temp_l, 1, &temp_l, 1))
    {
        return 1;
    }
    
    *temp = (((uint16)temp_h <<2)  + ((uint16)temp_l >> 6)) * 25;
    
    if(temp_h & 0x80 == 1)//�¶��Ƿ�Ϊ0������
    {
        *sign = 1;
    }
    
    return 0;
}