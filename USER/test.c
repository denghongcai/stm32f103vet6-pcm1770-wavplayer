#include "stm32f10x.h"
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"  
#include "mmc_sd.h"
#include "fat.h"
#include "sysfile.h"
#include "wavplay.h"
//Mini STM32开发板范例代码25
//图片显示 实验
//正点原子@ALIENTEK
//2010.6.18
u32 sd_Capacity;
extern u8 volume;			 					
int main(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	u8 key;
	SD_Error i;		  
	FileInfoStruct *FileInfo;	   		 
	u16 wav_cnt=0;//当前目录下WAV文件的个数
	u16 index=0;  //当前选择的文件编号	   
	u16 time=0;    	     	  					   
	SystemInit();
	
	//中断向量表初始化
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
	
	
	delay_init(72);		//延时初始化
	uart_init(72,9600); //串口1初始化  
	KEY_Init();			//按键初始化
	LED_Init();         //LED初始化
	i=SD_InitAndConfig();
	IIS_PCM1770_Init();
	IIS_Config(I2S_AudioFreq_44k);
	while(FAT_Init())//FAT 错误
	{
		printf("fat wrong");  
		
		if(i!=SD_OK)printf("SD wrong!");//SD卡初始化失败 
			  
		delay_ms(500);
		printf("%c",FAT_Init());
		delay_ms(500);
		LED1=!LED1;	   
	}
	printf("FAT init\n");
	while(SysInfoGet(1))//得到音乐文件夹  
	{
		printf("can't find file");  
		delay_ms(500);  
		FAT_Init();
		SD_Init();
		LED1=!LED1;
		//LCD_Fill(60,130,240,170,WHITE);//清除显示			  
		delay_ms(500);		
	}
	printf("start to play\n"); 
	delay_ms(1000);
	Cur_Dir_Cluster=PICCLUSTER;
	volume=254;//预置音量
	while(1)
	{	    			 
		wav_cnt=0;	 
		Get_File_Info(Cur_Dir_Cluster,FileInfo,T_WAV,&wav_cnt);//获取当前文件夹下面的目标文件个数 		    
		if(wav_cnt==0)//没有WAV文件
		{
			//LCD_Clear(WHITE);//清屏	   
			while(1)
			{	  
				if(time%2==0)printf("No WAV file");		 
				else ;//LCD_Clear(WHITE);
				time++;
				delay_ms(300);
			}
		}								   
		FileInfo=&F_Info[0];//开辟暂存空间.
		index=1;
		while(1)
		{
			Get_File_Info(Cur_Dir_Cluster,FileInfo,T_WAV,&index);//得到文件信息	 
			//LCD_Clear(WHITE);//清屏,加载下一幅图片的时候,一定清屏
			//POINT_COLOR=RED; 				
			printf(FileInfo->F_Name);//显示文件名字
			printf(" 解码开始\n");
			Playwav(FileInfo);//开始播放			     			
			key=KEY_Scan();
			if(key==1)break;//下一首
			else if(key==2)//上一首
			{
				if(index>1)index-=2;
				else index=wav_cnt-1;
				break;
			}
			delay_ms(500);
			index++;
			if(index>wav_cnt)index=1;//播放第一首	  	 		 
		}
	}			   		 			  
}		 





