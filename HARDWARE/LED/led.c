#include "stm32f10x.h"	   
#include "led.h"
		    
	 
//LED IO初始化
void LED_Init(void)
{
	RCC->APB2ENR|=1<<2;    //使能PA3时钟
  	 		   	 
	GPIOA->CRL&=0XFFFF0FFF; 
	GPIOA->CRL|=0X00003000;	 
    GPIOA->ODR|=1<<3;      //PA3输出高											  
}






