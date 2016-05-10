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
#include "LEDS.h"
#include "SD.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
/* EDMA callback function array */
static void (*cb_Fxn[EDMA3_NUM_TCC]) (unsigned int tcc, unsigned int status);

/*****************************************************************************
This buffer holds the full path to the current working directory.  Initially
it is root ("/").
******************************************************************************/
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";
static char g_cCmdBuf[512];

/*****************************************************************************
A temporary data buffer used for write file paths
******************************************************************************/
static char g_cWrBuf[PATH_BUF_SIZE] = "/";


/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
volatile unsigned int sdBlkSize = HSMMCSD_BLK_SIZE;
volatile unsigned int callbackOccured = 0;
volatile unsigned int xferCompFlag = 0;
volatile unsigned int dataTimeout = 0;
volatile unsigned int cmdCompFlag = 0;
volatile unsigned int cmdTimeout = 0;
volatile unsigned int errFlag = 0;
mmcsdCtrlInfo  ctrlInfo;
mmcsdCardInfo sdCard;
fatDevice fat_devices[DRIVE_NUM_MAX];
volatile unsigned int g_sPState = 0;
volatile unsigned int g_sCState = 0;

#pragma DATA_ALIGN(g_sFatFs, SOC_CACHELINE_SIZE);
static FATFS g_sFatFs;

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
    HSMMCSDEdmaInit();

    /* Perform pin-mux for HSMMCSD pins. */
    HSMMCSDPinMuxSetup();

    /* Enable module clock for HSMMCSD. */
    HSMMCSDModuleClkConfig();

    /* Basic controller initializations */
    HSMMCSDControllerSetup();

    /* Initialize the MMCSD controller */
    MMCSDCtrlInit(&ctrlInfo);

    MMCSDIntEnable(&ctrlInfo);
}

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE bValue)
{
	unsigned int status;

	if (DRIVE_NUM_MAX <= bValue)
	{
		return STA_NODISK;
	}


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
#define DEBUG 0
#if DEBUG
			if (card->cardType == MMCSD_CARD_SD)
			{
				UARTPuts("\r\nSD Card ", 9U);
				UARTPuts("version : ",11U);
				UARTPutNum(card->sd_ver);

				if (card->highCap)
				{
					UARTPuts(", High Capacity", 20U);
				}

				if (card->tranSpeed == SD_TRANSPEED_50MBPS)
				{
					UARTPuts(", High Speed", 15U);
				}
			}
			else if (card->cardType == MMCSD_CARD_MMC)
			{
				UARTPuts("\r\nMMC Card ", 10U);
			}
#endif
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

/*-----------------------------------------------------------------------*/
/* This function reads sector(s) from the disk drive                     */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE drv, BYTE* buff, DWORD sector, BYTE count)
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



/*-----------------------------------------------------------------------*/
/* This function writes sector(s) to the disk drive                     */
/*-----------------------------------------------------------------------*/
DRESULT disk_write (BYTE ucDrive, const BYTE* buff, DWORD sector, BYTE count)
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

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl ( BYTE drv, BYTE ctrl, void *buff)
{
	return RES_OK;
}

/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */

DWORD get_fattime (void)
{
    return    ((2007UL-1980) << 25) // Year = 2007
            | (6UL << 21)           // Month = June
            | (5UL << 16)           // Day = 5
            | (11U << 11)           // Hour = 11
            | (38U << 5)            // Min = 38
            | (0U >> 1)             // Sec = 0
            ;
}


/*
 * Check command status
 */

unsigned int HSMMCSDCmdStatusGet(mmcsdCtrlInfo *ctrl)
{
    unsigned int status = 0;

    while ((cmdCompFlag == 0) && (cmdTimeout == 0));

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

unsigned int HSMMCSDXferStatusGet(mmcsdCtrlInfo *ctrl)
{
    unsigned int status = 0;
    volatile unsigned int timeOut = 0xFFFF;

    while ((xferCompFlag == 0) && (dataTimeout == 0));

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
        while(callbackOccured == 0 && ((timeOut--) != 0));
        callbackOccured = 0;

        if(timeOut == 0)
        {
            status = 0;
        }
    }

    ctrlInfo.dmaEnable = 0;

    return status;
}

void HSMMCSDRxDmaConfig(void *ptr, unsigned int blkSize, unsigned int nblks)
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

void HSMMCSDTxDmaConfig(void *ptr, unsigned int blkSize, unsigned int blks)
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

void HSMMCSDXferSetup(mmcsdCtrlInfo *ctrl, unsigned char rwFlag, void *ptr, unsigned int blkSize, unsigned int nBlks)
{
    callbackOccured = 0;
    xferCompFlag = 0;

    if (rwFlag == 1)
    {
        HSMMCSDRxDmaConfig(ptr, blkSize, nBlks);
    }
    else
    {
        HSMMCSDTxDmaConfig(ptr, blkSize, nBlks);
    }

    ctrl->dmaEnable = 1;
    HSMMCSDBlkLenSet(ctrl->memBase, blkSize);
}


