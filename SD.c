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
#include "hw_hs_mmcsd.h"
#include "interrupt.h"
#include "mmcsd_proto.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "GPIO.h"
#include "INTERRUPTS.h"
#include "LEDS.h"
#include "RTCC.h"
#include "SD.h"
#include "UART.h"

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
#pragma DATA_ALIGN(g_sFatFs, SOC_CACHELINE_SIZE);
#pragma DATA_SECTION(g_sFatFs, ".l3_memory");
static FATFS g_sFatFs ;

TYPE_FAT_DEVICE fat_devices[DRIVE_NUM_MAX];
mmcsdCardInfo sdCard;

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
    /* Perform pin-mux for HSMMCSD pins. */
    HSMMCSDPinMuxSetup();

    /* Enable module clock for HSMMCSD. */
    HSMMCSDModuleClkConfig();

    /* Initialize the MMCSD controller */
    SD_SetUpController(SOC_MMCHS_0_REGS);

    SD_CardInit();

    SD_SetCardActionFlag();
}

/******************************************************************************/
/* SD_HSMMCSDControllerSetup
 *
 * Sets up the variables for SD card interface.
 *                                                                            */
/******************************************************************************/
void SD_HSMMCSDControllerSetup(void)
{
	/*
    ctrlInfo.memBase = MMCSD_INST_BASE;
    ctrlInfo.ctrlInit = HSMMCSDControllerInit;
    ctrlInfo.xferSetup = SD_HSMMCSDXferSetup;
    ctrlInfo.cmdStatusGet = SD_HSMMCSDCmdStatusGet;
    ctrlInfo.xferStatusGet = SD_HSMMCSDXferStatusGet;
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
    */
}

/******************************************************************************/
/* SD_SetUpController
 *
 * Sets up the SD card controller
 *                                                                            */
/******************************************************************************/
unsigned int SD_SetUpController(unsigned int baseAddr)
{
    int status = 0;

    /*Refer to the MMC Host and Bus configuration steps in TRM */
    /* controller Reset */
    status = HSMMCSDSoftReset(baseAddr);

    if (status != 0)
    {
    	UART_PrintString("HS MMC/SD Reset failed\n\r");
    }

    /* Lines Reset */
    HSMMCSDLinesReset(baseAddr, HS_MMCSD_ALL_RESET);

    /* Set supported voltage list */
    HSMMCSDSupportedVoltSet(baseAddr, HS_MMCSD_SUPPORT_VOLT_1P8 |
                                                HS_MMCSD_SUPPORT_VOLT_3P0);

    HSMMCSDSystemConfig(baseAddr, HS_MMCSD_AUTOIDLE_ENABLE);

    /* Set the bus width */
    HSMMCSDBusWidthSet(baseAddr, HS_MMCSD_BUS_WIDTH_1BIT );

    /* Set the bus voltage */
    HSMMCSDBusVoltSet(baseAddr, HS_MMCSD_BUS_VOLT_3P0);

    /* Bus power on */
    status = HSMMCSDBusPower(baseAddr, HS_MMCSD_BUS_POWER_ON);

    if (status != 0)
    {
    	UART_PrintString("HS MMC/SD Power on failed\n\r");
    }

    /* Set the initialization frequency */
    status = HSMMCSDBusFreqSet(baseAddr, 96000000, 400000, 0);
    if (status != 0)
    {
    	UART_PrintString("HS MMC/SD Bus Frequency set failed\n\r");
    }

    status = HSMMCSDInitStreamSend(baseAddr);

    status = (status == 0) ? 1 : 0;

    return status;
}

/******************************************************************************/
/* SD_IsCardInserted
 *
 * Returns TRUE if there is an SD card inserted.
 *                                                                            */
/******************************************************************************/
unsigned char SD_IsCardInserted(void)
{
	if(HSMMCSDIsCardInserted(SOC_MMCHS_0_REGS) == 1)
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

}

/******************************************************************************/
/* SD_DiskInitialize
 *
 * Initializes the SD disk
 *                                                                            */
