#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//Mini STM32������
//�������� ��������			 
//����ԭ��@ALIENTEK
//2010/5/27 

#define KEY1 PEin(2)   
#define KEY2 PEin(3)		
#define KEY3 PEin(4)	
#define KEY4 PEin(5)	
	 
void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(void);  //����ɨ�躯��					    
#endif
