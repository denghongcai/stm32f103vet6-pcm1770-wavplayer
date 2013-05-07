#include "dma.h"	

//u16 DMA1_MEM_LEN;//����DMAÿ�����ݴ��͵ĳ��� 		    
////DMA1�ĸ�ͨ������
////����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
////�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
////DMA_CHx:DMAͨ��CHx
////cpar:�����ַ
////cmar:�洢����ַ
////cndtr:���ݴ�����
//void MYDMA_Config(DMA_Channel_TypeDef *DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
//{
//	u32 DR_Base;  //��������,��֪��Ϊʲô.��Ҫ����
//	RCC->AHBENR|=1<<0;//����DMA1ʱ��
//	DR_Base=cpar;
//	DMA_CHx->CPAR=DR_Base; 	 //DMA1 �����ַ 
//	DMA_CHx->CMAR=(u32)cmar; //DMA1,�洢����ַ
//	DMA1_MEM_LEN=cndtr;      //����DMA����������
//	DMA_CHx->CNDTR=cndtr;    //DMA1,����������
//	DMA_CHx->CCR=0X00000000;//��λ
//	DMA_CHx->CCR|=1<<4;  //�Ӵ洢����
//	DMA_CHx->CCR|=0<<5;  //��ͨģʽ
//	DMA_CHx->CCR|=0<<6;  //�����ַ������ģʽ
//	DMA_CHx->CCR|=1<<7;  //�洢������ģʽ
//	DMA_CHx->CCR|=0<<8;  //�������ݿ��Ϊ8λ
//	DMA_CHx->CCR|=0<<10; //�洢�����ݿ��8λ
//	DMA_CHx->CCR|=1<<12; //�е����ȼ�
//	DMA_CHx->CCR|=0<<14; //�Ǵ洢�����洢��ģʽ		  	
//}
//
////����һ��DMA����
//void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
//{
//	DMA_CHx->CCR&=~(1<<0);       //�ر�DMA���� 
//	DMA_CHx->CNDTR=DMA1_MEM_LEN; //DMA1,���������� 
//	DMA_CHx->CCR|=1<<0;          //����DMA����
//}	  

/*
FlagStatus DMA_GetFlagStatusReg(u32 DMA_FLAG)
{
  	FlagStatus bitstatus=RESET;	  
 	//��ȡDMA2�жϼĴ�����ֵ,���ָ��DMA��״̬��־ 
  	if(((DMA2->ISR)&DMA_FLAG)!=(u32)RESET)bitstatus=SET;//��־λ��λ
  	else bitstatus=RESET;//��־λ����
  	return  bitstatus;//����DMA״̬��־
}
*/

void DMA_TxConfiguration(u32 *BufferSRC, u32 BufferSize)
{
	u32 tmpreg=0;
    DMA2->IFCR=(DMA2_FLAG_TC4|DMA2_FLAG_TE4|DMA2_FLAG_HT4|DMA2_FLAG_GL4);
    DMA2_Channel4->CCR&=DISABLE;//DMA2 Channel4ȡ��ʹ��
	tmpreg=DMA2_Channel4->CCR;//����DMA2 Channel4
	tmpreg&=0xFFFF800F;
  	tmpreg|=DMA_DIR_PeripheralDST|DMA_Mode_Normal|
            DMA_PeripheralInc_Disable|DMA_MemoryInc_Enable|
            DMA_PeripheralDataSize_Word|DMA_MemoryDataSize_Word|
            DMA_Priority_High|DMA_M2M_Disable;
  	DMA2_Channel4->CCR=tmpreg;
  	DMA2_Channel4->CNDTR=BufferSize/4;
  	DMA2_Channel4->CPAR=0x40018080;
  	DMA2_Channel4->CMAR=(u32)BufferSRC;
    DMA2_Channel4->CCR|=ENABLE;//DMA2 Channel4ʹ��
}

void DMA_RxConfiguration(u32 *BufferDST, u32 BufferSize)
{
	u32 tmpreg=0;
    DMA2->IFCR=DMA2_FLAG_TC4|DMA2_FLAG_TE4|DMA2_FLAG_HT4|DMA2_FLAG_GL4;
    DMA2_Channel4->CCR&=DISABLE;//DMA2 Channel4ȡ��ʹ��
	tmpreg=DMA2_Channel4->CCR;//����DMA2 Channel4
	tmpreg&=0xFFFF800F;
  	tmpreg|=DMA_DIR_PeripheralSRC|DMA_Mode_Normal|
            DMA_PeripheralInc_Disable|DMA_MemoryInc_Enable|
            DMA_PeripheralDataSize_Word|DMA_MemoryDataSize_Word|
            DMA_Priority_High|DMA_M2M_Disable;
  	DMA2_Channel4->CCR=tmpreg;
  	DMA2_Channel4->CNDTR=BufferSize/4;
  	DMA2_Channel4->CPAR=0x40018080;
  	DMA2_Channel4->CMAR=(u32)BufferDST;	  	
    DMA2_Channel4->CCR|=ENABLE;//DMA2 Channel4ʹ��
}























