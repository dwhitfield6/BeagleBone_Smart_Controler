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
static unsigned int response[4];
static unsigned int OCR;
static unsigned int HighCap;
static unsigned char CID[16];
static unsigned int RCA;
static unsigned int CSD[4];
static unsigned int TransSpeed;
static unsigned int BlockLength;
static unsigned long long Size;
static unsigned int NumberBlocks;
static unsigned int SCR[2];
static unsigned int SD_Version;
static unsigned int BusWidth;
static unsigned short BytesWritten;
static FRESULT Result;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned char SD_Buffer[SD_BUFFER_SIZE];

#pragma DATA_ALIGN(g_sFatFs, SOC_CACHELINE_SIZE);
FATFS g_sFatFs;

#pragma DATA_ALIGN(fileWrite, SOC_CACHELINE_SIZE);
static FIL fileWrite;

#pragma DATA_ALIGN(FileDataBuffer, SOC_CACHELINE_SIZE);
char FileDataBuffer[FILE_DATA_BUFFER_SIZE];

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

    memset(SD_Buffer, 0, sizeof(SD_Buffer));
    SD_SetCardActionFlag();

    if(SD_IsCardInserted())
	{
		SD_CardInit();
		Result = f_mount(0, &g_sFatFs);
		Result = f_open (&fileWrite, "Log5.txt", FA_WRITE | FA_CREATE_NEW | FA_OPEN_ALWAYS);
		sprintf(FileDataBuffer, "This is a test.");
		Result = f_write (&fileWrite, FileDataBuffer, strlen(FileDataBuffer), &BytesWritten);
		Result = f_close (&fileWrite);
	}
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
/* SD_IsInitialized
 *
 * Returns true if the Card is initialized.
 *                                                                            */
/******************************************************************************/
unsigned char SD_IsInitialized(void)
{
	return SD_CardInitialized;
}

/******************************************************************************/
/* SD_CardInit
 *
 * Initializes the SD card.
 *                                                                            */
