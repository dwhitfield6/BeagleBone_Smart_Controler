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

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <string.h>

#include "beaglebone.h"
#include "diskio.h"
#include "edma.h"
#include "edma_event.h"
#include "ff.h"
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_hs_mmcsd.h"
#include "hs_mmcsd.h"
#include "hs_mmcsdlib.h"
#include "hw_types.h"
#include "interrupt.h"
#include "mmcsd_proto.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "GPIO.h"
#include "INTERRUPTS.h"
#include "LEDS.h"
#include "RTCC.h"
#include "SD.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
static unsigned char SD_CardActionFlag = FALSE;
static ENUM_SD_CARD_STATE SD_CardState = CARD_NOTPRESENT;
static unsigned char SD_CardInitialized = FALSE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
volatile unsigned int sdBlkSize = HSMMCSD_BLK_SIZE;
volatile unsigned int SD_callbackOccured = 0;
volatile unsigned int xferCompFlag = 0;
volatile unsigned int dataTimeout = 0;
volatile unsigned int cmdCompFlag = 0;
volatile unsigned int cmdTimeout = 0;
volatile unsigned int errFlag = 0;
mmcsdCtrlInfo  ctrlInfo;
mmcsdCardInfo sdCard;
TYPE_FAT_DEVICE fat_devices[DRIVE_NUM_MAX];
volatile unsigned int g_sPState = 0;
volatile unsigned int g_sCState = 0;

#pragma DATA_ALIGN(g_sFatFs, SOC_CACHELINE_SIZE);
static FATFS g_sFatFs;

void (*cb_Fxn[EDMA3_NUM_TCC]) (unsigned int tcc, unsigned int status);
char FileDataBuffer[FILE_DATA_BUFFER_SIZE];
unsigned long BytesWritten;
unsigned long BytesRead;
FRESULT Result;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_SD
 *
 * Initializes the SD card.
 *                                                                            */
/******************************************************************************/
void Init_SD(void)
{
    /* Configure the EDMA clocks. */
    EDMAModuleClkConfig();

    /* Configure EDMA to service the HSMMCSD events. */
    SD_DMA_HSMMCSD();

    /* Perform pin-mux for HSMMCSD pins. */
    HSMMCSDPinMuxSetup();

    /* Enable module clock for HSMMCSD. */
    HSMMCSDModuleClkConfig();

    /* Basic controller initializations */
    SD_HSMMCSDControllerSetup();

    /* Initialize the MMCSD controller */
    MMCSDCtrlInit(&ctrlInfo);

    MMCSDIntEnable(&ctrlInfo);

    SD_SetCardActionFlag();
}

/******************************************************************************/
/* SD_IsCardInserted
 *
 * Returns TRUE if there is an SD card inserted.
 *                                                                            */
