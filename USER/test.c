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
//Mini STM32�����巶������25
//ͼƬ��ʾ ʵ��
//����ԭ��@ALIENTEK
//2010.6.18
u32 sd_Capacity;
extern u8 volume;			 					
int main(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	u8 key;
	SD_Error i;		  
	FileInfoStruct *FileInfo;	   		 
	u16 wav_cnt=0;//��ǰĿ¼��WAV�ļ��ĸ���
	u16 index=0;  //��ǰѡ����ļ����	   
	u16 time=0;    	     	  					   
	SystemInit();
	
	//�ж��������ʼ��
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
	
	
	delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��  
	KEY_Init();			//������ʼ��
	LED_Init();         //LED��ʼ��
	i=SD_InitAndConfig();
	IIS_PCM1770_Init();
	IIS_Config(I2S_AudioFreq_44k);
	while(FAT_Init())//FAT ����
	{
		printf("fat wrong");  
		
		if(i!=SD_OK)printf("SD wrong!");//SD����ʼ��ʧ�� 
			  
		delay_ms(500);
		printf("%c",FAT_Init());
		delay_ms(500);
		LED1=!LED1;	   
	}
	printf("FAT init\n");
	while(SysInfoGet(1))//�õ������ļ���  
	{
		printf("can't find file");  
		delay_ms(500);  
		FAT_Init();
		SD_Init();
		LED1=!LED1;
		//LCD_Fill(60,130,240,170,WHITE);//�����ʾ			  
		delay_ms(500);		
	}
	printf("start to play\n"); 
	delay_ms(1000);
	Cur_Dir_Cluster=PICCLUSTER;
	volume=254;//Ԥ������
	while(1)
	{	    			 
		wav_cnt=0;	 
		Get_File_Info(Cur_Dir_Cluster,FileInfo,T_WAV,&wav_cnt);//��ȡ��ǰ�ļ��������Ŀ���ļ����� 		    
		if(wav_cnt==0)//û��WAV�ļ�
		{
			//LCD_Clear(WHITE);//����	   
			while(1)
			{	  
				if(time%2==0)printf("No WAV file");		 
				else ;//LCD_Clear(WHITE);
				time++;
				delay_ms(300);
			}
		}								   
		FileInfo=&F_Info[0];//�����ݴ�ռ�.
		index=1;
		while(1)
		{
			Get_File_Info(Cur_Dir_Cluster,FileInfo,T_WAV,&index);//�õ��ļ���Ϣ	 
			//LCD_Clear(WHITE);//����,������һ��ͼƬ��ʱ��,һ������
			//POINT_COLOR=RED; 				
			printf(FileInfo->F_Name);//��ʾ�ļ�����
			printf(" ���뿪ʼ\n");
			Playwav(FileInfo);//��ʼ����			     			
			key=KEY_Scan();
			if(key==1)break;//��һ��
			else if(key==2)//��һ��
			{
				if(index>1)index-=2;
				else index=wav_cnt-1;
				break;
			}
			delay_ms(500);
			index++;
			if(index>wav_cnt)index=1;//���ŵ�һ��	  	 		 
		}
	}			   		 			  
}		 





