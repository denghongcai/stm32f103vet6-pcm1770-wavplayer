#ifndef __WAV_H
#define __WAV_H	
#include "sys.h"
#include "mmc_sd.h"
#include "fat.h"

typedef struct 
{
	u8 chRIFF[4];
	u32 wavlen;
	u8 chWAV[4];
	u8 chFMT[4];
	u16 formart;
	u16 CHnum;
	u32 SampleRate;
	u32 speed;
	u16 ajust;
	u16 SampleBits;
	u8 chDATA[4];
	u32 DATAlen;
}WAV_file;


u8 WAV_Init(u8* pbuf);
u8 Playwav(FileInfoStruct *FileName);
u8 Check_Ifo(u8* pbuf1,u8* pbuf2);
u32 Get_num(u8* pbuf,u8 len);

#endif
