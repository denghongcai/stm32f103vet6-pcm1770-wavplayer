#include "sysfile.h"
#include "fat.h"
//Mini STM32开发板
//系统文件查找代码					  
//正点原子@ALIENTEK
//2010/6/18
				   
u32 PICCLUSTER=0;//图片文件夹地址	 
u32 sys_ico[9];  //系统图标缓存区!不能篡改!
u32 file_ico[4]; //文件图标缓存区 folder;mus;pic;book;
 											    
//系统文件夹		  
const unsigned char *folder[1]=
{
"WAV",	 
};		   

//系统文件名定义

//获取系统文件的存储地址
//次步出错,则无法启动!!!
//返回0，成功。返回其他，错误代码	   
//sel:0 系统文件
//sel:1 图片文件夹
u8 SysInfoGet(u8 sel)
{			 		   
	u32 cluster=0;
	FileInfoStruct t_file;  	  						    	 
	//得到根目录的簇号
	if(FAT32_Enable)cluster=FirstDirClust;
	else cluster=0;			   

	if(sel==1)//查找WAV文件夹
	{	
		t_file=F_Search(cluster,(unsigned char *)folder[0],T_FILE);//查找WAV文件夹
		if(t_file.F_StartCluster==0)return 1;//WAV文件夹丢失
		PICCLUSTER=t_file.F_StartCluster;//WAV文件夹所在簇号	 
	}
	return 0;//成功
}

			 






