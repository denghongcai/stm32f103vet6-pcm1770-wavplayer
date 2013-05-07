#include "dma.h"	

//u16 DMA1_MEM_LEN;//保存DMA每次数据传送的长度 		    
////DMA1的各通道配置
////这里的传输形式是固定的,这点要根据不同的情况来修改
////从存储器->外设模式/8位数据宽度/存储器增量模式
////DMA_CHx:DMA通道CHx
////cpar:外设地址
////cmar:存储器地址
////cndtr:数据传输量
//void MYDMA_Config(DMA_Channel_TypeDef *DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
//{
//	u32 DR_Base;  //做缓冲用,不知道为什么.非要不可
//	RCC->AHBENR|=1<<0;//开启DMA1时钟
//	DR_Base=cpar;
//	DMA_CHx->CPAR=DR_Base; 	 //DMA1 外设地址 
//	DMA_CHx->CMAR=(u32)cmar; //DMA1,存储器地址
//	DMA1_MEM_LEN=cndtr;      //保存DMA传输数据量
//	DMA_CHx->CNDTR=cndtr;    //DMA1,传输数据量
//	DMA_CHx->CCR=0X00000000;//复位
//	DMA_CHx->CCR|=1<<4;  //从存储器读
//	DMA_CHx->CCR|=0<<5;  //普通模式
//	DMA_CHx->CCR|=0<<6;  //外设地址非增量模式
//	DMA_CHx->CCR|=1<<7;  //存储器增量模式
//	DMA_CHx->CCR|=0<<8;  //外设数据宽度为8位
//	DMA_CHx->CCR|=0<<10; //存储器数据宽度8位
//	DMA_CHx->CCR|=1<<12; //中等优先级
//	DMA_CHx->CCR|=0<<14; //非存储器到存储器模式		  	
//}
//
////开启一次DMA传输
//void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
//{
//	DMA_CHx->CCR&=~(1<<0);       //关闭DMA传输 
//	DMA_CHx->CNDTR=DMA1_MEM_LEN; //DMA1,传输数据量 
//	DMA_CHx->CCR|=1<<0;          //开启DMA传输
//}	  

/*
FlagStatus DMA_GetFlagStatusReg(u32 DMA_FLAG)
{
  	FlagStatus bitstatus=RESET;	  
 	//获取DMA2中断寄存器的值,检查指定DMA的状态标志 
  	if(((DMA2->ISR)&DMA_FLAG)!=(u32)RESET)bitstatus=SET;//标志位置位
  	else bitstatus=RESET;//标志位清零
  	return  bitstatus;//返回DMA状态标志
}
*/

void DMA_TxConfiguration(u32 *BufferSRC, u32 BufferSize)
{
	u32 tmpreg=0;
    DMA2->IFCR=(DMA2_FLAG_TC4|DMA2_FLAG_TE4|DMA2_FLAG_HT4|DMA2_FLAG_GL4);
    DMA2_Channel4->CCR&=DISABLE;//DMA2 Channel4取消使能
	tmpreg=DMA2_Channel4->CCR;//配置DMA2 Channel4
	tmpreg&=0xFFFF800F;
  	tmpreg|=DMA_DIR_PeripheralDST|DMA_Mode_Normal|
            DMA_PeripheralInc_Disable|DMA_MemoryInc_Enable|
            DMA_PeripheralDataSize_Word|DMA_MemoryDataSize_Word|
            DMA_Priority_High|DMA_M2M_Disable;
  	DMA2_Channel4->CCR=tmpreg;
  	DMA2_Channel4->CNDTR=BufferSize/4;
  	DMA2_Channel4->CPAR=0x40018080;
  	DMA2_Channel4->CMAR=(u32)BufferSRC;
    DMA2_Channel4->CCR|=ENABLE;//DMA2 Channel4使能
}

void DMA_RxConfiguration(u32 *BufferDST, u32 BufferSize)
{
	u32 tmpreg=0;
    DMA2->IFCR=DMA2_FLAG_TC4|DMA2_FLAG_TE4|DMA2_FLAG_HT4|DMA2_FLAG_GL4;
    DMA2_Channel4->CCR&=DISABLE;//DMA2 Channel4取消使能
	tmpreg=DMA2_Channel4->CCR;//配置DMA2 Channel4
	tmpreg&=0xFFFF800F;
  	tmpreg|=DMA_DIR_PeripheralSRC|DMA_Mode_Normal|
            DMA_PeripheralInc_Disable|DMA_MemoryInc_Enable|
            DMA_PeripheralDataSize_Word|DMA_MemoryDataSize_Word|
            DMA_Priority_High|DMA_M2M_Disable;
  	DMA2_Channel4->CCR=tmpreg;
  	DMA2_Channel4->CNDTR=BufferSize/4;
  	DMA2_Channel4->CPAR=0x40018080;
  	DMA2_Channel4->CMAR=(u32)BufferDST;	  	
    DMA2_Channel4->CCR|=ENABLE;//DMA2 Channel4使能
}