/*
** This function is used as a callback from EDMA3 Completion Handler.
*/
void callback(unsigned int tccNum, unsigned int status)
{
    callbackOccured = 1;
    EDMA3DisableTransfer(EDMA_INST_BASE, tccNum, EDMA3_TRIG_MODE_EVENT);
}

void Edma3CompletionIsr(void)
{
    volatile unsigned int pendingIrqs;
    volatile unsigned int isIPR = 0;

    unsigned int indexl;
    unsigned int Cnt = 0;

    indexl = 1;

    isIPR = EDMA3GetIntrStatus(EDMA_INST_BASE);

    if(isIPR)
    {
        while ((Cnt < EDMA3CC_COMPL_HANDLER_RETRY_COUNT)&& (indexl != 0u))
        {
            indexl = 0u;
            pendingIrqs = EDMA3GetIntrStatus(EDMA_INST_BASE);

            while (pendingIrqs)
            {
                if((pendingIrqs & 1u) == TRUE)
                {
                    /**
                    * If the user has not given any callback function
                    * while requesting the TCC, its TCC specific bit
                    * in the IPR register will NOT be cleared.
                    */
                    /* here write to ICR to clear the corresponding IPR bits */

                    EDMA3ClrIntr(EDMA_INST_BASE, indexl);

                    if (cb_Fxn[indexl] != NULL)
                    {
                        (*cb_Fxn[indexl])(indexl, EDMA3_XFER_COMPLETE);
                    }
                }
                ++indexl;
                pendingIrqs >>= 1u;
            }
            Cnt++;
        }
    }
}

void Edma3CCErrorIsr(void)
{
    volatile unsigned int pendingIrqs;
    volatile unsigned int evtqueNum = 0;  /* Event Queue Num */
    volatile unsigned int isIPRH = 0;
    volatile unsigned int isIPR = 0;
    volatile unsigned int Cnt = 0u;
    volatile unsigned int index;

    pendingIrqs = 0u;
    index = 1u;

    isIPR  = EDMA3GetIntrStatus(EDMA_INST_BASE);
    isIPRH = EDMA3IntrStatusHighGet(EDMA_INST_BASE);

    if((isIPR | isIPRH ) || (EDMA3QdmaGetErrIntrStatus(EDMA_INST_BASE) != 0)
        || (EDMA3GetCCErrStatus(EDMA_INST_BASE) != 0))
    {
        /* Loop for EDMA3CC_ERR_HANDLER_RETRY_COUNT number of time,
         * breaks when no pending interrupt is found
         */
        while ((Cnt < EDMA3CC_ERR_HANDLER_RETRY_COUNT)
                    && (index != 0u))
        {
            index = 0u;

            if(isIPR)
            {
                   pendingIrqs = EDMA3GetErrIntrStatus(EDMA_INST_BASE);
            }
            else
            {
                   pendingIrqs = EDMA3ErrIntrHighStatusGet(EDMA_INST_BASE);
            }

            while (pendingIrqs)
            {
                   /*Process all the pending interrupts*/
                   if(TRUE == (pendingIrqs & 1u))
                   {
                      /* Write to EMCR to clear the corresponding EMR bits.
                       */
                        /*Clear any SER*/

                        if(isIPR)
                        {
                             EDMA3ClrMissEvt(EDMA_INST_BASE, index);
                        }
                        else
                        {
                             EDMA3ClrMissEvt(EDMA_INST_BASE, index + 32);
                        }
                   }
                   ++index;
                   pendingIrqs >>= 1u;
            }
            index = 0u;
            pendingIrqs = EDMA3QdmaGetErrIntrStatus(EDMA_INST_BASE);
            while (pendingIrqs)
            {
                /*Process all the pending interrupts*/
                if(TRUE == (pendingIrqs & 1u))
                {
                    /* Here write to QEMCR to clear the corresponding QEMR bits*/
                    /*Clear any QSER*/
                    EDMA3QdmaClrMissEvt(EDMA_INST_BASE, index);
                }
                ++index;
                pendingIrqs >>= 1u;
            }
            index = 0u;


            pendingIrqs = EDMA3GetCCErrStatus(EDMA_INST_BASE);
            if (pendingIrqs != 0u)
            {
            /* Process all the pending CC error interrupts. */
            /* Queue threshold error for different event queues.*/
            for (evtqueNum = 0u; evtqueNum < SOC_EDMA3_NUM_EVQUE; evtqueNum++)
                {
                if((pendingIrqs & (1u << evtqueNum)) != 0u)
                {
                        /* Clear the error interrupt. */
                        EDMA3ClrCCErr(EDMA_INST_BASE, (1u << evtqueNum));
                    }
                }

            /* Transfer completion code error. */
            if ((pendingIrqs & (1 << EDMA3CC_CCERR_TCCERR_SHIFT)) != 0u)
            {
                EDMA3ClrCCErr(EDMA_INST_BASE,
                                      (0x01u << EDMA3CC_CCERR_TCCERR_SHIFT));
            }
                ++index;
            }
            Cnt++;
        }
    }

}

