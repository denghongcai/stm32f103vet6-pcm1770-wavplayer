#include "mmc_sd.h"
#include "dma.h" 

extern u32 sd_Capacity;
static u32 CardType= SDIO_STD_CAPACITY_SD_CARD_V1_1;
static u32 CSD_Tab[4], CID_Tab[4], RCA = 0;
volatile SD_Error TransferError = SD_OK;
vu32 TransferEnd = 0;
/*******************************************************************************
* Function Name  : SD_Init
*******************************************************************************/
SD_Error SD_Init(void)
{
  	SD_Error errorstatus=SD_OK;

  	RCC->APB2ENR|=1<<4;    //使能PORTC时钟
  	RCC->APB2ENR|=1<<5;    //使能PORTD时钟
  	RCC->AHBENR|=1<<10;    //使能SDIO时钟
  	RCC->AHBENR|=1<<1;    //使能DMA2时钟
  	// 设置PC8~PC11为D0~D3, PC12为CLK,PD2为控制线
  	GPIOC->CRH&=0XFFF00000;
	GPIOC->CRH|=0X000BBBBB;//PC8~PC12复用推挽输出
	GPIOC->ODR|=0X1F<<8;      //输出高
 	GPIOD->CRL&=0XFFFFF0FF;
	GPIOD->CRL|=0X00000B00;//PD2推挽输出
	GPIOD->ODR|=1<<2;      //PD2输出高
	//SDIO_DeInit
	SDIO->POWER=0x00000000;
	SDIO->CLKCR=0x00000000;
	SDIO->ARG=0x00000000;
	SDIO->CMD=0x00000000;
	SDIO->DTIMER=0x00000000;
	SDIO->DLEN=0x00000000;
	SDIO->DCTRL=0x00000000;
	SDIO->ICR=0x00C007FF;
	SDIO->MASK=0x00000000;

  	errorstatus=SD_PowerON();
  	if (errorstatus!=SD_OK)
  	{
    	return(errorstatus);//响应超时(等待CMDSENT标志)
  	}
  	errorstatus=SD_InitializeCards();
  	if (errorstatus!=SD_OK)
  	{
    	return(errorstatus);//响应超时(等待CMDSENT标志)
  	}
  	return(errorstatus);
}
/*******************************************************************************
* Function Name  : SD_PowerON
*******************************************************************************/
SD_Error SD_PowerON(void)
{
  	u32 response=0, count = 0;
  	int validvoltage = 0;
  	u32 SDType = SD_STD_CAPACITY;
	SDIO_INIT(SDIO_INIT_CLK_DIV,SDIO_BusWide_1b);//配置SDIO外设,HCLK=72MHz,SDIOCLK=72MHz,SDIO_CK=HCLK/(178+2)=400KHz 	
  	SDIO_SetPowerState(SDIO_PowerState_ON);//开电源
  	SDIO_ClockCmd(ENABLE);//使能SDIO时钟
  	SDIO_SendCMD(0x00,SDIO_GO_IDLE_STATE,SDIO_Response_No);//CMD0: GO_IDLE_STATE
  	if(CmdError()!=SD_OK)return(CmdError());//等待超时
  	SDIO_SendCMD(SD_CHECK_PATTERN,SDIO_SEND_IF_COND,SDIO_Response_Short);//CMD8: SEND_IF_COND检查SD卡操作环境,返回R7
  	if(CmdResp7Error()==SD_OK)
  	{
    	CardType=SDIO_STD_CAPACITY_SD_CARD_V2_0; //SD Card 2.0
    	SDType=SD_HIGH_CAPACITY;
  	}
  	SDIO_SendCMD(0x00,SDIO_APP_CMD,SDIO_Response_Short);//CMD55
  	if(CmdResp1Error(SDIO_APP_CMD)==SD_OK)//若等待超时则是MMC卡,否则SD card 2.0或1.X
  	{
    	while ((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))//是SD卡,发送CMD41 SD_APP_OP_COND
    	{
	  		SDIO_SendCMD(0x00,SDIO_APP_CMD,SDIO_Response_Short);//CMD55
      		if(CmdResp1Error(SDIO_APP_CMD)!=SD_OK)return(CmdResp1Error(SDIO_APP_CMD));
	  		SDIO_SendCMD(SD_VOLTAGE_WINDOW_SD|SDType,SDIO_SD_APP_OP_COND,SDIO_Response_Short);
      		if(CmdResp3Error()!=SD_OK)return(CmdResp3Error());
      		response=SDIO_GetResponse(SDIO_RESP1);
      		validvoltage=(int)(((response>>31)==1)?1:0);
      		count++;
    	}
    	if(count>=SD_MAX_VOLT_TRIAL)return(SD_INVALID_VOLTRANGE);
    	if(response&=SD_HIGH_CAPACITY)CardType=SDIO_HIGH_CAPACITY_SD_CARD;
  	}//否则是MMC Card
  	return(SD_OK);
}
/*******************************************************************************
* Function Name  : SD_PowerOFF
*******************************************************************************/
SD_Error SD_PowerOFF(void)
{
  	SDIO_SetPowerState(SDIO_PowerState_OFF);
  	return(SD_OK);
}
/*******************************************************************************
* Function Name  : SD_InitializeCards
*******************************************************************************/
SD_Error SD_InitializeCards(void)
{
  	u16 rca = 0x01;
  	if((SDIO->POWER&(~0xFFFFFFFC))==SDIO_PowerState_OFF)return(SD_REQUEST_NOT_APPLICABLE);

  	if(CardType!=SDIO_SECURE_DIGITAL_IO_CARD)
  	{
		SDIO_SendCMD(0x0,SDIO_ALL_SEND_CID,SDIO_Response_Long);//发送CMD2:ALL_SEND_CID
    	if(CmdResp2Error()!=SD_OK)return(CmdResp2Error());
	    CID_Tab[0]=SDIO_GetResponse(SDIO_RESP1);
	    CID_Tab[1]=SDIO_GetResponse(SDIO_RESP2);
	    CID_Tab[2]=SDIO_GetResponse(SDIO_RESP3);
	    CID_Tab[3]=SDIO_GetResponse(SDIO_RESP4);
  	}
  	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)||(CardType==SDIO_SECURE_DIGITAL_IO_COMBO_CARD)||(CardType==SDIO_HIGH_CAPACITY_SD_CARD))
  	{
		SDIO_SendCMD(0x00,SDIO_SET_REL_ADDR,SDIO_Response_Short);//发送CMD3:SET_REL_ADDR
    	if(CmdResp6Error(SDIO_SET_REL_ADDR,&rca)!=SD_OK)return(CmdResp6Error(SDIO_SET_REL_ADDR,&rca));//得到RCA值
  	}
  	if(CardType!=SDIO_SECURE_DIGITAL_IO_CARD)
  	{
    	RCA=rca;
		SDIO_SendCMD(rca<<16,SDIO_SEND_CSD,SDIO_Response_Long);//发送CMD9:SEND_CSD
    	if (CmdResp2Error()!=SD_OK)return(CmdResp2Error());
	    CSD_Tab[0]=SDIO_GetResponse(SDIO_RESP1);
	    CSD_Tab[1]=SDIO_GetResponse(SDIO_RESP2);
	    CSD_Tab[2]=SDIO_GetResponse(SDIO_RESP3);
	    CSD_Tab[3]=SDIO_GetResponse(SDIO_RESP4);
  	}
  	return(SD_OK);
}
/*******************************************************************************
* Function Name  : SD_GetCardInfo
*******************************************************************************/
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
	u8 tmp=0;
	cardinfo->CardType=(u8)CardType;
	cardinfo->RCA=(u16)RCA;	
	tmp=(u8)((CSD_Tab[0]&0xFF000000)>>24);//Byte0
	cardinfo->SD_csd.CSDStruct=(tmp&0xC0)>>6;
	cardinfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;
	cardinfo->SD_csd.Reserved1=tmp&0x03;		
	tmp=(u8)((CSD_Tab[0]&0x00FF0000)>>16);//Byte1
	cardinfo->SD_csd.TAAC=tmp;	
	tmp=(u8)((CSD_Tab[0]&0x0000FF00)>>8);//Byte2
	cardinfo->SD_csd.NSAC=tmp;	
	tmp=(u8)(CSD_Tab[0]&0x000000FF);//Byte3
	cardinfo->SD_csd.MaxBusClkFrec=tmp;	
	tmp=(u8)((CSD_Tab[1]&0xFF000000)>>24);//Byte4
	cardinfo->SD_csd.CardComdClasses=tmp<<4;	
	tmp=(u8)((CSD_Tab[1]&0x00FF0000)>>16);//Byte5
	cardinfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;
	cardinfo->SD_csd.RdBlockLen=tmp&0x0F;	
	tmp=(u8)((CSD_Tab[1]&0x0000FF00)>>8);//Byte6
	cardinfo->SD_csd.PartBlockRead=(tmp&0x80)>>7;
	cardinfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6;
	cardinfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5;
	cardinfo->SD_csd.DSRImpl=(tmp&0x10)>>4;	
	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0))
	{
		cardinfo->SD_csd.DeviceSize=(tmp&0x03)<<10;		
		tmp=(u8)(CSD_Tab[1]&0x000000FF);//Byte7
		cardinfo->SD_csd.DeviceSize|=(tmp)<<2;		
		tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24);//Byte8
		cardinfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;		
		cardinfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
		cardinfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);		
		tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);//Byte9
		cardinfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
		cardinfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
		cardinfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;		
		tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8);//Byte10
		cardinfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;		
		cardinfo->CardCapacity=(cardinfo->SD_csd.DeviceSize+1);
		cardinfo->CardCapacity*=(1<<(cardinfo->SD_csd.DeviceSizeMul+2));
		cardinfo->CardBlockSize=1<<(cardinfo->SD_csd.RdBlockLen);
		cardinfo->CardCapacity*=cardinfo->CardBlockSize;
	}
	else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)
	{
		tmp=(u8)(CSD_Tab[1]&0x000000FF);//Byte7
		cardinfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;		
		tmp=(u8)((CSD_Tab[2]&0xFF000000)>>24);//Byte8
		cardinfo->SD_csd.DeviceSize|=(tmp<<8);		
		tmp=(u8)((CSD_Tab[2]&0x00FF0000)>>16);//Byte9
		cardinfo->SD_csd.DeviceSize|=(tmp);		
		tmp=(u8)((CSD_Tab[2]&0x0000FF00)>>8);//Byte10		
		cardinfo->CardCapacity=(cardinfo->SD_csd.DeviceSize+1)*512*1024;
		cardinfo->CardBlockSize=512;
	}
	cardinfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
	cardinfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;		
	tmp=(u8)(CSD_Tab[2]&0x000000FF);/*Byte11*/
	cardinfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
	cardinfo->SD_csd.WrProtectGrSize=(tmp&0x7F);		
	tmp=(u8)((CSD_Tab[3]&0xFF000000)>>24);/*Byte12*/
	cardinfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
	cardinfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
	cardinfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
	cardinfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;		
	tmp=(u8)((CSD_Tab[3]&0x00FF0000)>>16);/*Byte13*/
	cardinfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
	cardinfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
	cardinfo->SD_csd.Reserved3=0;
	cardinfo->SD_csd.ContentProtectAppli=(tmp&0x01);		
	tmp=(u8)((CSD_Tab[3]&0x0000FF00)>>8);/*Byte14*/
	cardinfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
	cardinfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
	cardinfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
	cardinfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
	cardinfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
	cardinfo->SD_csd.ECC=(tmp&0x03);		
	tmp=(u8)(CSD_Tab[3]&0x000000FF);/*Byte15*/
	cardinfo->SD_csd.CSD_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_csd.Reserved4=1;		
	tmp=(u8)((CID_Tab[0]&0xFF000000)>>24);/*Byte0*/
	cardinfo->SD_cid.ManufacturerID=tmp;	
	tmp=(u8)((CID_Tab[0]&0x00FF0000)>>16);/*Byte1*/
	cardinfo->SD_cid.OEM_AppliID=tmp<<8;	
	tmp=(u8)((CID_Tab[0]&0x000000FF00)>>8);/*Byte2*/
	cardinfo->SD_cid.OEM_AppliID|=tmp;	
	tmp=(u8)(CID_Tab[0]&0x000000FF);/*Byte3*/
	cardinfo->SD_cid.ProdName1=tmp<<24;	
	tmp=(u8)((CID_Tab[1]&0xFF000000)>>24);/*Byte4*/
	cardinfo->SD_cid.ProdName1|=tmp<<16;	
	tmp=(u8)((CID_Tab[1]&0x00FF0000)>>16);/*Byte5*/
	cardinfo->SD_cid.ProdName1|=tmp<<8;	
	tmp=(u8)((CID_Tab[1]&0x0000FF00)>>8);/*Byte6*/
	cardinfo->SD_cid.ProdName1|=tmp;	
	tmp=(u8)(CID_Tab[1]&0x000000FF);/*Byte7*/
	cardinfo->SD_cid.ProdName2=tmp;	
	tmp=(u8)((CID_Tab[2]&0xFF000000)>>24);/*Byte8*/
	cardinfo->SD_cid.ProdRev=tmp;	
	tmp=(u8)((CID_Tab[2]&0x00FF0000)>>16);/*Byte9*/
	cardinfo->SD_cid.ProdSN=tmp<<24;	
	tmp=(u8)((CID_Tab[2]&0x0000FF00)>>8);/*Byte10*/
	cardinfo->SD_cid.ProdSN|=tmp<<16;	
	tmp=(u8)(CID_Tab[2]&0x000000FF);/*Byte11*/
	cardinfo->SD_cid.ProdSN|=tmp<<8;	
	tmp=(u8)((CID_Tab[3]&0xFF000000)>>24);/*Byte12*/
	cardinfo->SD_cid.ProdSN|=tmp;	
	tmp=(u8)((CID_Tab[3]&0x00FF0000)>>16);/*Byte13*/
	cardinfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
	cardinfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;	
	tmp=(u8)((CID_Tab[3]&0x0000FF00)>>8);/*Byte14*/
	cardinfo->SD_cid.ManufactDate|=tmp;	
	tmp=(u8)(CID_Tab[3]&0x000000FF);/*Byte15*/
	cardinfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_cid.Reserved2=1;
	sd_Capacity=cardinfo->CardCapacity;
	return(SD_OK);
}
/*******************************************************************************
* Function Name  : SD_GetCapacity
*******************************************************************************/
u32 SD_GetCapacity(void)//得到SD卡容量
{
	u32 Capacity;
	SD_CardInfo SDCardInfo;
	SD_GetCardInfo(&SDCardInfo);
	Capacity=SDCardInfo.CardCapacity;
	return Capacity;
}
/*******************************************************************************
* Function Name  : SD_EnableWideBusOperation
*******************************************************************************/
SD_Error SD_EnableWideBusOperation(u32 WideMode)
{
  	if(CardType==SDIO_MULTIMEDIA_CARD)return(SD_UNSUPPORTED_FEATURE);//MMC卡不支持此特性
  	else if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)||(CardType==SDIO_HIGH_CAPACITY_SD_CARD))
  	{
    	if (WideMode==SDIO_BusWide_8b)return(SD_UNSUPPORTED_FEATURE);
    	else if(WideMode==SDIO_BusWide_4b&&SDEnWideBus(ENABLE)==SD_OK)SDIO_INIT(SDIO_TRANSFER_CLK_DIV,SDIO_BusWide_4b);//配置SDIO外设
    	else if(SDEnWideBus(DISABLE)==SD_OK)SDIO_INIT(SDIO_TRANSFER_CLK_DIV,SDIO_BusWide_1b);//配置SDIO外设
  	}
  return(SD_OK);
}
/*******************************************************************************
* Function Name  : SD_SelectDeselect
*******************************************************************************/
SD_Error SD_SelectDeselect(u32 addr)
{
  	SD_Error errorstatus=SD_OK;
  	SDIO_SendCMD(addr,SDIO_SEL_DESEL_CARD,SDIO_Response_Short);//发送CMD7:SDIO_SEL_DESEL_CARD
  	errorstatus=CmdResp1Error(SDIO_SEL_DESEL_CARD);
  	return(errorstatus);
}
/*******************************************************************************
* Function Name  : SD_ReadBlock
*******************************************************************************/
SD_Error SD_ReadBlock(u32 addr, u32 *readbuff, u16 BlockSize)
{
	u8 power=0;	
	if(NULL==readbuff)return(SD_INVALID_PARAMETER);	
  	SDIO_DataCFG(0x00,SDIO_DataBlockSize_1b,SDIO_TransferDir_ToCard,SDIO_DPSM_Disable);//清DPSM设置
  	SDIO_DMACmd(DISABLE);
  	if(SDIO_GetResponse(SDIO_RESP1)&SD_CARD_LOCKED)return(SD_LOCK_UNLOCK_FAILED);
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)BlockSize=512;
	else addr<<=9;
	if((BlockSize>0)&&(BlockSize<=2048)&&((BlockSize&(BlockSize-1))==0))
	{
	    power=convert_from_bytes_to_power_of_two(BlockSize);
		SDIO_SendCMD(BlockSize,SDIO_SET_BLOCKLEN,SDIO_Response_Short);//设置扇区大小
	    if(CmdResp1Error(SDIO_SET_BLOCKLEN)!=SD_OK)return CmdResp1Error(SDIO_SET_BLOCKLEN);
	}
	else return(SD_INVALID_PARAMETER);
  	SDIO_DataCFG(BlockSize,power<<4,SDIO_TransferDir_ToSDIO,SDIO_DPSM_Enable);
  	SDIO_SendCMD(addr,SDIO_READ_SINGLE_BLOCK,SDIO_Response_Short);//发送CMD17:READ_SINGLE_BLOCK 
  	if(CmdResp1Error(SDIO_READ_SINGLE_BLOCK)!=SD_OK)return CmdResp1Error(SDIO_READ_SINGLE_BLOCK);	 
	//DMA mode	
	SDIO_ITConfig(SDIO_IT_DCRCFAIL|SDIO_IT_DTIMEOUT|SDIO_IT_DATAEND|SDIO_IT_RXOVERR|SDIO_IT_STBITERR,ENABLE);
	SDIO_DMACmd(ENABLE);
	DMA_RxConfiguration(readbuff,BlockSize);
	while(DMA_GetFlagStatus(DMA2_FLAG_TC4)==RESET){}
	return(SD_OK);
}
/*******************************************************************************
* Function Name  : SD_WriteBlock
*******************************************************************************/
SD_Error SD_WriteBlock(u32 addr, u32 *writebuff, u16 BlockSize)
{
	SD_Error errorstatus=SD_OK;
	u8 power=0,cardstate=0;
	u32 timeout=0,cardstatus=0;
	if(writebuff==NULL)return(SD_INVALID_PARAMETER);	
	SDIO_DataCFG(0x00,SDIO_DataBlockSize_1b,SDIO_TransferDir_ToCard,SDIO_DPSM_Disable);
	SDIO_DMACmd(DISABLE);	
	if(SDIO_GetResponse(SDIO_RESP1)&SD_CARD_LOCKED)return(SD_LOCK_UNLOCK_FAILED);	
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)BlockSize=512;
	else addr<<=9;
	if((BlockSize>0)&&(BlockSize<=2048)&&((BlockSize&(BlockSize-1))==0))//设置扇区大小
	{
		power=convert_from_bytes_to_power_of_two(BlockSize);
		SDIO_SendCMD(BlockSize,SDIO_SET_BLOCKLEN,SDIO_Response_Short);
		if(CmdResp1Error(SDIO_SET_BLOCKLEN)!=SD_OK)return(CmdResp1Error(SDIO_SET_BLOCKLEN));
	}
	else return(SD_INVALID_PARAMETER);	
	SDIO_SendCMD(RCA<<16,SDIO_SEND_STATUS,SDIO_Response_Short);//等待卡准备好接收数据
	if(CmdResp1Error(SDIO_SEND_STATUS)!=SD_OK)return(CmdResp1Error(SDIO_SEND_STATUS));	
	cardstatus=SDIO_GetResponse(SDIO_RESP1);
	timeout=SD_DATATIMEOUT;	
	while(((cardstatus&0x00000100)==0)&&(timeout>0))
	{
		timeout--;
		SDIO_SendCMD(RCA<<16,SDIO_SEND_STATUS,SDIO_Response_Short);
		if(CmdResp1Error(SDIO_SEND_STATUS)!=SD_OK)return(CmdResp1Error(SDIO_SEND_STATUS));
		cardstatus=SDIO_GetResponse(SDIO_RESP1);
	}	
	if(timeout==0)return(SD_ERROR);
	SDIO_SendCMD(addr,SDIO_WRITE_SINGLE_BLOCK,SDIO_Response_Short);//发送CMD24:WRITE_SINGLE_BLOCK
	if(CmdResp1Error(SDIO_WRITE_SINGLE_BLOCK)!=SD_OK)return(CmdResp1Error(SDIO_WRITE_SINGLE_BLOCK));
	SDIO_DataCFG(BlockSize,power<<4,SDIO_TransferDir_ToCard,SDIO_DPSM_Enable);	
	//DMA模式
	SDIO_ITConfig(SDIO_IT_DCRCFAIL|SDIO_IT_DTIMEOUT|SDIO_IT_DATAEND|SDIO_IT_TXUNDERR|SDIO_IT_STBITERR,ENABLE);
	DMA_TxConfiguration(writebuff,BlockSize);
	SDIO_DMACmd(ENABLE);
	while(DMA_GetFlagStatus(DMA2_FLAG_TC4)==RESET){}
	while((TransferEnd==0)&&(TransferError==SD_OK)){}
	if(TransferError!=SD_OK)return(TransferError);
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清静态标志
	errorstatus=IsCardProgramming(&cardstate);//等待卡进入编程模式	
	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=IsCardProgramming(&cardstate);
	}
	return(errorstatus);
}
/*******************************************************************************
* Function Name  : CmdError
*******************************************************************************/
static SD_Error CmdError(void)
{
  	u32 timeout=SDIO_CMD0TIMEOUT;
  	while((timeout>0)&&(SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT)==RESET)){timeout--;}
  	if(timeout==0)return(SD_CMD_RSP_TIMEOUT);
  	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清所有静态标志
  	return(SD_OK);
}
/*******************************************************************************
* Function Name  : CmdResp7Error
*******************************************************************************/
static SD_Error CmdResp7Error(void)
{
  	u32 status;
  	u32 timeout=SDIO_CMD0TIMEOUT;
  	status=SDIO->STA;
	while(!(status&(SDIO_FLAG_CCRCFAIL|SDIO_FLAG_CMDREND|SDIO_FLAG_CTIMEOUT))&&(timeout>0))
  	{
    	timeout--;
    	status=SDIO->STA;
  	}
  	if((timeout==0)||(status&SDIO_FLAG_CTIMEOUT))
  	{
    	SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);//不是SD V2.0或电压范围不支持
    	return(SD_CMD_RSP_TIMEOUT);
  	}
  	if (status&SDIO_FLAG_CMDREND)
  	{
    	SDIO_ClearFlag(SDIO_FLAG_CMDREND);//是SD V2.0
    	return(SD_OK);
  	}
  	return(SD_OK);
}
/*******************************************************************************
* Function Name  : CmdResp1Error
*******************************************************************************/
static SD_Error CmdResp1Error(u8 cmd)
{
  	u32 status;
  	u32 response_r1;
  	status=SDIO->STA;
  	while(!(status&(SDIO_FLAG_CCRCFAIL|SDIO_FLAG_CMDREND|SDIO_FLAG_CTIMEOUT)))
  	{
    	status=SDIO->STA;
  	}
  	if(status&SDIO_FLAG_CTIMEOUT)
  	{
    	SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
    	return(SD_CMD_RSP_TIMEOUT);
  	}
  	else if(status&SDIO_FLAG_CCRCFAIL)
  	{
    	SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    	return(SD_CMD_CRC_FAIL);
  	}
  	if(SDIO_GetCommandResponse()!=cmd)return(SD_ILLEGAL_CMD);//检查得到的回应是否与命的要求相符
  	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清所有静态标志
  	response_r1=SDIO_GetResponse(SDIO_RESP1);//已得到回应,分析之
	if((response_r1&SD_OCR_ERRORBITS)==SD_ALLZERO)return(SD_OK);
	if(response_r1&SD_OCR_ADDR_OUT_OF_RANGE)return(SD_ADDR_OUT_OF_RANGE);
	if(response_r1&SD_OCR_ADDR_MISALIGNED)return(SD_ADDR_MISALIGNED);
	if(response_r1&SD_OCR_BLOCK_LEN_ERR)return(SD_BLOCK_LEN_ERR);
	if(response_r1&SD_OCR_ERASE_SEQ_ERR)return(SD_ERASE_SEQ_ERR);
	if(response_r1&SD_OCR_BAD_ERASE_PARAM)return(SD_BAD_ERASE_PARAM);
	if(response_r1&SD_OCR_WRITE_PROT_VIOLATION)return(SD_WRITE_PROT_VIOLATION);
	if(response_r1&SD_OCR_LOCK_UNLOCK_FAILED)return(SD_LOCK_UNLOCK_FAILED);
	if(response_r1&SD_OCR_COM_CRC_FAILED)return(SD_COM_CRC_FAILED);
	if(response_r1&SD_OCR_ILLEGAL_CMD)return(SD_ILLEGAL_CMD);
	if(response_r1&SD_OCR_CARD_ECC_FAILED)return(SD_CARD_ECC_FAILED);
	if(response_r1&SD_OCR_CC_ERROR)return(SD_CC_ERROR);
	if(response_r1&SD_OCR_GENERAL_UNKNOWN_ERROR)return(SD_GENERAL_UNKNOWN_ERROR);
	if(response_r1&SD_OCR_STREAM_READ_UNDERRUN)return(SD_STREAM_READ_UNDERRUN);
	if(response_r1&SD_OCR_STREAM_WRITE_OVERRUN)return(SD_STREAM_WRITE_OVERRUN);
	if(response_r1&SD_OCR_CID_CSD_OVERWRIETE)return(SD_CID_CSD_OVERWRITE);
	if(response_r1&SD_OCR_WP_ERASE_SKIP)return(SD_WP_ERASE_SKIP);
	if(response_r1&SD_OCR_CARD_ECC_DISABLED)return(SD_CARD_ECC_DISABLED);
	if(response_r1&SD_OCR_ERASE_RESET)return(SD_ERASE_RESET);
	if(response_r1&SD_OCR_AKE_SEQ_ERROR)return(SD_AKE_SEQ_ERROR);
	return(SD_OK);
}
/*******************************************************************************
* Function Name  : CmdResp3Error
*******************************************************************************/
static SD_Error CmdResp3Error(void)
{
	u32 status;	
	status=SDIO->STA;	
	while(!(status&(SDIO_FLAG_CCRCFAIL|SDIO_FLAG_CMDREND|SDIO_FLAG_CTIMEOUT))){status=SDIO->STA;}
	if(status&SDIO_FLAG_CTIMEOUT)
	{
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
		return(SD_CMD_RSP_TIMEOUT);
	}
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清所有静态标志
	return(SD_OK);
}
/*******************************************************************************
* Function Name  : CmdResp2Error
*******************************************************************************/
static SD_Error CmdResp2Error(void)
{
	u32 status;	
	status=SDIO->STA;	
	while(!(status&(SDIO_FLAG_CCRCFAIL|SDIO_FLAG_CTIMEOUT|SDIO_FLAG_CMDREND))){status=SDIO->STA;}	
	if(status&SDIO_FLAG_CTIMEOUT)
	{
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
		return(SD_CMD_RSP_TIMEOUT);
	}
	else if(status&SDIO_FLAG_CCRCFAIL)
	{
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
		return(SD_CMD_CRC_FAIL);
	}	
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清所有静态标志	
	return(SD_OK);
}
/*******************************************************************************
* Function Name  : CmdResp6Error
*******************************************************************************/
static SD_Error CmdResp6Error(u8 cmd, u16 *prca)
{
	u32 status;
	u32 response_r1;
	status=SDIO->STA;
	while(!(status&(SDIO_FLAG_CCRCFAIL|SDIO_FLAG_CTIMEOUT|SDIO_FLAG_CMDREND))){status=SDIO->STA;}	
	if(status&SDIO_FLAG_CTIMEOUT)
	{
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
		return(SD_CMD_RSP_TIMEOUT);
	}
	else if(status&SDIO_FLAG_CCRCFAIL)
	{
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
		return(SD_CMD_CRC_FAIL);
	}
	if(SDIO_GetCommandResponse()!=cmd)return(SD_ILLEGAL_CMD);//检查得到的回应是否与命的要求相符	
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清所有静态标志
	response_r1=SDIO_GetResponse(SDIO_RESP1);//已得到回应,分析之	
	if(SD_ALLZERO==(response_r1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED)))
	{
		*prca=(u16)(response_r1>>16);
		return(SD_OK);
	}	
	if(response_r1&SD_R6_GENERAL_UNKNOWN_ERROR)return(SD_GENERAL_UNKNOWN_ERROR);
	if(response_r1&SD_R6_ILLEGAL_CMD)return(SD_ILLEGAL_CMD);
	if(response_r1&SD_R6_COM_CRC_FAILED)return(SD_COM_CRC_FAILED);
	return(SD_OK);
}
/*******************************************************************************
* Function Name  : SDEnWideBus
*******************************************************************************/
static SD_Error SDEnWideBus(FunctionalState NewState)
{	
	u32 scr[2]={0,0};	
	if(SDIO_GetResponse(SDIO_RESP1)&SD_CARD_LOCKED)return(SD_LOCK_UNLOCK_FAILED);		
	if(FindSCR(RCA,scr)!=SD_OK)return(FindSCR(RCA,scr));//读SCR寄存器	
	if(NewState==ENABLE)//若高速总线操作已使能
	{
		if((scr[1]&SD_WIDE_BUS_SUPPORT)!=SD_ALLZERO)//若卡支持高速总线操作
		{
			SDIO_SendCMD(RCA<<16,SDIO_APP_CMD,SDIO_Response_Short);//发送CMD55:APP_CMD	
			if(CmdResp1Error(SDIO_APP_CMD)!=SD_OK)return(CmdResp1Error(SDIO_APP_CMD));
			SDIO_SendCMD(0x2,SDIO_APP_SD_SET_BUSWIDTH,SDIO_Response_Short);//发送CMD6:APP_CMD		
			if(CmdResp1Error(SDIO_APP_SD_SET_BUSWIDTH)!=SD_OK)return(CmdResp1Error(SDIO_APP_SD_SET_BUSWIDTH));
			return(SD_OK);
		}
		else return(SD_REQUEST_NOT_APPLICABLE);
	}//若高速总线操作未使能
	else
	{
		if((scr[1]&SD_SINGLE_BUS_SUPPORT)!=SD_ALLZERO)//若卡支持1bitmode总线操作
		{
			SDIO_SendCMD(RCA<<16,SDIO_APP_CMD,SDIO_Response_Short);//发送CMD55:APP_CMD		
			if(CmdResp1Error(SDIO_APP_CMD)!=SD_OK)return(CmdResp1Error(SDIO_APP_CMD));
			SDIO_SendCMD(0x00,SDIO_APP_SD_SET_BUSWIDTH,SDIO_Response_Short);//发送CMD6:APP_CMD
			if(CmdResp1Error(SDIO_APP_SD_SET_BUSWIDTH)!=SD_OK)return(CmdResp1Error(SDIO_APP_SD_SET_BUSWIDTH));	
			return(SD_OK);
		}
		else return(SD_REQUEST_NOT_APPLICABLE);
	}
}
/*******************************************************************************
* Function Name  : IsCardProgramming
*******************************************************************************/
static SD_Error IsCardProgramming(u8 *pstatus)
{
	vu32 respR1=0,status=0;	
	SDIO_SendCMD(RCA<<16,SDIO_SEND_STATUS,SDIO_Response_Short);	
	status=SDIO->STA;
	while(!(status&(SDIO_FLAG_CCRCFAIL|SDIO_FLAG_CMDREND|SDIO_FLAG_CTIMEOUT))){status=SDIO->STA;}	
	if(status&SDIO_FLAG_CTIMEOUT)
	{
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
		return(SD_CMD_RSP_TIMEOUT);
	}
	else if(status&SDIO_FLAG_CCRCFAIL)
	{
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
		return(SD_CMD_CRC_FAIL);
	}	
	status=(u32)SDIO_GetCommandResponse();	
	if(status!=SDIO_SEND_STATUS)return(SD_ILLEGAL_CMD);//检查得到的回应是否与命的要求相符	
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清所有静态标志	
	respR1=SDIO_GetResponse(SDIO_RESP1);//已得到回应,分析之	
	*pstatus=(u8)((respR1>>9)&0x0000000F);//查看卡的状态	
	if((respR1&SD_OCR_ERRORBITS)==SD_ALLZERO)return(SD_OK);	
	if(respR1&SD_OCR_ADDR_OUT_OF_RANGE)return(SD_ADDR_OUT_OF_RANGE);	
	if(respR1&SD_OCR_ADDR_MISALIGNED)return(SD_ADDR_MISALIGNED);	
	if(respR1&SD_OCR_BLOCK_LEN_ERR)return(SD_BLOCK_LEN_ERR);	
	if(respR1&SD_OCR_ERASE_SEQ_ERR)return(SD_ERASE_SEQ_ERR);	
	if(respR1&SD_OCR_BAD_ERASE_PARAM)return(SD_BAD_ERASE_PARAM);	
	if(respR1&SD_OCR_WRITE_PROT_VIOLATION)return(SD_WRITE_PROT_VIOLATION);	
	if(respR1&SD_OCR_LOCK_UNLOCK_FAILED)return(SD_LOCK_UNLOCK_FAILED);	
	if(respR1&SD_OCR_COM_CRC_FAILED)return(SD_COM_CRC_FAILED);
	if(respR1&SD_OCR_ILLEGAL_CMD)return(SD_ILLEGAL_CMD);
	if(respR1&SD_OCR_CARD_ECC_FAILED)return(SD_CARD_ECC_FAILED);	
	if(respR1&SD_OCR_CC_ERROR)return(SD_CC_ERROR);
	if(respR1&SD_OCR_GENERAL_UNKNOWN_ERROR)return(SD_GENERAL_UNKNOWN_ERROR);
	if(respR1&SD_OCR_STREAM_READ_UNDERRUN)return(SD_STREAM_READ_UNDERRUN);
	if(respR1&SD_OCR_STREAM_WRITE_OVERRUN)return(SD_STREAM_WRITE_OVERRUN);
	if(respR1&SD_OCR_CID_CSD_OVERWRIETE)return(SD_CID_CSD_OVERWRITE);
	if(respR1&SD_OCR_WP_ERASE_SKIP)return(SD_WP_ERASE_SKIP);
	if(respR1&SD_OCR_CARD_ECC_DISABLED)return(SD_CARD_ECC_DISABLED);
	if(respR1&SD_OCR_ERASE_RESET)return(SD_ERASE_RESET);
	if(respR1&SD_OCR_AKE_SEQ_ERROR)return(SD_AKE_SEQ_ERROR);
	return(SD_OK);
}
/*******************************************************************************
* Function Name  : FindSCR
*******************************************************************************/
static SD_Error FindSCR(u16 rca, u32 *pscr)
{
	u32 index=0;
	u32 tempscr[2]={0,0};
	SDIO_SendCMD(0x08,SDIO_SET_BLOCKLEN,SDIO_Response_Short);//发送CMD55:APP_CMD设置扇区大小为8字节
	if(CmdResp1Error(SDIO_SET_BLOCKLEN)!=SD_OK)return(CmdResp1Error(SDIO_SET_BLOCKLEN));
	SDIO_SendCMD(RCA<<16,SDIO_APP_CMD,SDIO_Response_Short);//发送CMD55:APP_CMD		
	if(CmdResp1Error(SDIO_APP_CMD)!=SD_OK)return(CmdResp1Error(SDIO_APP_CMD));
	SDIO_DataCFG(0x08,SDIO_DataBlockSize_8b,SDIO_TransferDir_ToSDIO,SDIO_DPSM_Enable);	
	SDIO_SendCMD(0x0,SDIO_SD_APP_SEND_SCR,SDIO_Response_Short);//发送CMD51:APP_SEND		
	if(CmdResp1Error(SDIO_SD_APP_SEND_SCR)!=SD_OK)return(CmdResp1Error(SDIO_SD_APP_SEND_SCR));	
	while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR)))
	{
		if(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL)!=RESET)
		{
			*(tempscr+index)=SDIO_ReadData();
			index++;
		}
	}	
	if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT)!=RESET)
	{
		SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
		return(SD_DATA_TIMEOUT);
	}
	else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL)!=RESET)
	{
		SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
		return(SD_DATA_CRC_FAIL);
	}
	else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR)!=RESET)
	{
		SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
		return(SD_RX_OVERRUN);
	}
	else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR)!=RESET)
	{
		SDIO_ClearFlag(SDIO_FLAG_STBITERR);
		return(SD_START_BIT_ERR);
	}
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);//清所有静态标志	
	*(pscr+1)=((tempscr[0]&SD_0TO7BITS)<<24)|((tempscr[0]&SD_8TO15BITS)<<8)|((tempscr[0]&SD_16TO23BITS)>>8)|((tempscr[0]&SD_24TO31BITS)>>24);	
	*(pscr)=((tempscr[1]&SD_0TO7BITS)<<24)|((tempscr[1]&SD_8TO15BITS)<<8)|((tempscr[1]&SD_16TO23BITS)>>8)|((tempscr[1]&SD_24TO31BITS)>>24);	
	return(SD_OK);
}
/*******************************************************************************
* Function Name  : convert_from_bytes_to_power_of_two
*******************************************************************************/
static u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes)
{
  	u8 count=0;
  	while(NumberOfBytes!=1)
  	{
    	NumberOfBytes>>=1;
    	count++;
  	}
  	return(count);
}
/*******************************************************************************
* Function Name  : SD_InitAndConfig
*******************************************************************************/
SD_Error SD_InitAndConfig(void)
{
	SD_Error Status = SD_OK;
	SD_CardInfo SDCardInfo;
  	Status = SD_Init();	
  	if (Status == SD_OK)Status = SD_GetCardInfo(&SDCardInfo);
  	if (Status == SD_OK)Status = SD_SelectDeselect((u32) (SDCardInfo.RCA << 16));
  	if (Status == SD_OK)Status = SD_EnableWideBusOperation(SDIO_BusWide_4b);
  	return Status;
}
/*******************************************************************************
* Function Name  : SDIO_SendCMD
*******************************************************************************/
void SDIO_SendCMD(u32 Argument,u32 CmdIndex,u32 Response)
{
  	u32 tmpreg=0;
  	SDIO->ARG=Argument;
  	tmpreg=SDIO->CMD;
  	tmpreg &= 0xFFFFF800;
  	tmpreg|=(u32)CmdIndex|Response|0x00|0x400;
  	SDIO->CMD=tmpreg;
}
/*******************************************************************************
* Function Name  : SDIO_DataCFG
*******************************************************************************/
void SDIO_DataCFG(u32 DataLength,u32 BlockSize,u32 TransferDir,u32 DPSM)
{
	u32 tmpreg=0;
	SDIO->DTIMER=0x000FFFFF;
	SDIO->DLEN=DataLength;
	tmpreg=SDIO->DCTRL;
	tmpreg&=0xFFFFFF08;
	tmpreg|=(u32)BlockSize|TransferDir|0x00|DPSM;
	SDIO->DCTRL=tmpreg;
}
/*******************************************************************************
* Function Name  : SDIO_INIT
*******************************************************************************/
void SDIO_INIT(u32 ClockDiv,u32 BusWide)
{
  	u32 tmpreg=0;
  	tmpreg=SDIO->CLKCR;//获取SDIO CLKCR的值 
  	tmpreg&=0xFFFF8100;//清除各位
  	tmpreg|=(ClockDiv|0x00|0x00|BusWide|0x00|0x00); 
  	SDIO->CLKCR = tmpreg;//向SDIO CLKCR写数据             
}
/*******************************************************************************
* Function Name  : SDIO_ITConfig
*******************************************************************************/
void SDIO_ITConfig(u32 SDIO_IT, FunctionalState NewState)
{  
  	if (NewState!=DISABLE)SDIO->MASK|=SDIO_IT;//SDIO中断使能
  	else SDIO->MASK&=~SDIO_IT;//取消SDIO中断使能
}
/*******************************************************************************
* Function Name  : SDIO_GetFlagStatus
*******************************************************************************/
FlagStatus SDIO_GetFlagStatus(u32 SDIO_FLAG)
{ 
  	FlagStatus bitstatus = RESET;
  	if ((SDIO->STA&SDIO_FLAG)!=(u32)RESET)
  	{
    	bitstatus=SET;
		SDIO_ClearFlag(SDIO_FLAG);
  	}
  	else bitstatus = RESET;
  	return bitstatus;
}



