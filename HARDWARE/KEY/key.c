#include "stm32f10x.h"
#include "key.h"
#include "delay.h"
//Mini STM32������
//�������� ��������			 
//����ԭ��@ALIENTEK
//2010/5/27   

//������ʼ������
//�ر�ע�⣺�ڸú���֮��JTAG���޷�ʹ�ã�SWDҲ�޷�ʹ�ã�
//�����JTAG���棬�������θú�����
//PA0.13.15 ���ó�����

void KEY_Init(void)
{
	RCC->APB2ENR|=1<<6;     //ʹ��PORTEʱ��

	GPIOE->CRL&=0XFF0000FF;//PE2,3,4,5���ó�����	  
	GPIOE->CRL|=0X00888800;    
	GPIOE->ODR|=0x03C;	   //PA0,A8����
} 
//����������
//���ذ���ֵ
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2!!
u8 KEY_Scan(void)
{	 
	static u8 key_up=1;//�������ɿ���־	

	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY1==0)
		{
			return 1;
		}
		else if(KEY2==0)
		{
			return 2;
		}
		else if(KEY3==0)
		{
			return 3;
		}
		else if(KEY4==0)
		{
			return 4;
		}
	}else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	    
	return 0;// �ް�������
}




















