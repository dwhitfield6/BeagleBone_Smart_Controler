/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/14/16     16.0_DW0a   New project creation.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Contains functions to set up and control the SD card.
 *                                                                            */
/******************************************************************************/

#ifndef _SD_H_
#define _SD_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "diskio.h"
#include "edma.h"
#include "ff.h"
#include "mmcsd_proto.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define HSMMCSD_BLK_SIZE			512
#define DRIVE_NUM_MMCSD     		0
#define DRIVE_NUM_MAX      			2
#define MMCSD_INST_BASE    			(SOC_MMCHS_0_REGS)
#define MMCSD_INT_NUM       		(SYS_INT_MMCSD0INT)
#define EDMA_INST_BASE  			(SOC_EDMA30CC_0_REGS)
#define EDMA_COMPLTN_INT_NUM 		(SYS_INT_EDMACOMPINT)
#define EDMA_ERROR_INT_NUM  		(SYS_INT_EDMAERRINT)
#define MMCSD_TX_EDMA_CHAN			(EDMA3_CHA_MMCSD0_TX)
#define MMCSD_RX_EDMA_CHAN   		(EDMA3_CHA_MMCSD0_RX)
#define EVT_QUEUE_NUM      			0
#define HSMMCSD_IN_FREQ         	96000000 /* 96MHz */
#define HSMMCSD_INIT_FREQ    		400000   /* 400kHz */
#define HSMMCSD_CARD_DETECT_PINNUM 	6
#define PATH_BUF_SIZE   			512
#define DATA_BUF_SIZE   			64 * (2 * 512)
#define CMD_BUF_SIZE    			512
#define FILE_DATA_BUFFER_SIZE 		1024

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef struct t_fat_Device
{
    void *dev;
    FATFS *fs;
	unsigned int initDone;

}TYPE_FAT_DEVICE;

typedef enum e_sd_card_state
{
	CARD_NOTPRESENT,
	CARD_PRESENT,
}ENUM_SD_CARD_STATE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern volatile unsigned int sdBlkSize;
extern volatile unsigned int SD_callbackOccured;
extern volatile unsigned int xferCompFlag;
extern volatile unsigned int dataTimeout;
extern volatile unsigned int cmdCompFlag;
extern volatile unsigned int cmdTimeout;
extern volatile unsigned int errFlag;
extern mmcsdCtrlInfo  ctrlInfo;
extern mmcsdCardInfo sdCard;
extern TYPE_FAT_DEVICE fat_devices[DRIVE_NUM_MAX];
extern void (*cb_Fxn[EDMA3_NUM_TCC]) (unsigned int tcc, unsigned int status);
extern char FileDataBuffer[FILE_DATA_BUFFER_SIZE];
extern unsigned long BytesWritten;
extern unsigned long BytesRead;
extern FRESULT Result;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_SD(void);
unsigned char SD_IsCardInserted(void);
void SD_ReInitialize(void);
unsigned char SD_DiskInitialize(unsigned char bValue);
unsigned char SD_DiskStatus (unsigned char drv);
ENUM_DISK_RESULT SD_DiskRead(unsigned char drv, unsigned char* buff, unsigned long sector, unsigned char count);
ENUM_DISK_RESULT SD_DiskWrite (unsigned char ucDrive, const unsigned char* buff, unsigned long sector, unsigned char count);
ENUM_DISK_RESULT SD_DiskIOCTL(unsigned char drv, unsigned char ctrl, void *buff);
unsigned long SD_GetFatTime(void);
unsigned int SD_HSMMCSDCmdStatusGet(mmcsdCtrlInfo *ctrl);
unsigned int SD_HSMMCSDXferStatusGet(mmcsdCtrlInfo *ctrl);
void SD_HSMMCSDRxDmaConfig(void *ptr, unsigned int blkSize, unsigned int nblks);
void SD_HSMMCSDTxDmaConfig(void *ptr, unsigned int blkSize, unsigned int blks);
void SD_HSMMCSDXferSetup(mmcsdCtrlInfo *ctrl, unsigned char rwFlag, void *ptr, unsigned int blkSize, unsigned int nBlks);
void SD_DMA_Callback(unsigned int tccNum, unsigned int status);
void SD_DMA_ConfigureInterrupt(void);
void SD_DMA_Init(void);
void SD_DMA_HSMMCSD(void);
void SD_HSMMCSDControllerSetup(void);
void SD_HSMMCSDFsMount(unsigned int driveNum, void *ptr);
void SD_SetCardActionFlag(void);
void SD_ClearCardActionFlag(void);
unsigned char SD_GetCardActionFlag(void);
void SD_SetCardStatus(ENUM_SD_CARD_STATE state);
ENUM_SD_CARD_STATE SD_GetCardStatus(void);
void SD_SetInitialized(void);
void SD_ClearInitialized(void);
unsigned char SD_IsInitialized(void);

#endif
/******************************* End of file *********************************/