/******************************************************************************/
unsigned char SD_IsCardInserted(void)
{
	if((HSMMCSDCardPresent(&ctrlInfo)) == 1)
	{
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************/
/* SD_ReInitialize
 *
 * Re-Initializes all SD card variables.
 *                                                                            */
/******************************************************************************/
void SD_ReInitialize(void)
{
	SD_callbackOccured = 0;
	SD_ClearInitialized();
	xferCompFlag = 0;
	dataTimeout = 0;
	cmdCompFlag = 0;
	cmdTimeout = 0;

	/* Initialize the MMCSD controller */
	MMCSDCtrlInit(&ctrlInfo);
	MMCSDIntEnable(&ctrlInfo);
}

/******************************************************************************/
/* SD_DiskInitialize
 *
 * Initializes the SD disk
 *                                                                            */
/******************************************************************************/
unsigned char SD_DiskInitialize(unsigned char bValue)
{
	unsigned int status;

	if ((DRIVE_NUM_MMCSD == bValue) && (fat_devices[bValue].initDone != 1))
	{
		mmcsdCardInfo *card = (mmcsdCardInfo *) fat_devices[bValue].dev;

		/* SD Card init */
		status = MMCSDCardInit(card->ctrl);

		if (status == 0)
		{
			//UARTPuts("\r\nCard Init Failed \r\n", -1);

			return STA_NOINIT;
		}
		else
		{
			/* Set bus width */
			if (card->cardType == MMCSD_CARD_SD)
			{
				MMCSDBusWidthSet(card->ctrl);
			}

			/* Transfer speed */
			MMCSDTranSpeedSet(card->ctrl);
		}

		fat_devices[bValue].initDone = 1;
	}
	return 0;
}

/******************************************************************************/
/* SD_DiskStatus
 *
 * Returns the disk status.
 *                                                                            */
/******************************************************************************/
unsigned char SD_DiskStatus (unsigned char drv)
{
	return 0;
}

/******************************************************************************/
/* SD_DiskRead
 *
 * Reads sectors from the SD card.
 *                                                                            */
/******************************************************************************/
ENUM_DISK_RESULT SD_DiskRead(unsigned char drv, unsigned char* buff, unsigned long sector, unsigned char count)
{
	if (drv == DRIVE_NUM_MMCSD)
	{
		mmcsdCardInfo *card = fat_devices[drv].dev;

    	/* READ BLOCK */
		if (MMCSDReadCmdSend(card->ctrl, buff, sector, count) == 1)
		{
        	return RES_OK;
		}
    }
    return RES_ERROR;
}

/******************************************************************************/
/* SD_DiskWrite
 *
 * Writes sectors from the SD card.
 *                                                                            */
/******************************************************************************/
ENUM_DISK_RESULT SD_DiskWrite (unsigned char ucDrive, const unsigned char* buff, unsigned long sector, unsigned char count)
{
	if (ucDrive == DRIVE_NUM_MMCSD)
	{
		mmcsdCardInfo *card = fat_devices[ucDrive].dev;
    	/* WRITE BLOCK */
	    if(MMCSDWriteCmdSend(card->ctrl,(BYTE*) buff, sector, count) == 1)
		{
        	return RES_OK;
		}
	}

    return RES_ERROR;
}

/******************************************************************************/
/* SD_DiskIOCTL
 *
 * Checks the SD card control register.
 *                                                                            */
/******************************************************************************/
ENUM_DISK_RESULT SD_DiskIOCTL(unsigned char drv, unsigned char ctrl, void *buff)
{
	return RES_OK;
}

/******************************************************************************/
/* SD_GetFatTime
 *
 * Returns the FAT structured RTC time.
 *                                                                            */
/******************************************************************************/
unsigned long SD_GetFatTime(void)
{
	unsigned long temp = 0;

	temp = (CurrentTimeDate.Date.Year-1980) << 25;
	temp |= CurrentTimeDate.Date.Month << 21;
	temp |= CurrentTimeDate.Date.Day << 16;
	temp |= CurrentTimeDate.Time.Hour << 11;
	temp |= CurrentTimeDate.Time.Minute << 5;
	temp |= CurrentTimeDate.Time.Second >> 1;
	return temp;
}

/******************************************************************************/
/* SD_HSMMCSDCmdStatusGet
 *
 * Checks the command status.
 *                                                                            */
/******************************************************************************/
unsigned int SD_HSMMCSDCmdStatusGet(mmcsdCtrlInfo *ctrl)
{
    unsigned int status = 0;

    while ((cmdCompFlag == 0) && (cmdTimeout == 0))
    {
    	if((!SD_IsCardInserted()) || (!SD_IsInitialized()))
    	{
    		break;
    	}
    }

    if (cmdCompFlag)
    {
        status = 1;
        cmdCompFlag = 0;
    }

    if (cmdTimeout)
    {
        status = 0;
        cmdTimeout = 0;
    }

    return status;
}

/******************************************************************************/
/* SD_HSMMCSDXferStatusGet
 *
 * Gets the SD transfer status.
 *                                                                            */
/******************************************************************************/
unsigned int SD_HSMMCSDXferStatusGet(mmcsdCtrlInfo *ctrl)
{
    unsigned int status = 0;
    volatile unsigned int timeOut = 0xFFFF;

    while ((xferCompFlag == 0) && (dataTimeout == 0))
    {
    	if((!SD_IsCardInserted()) || (!SD_IsInitialized()))
    	{
    		break;
    	}
    }

    if (xferCompFlag)
    {
        status = 1;
        xferCompFlag = 0;
    }

    if (dataTimeout)
    {
        status = 0;
        dataTimeout = 0;
    }

    /* Also, poll for the callback */
    if (HWREG(ctrl->memBase + MMCHS_CMD) & MMCHS_CMD_DP)
    {
        while(SD_callbackOccured == 0 && ((timeOut--) != 0));
        SD_callbackOccured = 0;

        if(timeOut == 0)
        {
            status = 0;
        }
    }

    ctrlInfo.dmaEnable = 0;

    return status;
}

/******************************************************************************/
/* SD_HSMMCSDRxDmaConfig
 *
 * Configures the DMA for SD card receive.
 *                                                                            */
/******************************************************************************/
void SD_HSMMCSDRxDmaConfig(void *ptr, unsigned int blkSize, unsigned int nblks)
{
    EDMA3CCPaRAMEntry paramSet;

    paramSet.srcAddr    = ctrlInfo.memBase + MMCHS_DATA;
    paramSet.destAddr   = (unsigned int)ptr;
    paramSet.srcBIdx    = 0;
    paramSet.srcCIdx    = 0;
    paramSet.destBIdx   = 4;
    paramSet.destCIdx   = (unsigned short)blkSize;
    paramSet.aCnt       = 0x4;
    paramSet.bCnt       = (unsigned short)blkSize/4;
    paramSet.cCnt       = (unsigned short)nblks;
    paramSet.bCntReload = 0x0;
    paramSet.linkAddr   = 0xffff;
    paramSet.opt        = 0;

    /* Set OPT */
    paramSet.opt |= ((MMCSD_RX_EDMA_CHAN << EDMA3CC_OPT_TCC_SHIFT) & EDMA3CC_OPT_TCC);

    /* 1. Transmission complition interrupt enable */
    paramSet.opt |= (1 << EDMA3CC_OPT_TCINTEN_SHIFT);

    /* 2. Read FIFO : SRC Constant addr mode */
    paramSet.opt |= (1 << 0);

    /* 3. SRC FIFO width is 32 bit */
    paramSet.opt |= (2 << 8);

    /* 4.  AB-Sync mode */
    paramSet.opt |= (1 << 2);

    /* configure PaRAM Set */
    EDMA3SetPaRAM(EDMA_INST_BASE, MMCSD_RX_EDMA_CHAN, &paramSet);

    /* Enable the transfer */
    EDMA3EnableTransfer(EDMA_INST_BASE, MMCSD_RX_EDMA_CHAN, EDMA3_TRIG_MODE_EVENT);
}

/******************************************************************************/
/* SD_HSMMCSDTxDmaConfig
 *
 * Configures the DMA for SD card transmit.
 *                                                                            */
/******************************************************************************/
void SD_HSMMCSDTxDmaConfig(void *ptr, unsigned int blkSize, unsigned int blks)
{
    EDMA3CCPaRAMEntry paramSet;

    paramSet.srcAddr    = (unsigned int)ptr;
    paramSet.destAddr   = ctrlInfo.memBase + MMCHS_DATA;
    paramSet.srcBIdx    = 4;
    paramSet.srcCIdx    = blkSize;
    paramSet.destBIdx   = 0;
    paramSet.destCIdx   = 0;
    paramSet.aCnt       = 0x4;
    paramSet.bCnt       = (unsigned short)blkSize/4;
    paramSet.cCnt       = (unsigned short)blks;
    paramSet.bCntReload = 0x0;
    paramSet.linkAddr   = 0xffff;
    paramSet.opt        = 0;

    /* Set OPT */
    paramSet.opt |= ((MMCSD_TX_EDMA_CHAN << EDMA3CC_OPT_TCC_SHIFT) & EDMA3CC_OPT_TCC);

    /* 1. Transmission complition interrupt enable */
    paramSet.opt |= (1 << EDMA3CC_OPT_TCINTEN_SHIFT);

    /* 2. Read FIFO : DST Constant addr mode */
    paramSet.opt |= (1 << 1);

    /* 3. DST FIFO width is 32 bit */
    paramSet.opt |= (2 << 8);

    /* 4.  AB-Sync mode */
    paramSet.opt |= (1 << 2);

    /* configure PaRAM Set */
    EDMA3SetPaRAM(EDMA_INST_BASE, MMCSD_TX_EDMA_CHAN, &paramSet);

    /* Enable the transfer */
    EDMA3EnableTransfer(EDMA_INST_BASE, MMCSD_TX_EDMA_CHAN, EDMA3_TRIG_MODE_EVENT);
}

/******************************************************************************/
/* SD_HSMMCSDXferSetup
 *
 * Configures the DMA for SD card for transmit and receive.
 *                                                                            */
/******************************************************************************/
void SD_HSMMCSDXferSetup(mmcsdCtrlInfo *ctrl, unsigned char rwFlag, void *ptr, unsigned int blkSize, unsigned int nBlks)
{
	SD_callbackOccured = 0;
    xferCompFlag = 0;

    if (rwFlag == 1)
    {
    	SD_HSMMCSDRxDmaConfig(ptr, blkSize, nBlks);
    }
    else
    {
        SD_HSMMCSDTxDmaConfig(ptr, blkSize, nBlks);
    }

    ctrl->dmaEnable = 1;
    HSMMCSDBlkLenSet(ctrl->memBase, blkSize);
}

/******************************************************************************/
/* SD_DMA_Callback
 *
 * Function is used as a callback from EDMA3 Completion Handler.
 *                                                                            */
/******************************************************************************/
void SD_DMA_Callback(unsigned int tccNum, unsigned int status)
{
    SD_callbackOccured = 1;
    EDMA3DisableTransfer(EDMA_INST_BASE, tccNum, EDMA3_TRIG_MODE_EVENT);
}

/******************************************************************************/
/* SD_DMA_ConfigureInterrupt
 *
 * Configures SD card interrupts.
 *                                                                            */
/******************************************************************************/
void SD_DMA_ConfigureInterrupt(void)
{
    /* Registering EDMA3 Channel Controller transfer completion interrupt.  */
    IntRegister(EDMA_COMPLTN_INT_NUM, SD_DMA_ISR_EDMA3Complete);

    /* Setting the priority for EDMA3CC completion interrupt in AINTC. */
    IntPrioritySet(EDMA_COMPLTN_INT_NUM, DMA_COMPLETE_INTERRUPT_PRIORITY, AINTC_HOSTINT_ROUTE_IRQ);

    /* Registering EDMA3 Channel Controller Error Interrupt. */
    IntRegister(EDMA_ERROR_INT_NUM, SD_DMA_ISR_EDMA3Error);

    /* Setting the priority for EDMA3CC Error interrupt in AINTC. */
    IntPrioritySet(EDMA_ERROR_INT_NUM, DMA_ERROR_INTERRUPT_PRIORITY, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the EDMA3CC completion interrupt in AINTC. */
    IntSystemEnable(EDMA_COMPLTN_INT_NUM);

    /* Enabling the EDMA3CC Error interrupt in AINTC. */
    IntSystemEnable(EDMA_ERROR_INT_NUM);

    /* Registering HSMMC Interrupt handler */
    IntRegister(MMCSD_INT_NUM, SD_0_ISR);

    /* Setting the priority for EDMA3CC completion interrupt in AINTC. */
    IntPrioritySet(MMCSD_INT_NUM, SD_INTERRUPT_PRIORITY, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the HSMMC interrupt in AINTC. */
    IntSystemEnable(MMCSD_INT_NUM);

}

/******************************************************************************/
/* SD_DMA_Init
 *
 * Initializes the DMA for the SD card.
 *                                                                            */
/******************************************************************************/
void SD_DMA_Init(void)
{
    /* Initialization of EDMA3 */
    EDMA3Init(EDMA_INST_BASE, EVT_QUEUE_NUM);

    /* Configuring the AINTC to receive EDMA3 interrupts. */
    SD_DMA_ConfigureInterrupt();
}

/******************************************************************************/
/* SD_DMA_HSMMCSD
 *
 * Initializes the DMA for the SD card.
 *                                                                            */
/******************************************************************************/
void SD_DMA_HSMMCSD(void)
{
    /* Initializing the EDMA. */
	SD_DMA_Init();

    /* Request DMA Channel and TCC for MMCSD Transmit*/
    EDMA3RequestChannel(EDMA_INST_BASE, EDMA3_CHANNEL_TYPE_DMA,
                        MMCSD_TX_EDMA_CHAN, MMCSD_TX_EDMA_CHAN,
                        EVT_QUEUE_NUM);

    /* Registering Callback Function for TX*/
    cb_Fxn[MMCSD_TX_EDMA_CHAN] = &SD_DMA_Callback;

    /* Request DMA Channel and TCC for MMCSD Receive */
    EDMA3RequestChannel(EDMA_INST_BASE, EDMA3_CHANNEL_TYPE_DMA,
                        MMCSD_RX_EDMA_CHAN, MMCSD_RX_EDMA_CHAN,
                        EVT_QUEUE_NUM);

    /* Registering Callback Function for RX*/
    cb_Fxn[MMCSD_RX_EDMA_CHAN] = &SD_DMA_Callback;
}

/******************************************************************************/
/* SD_HSMMCSDControllerSetup
 *
 * Sets up the variables for SD card interface.
 *                                                                            */
/******************************************************************************/
void SD_HSMMCSDControllerSetup(void)
{
    ctrlInfo.memBase = MMCSD_INST_BASE;
    ctrlInfo.ctrlInit = HSMMCSDControllerInit;
    ctrlInfo.xferSetup = SD_HSMMCSDXferSetup;
    ctrlInfo.cmdStatusGet = SD_HSMMCSDCmdStatusGet;
    ctrlInfo.xferStatusGet = SD_HSMMCSDXferStatusGet;
    /* Use the funciton HSMMCSDCDPinStatusGet() to use the card presence
       using the controller.
    */
    ctrlInfo.cardPresent = HSMMCSDCardPresent;
    ctrlInfo.cmdSend = HSMMCSDCmdSend;
    ctrlInfo.busWidthConfig = HSMMCSDBusWidthConfig;
    ctrlInfo.busFreqConfig = HSMMCSDBusFreqConfig;
    ctrlInfo.intrMask = (HS_MMCSD_INTR_CMDCOMP | HS_MMCSD_INTR_CMDTIMEOUT |
                            HS_MMCSD_INTR_DATATIMEOUT | HS_MMCSD_INTR_TRNFCOMP);
    ctrlInfo.intrEnable = HSMMCSDIntEnable;
    ctrlInfo.busWidth = (SD_BUS_WIDTH_1BIT | SD_BUS_WIDTH_4BIT);
    ctrlInfo.highspeed = 1;
    ctrlInfo.ocr = (SD_OCR_VDD_3P0_3P1 | SD_OCR_VDD_3P1_3P2);
    ctrlInfo.card = &sdCard;
    ctrlInfo.ipClk = HSMMCSD_IN_FREQ;
    ctrlInfo.opClk = HSMMCSD_INIT_FREQ;
    ctrlInfo.cdPinNum = HSMMCSD_CARD_DETECT_PINNUM;
    sdCard.ctrl = &ctrlInfo;

    SD_callbackOccured = 0;
    xferCompFlag = 0;
    dataTimeout = 0;
    cmdCompFlag = 0;
    cmdTimeout = 0;
}

/******************************************************************************/
/* SD_HSMMCSDFsMount
 *
 * Mounts the SD card file system.
 *                                                                            */
/******************************************************************************/
void SD_HSMMCSDFsMount(unsigned int driveNum, void *ptr)
{
	SD_SetInitialized();
    f_mount(&g_sFatFs, "", driveNum); // f_mount(driveNum, &g_sFatFs);
    fat_devices[driveNum].dev = ptr;
    fat_devices[driveNum].fs = &g_sFatFs;
    fat_devices[driveNum].initDone = 0;
}

/******************************************************************************/
/* SD_SetCardActionFlag
 *
 * Sets the flag which states that a card was removed or inserted.
 *                                                                            */
/******************************************************************************/
void SD_SetCardActionFlag(void)
{
	SD_CardActionFlag = TRUE;
}

/******************************************************************************/
/* SD_ClearCardActionFlag
 *
 * Clears the flag which states that a card was removed or inserted.
 *                                                                            */
/******************************************************************************/
void SD_ClearCardActionFlag(void)
{
	SD_CardActionFlag = FALSE;
}

/******************************************************************************/
/* SD_GetCardActionFlag
 *
 * Gets the flag which states that a card was removed or inserted.
 *                                                                            */
/******************************************************************************/
unsigned char SD_GetCardActionFlag(void)
{
	return SD_CardActionFlag;
}

/******************************************************************************/
/* SD_SetCardStatus
 *
 * Sets the SD card state.
 *                                                                            */
/******************************************************************************/
void SD_SetCardStatus(ENUM_SD_CARD_STATE state)
{
	SD_CardState = state;
}

/******************************************************************************/
/* SD_GetCardStatus
 *
 * Gets the SD card state.
 *                                                                            */
/******************************************************************************/
ENUM_SD_CARD_STATE SD_GetCardStatus(void)
{
	return SD_CardState;
}

/******************************************************************************/
/* SD_SetInitialized
 *
 * Sets the SD initization flag.
 *                                                                            */
/******************************************************************************/
void SD_SetInitialized(void)
{
	SD_CardInitialized = TRUE;
}

/******************************************************************************/
/* SD_ClearInitialized
 *
 * Clears the SD initization flag.
 *                                                                            */
/******************************************************************************/
void SD_ClearInitialized(void)
{
	SD_CardInitialized = FALSE;
}

/******************************************************************************/
/* SD_GetCardStatus
 *
 * Gets the SD card state.
 *                                                                            */
/******************************************************************************/
unsigned char SD_IsInitialized(void)
{
	return SD_CardInitialized;
}

/******************************* End of file *********************************/
