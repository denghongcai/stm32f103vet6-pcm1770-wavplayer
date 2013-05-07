#ifndef __SDCARD_H
#define __SDCARD_H
#include "stm32f10x.h"
//命令地址
#define SDIO_GO_IDLE_STATE                       ((u8)0)
#define SDIO_SEND_OP_COND                        ((u8)1)
#define SDIO_ALL_SEND_CID                        ((u8)2)
#define SDIO_SET_REL_ADDR                        ((u8)3) 
#define SDIO_SET_DSR                             ((u8)4)
#define SDIO_SDIO_SEN_OP_COND                    ((u8)5)
#define SDIO_HS_SWITCH                           ((u8)6)
#define SDIO_SEL_DESEL_CARD                      ((u8)7)
#define SDIO_HS_SEND_EXT_CSD                     ((u8)8)
#define SDIO_SEND_CSD                            ((u8)9)
#define SDIO_SEND_CID                            ((u8)10)
#define SDIO_READ_DAT_UNTIL_STOP                 ((u8)11) 
#define SDIO_STOP_TRANSMISSION                   ((u8)12)
#define SDIO_SEND_STATUS                         ((u8)13)
#define SDIO_HS_BUSTEST_READ                     ((u8)14)
#define SDIO_GO_INACTIVE_STATE                   ((u8)15)
#define SDIO_SET_BLOCKLEN                        ((u8)16)
#define SDIO_READ_SINGLE_BLOCK                   ((u8)17)
#define SDIO_READ_MULT_BLOCK                     ((u8)18)
#define SDIO_HS_BUSTEST_WRITE                    ((u8)19)
#define SDIO_WRITE_DAT_UNTIL_STOP                ((u8)20) 
#define SDIO_SET_BLOCK_COUNT                     ((u8)23) 
#define SDIO_WRITE_SINGLE_BLOCK                  ((u8)24)
#define SDIO_WRITE_MULT_BLOCK                    ((u8)25)
#define SDIO_PROG_CID                            ((u8)26) 
#define SDIO_PROG_CSD                            ((u8)27)
#define SDIO_SET_WRITE_PROT                      ((u8)28)
#define SDIO_CLR_WRITE_PROT                      ((u8)29)
#define SDIO_SEND_WRITE_PROT                     ((u8)30)
#define SDIO_SD_ERASE_GRP_START                  ((u8)32) 
#define SDIO_SD_ERASE_GRP_END                    ((u8)33) 
#define SDIO_ERASE_GRP_START                     ((u8)35) 
#define SDIO_ERASE_GRP_END                       ((u8)36) 
#define SDIO_ERASE                               ((u8)38)
#define SDIO_FAST_IO                             ((u8)39) 
#define SDIO_GO_IRQ_STATE                        ((u8)40)
#define SDIO_LOCK_UNLOCK                         ((u8)42)
#define SDIO_APP_CMD                             ((u8)55)
#define SDIO_GEN_CMD                             ((u8)56)
#define SDIO_NO_CMD                              ((u8)64)
//特殊命令
#define SDIO_APP_SD_SET_BUSWIDTH                 ((u8)6) 
#define SDIO_SD_APP_STAUS                        ((u8)13)
#define SDIO_SD_APP_SEND_NUM_WRITE_BLOCKS        ((u8)22)
#define SDIO_SD_APP_OP_COND                      ((u8)41)
#define SDIO_SD_APP_SET_CLR_CARD_DETECT          ((u8)42) 
#define SDIO_SD_APP_SEND_SCR                     ((u8)51) 
#define SDIO_SDIO_RW_DIRECT                      ((u8)52) 
#define SDIO_SDIO_RW_EXTENDED                    ((u8)53) 
//特殊安全命令
#define SDIO_SD_APP_GET_MKB                      ((u8)43) /* For SD Card only */
#define SDIO_SD_APP_GET_MID                      ((u8)44) /* For SD Card only */
#define SDIO_SD_APP_SET_CER_RN1                  ((u8)45) /* For SD Card only */
#define SDIO_SD_APP_GET_CER_RN2                  ((u8)46) /* For SD Card only */
#define SDIO_SD_APP_SET_CER_RES2                 ((u8)47) /* For SD Card only */
#define SDIO_SD_APP_GET_CER_RES1                 ((u8)48) /* For SD Card only */
#define SDIO_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((u8)18) /* For SD Card only */
#define SDIO_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((u8)25) /* For SD Card only */
#define SDIO_SD_APP_SECURE_ERASE                 ((u8)38) /* For SD Card only */
#define SDIO_SD_APP_CHANGE_SECURE_AREA           ((u8)49) /* For SD Card only */
#define SDIO_SD_APP_SECURE_WRITE_MKB             ((u8)48) /* For SD Card only */
//////////////////////////////////////////////////////
#define NULL 0
#define SDIO_STATIC_FLAGS               ((u32)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((u32)0x00002710)
#define SD_OCR_ADDR_OUT_OF_RANGE        ((u32)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((u32)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((u32)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((u32)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((u32)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((u32)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((u32)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((u32)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((u32)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((u32)0x00200000)
#define SD_OCR_CC_ERROR                 ((u32)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((u32)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((u32)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((u32)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((u32)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((u32)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((u32)0x00004000)
#define SD_OCR_ERASE_RESET              ((u32)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((u32)0x00000008)
#define SD_OCR_ERRORBITS                ((u32)0xFDFFE008)
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((u32)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((u32)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((u32)0x00008000)
#define SD_VOLTAGE_WINDOW_SD            ((u32)0x80100000)
#define SD_HIGH_CAPACITY                ((u32)0x40000000)
#define SD_STD_CAPACITY                 ((u32)0x00000000)
#define SD_CHECK_PATTERN                ((u32)0x000001AA)
#define SD_MAX_VOLT_TRIAL               ((u32)0x0000FFFF)
#define SD_ALLZERO                      ((u32)0x00000000)
#define SD_WIDE_BUS_SUPPORT             ((u32)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((u32)0x00010000)
#define SD_CARD_LOCKED                  ((u32)0x02000000)
#define SD_CARD_PROGRAMMING             ((u32)0x00000007)
#define SD_CARD_RECEIVING               ((u32)0x00000006)
#define SD_DATATIMEOUT                  ((u32)0x000FFFFF)
#define SD_0TO7BITS                     ((u32)0x000000FF)
#define SD_8TO15BITS                    ((u32)0x0000FF00)
#define SD_16TO23BITS                   ((u32)0x00FF0000)
#define SD_24TO31BITS                   ((u32)0xFF000000)
#define SDIO_SEND_IF_COND               ((u32)0x00000008)
#define SDIO_INIT_CLK_DIV                  ((u8)0xB2)
#define SDIO_TRANSFER_CLK_DIV              ((u8)0x1)
typedef enum
{
  	SD_NO_TRANSFER  = 0,
  	SD_TRANSFER_IN_PROGRESS
} 	SDTransferState;
typedef enum
{
  //特殊错误码
  	SD_CMD_CRC_FAIL                    = (1), /* Command response received (but CRC check failed) */
  	SD_DATA_CRC_FAIL                   = (2), /* Data bock sent/received (CRC check Failed) */
  	SD_CMD_RSP_TIMEOUT                 = (3), /* Command response timeout */
  	SD_DATA_TIMEOUT                    = (4), /* Data time out */
  	SD_TX_UNDERRUN                     = (5), /* Transmit FIFO under-run */
  	SD_RX_OVERRUN                      = (6), /* Receive FIFO over-run */
  	SD_START_BIT_ERR                   = (7), /* Start bit not detected on all data signals in widE bus mode */
  	SD_CMD_OUT_OF_RANGE                = (8), /* CMD's argument was out of range.*/
  	SD_ADDR_MISALIGNED                 = (9), /* Misaligned address */
  	SD_BLOCK_LEN_ERR                   = (10), /* Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
  	SD_ERASE_SEQ_ERR                   = (11), /* An error in the sequence of erase command occurs.*/
  	SD_BAD_ERASE_PARAM                 = (12), /* An Invalid selection for erase groups */
  	SD_WRITE_PROT_VIOLATION            = (13), /* Attempt to program a write protect block */
  	SD_LOCK_UNLOCK_FAILED              = (14), /* Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
  	SD_COM_CRC_FAILED                  = (15), /* CRC check of the previous command failed */
  	SD_ILLEGAL_CMD                     = (16), /* Command is not legal for the card state */
  	SD_CARD_ECC_FAILED                 = (17), /* Card internal ECC was applied but failed to correct the data */
  	SD_CC_ERROR                        = (18), /* Internal card controller error */
  	SD_GENERAL_UNKNOWN_ERROR           = (19), /* General or Unknown error */
  	SD_STREAM_READ_UNDERRUN            = (20), /* The card could not sustain data transfer in stream read operation. */
  	SD_STREAM_WRITE_OVERRUN            = (21), /* The card could not sustain data programming in stream mode */
  	SD_CID_CSD_OVERWRITE               = (22), /* CID/CSD overwrite error */
  	SD_WP_ERASE_SKIP                   = (23), /* only partial address space was erased */
  	SD_CARD_ECC_DISABLED               = (24), /* Command has been executed without using internal ECC */
  	SD_ERASE_RESET                     = (25), /* Erase sequence was cleared before executing because an out of erase sequence command was received */
  	SD_AKE_SEQ_ERROR                   = (26), /* Error in sequence of authentication. */
  	SD_INVALID_VOLTRANGE               = (27),
  	SD_ADDR_OUT_OF_RANGE               = (28),
  	SD_SWITCH_ERROR                    = (29),
  	SD_SDIO_DISABLED                   = (30),
  	SD_SDIO_FUNCTION_BUSY              = (31),
  	SD_SDIO_FUNCTION_FAILED            = (32),
  	SD_SDIO_UNKNOWN_FUNCTION           = (33),
  	//标准错误码
  	SD_INTERNAL_ERROR, 
  	SD_NOT_CONFIGURED,
  	SD_REQUEST_PENDING, 
  	SD_REQUEST_NOT_APPLICABLE, 
  	SD_INVALID_PARAMETER,  
  	SD_UNSUPPORTED_FEATURE,  
  	SD_UNSUPPORTED_HW,  
  	SD_ERROR,  
  	SD_OK,  
} SD_Error;
typedef struct       //Card Specific Data 
{
  	vu8  CSDStruct;            /* CSD structure */
  	vu8  SysSpecVersion;       /* System specification version */
  	vu8  Reserved1;            /* Reserved */
  	vu8  TAAC;                 /* Data read access-time 1 */
  	vu8  NSAC;                 /* Data read access-time 2 in CLK cycles */
  	vu8  MaxBusClkFrec;        /* Max. bus clock frequency */
  	vu16 CardComdClasses;      /* Card command classes */
  	vu8  RdBlockLen;           /* Max. read data block length */
  	vu8  PartBlockRead;        /* Partial blocks for read allowed */
  	vu8  WrBlockMisalign;      /* Write block misalignment */
  	vu8  RdBlockMisalign;      /* Read block misalignment */
  	vu8  DSRImpl;              /* DSR implemented */
  	vu8  Reserved2;            /* Reserved */
  	vu32 DeviceSize;           /* Device Size */
  	vu8  MaxRdCurrentVDDMin;   /* Max. read current @ VDD min */
  	vu8  MaxRdCurrentVDDMax;   /* Max. read current @ VDD max */
  	vu8  MaxWrCurrentVDDMin;   /* Max. write current @ VDD min */
  	vu8  MaxWrCurrentVDDMax;   /* Max. write current @ VDD max */
  	vu8  DeviceSizeMul;        /* Device size multiplier */
  	vu8  EraseGrSize;          /* Erase group size */
  	vu8  EraseGrMul;           /* Erase group size multiplier */
  	vu8  WrProtectGrSize;      /* Write protect group size */
  	vu8  WrProtectGrEnable;    /* Write protect group enable */
  	vu8  ManDeflECC;           /* Manufacturer default ECC */
  	vu8  WrSpeedFact;          /* Write speed factor */
  	vu8  MaxWrBlockLen;        /* Max. write data block length */
  	vu8  WriteBlockPaPartial;  /* Partial blocks for write allowed */
  	vu8  Reserved3;            /* Reserded */
  	vu8  ContentProtectAppli;  /* Content protection application */
  	vu8  FileFormatGrouop;     /* File format group */
  	vu8  CopyFlag;             /* Copy flag (OTP) */
  	vu8  PermWrProtect;        /* Permanent write protection */
  	vu8  TempWrProtect;        /* Temporary write protection */
  	vu8  FileFormat;           /* File Format */
  	vu8  ECC;                  /* ECC code */
  	vu8  CSD_CRC;              /* CSD CRC */
  	vu8  Reserved4;            /* always 1*/
} SD_CSD;
typedef struct      /*Card Identification Data*/
{
  	vu8  ManufacturerID;       /* ManufacturerID */
  	vu16 OEM_AppliID;          /* OEM/Application ID */
  	vu32 ProdName1;            /* Product Name part1 */
  	vu8  ProdName2;            /* Product Name part2*/
  	vu8  ProdRev;              /* Product Revision */
  	vu32 ProdSN;               /* Product Serial Number */
  	vu8  Reserved1;            /* Reserved1 */
  	vu16 ManufactDate;         /* Manufacturing Date */
  	vu8  CID_CRC;              /* CID CRC */
  	vu8  Reserved2;            /* always 1 */
} SD_CID;
typedef struct
{
  	SD_CSD SD_csd;
  	SD_CID SD_cid;
  	u32 CardCapacity; //卡容量
  	u32 CardBlockSize; //扇区大小
  	u16 RCA;
  	u8 CardType;
} SD_CardInfo;

//储存卡类型
#define SDIO_STD_CAPACITY_SD_CARD_V1_1     ((u32)0x0)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0     ((u32)0x1)
#define SDIO_HIGH_CAPACITY_SD_CARD         ((u32)0x2)
#define SDIO_MULTIMEDIA_CARD               ((u32)0x3)
#define SDIO_SECURE_DIGITAL_IO_CARD        ((u32)0x4)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD    ((u32)0x5)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD  ((u32)0x6)
#define SDIO_HIGH_CAPACITY_MMC_CARD        ((u32)0x7)
//个人宏定义
#define SDIO_DMACmd(x) 	*(vu32 *)(PERIPH_BB_BASE+(((SDIO_BASE-PERIPH_BASE)+0x2C)*32)+12)=(u32)x;
#define SDIO_ClockCmd(x) *(vu32 *)(PERIPH_BB_BASE+(((SDIO_BASE-PERIPH_BASE)+0x04)*32)+32)=(u32)x;
#define SDIO_ClearFlag(x) SDIO->ICR=x;
#define SDIO_GetCommandResponse()  (u8)(SDIO->RESPCMD)
#define SDIO_GetResponse(x)	 (*(vu32 *)((SDIO_BASE+0x14)+x))
#define SDIO_ReadData()  (SDIO->FIFO)
#define SDIO_SetPowerState(x)  SDIO->POWER&=0xFFFFFFFC;SDIO->POWER|=x;
//函数声明
SD_Error SD_Init(void);
SD_Error SD_PowerON(void);
SD_Error SD_PowerOFF(void);
SD_Error SD_InitializeCards(void);
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo);
u32 SD_GetCapacity(void);//得到SD卡容量
SD_Error SD_EnableWideBusOperation(u32 WideMode);
SD_Error SD_SelectDeselect(u32 addr);
SD_Error SD_ReadBlock(u32 addr, u32 *readbuff, u16 BlockSize);
SD_Error SD_ReadMultiBlocks(u32 addr, u32 *readbuff, u16 BlockSize, u32 NumberOfBlocks);
SD_Error SD_WriteBlock(u32 addr, u32 *writebuff, u16 BlockSize);
SD_Error SD_WriteMultiBlocks(u32 addr, u32 *writebuff, u16 BlockSize, u32 NumberOfBlocks);
static SD_Error CmdError(void);
static SD_Error CmdResp1Error(u8 cmd);
static SD_Error CmdResp7Error(void);
static SD_Error CmdResp3Error(void);
static SD_Error CmdResp2Error(void);
static SD_Error CmdResp6Error(u8 cmd, u16 *prca);
static SD_Error SDEnWideBus(FunctionalState NewState);
static SD_Error IsCardProgramming(u8 *pstatus);
static SD_Error FindSCR(u16 rca, u32 *pscr);
static u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes);
SD_Error SD_InitAndConfig(void);
void SDIO_ITConfig(u32 SDIO_IT, FunctionalState NewState);
void SDIO_SendCMD(u32 Argument,u32 CmdIndex,u32 Response);
void SDIO_DataCFG(u32 DataLength,u32 BlockSize,u32 TransferDir,u32 DPSM);
void SDIO_INIT(u32 ClockDiv,u32 BusWide);
FlagStatus SDIO_GetFlagStatus(u32 SDIO_FLAG);
//SD_Error SD_SetDeviceMode(u32 Mode);
//SDTransferState SD_GetTransferState(void);
//SD_Error SD_StopTransfer(void);
//SD_Error SD_Erase(u32 startaddr, u32 endaddr);
//SD_Error SD_SendStatus(u32 *pcardstatus);
//SD_Error SD_SendSDStatus(u32 *psdstatus);
//SD_Error SD_ProcessIRQSrc(void);
#endif 
