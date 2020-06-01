#include "clock_plate.h"
#include "oled.h"
#include "uart.h"
#include "delay.h"


#define SECOND_HAND    1     //����
#define MINUTE_HAND    2     //����
#define HOUR_HAND      3     //ʱ��

#define CLOCK_RADIUS   31    //�ӱ���Χ�뾶
#define CIRCLE_X       34    //�ӱ���������x
#define CIRCLE_Y       31    //�ӱ���������y

#define CLOCK_SCALE_RADIUS   CLOCK_RADIUS - 2    //�̶ȵ�λ�ð뾶

#define SECOND_HAND_LENGTH   28    //���볤��
#define MINUTE_HAND_LENGTH   27    //���볤��
#define HOUR_HAND_LENGTH     18    //ʱ�볤��

uint32 angle_tan[] = {
//0��  6��  12��  18��  24��  30��  36��  42��  48��   54��    60��  66��   72��   78��   84��    90��
  0,1051,2126,3249,4452,5773,7265,9004,11106,13763,17320,22460,30776,47046,95143,0xffffffff
};//tan������1000��

//Ϊ�˱���ÿ�ζ����¼���λ�ã������齫λ�ñ���������
//ÿ������Ԫ��ռ16��λ����8λ���y���꣬��8λ���x����
uint16 second_coordinate[16];  //��������
uint16 minute_coordinate[16];  //��������
uint16 hour_coordinate[16];    //Сʱ����
uint16 scale_coordinate[12];   //�̶ȵ�����

clock_time pre_time; //ǰһ��ʱ��

/**************************************************************************************************
 *@����            clock_plate_init
 *
 *@����            ��ʼ���ӱ���
 *
*@�������       
 *                
 *@����            ��
 *
 *�������         ��
 * 
 *@����     
 **************************************************************************************************
 */
void clock_plate_init(void)
{
    uint8 x,y;
    
    clock_calculate_coordinate(SECOND_HAND,CLOCK_SCALE_RADIUS);//�������ɿ̶�����
    for(uint8 i = 0; i < 60; i++)
    {
      if(i%5 == 0)//����12���̶ȵ�����
      {
          clock_get_coordinate(i,SECOND_HAND,&x,&y);
          scale_coordinate[i/5] = y;  //��¼y����
          scale_coordinate[i/5] <<= 8;
          scale_coordinate[i/5] |= x; //��¼x����
      }
    }
    
    clock_calculate_coordinate(SECOND_HAND,SECOND_HAND_LENGTH); //����������������
    clock_calculate_coordinate(MINUTE_HAND,MINUTE_HAND_LENGTH); //�������ɷ�������
    clock_calculate_coordinate(HOUR_HAND,HOUR_HAND_LENGTH); //��������ʱ������

    //OLED_DrawCircle(CIRCLE_X,CIRCLE_Y,CLOCK_RADIUS,1,0);//�����ӱ���Χ
    for(uint8 i = 0; i < 12; i++) //����12���̶�
    {
      x = (uint8)scale_coordinate[i];
      y = (uint8)(scale_coordinate[i]>>8);
      OLED_SetPixel(x,y,1);
    }
}
/**************************************************************************************************
 *@����            clock_run
 *
 *@����            ���б��̺���
 *
*@�������         time - ʱ��ṹ��
 *                
 *@����            ��
 *
 *�������         ��
 * 
 *@����     
 **************************************************************************************************
 */
void clock_run(clock_time time)
{ 
    clock_show_time(pre_time,0);//����ǰһ��ʱ��켣
    clock_show_time(time,1);//��ʾ��ǰʱ��
    pre_time = time;//��¼����ʱ��
}
/**************************************************************************************************
 *@����            clock_show_time
 *
 *@����            ������ʾʱ�亯��
 *
*@�������         time - ʱ��ṹ��
 *                 state - ״̬
 *               
 *@����            ��
 *
 *�������         ��
 * 
 *@����     
 **************************************************************************************************
 */
void clock_show_time(clock_time time,uint8 state)
{
    uint8 x,y;
    
    if(time.hours > 12) //���Ϊ24Сʱ
    {
      time.hours -= 12;//תΪ12Сʱ��
    }
    
    if(state)
    {
        OLED_ShowChar(CIRCLE_X-5,2,'1',12); //����1
        OLED_ShowChar(CIRCLE_X,2,'2',12); //����2
        OLED_ShowChar(CIRCLE_X-2,50,'6',12);//����6
        OLED_ShowChar(CIRCLE_X+24,25,'3',12);//����3
        OLED_ShowChar(CIRCLE_X-29,25,'9',12);//����9

        OLED_DrawCircle(CIRCLE_X,CIRCLE_Y,2,1,1);//����ת��
    }

    clock_get_coordinate(time.hours*5+time.minutes/12,HOUR_HAND,&x,&y);
    OLED_DrawLine(CIRCLE_X,CIRCLE_Y,x,y,state); //����ʱ��
    
    clock_get_coordinate(time.minutes,MINUTE_HAND,&x,&y);
    OLED_DrawLine(CIRCLE_X,CIRCLE_Y,x,y,state); //���Ʒ���
    
    clock_get_coordinate(time.seconds,SECOND_HAND,&x,&y);
    OLED_DrawLine(CIRCLE_X,CIRCLE_Y,x,y,state); //��������
}
/**************************************************************************************************
 *@����            clock_calculate_coordinate
 *
 *@����            ������켣���²�����15-30������
 *
*@�������        clock_hand - ���ʱ�롢���롢����
 *                length - ָ�볤��
 *
 *@����            ��
 * 
 *@����     
 **************************************************************************************************
 */