/******************************************************************************/
unsigned int SD_CardInit(void)
{
	unsigned char status = 0;
	unsigned int retry = 0xFFFF;
	unsigned int speed;

	if(!SD_IsInitialized())
	{

		SD_SetUpController(SOC_MMCHS_0_REGS);

		/* CMD0 - reset card */
		status = SD_SendCommand(SOC_MMCHS_0_REGS, 0, 0, 0, 512, SD_RESPONSE_NONE, response);

		if (status == 0)
		{
			return 0;
		}

		/* ACMD55 - find out if SD or MMC */
		status = SD_SendAppCommand(SOC_MMCHS_0_REGS, 55, 0, 0, 512, SD_RESPONSE_48BITS, response);

		if (status == 1)
		/* SD Card */
		{
			/* CMD0 - reset card */
			status = SD_SendCommand(SOC_MMCHS_0_REGS, 0, 0, 0, 512, SD_RESPONSE_NONE, response);

			if (status == 0)
			{
				return 0;
			}

			/* CMD8 - send oper voltage */
			status = SD_SendCommand(SOC_MMCHS_0_REGS, 8, SD_CHECK_PATTERN | SD_VOLT_2P7_3P6, 0, 512, SD_RESPONSE_48BITS, response);

			if (status == 0)
			{
				/* If the cmd fails, it can be due to version < 2.0, since
				 * we are currently supporting high voltage cards only
				 */
			}

			/* Go ahead and send ACMD41, with host capabilities */
			status = SD_SendAppCommand(SOC_MMCHS_0_REGS, 41, SD_OCR_HIGH_CAPACITY | SD_OCR_VDD_WILDCARD, 0, 512, SD_RESPONSE_48BITS, response);

			if (status == 0)
			{
				return 0;
			}

			/* Poll until we get the card status (BIT31 of OCR) is powered up */
			do
			{
				status = SD_SendAppCommand(SOC_MMCHS_0_REGS, 41, SD_OCR_HIGH_CAPACITY | SD_OCR_VDD_WILDCARD, 0, 512, SD_RESPONSE_48BITS, response);
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
			status = SD_SendCommand(SOC_MMCHS_0_REGS, 2, 0, 0, 512, SD_RESPONSE_136BITS, response);

			memcpy(CID, response, 16);

			if (status == 0)
			{
				return 0;
			}

			/* Send CMD3, to get the card relative address */
			status = SD_SendCommand(SOC_MMCHS_0_REGS, 3, 0, 0, 512, SD_RESPONSE_48BITS, response);

			RCA = SD_RCA_ADDR(response[0]);

			if (status == 0)
			{
				return 0;
			}

			/* Send CMD9, to get the card specific data */
			status = SD_SendCommand(SOC_MMCHS_0_REGS, 9, RCA << 16, 0, 512, SD_RESPONSE_136BITS, response);

			memcpy(CSD, response, 16);

			if (status == 0)
			{
				return 0;
			}

			if ((CSD[3] & 0xC0000000) >> 30)
			{
				TransSpeed = SD_CSD1_TRANSPEED(CSD[3], CSD[2], CSD[1], CSD[0]);
				BlockLength = 1 << (SD_CSD1_RDBLKLEN(CSD[3], CSD[2], CSD[1], CSD[0]));

				if((CSD[3] & (0xFL << 26) >> 26) == 4)
				{
					/* get EXT_CSD */
					status = SD_SendCommand(SOC_MMCHS_0_REGS, 8, 0, 1, 512, SD_RESPONSE_READ | SD_RESPONSE_DATA, response);

					if (status == 0)
					{
						return 0;
					}

					SD_ReceiveData(SOC_MMCHS_0_REGS, SD_Buffer, 512);
					Size = ((unsigned int)SD_Buffer[212]) + ((unsigned int)SD_Buffer[213] << 8) + ((unsigned int)SD_Buffer[214] << 16) + ((unsigned int)SD_Buffer[215] << 24);
					Size *= 512;
					NumberBlocks = Size / BlockLength;
				}
				else
				{
					Size = SD_CARD_SIZE;
					Size *= 1024;
					Size *= 1024;
					Size *= 1024;
					NumberBlocks = Size / BlockLength;
				}
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
				status = SD_SendCommand(SOC_MMCHS_0_REGS, 16, 512, 0, 512, SD_RESPONSE_NONE, response);

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
			status = SD_SendCommand(SOC_MMCHS_0_REGS, 7, RCA << 16, 0, 512, SD_RESPONSE_BUSY, response);

			if (status == 0)
			{
				return 0;
			}

			/*
			 * Send ACMD51, to get the SD Configuration register details.
			 * Note, this needs data transfer (on data lines).
			 */
			status = SD_SendCommand(SOC_MMCHS_0_REGS, 55, RCA << 16, 0, 512, SD_RESPONSE_48BITS, response);

			if (status == 0)
			{
				return 0;
			}

			status = SD_SendCommand(SOC_MMCHS_0_REGS, 51, RCA << 16, 1, 8, SD_RESPONSE_READ | SD_RESPONSE_DATA, response);
			if (status == 0)
			{
				return 0;
			}

			SD_ReceiveData(SOC_MMCHS_0_REGS, SD_Buffer, 8);

			SCR[0] = (SD_Buffer[3] << 24) | (SD_Buffer[2] << 16) | \
							   (SD_Buffer[1] << 8) | (SD_Buffer[0]);
			SCR[1] = (SD_Buffer[7] << 24) | (SD_Buffer[6] << 16) | \
									   (SD_Buffer[5] << 8) | (SD_Buffer[4]);

			SD_Version = SCR[0] & 0x0F;
			BusWidth = (SCR[0] & 0xF00) >> 8;

			/* change the bus width ACMD6*/
			status = SD_SendAppCommand(SOC_MMCHS_0_REGS, 6, SD_BUS_WIDTH_4BIT >> 1, 0, 512, SD_RESPONSE_48BITS, response);
			if (status == 0)
			{
				return 0;
			}

			HSMMCSDBusWidthSet(SOC_MMCHS_0_REGS, HS_MMCSD_BUS_WIDTH_4BIT);

			/* Get Transfer speed with CMD6 */
			status = SD_SendCommand(SOC_MMCHS_0_REGS, 6, ((SD_SWITCH_MODE & SD_CMD6_GRP1_SEL) | (SD_CMD6_GRP1_HS)), 1, 64, SD_RESPONSE_READ | SD_RESPONSE_DATA, response);
			if (status == 0)
			{
				return 0;
			}

			SD_ReceiveData(SOC_MMCHS_0_REGS, SD_Buffer, 64);

			speed = TransSpeed;

			if ((SD_Buffer[16] & 0xF) == SD_CMD6_GRP1_HS)
			{
				TransSpeed = SD_TRANSPEED_50MBPS;
			}

			if (speed == SD_TRANSPEED_50MBPS)
			{
				status = HSMMCSDBusFreqSet(SOC_MMCHS_0_REGS, 96000000, 50000000, 0);
			}
			else
			{
				status = HSMMCSDBusFreqSet(SOC_MMCHS_0_REGS, 96000000, 25000000, 0);
			}

			if (status != 0)
			{
				return 0;
			}

			/*
			 * Send ACMD51, to get the SD Configuration register details.
			 * Note, this needs data transfer (on data lines).
			 */
			status = SD_SendCommand(SOC_MMCHS_0_REGS, 55, RCA << 16, 0, 512, SD_RESPONSE_48BITS, response);

			if (status == 0)
			{
				return 0;
			}

			status = SD_SendCommand(SOC_MMCHS_0_REGS, 51, RCA << 16, 1, 8, SD_RESPONSE_READ | SD_RESPONSE_DATA, response);
			if (status == 0)
			{
				return 0;
			}

			SD_ReceiveData(SOC_MMCHS_0_REGS, SD_Buffer, 8);

			SCR[0] = (SD_Buffer[3] << 24) | (SD_Buffer[2] << 16) | \
							   (SD_Buffer[1] << 8) | (SD_Buffer[0]);
			SCR[1] = (SD_Buffer[7] << 24) | (SD_Buffer[6] << 16) | \
									   (SD_Buffer[5] << 8) | (SD_Buffer[4]);

			SD_Version = SCR[0] & 0x0F;
			BusWidth = (SCR[0] & 0xF00) >> 8;

		}
		else
		{
			return 0;
		}
		SD_SetInitialized();
		return 1;
	}
	return 1;
}

/******************************************************************************/
/* SD_SendCommand
 *
 * Sends a command to the SD card.
 *                                                                            */
/******************************************************************************/
unsigned char SD_SendCommand(unsigned int baseAddr, unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_SD_RESPONSE type, unsigned int* response)
{
		unsigned int cmdType = HS_MMCSD_CMD_TYPE_NORMAL;
	    unsigned int dataPresent;
	    unsigned int status = 0;
	    unsigned int rspType;
	    unsigned int cmdDir;
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

	    HSMMCSDIntrStatusDisable(baseAddr, MMCHS_ISE_BRR_SIGEN);
		HSMMCSDIntrStatusDisable(baseAddr, MMCHS_ISE_BWR_SIGEN);
	    if (dataPresent)
	    {
	    	HSMMCSDIntrStatusEnable(baseAddr, MMCHS_ISE_BRR_SIGEN);
	    	HSMMCSDIntrStatusEnable(baseAddr, MMCHS_ISE_BWR_SIGEN);
	    	HSMMCSDIntrStatusEnable(baseAddr, MMCHS_ISE_TC_SIGEN);
	    	HSMMCSDIntrStatusClear(baseAddr, MMCHS_ISE_TC_SIGEN);
	        HSMMCSDDataTimeoutSet(baseAddr, HS_MMCSD_DATA_TIMEOUT(27));
	    }

	    /* enable command event flags */
	    HSMMCSDIntrStatusEnable(baseAddr, MMCHS_ISE_CTO_SIGEN);
	    HSMMCSDIntrStatusEnable(baseAddr, MMCHS_ISE_CC_SIGEN);

	    /* clear timeout */
	    HSMMCSDIntrStatusClear(baseAddr, MMCHS_STAT_CTO);

	    /* clear command complete */
	    HSMMCSDIntrStatusClear(baseAddr, MMCHS_STAT_CC);

        /* set the block length */
        HSMMCSDBlkLenSet(baseAddr, blocksize);

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

/******************************************************************************/
/* SD_SendAppCommand
 *
 * Sends an application specific command to the SD card.
 *                                                                            */
/******************************************************************************/
unsigned char SD_SendAppCommand(unsigned int baseAddr, unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_SD_RESPONSE type, unsigned int* response)
{
	if(SD_SendCommand(baseAddr, 55, RCA << 16, 0, blocksize, SD_RESPONSE_48BITS, response))
	{
		if(SD_SendCommand(baseAddr, command, argument, nblks, blocksize, type, response))
		{
			return TRUE;
		}
	}
	return FALSE;
}

/******************************************************************************/
/* SD_ReceiveData
 *
 * Sets up the mmc module for receive.
 *
 * len has to be less than 1024 bytes.
 *                                                                            */
/******************************************************************************/
void SD_ReceiveData(unsigned int baseAddr, unsigned char* p_buffer, unsigned int length)
{
	while(!(HWREG(baseAddr + MMCHS_PSTATE) & MMCHS_PSTATE_BRE));
	HSMMCSDDataGet(baseAddr, p_buffer, length);
}

/******************************************************************************/
/* SD_ReceiveData
 *
 * Sets up the mmc module for receive.
 *
 * len has to be less than 1024 bytes.
 *                                                                            */
/******************************************************************************/
void SD_TransmitData(unsigned int baseAddr, unsigned char* p_buffer, unsigned int length)
{
	unsigned int i;

	while(!(HWREG(baseAddr + MMCHS_PSTATE) & MMCHS_PSTATE_BWE));

	for (i = 0; i < length/4; i++)
	{
		 HWREG(baseAddr + MMCHS_DATA) = ((unsigned int*)p_buffer)[i];
	}
}

/******************************************************************************/
/* SD_WriteBlocks
 *
 * Writes data to the SD card.
 *                                                                            */
/******************************************************************************/
unsigned int SD_WriteBlocks(unsigned int baseAddr, unsigned int block, unsigned int nblks, unsigned char *ptr)
{
    unsigned int status = 0;
    unsigned int address;

    /*
     * Address is in blks for high cap cards and in actual bytes
     * for standard capacity cards
     */
    if (HighCap)
    {
        address = block;
    }
    else
    {
        address = block * BlockLength;
    }

    if (nblks > 1)
    {
    	while(nblks)
		{
			status = SD_SendCommand(SOC_MMCHS_0_REGS, 24, address, 1, 512, SD_RESPONSE_WRITE | SD_RESPONSE_DATA, response);

			if (status == 0)
			{
				return 0;
			}

			while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_BWR));

			SD_TransmitData(SOC_MMCHS_0_REGS, ptr, 512);

			while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_TC));

			ptr+=512;
			if (HighCap)
			{
				address++;
			}
			else
			{
				address += BlockLength;
			}

			nblks--;
		}
    }
    else
    {
    	status = SD_SendCommand(SOC_MMCHS_0_REGS, 24, address, 1, 512, SD_RESPONSE_WRITE | SD_RESPONSE_DATA, response);

    	if (status == 0)
		{
			return 0;
		}

    	while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_BWR));

    	SD_TransmitData(SOC_MMCHS_0_REGS, (unsigned char*) ptr, 512);

    	while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_TC));
    }

    return 1;
}