/******************************************************************************/
unsigned char SD_DiskInitialize(unsigned char bValue)
{

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

unsigned int SD_CardInit(void)
{
	unsigned int response[4];
	unsigned char status = 0;
	unsigned int OCR;
	unsigned int HighCap;
	unsigned char CID[16];
	unsigned int retry = 0xFFFF;
	unsigned int RCA;
	unsigned char CSD[16];
	unsigned int TransSpeed;
	unsigned int BlockLength;
	unsigned int Size;
	unsigned int NumberBlocks;

	/* CMD0 - reset card */
	status = SD_SendCommand(SOC_MMCHS_0_REGS, 0, 0, SD_RESPONSE_NONE, &response);

	if (status == 0)
	{
		return 0;
	}

	/* ACMD55 - find out if SD or MMC */
	status = SD_SendAppCommand(SOC_MMCHS_0_REGS, 55, 0, SD_RESPONSE_48BITS, response);

	if (status == 1)
	/* SD Card */
	{
		//ctrl->card->cardType = MMCSD_CARD_SD;

		/* CMD0 - reset card */
		status = SD_SendCommand(SOC_MMCHS_0_REGS, 0, 0, SD_RESPONSE_NONE, response);

		if (status == 0)
		{
			return 0;
		}

		/* CMD8 - send oper voltage */
		status = SD_SendCommand(SOC_MMCHS_0_REGS, 8, SD_CHECK_PATTERN | SD_VOLT_2P7_3P6, SD_RESPONSE_48BITS, response);

		if (status == 0)
		{
			/* If the cmd fails, it can be due to version < 2.0, since
			 * we are currently supporting high voltage cards only
			 */
		}

		/* Go ahead and send ACMD41, with host capabilities */
		status = SD_SendAppCommand(SOC_MMCHS_0_REGS, 41, SD_OCR_HIGH_CAPACITY | SD_OCR_VDD_WILDCARD, SD_RESPONSE_48BITS, &response);

		if (status == 0)
		{
			return 0;
		}

		/* Poll until we get the card status (BIT31 of OCR) is powered up */
		do
		{
				status = SD_SendAppCommand(SOC_MMCHS_0_REGS, 41, SD_OCR_HIGH_CAPACITY | SD_OCR_VDD_WILDCARD, SD_RESPONSE_48BITS, &response);
		}
		while (!(response[0] & ((unsigned int)BIT(31))) && retry--);

		if (retry == 0)
		{
			/* No point in continuing */
			return 0;
		}

		OCR = response[0];

		HighCap = (OCR & SD_OCR_HIGH_CAPACITY) ? 1 : 0;

		/* Send CMD2, to get the card identification register */
		status = SD_SendCommand(SOC_MMCHS_0_REGS, 2, 0, SD_RESPONSE_136BITS, response);

		memcpy(CID, response, 16);

		if (status == 0)
		{
			return 0;
		}

		/* Send CMD3, to get the card relative address */
		status = SD_SendCommand(SOC_MMCHS_0_REGS, 3, 0, SD_RESPONSE_48BITS, response);

		RCA = SD_RCA_ADDR(response[0]);

		if (status == 0)
		{
			return 0;
		}

        /* Send CMD9, to get the card specific data */
		status = SD_SendCommand(SOC_MMCHS_0_REGS, 9, RCA << 16, SD_RESPONSE_136BITS, response);

        memcpy(CSD, response, 16);

        if (status == 0)
        {
            return 0;
        }

        if ((CSD[3] & 0xC0000000) >> 30)
        {
            TransSpeed = SD_CSD1_TRANSPEED(CSD[3], CSD[2], CSD[1], CSD[0]);
            BlockLength = 1 << (SD_CSD1_RDBLKLEN(CSD[3], CSD[2], CSD[1], CSD[0]));
            Size = SD_CSD1_DEV_SIZE(CSD[3], CSD[2], CSD[1], CSD[0]);
            NumberBlocks = Size / BlockLength;
        }
        else
        {
        	TransSpeed = SD_CSD0_TRANSPEED(CSD[3], CSD[2], CSD[1], CSD[0]);
			BlockLength = 1 << (SD_CSD0_RDBLKLEN(CSD[3], CSD[2], CSD[1], CSD[0]));
			Size = SD_CSD0_DEV_SIZE(CSD[3], CSD[2], CSD[1], CSD[0]);
			NumberBlocks = Size / BlockLength;
        }

        /* Set data block length to 512 (for byte addressing cards) */
        if(!(HighCap))
        {
        	status = SD_SendCommand(SOC_MMCHS_0_REGS, 16, 512, SD_RESPONSE_NONE, response);

            if (status == 0)
            {
                return 0;
            }
            else
            {
            	BlockLength = 512;
            }
        }

        /* Select the card */
        status = SD_SendCommand(SOC_MMCHS_0_REGS, 7, RCA << 16, SD_RESPONSE_BUSY, response);

        if (status == 0)
        {
            return 0;
        }
#if 0
        /*
         * Send ACMD51, to get the SD Configuration register details.
         * Note, this needs data transfer (on data lines).
         */
        status = SD_SendCommand(SOC_MMCHS_0_REGS, 55, RCA << 16, SD_RESPONSE_48BITS, response);

        if (status == 0)
        {
            return 0;
        }

        ctrl->xferSetup(ctrl, 1, dataBuffer, 8, 1);

        cmd.idx = SD_CMD(51);
        cmd.flags = SD_CMDRSP_READ | SD_CMDRSP_DATA;
        cmd.arg = card->rca << 16;
        cmd.nblks = 1;
        cmd.data = (signed char*)dataBuffer;

        status = MMCSDCmdSend(ctrl,&cmd);
        if (status == 0)
        {
            return 0;
        }

        status = ctrl->xferStatusGet(ctrl);

        if (status == 0)
        {
            return 0;
        }

        /* Invalidate the data cache. */
        card->raw_scr[0] = (dataBuffer[3] << 24) | (dataBuffer[2] << 16) | \
		                   (dataBuffer[1] << 8) | (dataBuffer[0]);
        card->raw_scr[1] = (dataBuffer[7] << 24) | (dataBuffer[6] << 16) | \
                                   (dataBuffer[5] << 8) | (dataBuffer[4]);

        card->sd_ver = SD_CARD_VERSION(card);
        card->busWidth = SD_CARD_BUSWIDTH(card);
#endif
    }
    else
    {
        return 0;
    }

    return 1;
}

unsigned char SD_SendCommand(unsigned int baseAddr, unsigned int command, unsigned int argument, ENUM_SD_RESPONSE type, unsigned int* response)
{
		unsigned int cmdType = HS_MMCSD_CMD_TYPE_NORMAL;
	    unsigned int dataPresent;
	    unsigned int status = 0;
	    unsigned int rspType;
	    unsigned int cmdDir;
	    unsigned int nblks;
	    unsigned int cmd;

	    if (type & SD_RESPONSE_STOP)
	    {
	        cmdType = HS_MMCSD_CMD_TYPE_SUSPEND;
	    }
	    else if (type & SD_RESPONSE_FS)
	    {
	        cmdType = HS_MMCSD_CMD_TYPE_FUNCSEL;
	    }
	    else if (type & SD_RESPONSE_ABORT)
	    {
	        cmdType = HS_MMCSD_CMD_TYPE_ABORT;
	    }

	    cmdDir = (type & SD_RESPONSE_READ) ? \
	              HS_MMCSD_CMD_DIR_READ : HS_MMCSD_CMD_DIR_WRITE;

	    dataPresent = (type & SD_RESPONSE_DATA) ? 1 : 0;
	    nblks = (dataPresent == 1) ? 1 : 0;

	    if (type & SD_RESPONSE_NONE)
	    {
	        rspType = HS_MMCSD_NO_RESPONSE;
	    }
	    else if (type & SD_RESPONSE_136BITS)
	    {
	        rspType = HS_MMCSD_136BITS_RESPONSE;
	    }
	    else if (type & SD_RESPONSE_BUSY)
	    {
	        rspType = HS_MMCSD_48BITS_BUSY_RESPONSE;
	    }
	    else
	    {
	        rspType = HS_MMCSD_48BITS_RESPONSE;
	    }

	    cmd = HS_MMCSD_CMD(command, cmdType, rspType, cmdDir);

	    if (dataPresent)
	    {
	        HSMMCSDIntrStatusClear(baseAddr, HS_MMCSD_STAT_TRNFCOMP);

	        HSMMCSDDataTimeoutSet(baseAddr, HS_MMCSD_DATA_TIMEOUT(27));
	    }

	    /* enable command event flags */
	    HSMMCSDIntrStatusEnable(baseAddr, MMCHS_ISE_CTO_SIGEN);
	    HSMMCSDIntrStatusEnable(baseAddr, MMCHS_ISE_CC_SIGEN);

	    /* clear timeout */
	    HSMMCSDIntrStatusClear(baseAddr, MMCHS_STAT_CTO);

	    /* clear command complete */
	    HSMMCSDIntrStatusClear(baseAddr, MMCHS_STAT_CC);

	    HSMMCSDCommandSend(baseAddr, cmd, argument, (void*)dataPresent, nblks, 0);

	    while((!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_CTO)) && (!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_CC)) && (!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_CREM)));

	    if(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_CC)
	    {
	    	status = 1;
	    }

	    if (status == 1)
	    {
	        HSMMCSDResponseGet(baseAddr, response);
	    }

	    return status;
}

unsigned char SD_SendAppCommand(unsigned int baseAddr, unsigned int command, unsigned int argument, ENUM_SD_RESPONSE type, unsigned int* response)
{
	if(SD_SendCommand(baseAddr, 55, 0, SD_RESPONSE_48BITS, response))
	{
		if(SD_SendCommand(baseAddr, command, argument, type, response))
		{
			return PASS;
		}
	}
	return FAIL;
}


/******************************* End of file *********************************/
