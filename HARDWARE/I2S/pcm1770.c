#include "pcm1770.h" 
#include "delay.h" 
#include "usart.h"  
//Mini STM32开发板

//正点原子@ALIENTEK
//2010/6/13
//V1.0
#define PCM_PD (1 << 14)//PB14 PD 口接到主ic reset 脚
#define PCM_PD_SET_L GPIOB->ODR&=~(PCM_PD) //GPIOB->ODR = (GPIOB->ODR & ~PCM_PD) | (x ? PCM_PD : 0);
#define PCM_PD_SET_H GPIOB->ODR|=(PCM_PD)
#define PCM_CS (1 << 1)//PB1
#define PCM_CS_SET_L GPIOB->ODR&=~(PCM_CS) //GPIOB->ODR = (GPIOB->ODR & ~PCM_CS) | (x ? PCM_CS : 0);
#define PCM_CS_SET_H GPIOB->ODR|=(PCM_CS)
#define PCM_CLK (1 << 1)//PA1
#define PCM_CLK_SET_L GPIOA->ODR&=~(PCM_CLK)// GPIOB->ODR = (GPIOB->ODR & ~PCM_CLK) | (x ? PCM_CLK : 0);
#define PCM_CLK_SET_H GPIOA->ODR|=(PCM_CLK)
#define PCM_DAT (1 << 2)//PA2
#define PCM_DAT_SET_L GPIOA->ODR&=~(PCM_DAT) //GPIOB->ODR = (GPIOB->ODR & ~PCM_DAT) | (x ? PCM_DAT : 0);
#define PCM_DAT_SET_H GPIOA->ODR|=(PCM_DAT)
#define C_VOLUME_MAX 0x3F

#define OUTPUT_BUFFER_SIZE 1024

u16 CurrDataCounterEnd;

static vu8 pcm1770_init_need=1;//判断是否初始化pcm1770

static vu8 s_Volume;//音量大小

vu16 * /*volatile*/ P_AudioBuff;
																											                                                                                                                                                                                                                                                                                                              
void PCM1770_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//硬件复位
	Delay(50); //重要
	PCM_PD_SET_L;
	Delay(1000);
	PCM_PD_SET_H;
	Delay(100);	
	PCM_CS_SET_H ;
	PCM_CLK_SET_H;
	PCM_DAT_SET_H;
	s_Volume = C_VOLUME_MAX-2;//默认一半音量
	PCM_WriteData(0x01, s_Volume); //右耳机音量设为中间大小
	PCM_WriteData(0x02, s_Volume); //右耳机音量设为中间大小
	PCM_WriteData(0x03, 0x94); //256fs IIS 格式 stm32 的iis mclk 规定为256fs ( 84= 16 right) analog input
	PCM_WriteData(0x04, 0x00); //默认值
}  

									
void PCM_WriteData(const u8 Reg, const u8 Data)
{
	vu16 TrasferData, i;
	TrasferData = Data;
	TrasferData |= (Reg<<8)&0xff00;
	PCM_CS_SET_L;//select
	Delay(5);
	for (i = 0; i < 16; i++)
	{//传输时MSB first
		PCM_CLK_SET_L;
		if (TrasferData&(0x8000>>i))
		{
			PCM_DAT_SET_H;
		}
		else
		{
			PCM_DAT_SET_L;
		}
		Delay(5);//等数据稳定
		PCM_CLK_SET_H; //上升沿写入
		Delay(5);//等待从机读数据	 
	}
	PCM_CLK_SET_H;
	PCM_DAT_SET_H;
	PCM_CS_SET_H;//relase
	Delay(5);
}

void Volume_Dec(void)
{
	if (s_Volume > 0)
	{
		s_Volume--;
		PCM_WriteData(0x01, s_Volume);//左右耳机静音，左耳机音量设为中间大小
		PCM_WriteData(0x02, s_Volume);//右耳机音量设为中间大小
	}
}