static void clock_calculate_coordinate(uint8 clock_hand,uint8 length)
{
    uint8 x,y;
    uint8 rad = 0;
    uint16 *coordinate_array;
    double value = 0;
    double angel = 0;
    double cal_x,cal_y;
    double all_value;
    
    OLED_Clear_Ram();//���Դ�
    
    OLED_DrawCircle(CIRCLE_X,CIRCLE_Y,length,1,0);//���ƹ켣
    rad = length;
    
    switch(clock_hand) //�ж�ָ������
    {
        case SECOND_HAND:  //����
          coordinate_array = second_coordinate;
          break;
        case MINUTE_HAND: //����
          coordinate_array = minute_coordinate;
          break;
        case HOUR_HAND:  //ʱ��
          coordinate_array = hour_coordinate;
          break;
        default:
          break;
    }
    
    for(uint8 k = 0; k < 16; k++) //����켣���²�15-30������
    {
       angel = angle_tan[k];
       all_value = 977889999;
       for(uint8 i = CIRCLE_X - 1; i <= CIRCLE_X + rad; i++)
       {
          for(uint8 j = CIRCLE_Y - 1; j <= CIRCLE_Y + rad; j++)
          {
              if(OLED_GetPixel(i,j)) //ɨ����Ĺ켣
              {              
                  cal_x = i-CIRCLE_X;
                  cal_y = j-CIRCLE_Y;
                  value =cal_y*10000 - angel*cal_x;
                  if(value < 0)//��������
                  {
                      value = -value;
                  }
                  if(all_value - value> 0) //Ѱ������ʵ�����
                  {
                      all_value = value;
                      x = i;
                      y = j;                           
                  }                                                 
              }
          }
       }
      coordinate_array[k] = y;  //��¼y����
      coordinate_array[k] <<= 8;
      coordinate_array[k] |= x; //��¼x����
    }
    OLED_Clear_Ram();//���Դ�
}   
/**************************************************************************************************
 *@����            clock_get_coordinate
 *
 *@����            ��ȡ����
 *
*@�������        clock_num - ʱ�����֣�0-59��
 *                clock_hand - ���ʱ�롢���롢����
 *
 *@����            ��
 *
 *�������       x - x����
 * 
 *               y - y����
 *��
 * 
 *@����     ��
 **************************************************************************************************
 */
void clock_get_coordinate(uint8 clock_num,uint8 clock_hand,uint8 *x,uint8 *y)
{
    uint8 num = 0;
    uint8 area_type;
    uint8 rad = 0;
    uint16 *coordinate_array;

    switch(clock_hand) //�ж�ָ������
    {
        case SECOND_HAND:  //����
          coordinate_array = second_coordinate;
          rad = SECOND_HAND_LENGTH;  //���볤��
          break;
        case MINUTE_HAND: //����
          coordinate_array = minute_coordinate;
          rad = MINUTE_HAND_LENGTH;  //���볤��
          break;
        case HOUR_HAND:  //ʱ��
          coordinate_array = hour_coordinate;
          rad = HOUR_HAND_LENGTH;  //ʱ�볤��
          break;
        default:
          break;
    }
    
    num = clock_convert_num(clock_num,&area_type);//ת��ʱ������Ϊ15-30����������
    *x = (uint8)coordinate_array[num-15];
    *y = (uint8)(coordinate_array[num-15]>>8);
    
     switch(area_type) //�ж����������������ͣ��Ա㻻��ó���Ӧ����
    {
        case 0:  //0-15
          *y = CIRCLE_Y + CIRCLE_Y - *y;
          break;
        case 1:  //15-30 
          break;
        case 2:    //30-45
          *x = CIRCLE_X + CIRCLE_X - *x;
          break;
        case 3:   //45-60   
          *x = CIRCLE_X + CIRCLE_X - *x;
          *y = CIRCLE_Y + CIRCLE_Y - *y;   
          break;
        case 4:   //����0�Ĵ���
          *y -= rad + rad;//y�����ȥ2��rad
          break;
        case 5:   //����45�Ĵ���
          *x -= rad + rad;//x�����ȥ2��rad
          break;
        default:
          break;
    }            
}
/**************************************************************************************************
 *@����            clock_convert_num
 *
 *@����            ת��ʱ������Ϊ15-30֮������֡������������
 *
*@�������        num - ʱ�����֣�0-59��
 *
 *@����            ��
 *
 *�������       area_type - ��������
 * 
 *��
 * 
 *@����     result - ���֣�15-30��
 **************************************************************************************************
 */
//������ת����15-30��֮�������������ֵ���������
static uint8 clock_convert_num(uint8 num,uint8 *area_type)
{
    uint8 result = 0;
    if(num < 15) //0-15
    {
        result = 30 - num;
        *area_type = 0;
        if(num == 0) //����0���⴦��
        {
            *area_type = 4;
        }
    }
    else if(num < 30) //15-30
    {
        result = num;
        *area_type = 1;
    }
    else if(num < 45) //30-45
    {
        result = 60 - num;
        *area_type = 2;
    }
    else if(num < 60) //45-60
    {
        result = num - 30;
        *area_type = 3;
        if(num == 45) //ʱ��45���⴦��
        {
            *area_type = 5; 
        }
    }
    return result;
}
