#include "wavplay.h"
#include "delay.h"
#include "key.h"
#include "pcm1770.h"

WAV_file wav1;//wav文件
u8 wav_buf[2048];
u16 DApc;
u8 CHanalnum;
u8 Bitnum;
u8 DACdone;
extern u8 volume = 0x19;
FileInfoStruct *CurFile;//当前解码/操作的文件	 
u8 WAV_Init(u8* pbuf)//初始化并显示文件信息
{
	if(Check_Ifo(pbuf,"RIFF"))return 1;//RIFF标志错误
	wav1.wavlen=Get_num(pbuf+4,4);//文件长度，数据偏移4byte
	if(Check_Ifo(pbuf+8,"WAVE"))return 2;//WAVE标志错误
	if(Check_Ifo(pbuf+12,"fmt "))return 3;//fmt标志错误
	wav1.formart=Get_num(pbuf+20,2);//格式类别
	wav1.CHnum=Get_num(pbuf+22,2);//通道数
	CHanalnum=wav1.CHnum;
	wav1.SampleRate=Get_num(pbuf+24,4);//采样率
	wav1.speed=Get_num(pbuf+28,4);//音频传送速率
	wav1.ajust=Get_num(pbuf+32,2);//数据块调速数
	wav1.SampleBits=Get_num(pbuf+34,2);//样本数据位数
	Bitnum=wav1.SampleBits;
	if(Check_Ifo(pbuf+36,"data"))return 4;//data标志错误
	wav1.DATAlen=Get_num(pbuf+40,4);//数据长度	
	///////////////////////////////////////////////
	if(wav1.wavlen<0x100000)
	{
		printf("%d",(wav1.wavlen)>>10);//文件长度
		printf("Kb\n");
	}
	else
	{
		printf("%d",(wav1.wavlen)>>20);//文件长度
		printf("Mb\n");
	}
	if(wav1.formart==1)printf("WAV PCM\n");
	if(wav1.CHnum==1)printf("single\n");
	else printf("stereo\n");
	printf("%d",(wav1.SampleRate)/1000);//采样率
	printf("KHz\n");
	printf("%d",(wav1.speed)/1000);//传送速度
	printf("bps\n");
	printf("%d",wav1.SampleBits);//样本数据位数
	printf("bit\n");
	return 0;
}

u8 Playwav(FileInfoStruct *FileName)
{
	int pcm1770_init_need = 1;
	u16 i,times;
	CurFile=FileName;
	if(CurFile->F_Type!=T_WAV)return 1;
	F_Open(CurFile);
	F_Read(CurFile,wav_buf);//先读512字节到
	F_Read(CurFile,wav_buf+512);//再读512字节
	while(WAV_Init(wav_buf))printf("format illegal!\n");
	//根据采样率（wav1.SampleRate）I2S采样频率，进行DA转换
	IIS_Config(wav1.SampleRate);
	DACdone=0;
	DApc = 0;
	F_Read(CurFile,wav_buf+1024);
	F_Read(CurFile,wav_buf+1536);
	DMA_Transmit((uint32_t)(wav_buf), 1024);//启动DMA转换
	times=(wav1.DATAlen>>12-1);
	for(i=0;i<times;i++)//循环一次转换1KB数据
	{	
		if(pcm1770_init_need ==1 && i == 2)
		{
			//PCM_WriteData(0x01, volume);
			//PCM_WriteData(0x02, volume);
			pcm1770_init_need = 0;
		}
		while(!DApc);//等待DMA转换完成一半
		DApc = 0;
		F_Read(CurFile,wav_buf);//先读512字节
		F_Read(CurFile,wav_buf+512);//再读512字节
		while(!DACdone);//等待DMA转换完成
		DACdone=0;
		F_Read(CurFile,wav_buf+1024);
		F_Read(CurFile,wav_buf+1536);
		
		if((KEY1&KEY2&KEY3&KEY4)==0)
		{			
			if(KEY1==0||KEY2==0)
			{
				DMA_Cmd(DMA1_Channel5, DISABLE);
				break;
			}//关DMA
			if(KEY3==0)
				Volume_Dec();
			if(KEY4==0)
				Volume_Add();
		}
		
	}
	DMA_Cmd(DMA1_Channel5, DISABLE);
	return 0;
}

u8 Check_Ifo(u8* pbuf1,u8* pbuf2)
{
	u8 i;
	for(i=0;i<4;i++)if(pbuf1[i]!=pbuf2[i])return 1;//不同
	return 0;//相同
}

u32 Get_num(u8* pbuf,u8 len)
{
    u32 num;
	if(len==2)num=(pbuf[1]<<8)|pbuf[0];
	else if(len==4)num=(pbuf[3]<<24)|(pbuf[2]<<16)|(pbuf[1]<<8)|pbuf[0];
	return num;
}