void Volume_Add(void)
{
	if (s_Volume < C_VOLUME_MAX)
	{
		s_Volume++;
		PCM_WriteData(0x01, s_Volume);//左右耳机静音，左耳机音量设为中间大小
		PCM_WriteData(0x02, s_Volume);//右耳机音量设为中间大小
	}
}

void PCM1770_VolumeSet(vu8 vol)
{
	s_Volume = vol;
	PCM_WriteData(0x01, s_Volume);//左右耳机静音，左耳机音量设为中间大小
	PCM_WriteData(0x02, s_Volume);//右耳机音量设为中间大小
}

void PCM1770_Mute(void)
{
	PCM_WriteData(0x01, 0xc0);//左右耳机静音，左耳机音量设为中间大小
	PCM_WriteData(0x02, 0x00);//右耳机音量设为中间大小
}

u8 GetCurrentVolume(void)
{
	return s_Volume;
}

void Delay(u32 Num)
{
	vu32 Count = Num*4;
	while (--Count);
}

void IIS_PCM1770_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable peripheral clocks --------------------------------------------------*/\
	/* GPIOB and AFIO clocks enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	/* Configure SPI2 pins: CK, WS and SD ---------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Configure SPI2 MCK --256fs-------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

DMA_InitTypeDef DMA_InitStructure;

void IIS_Config(uint32_t freq)
{
	I2S_InitTypeDef I2S_InitStructure;
	/* 复位 SPI2 外设到缺省状态 */
	SPI_I2S_DeInit(SPI2);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //DMA1 通道5
	/* I2S peripheral configuration */
	I2S_InitStructure.I2S_Standard = I2S_Standard_LSB;//I2S_Standard_Phillips;
	I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;// I2S_DataFormat_24b;
	I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
	I2S_InitStructure.I2S_AudioFreq = freq;//I2S_AudioFreq_16k;
	I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
	/* I2S2 Master Transmitter -----------*/
	/* I2S2 configuration */
	I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
	I2S_Init(SPI2, &I2S_InitStructure);
	/* SPI_MASTER_Tx_DMA_Channel configuration ---------------------------------*/
	DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPI2_BASE+0x0c;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)P_AudioBuff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = OUTPUT_BUFFER_SIZE;//BufferSize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	/* Enable SPI_MASTER DMA Tx request */
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
	I2S_Cmd(SPI2, ENABLE);
	DMA_ITConfig(DMA1_Channel5, DMA_IT_HT|DMA_IT_TC, ENABLE);
	//DMA_Cmd(DMA1_Channel5, ENABLE);

	pcm1770_init_need=1;
}

void DMA_Transmit(u32 addr, u32 size)
{
	DMA1_Channel5->CCR &= (uint16_t)(~DMA_CCR1_EN);//DMA_Cmd(DMA1_Channel5, DISABLE);
	DMA_InitStructure.DMA_MemoryBaseAddr = addr;
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel5, ENABLE);
	/*
	if (pcm1770_init_need!=0)
	{
		pcm1770_init_need=0; //pcm1770 需要在iis接口有信号至少1ms后进行配置 配置信息才有用
		PCM1770_Init();
		printf("Init success!");
	}
	*/
}

void DMA1_Channel5_IRQHandler()
{
  if(DMA_GetITStatus(DMA1_IT_TC5))
  {
			CurrDataCounterEnd=DMA_GetCurrDataCounter(DMA1_Channel5);
			DMA_ClearITPendingBit(DMA1_IT_TC5);
			DACdone = 1;
			//DMA_Transmit((uint32_t)(wav_buf), 512);
			
  }
	if(DMA_GetITStatus(DMA1_IT_HT5))
	{
			DMA_ClearITPendingBit(DMA1_IT_HT5);
		  DApc = 1;
	}
	
  if (pcm1770_init_need!=0)
	{
		pcm1770_init_need=0;	 //pcm1770 需要在iis接口有信号至少1ms后进行配置 配置信息才有用
		PCM1770_Init();
		printf("PCM 1770 Init success!\n");
	}
	
}

