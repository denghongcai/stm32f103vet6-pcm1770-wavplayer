#include "wavplay.h"
#include "delay.h"
#include "key.h"
#include "pcm1770.h"

WAV_file wav1;//wav�ļ�
u8 wav_buf[2048];
u16 DApc;
u8 CHanalnum;
u8 Bitnum;
u8 DACdone;
extern u8 volume = 0x19;
FileInfoStruct *CurFile;//��ǰ����/�������ļ�	 
u8 WAV_Init(u8* pbuf)//��ʼ������ʾ�ļ���Ϣ
{
	if(Check_Ifo(pbuf,"RIFF"))return 1;//RIFF��־����
	wav1.wavlen=Get_num(pbuf+4,4);//�ļ����ȣ�����ƫ��4byte
	if(Check_Ifo(pbuf+8,"WAVE"))return 2;//WAVE��־����
	if(Check_Ifo(pbuf+12,"fmt "))return 3;//fmt��־����
	wav1.formart=Get_num(pbuf+20,2);//��ʽ���
	wav1.CHnum=Get_num(pbuf+22,2);//ͨ����
	CHanalnum=wav1.CHnum;
	wav1.SampleRate=Get_num(pbuf+24,4);//������
	wav1.speed=Get_num(pbuf+28,4);//��Ƶ��������
	wav1.ajust=Get_num(pbuf+32,2);//���ݿ������
	wav1.SampleBits=Get_num(pbuf+34,2);//��������λ��
	Bitnum=wav1.SampleBits;
	if(Check_Ifo(pbuf+36,"data"))return 4;//data��־����
	wav1.DATAlen=Get_num(pbuf+40,4);//���ݳ���	
	///////////////////////////////////////////////
	if(wav1.wavlen<0x100000)
	{
		printf("%d",(wav1.wavlen)>>10);//�ļ�����
		printf("Kb\n");
	}
	else
	{
		printf("%d",(wav1.wavlen)>>20);//�ļ�����
		printf("Mb\n");
	}
	if(wav1.formart==1)printf("WAV PCM\n");
	if(wav1.CHnum==1)printf("single\n");
	else printf("stereo\n");
	printf("%d",(wav1.SampleRate)/1000);//������
	printf("KHz\n");
	printf("%d",(wav1.speed)/1000);//�����ٶ�
	printf("bps\n");
	printf("%d",wav1.SampleBits);//��������λ��
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
	F_Read(CurFile,wav_buf);//�ȶ�512�ֽڵ�
	F_Read(CurFile,wav_buf+512);//�ٶ�512�ֽ�
	while(WAV_Init(wav_buf))printf("format illegal!\n");
	//���ݲ����ʣ�wav1.SampleRate��I2S����Ƶ�ʣ�����DAת��
	IIS_Config(wav1.SampleRate);
	DACdone=0;
	DApc = 0;
	F_Read(CurFile,wav_buf+1024);
	F_Read(CurFile,wav_buf+1536);
	DMA_Transmit((uint32_t)(wav_buf), 1024);//����DMAת��
	times=(wav1.DATAlen>>12-1);
	for(i=0;i<times;i++)//ѭ��һ��ת��1KB����
	{	
		if(pcm1770_init_need ==1 && i == 2)
		{
			//PCM_WriteData(0x01, volume);
			//PCM_WriteData(0x02, volume);
			pcm1770_init_need = 0;
		}
		while(!DApc);//�ȴ�DMAת�����һ��
		DApc = 0;
		F_Read(CurFile,wav_buf);//�ȶ�512�ֽ�
		F_Read(CurFile,wav_buf+512);//�ٶ�512�ֽ�
		while(!DACdone);//�ȴ�DMAת�����
		DACdone=0;
		F_Read(CurFile,wav_buf+1024);
		F_Read(CurFile,wav_buf+1536);
		
		if((KEY1&KEY2&KEY3&KEY4)==0)
		{			
			if(KEY1==0||KEY2==0)
			{
				DMA_Cmd(DMA1_Channel5, DISABLE);
				break;
			}//��DMA
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
	for(i=0;i<4;i++)if(pbuf1[i]!=pbuf2[i])return 1;//��ͬ
	return 0;//��ͬ
}

u32 Get_num(u8* pbuf,u8 len)
{
    u32 num;
	if(len==2)num=(pbuf[1]<<8)|pbuf[0];
	else if(len==4)num=(pbuf[3]<<24)|(pbuf[2]<<16)|(pbuf[1]<<8)|pbuf[0];
	return num;
}




