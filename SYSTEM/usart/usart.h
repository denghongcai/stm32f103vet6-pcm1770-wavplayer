#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"
#include "stdio.h"	 
//Mini STM32开发板
//串口1初始化			   
//正点原子@ALIENTEK
//2010/5/27
	  	
extern u8 USART_RX_BUF[64];     //接收缓冲,最大63个字节.末字节为换行符 
extern u8 USART_RX_STA;         //接收状态标记	


//#define EN_USART1_RX 使能串口1接收
void uart_init(u32 pclk2,u32 bound);

#endif	   
