void HSMMCSDIsr(void)
{
    volatile unsigned int status = 0;

    status = HSMMCSDIntrStatusGet(ctrlInfo.memBase, 0xFFFFFFFF);

    HSMMCSDIntrStatusClear(ctrlInfo.memBase, status);

    if (status & HS_MMCSD_STAT_CMDCOMP)
    {
        cmdCompFlag = 1;
    }

    if (status & HS_MMCSD_STAT_ERR)
    {
        errFlag = status & 0xFFFF0000;

        if (status & HS_MMCSD_STAT_CMDTIMEOUT)
        {
            cmdTimeout = 1;
        }

        if (status & HS_MMCSD_STAT_DATATIMEOUT)
        {
            dataTimeout = 1;
        }
    }

    if (status & HS_MMCSD_STAT_TRNFCOMP)
    {
        xferCompFlag = 1;
    }
}


/*
** This function configures the AINTC to receive EDMA3 interrupts.
*/
void EDMA3AINTCConfigure(void)
{
    /* Initializing the ARM Interrupt Controller. */
    IntAINTCInit();

    /* Registering EDMA3 Channel Controller transfer completion interrupt.  */
    IntRegister(EDMA_COMPLTN_INT_NUM, Edma3CompletionIsr);

    /* Setting the priority for EDMA3CC completion interrupt in AINTC. */
    IntPrioritySet(EDMA_COMPLTN_INT_NUM, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Registering EDMA3 Channel Controller Error Interrupt. */
    IntRegister(EDMA_ERROR_INT_NUM, Edma3CCErrorIsr);

    /* Setting the priority for EDMA3CC Error interrupt in AINTC. */
    IntPrioritySet(EDMA_ERROR_INT_NUM, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the EDMA3CC completion interrupt in AINTC. */
    IntSystemEnable(EDMA_COMPLTN_INT_NUM);

    /* Enabling the EDMA3CC Error interrupt in AINTC. */
    IntSystemEnable(EDMA_ERROR_INT_NUM);

    /* Registering HSMMC Interrupt handler */
    IntRegister(MMCSD_INT_NUM, HSMMCSDIsr);

    /* Setting the priority for EDMA3CC completion interrupt in AINTC. */
    IntPrioritySet(MMCSD_INT_NUM, 0, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the HSMMC interrupt in AINTC. */
    IntSystemEnable(MMCSD_INT_NUM);

    /* Enabling IRQ in CPSR of ARM processor. */
    IntMasterIRQEnable();
}


/*
** Powering up, initializing and registering interrupts for EDMA.
*/

void EDMA3Initialize(void)
{
    /* Initialization of EDMA3 */
    EDMA3Init(EDMA_INST_BASE, EVT_QUEUE_NUM);

    /* Configuring the AINTC to receive EDMA3 interrupts. */
    EDMA3AINTCConfigure();
}

void HSMMCSDEdmaInit(void)
{
    /* Initializing the EDMA. */
    EDMA3Initialize();

    /* Request DMA Channel and TCC for MMCSD Transmit*/
    EDMA3RequestChannel(EDMA_INST_BASE, EDMA3_CHANNEL_TYPE_DMA,
                        MMCSD_TX_EDMA_CHAN, MMCSD_TX_EDMA_CHAN,
                        EVT_QUEUE_NUM);

    /* Registering Callback Function for TX*/
    cb_Fxn[MMCSD_TX_EDMA_CHAN] = &callback;

    /* Request DMA Channel and TCC for MMCSD Receive */
    EDMA3RequestChannel(EDMA_INST_BASE, EDMA3_CHANNEL_TYPE_DMA,
                        MMCSD_RX_EDMA_CHAN, MMCSD_RX_EDMA_CHAN,
                        EVT_QUEUE_NUM);

    /* Registering Callback Function for RX*/
    cb_Fxn[MMCSD_RX_EDMA_CHAN] = &callback;
}

/*
** Initialize the MMCSD controller structure for use
*/
void HSMMCSDControllerSetup(void)
{
    ctrlInfo.memBase = MMCSD_INST_BASE;
    ctrlInfo.ctrlInit = HSMMCSDControllerInit;
    ctrlInfo.xferSetup = HSMMCSDXferSetup;
    ctrlInfo.cmdStatusGet = HSMMCSDCmdStatusGet;
    ctrlInfo.xferStatusGet = HSMMCSDXferStatusGet;
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

    callbackOccured = 0;
    xferCompFlag = 0;
    dataTimeout = 0;
    cmdCompFlag = 0;
    cmdTimeout = 0;
}

void HSMMCSDFsMount(unsigned int driveNum, void *ptr)
{
    strcpy(g_cCwdBuf, "/");
    strcpy(g_cCmdBuf, "\0");
    g_sPState = 0;
    g_sCState = 0;
    f_mount(&g_sFatFs, "", driveNum); // f_mount(driveNum, &g_sFatFs);
    fat_devices[driveNum].dev = ptr;
    fat_devices[driveNum].fs = &g_sFatFs;
    fat_devices[driveNum].initDone = 0;
}

/*-----------------------------------------------------------------------*/
/* Returns the current status of a drive                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (BYTE drv)
{
	return 0;
}

/******************************* End of file *********************************/
