#ifndef __FLASH_H
#define __FLASH_H			    
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//W25X16 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/13 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved

extern u8 DACdone;
extern u8 wav_buf[2048];
extern u16 DApc;

void PCM1770_Init(void);	//PCM1770��ʼ��
void PCM_WriteData(const u8, const u8);  	    //PCM1770��������д��
void Volume_Dec(void);        //����-
void Volume_Add(void);  	//����+
void PCM1770_VolumeSet(vu8);  //PCM1770��������
void PCM1770_Mute(void);	//PCM1770����
u8 GetCurrentVolume(void);   //��ȡ��ǰ����
void Delay(u32);	//��ʱ
void IIS_PCM1770_Init(void);	//iis��ʼ��
void IIS_Config(uint32_t);	//iis����
void DMA_Transmit(u32, u32);	//DMA����

#endif
