/******************************************************************************/
/* SD_ReadBlocks
 *
 * Reads data from the SD card.
 *                                                                            */
/******************************************************************************/
unsigned int SD_ReadBlocks(unsigned int baseAddr, unsigned int block, unsigned int nblks, unsigned char *ptr)
{
    unsigned int status = 0;
    unsigned int address;

    /*
     * Address is in blks for high cap cards and in actual bytes
     * for standard capacity cards
     */
    if (HighCap)
    {
        address = block;
    }
    else
    {
        address = block * BlockLength;
    }


    if (nblks > 1)
    {
		while(nblks)
		{
			status = SD_SendCommand(baseAddr, 17, address, 1, 512, SD_RESPONSE_READ | SD_RESPONSE_DATA, response);

			if (status == 0)
			{
				return 0;
			}

			while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_BRR));

			SD_ReceiveData(baseAddr, ptr, 512);

			while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_TC));

			ptr+=512;
			if (HighCap)
			{
				address++;
			}
			else
			{
				address += BlockLength;
			}

			nblks--;
		}
    }
    else
    {
		status = SD_SendCommand(baseAddr, 17, address, 1, 512, SD_RESPONSE_READ | SD_RESPONSE_DATA, response);

		if (status == 0)
		{
			return 0;
		}

		while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_BRR));

		SD_ReceiveData(baseAddr, (unsigned char*) ptr, 512);

		while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_TC));
    }

    return 1;
}

/******************************************************************************/
/* SD_GetNumberBlocks
 *
 * Returns the number of blocks on the SD card.
 *                                                                            */
/******************************************************************************/
unsigned int SD_GetNumberBlocks(void)
{
	return NumberBlocks;
}

/******************************* End of file *********************************/
