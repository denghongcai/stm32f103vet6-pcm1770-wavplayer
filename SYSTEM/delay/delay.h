#ifndef __DELAY_H
#define __DELAY_H 			   
#include "stm32f10x.h"
//Mini STM32开发板
//使用SysTick的普通计数模式对延迟进行管理
//包括delay_us,delay_ms
//正点原子@ALIENTEK
//2010/5/27
//V1.2
void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);

#endif





























