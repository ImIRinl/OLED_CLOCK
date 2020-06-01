#ifndef DS3231_H
#define DS3231_H

#include "hal_types.h"

typedef struct 
{
  uint8 seconds;  //�루0-59��
  uint8 minutes;  //�֣�0-59��
  uint8 hours;    //ʱ��0-23��
  uint8 week;     //�ܣ�1-7��
  uint8 day;      //�գ�1-31��
  uint8 month;    //�£�1-12��
  uint8 year;     //�꣨0-99����2000��ʼ����
}ds3231_time;

extern void ds3231_init(void);
extern uint8 ds3231_get_time(ds3231_time *time);
extern uint8 ds3231_set_time(ds3231_time *time);
extern uint8 ds3231_get_temp(uint16 *temp, uint8 *sign);
extern uint8 ds3231_start_temp_convert(void);

#endif