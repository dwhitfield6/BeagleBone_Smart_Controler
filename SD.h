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
#include "ff.h"
#include "mmcsd_proto.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define HSMMCSD_BLK_SIZE               512
#define DRIVE_NUM_MMCSD     0
#define DRIVE_NUM_MAX      2

/* MMCSD instance related macros. */
#define MMCSD_INST_BASE                (SOC_MMCHS_0_REGS)
#define MMCSD_INT_NUM                  (SYS_INT_MMCSD0INT)

/* EDMA instance related macros. */
#define EDMA_INST_BASE                 (SOC_EDMA30CC_0_REGS)
#define EDMA_COMPLTN_INT_NUM           (SYS_INT_EDMACOMPINT)
#define EDMA_ERROR_INT_NUM             (SYS_INT_EDMAERRINT)

/* EDMA Events */
#define MMCSD_TX_EDMA_CHAN             (EDMA3_CHA_MMCSD0_TX)
#define MMCSD_RX_EDMA_CHAN             (EDMA3_CHA_MMCSD0_RX)

/* EDMA3 Event queue number. */
#define EVT_QUEUE_NUM                  0

#define HSMMCSD_IN_FREQ                96000000 /* 96MHz */
#define HSMMCSD_INIT_FREQ              400000   /* 400kHz */
#define HSMMCSD_CARD_DETECT_PINNUM     6

/*****************************************************************************
Defines the size of the buffers that hold the path, or temporary data from
the memory card.  There are two buffers allocated of this size.  The buffer
size must be large enough to hold the longest expected full path name,
including the file name, and a trailing null character.
******************************************************************************/
#define PATH_BUF_SIZE   512

/* Defines size of the buffers that hold temporary data. */
#define DATA_BUF_SIZE   64 * (2 * 512)
/*****************************************************************************
Defines the size of the buffer that holds the command line.
******************************************************************************/
#define CMD_BUF_SIZE    512

typedef struct _fatDevice
{
    /* Pointer to underlying device/controller */
    void *dev;

    /* File system pointer */
    FATFS *fs;

	/* state */
	unsigned int initDone;

}fatDevice;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern volatile unsigned int sdBlkSize;
extern volatile unsigned int callbackOccured;
extern volatile unsigned int xferCompFlag;
extern volatile unsigned int dataTimeout;
extern volatile unsigned int cmdCompFlag;
extern volatile unsigned int cmdTimeout;
extern volatile unsigned int errFlag;
extern mmcsdCtrlInfo  ctrlInfo;
extern mmcsdCardInfo sdCard;
extern fatDevice fat_devices[DRIVE_NUM_MAX];

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_SD(void);
DSTATUS disk_initialize(BYTE bValue);
DRESULT disk_read(BYTE drv, BYTE* buff, DWORD sector, BYTE count);
DRESULT disk_write (BYTE ucDrive, const BYTE* buff, DWORD sector, BYTE count);
DRESULT disk_ioctl ( BYTE drv, BYTE ctrl, void *buff);
DWORD get_fattime (void);
unsigned int HSMMCSDCmdStatusGet(mmcsdCtrlInfo *ctrl);
unsigned int HSMMCSDXferStatusGet(mmcsdCtrlInfo *ctrl);
void HSMMCSDRxDmaConfig(void *ptr, unsigned int blkSize, unsigned int nblks);
void HSMMCSDTxDmaConfig(void *ptr, unsigned int blkSize, unsigned int blks);
void HSMMCSDXferSetup(mmcsdCtrlInfo *ctrl, unsigned char rwFlag, void *ptr, unsigned int blkSize, unsigned int nBlks);
void callback(unsigned int tccNum, unsigned int status);
void Edma3CompletionIsr(void);
void Edma3CCErrorIsr(void);
void HSMMCSDIsr(void);
void EDMA3AINTCConfigure(void);
void EDMA3Initialize(void);
void HSMMCSDEdmaInit(void);
void HSMMCSDControllerSetup(void);
void HSMMCSDFsMount(unsigned int driveNum, void *ptr);
DSTATUS disk_status (BYTE drv);

#endif
/******************************* End of file *********************************/
