#include "stm32f10x.h"	   
#include "led.h"
		    
	 
//LED IO��ʼ��
void LED_Init(void)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PA3ʱ��
  	 		   	 
	GPIOA->CRL&=0XFFFF0FFF; 
	GPIOA->CRL|=0X00003000;	 
    GPIOA->ODR|=1<<3;      //PA3�����											  
}






