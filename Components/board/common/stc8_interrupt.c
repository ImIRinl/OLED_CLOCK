#include <ioSTC8.h>

/***********************˵��*****************************
*������������STC8��Ƭ����IARƽ̨�µĵ������ж�������ڵ�ַ��
*��ʽ��������ʾ;һ���жϲ������ͻ������Ӧ�жϵĺ�����ڡ�
********************************************************/


#pragma vector=INT0_VECTOR //�ⲿ�ж�0�ж�������ڵ�ַ
__interrupt void INT0_Server(void)
{

}


#pragma vector=Timer0_VECTOR //��ʱ��0�ж�������ڵ�ַ
__interrupt void Timer0_Server(void)
{

}


#pragma vector=INT1_VECTOR //�ⲿ�ж�1�ж�������ڵ�ַ
__interrupt void INT1_Server(void)
{

}


#pragma vector=Timer1_VECTOR //��ʱ��01�ж�������ڵ�ַ
__interrupt void Timer1_Server(void)
{

}


#pragma vector=UART1_VECTOR //����1�ж�������ڵ�ַ
__interrupt void UART1_Server(void)
{

}


#pragma vector=ADC_VECTOR //ADC�ж�������ڵ�ַ
__interrupt void ADC_Server(void)
{

}


#pragma vector=LVD_VECTOR //LVD�ж�������ڵ�ַ
__interrupt void LVD_Server(void)
{

}


#pragma vector=PCA_VECTOR //PCA�ж�������ڵ�ַ
__interrupt void PCA_Server(void)
{

}


#pragma vector=UART2_VECTOR //����2�ж�������ڵ�ַ
__interrupt void UART2_Server(void)
{

}


#pragma vector=SPI_VECTOR //SPI�ж�������ڵ�ַ
__interrupt void SPI_Server(void)
{

}


#pragma vector=INT2_VECTOR //�ⲿ�ж�2�ж�������ڵ�ַ
__interrupt void INT2_Server(void)
{

}


#pragma vector=INT3_VECTOR //�ⲿ�ж�3�ж�������ڵ�ַ
__interrupt void INT3_Server(void)
{

}


#pragma vector=Timer2_VECTOR //��ʱ��2�ж�������ڵ�ַ
__interrupt void Timer2_Server(void)
{

}


#pragma vector=INT4_VECTOR //�ⲿ�ж�4�ж�������ڵ�ַ
__interrupt void INT4_Server(void)
{

}


#pragma vector=UART3_VECTOR //����3�ж�������ڵ�ַ
__interrupt void UART3_Server(void)
{

}


#pragma vector=UART4_VECTOR //����4�ж�������ڵ�ַ
__interrupt void UART4_Server(void)
{

}


#pragma vector=Timer3_VECTOR //��ʱ��3�ж�������ڵ�ַ
__interrupt void Timer3_Server(void)
{

}


#pragma vector=Timer4_VECTOR //��ʱ��4�ж�������ڵ�ַ
__interrupt void Timer4_Server(void)
{

}


#pragma vector=CMP_VECTOR //CMP�ж�������ڵ�ַ
__interrupt void CMP_Server(void)
{

}


#pragma vector=PWM_VECTOR //PWM�ж�������ڵ�ַ
__interrupt void PWM_Server(void)
{

}


#pragma vector=PWMFD_VECTOR //PWMFD�ж�������ڵ�ַ
__interrupt void PWMFD_Server(void)
{

}


#pragma vector=I2C //I2C�ж�������ڵ�ַ
__interrupt void I2C_Server(void)
{

}


#pragma vector=CAN //CAN�ж�������ڵ�ַ
__interrupt void CAN_Server(void)
{

}
