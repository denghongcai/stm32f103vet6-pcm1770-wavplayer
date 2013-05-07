#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"						    					    

//void MYDMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//≈‰÷√DMA1_CHx
//void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);// πƒ‹DMA1_CHx
FlagStatus DMA_GetFlagStatus(u32 DMA_FLAG);
void DMA_TxConfiguration(u32 *BufferSRC, u32 BufferSize);
void DMA_RxConfiguration(u32 *BufferDST, u32 BufferSize);
		   
#endif






























