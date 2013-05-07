#ifndef __FLASH_H
#define __FLASH_H			    
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//W25X16 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/13 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved

extern u8 DACdone;
extern u8 wav_buf[2048];
extern u16 DApc;

void PCM1770_Init(void);	//PCM1770初始化
void PCM_WriteData(const u8, const u8);  	    //PCM1770控制数据写入
void Volume_Dec(void);        //音量-
void Volume_Add(void);  	//音量+
void PCM1770_VolumeSet(vu8);  //PCM1770设置音量
void PCM1770_Mute(void);	//PCM1770静音
u8 GetCurrentVolume(void);   //获取当前音量
void Delay(u32);	//延时
void IIS_PCM1770_Init(void);	//iis初始化
void IIS_Config(uint32_t);	//iis设置
void DMA_Transmit(u32, u32);	//DMA传送

#endif
















