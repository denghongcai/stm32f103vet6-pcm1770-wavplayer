#include "sysfile.h"
#include "fat.h"
//Mini STM32������
//ϵͳ�ļ����Ҵ���					  
//����ԭ��@ALIENTEK
//2010/6/18
				   
u32 PICCLUSTER=0;//ͼƬ�ļ��е�ַ	 
u32 sys_ico[9];  //ϵͳͼ�껺����!���ܴ۸�!
u32 file_ico[4]; //�ļ�ͼ�껺���� folder;mus;pic;book;
 											    
//ϵͳ�ļ���		  
const unsigned char *folder[1]=
{
"WAV",	 
};		   

//ϵͳ�ļ�������

//��ȡϵͳ�ļ��Ĵ洢��ַ
//�β�����,���޷�����!!!
//����0���ɹ��������������������	   
//sel:0 ϵͳ�ļ�
//sel:1 ͼƬ�ļ���
u8 SysInfoGet(u8 sel)
{			 		   
	u32 cluster=0;
	FileInfoStruct t_file;  	  						    	 
	//�õ���Ŀ¼�Ĵغ�
	if(FAT32_Enable)cluster=FirstDirClust;
	else cluster=0;			   

	if(sel==1)//����WAV�ļ���
	{	
		t_file=F_Search(cluster,(unsigned char *)folder[0],T_FILE);//����WAV�ļ���
		if(t_file.F_StartCluster==0)return 1;//WAV�ļ��ж�ʧ
		PICCLUSTER=t_file.F_StartCluster;//WAV�ļ������ڴغ�	 
	}
	return 0;//�ɹ�
}

			 






