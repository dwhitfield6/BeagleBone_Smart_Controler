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
/* Contains functions to configure and communicate with the EMMC device
 *  (MTFC4GACAAAM-1M WT).
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <string.h>

#include "beaglebone.h"
#include "ff.h"
#include "hw_control_AM335x.h"
#include "hw_cm_per.h"
#include "hw_types.h"
#include "gpio_v2.h"
#include "gpio.h"
#include "hs_mmcsd.h"
#include "mmcsd_proto.h"
#include "soc_AM335x.h"
#include "pin_mux.h"

#include "EMMC.h"
#include "INTERRUPTS.h"
#include "I2C.h"
#include "SD.h"
#include "UART.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
static unsigned char EMMC_CardInitialized = FALSE;
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
static unsigned int BusWidth;
static unsigned short BytesWritten;
static FRESULT Result;

#pragma DATA_ALIGN(fileWrite, SOC_CACHELINE_SIZE);
static FIL fileWrite;

#pragma DATA_ALIGN(FileDataBuffer, SOC_CACHELINE_SIZE);
static char FileDataBuffer[FILE_DATA_BUFFER_SIZE];

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned char EMMC_Buffer[EMMC_BUFFER_SIZE];

#pragma DATA_ALIGN(g_EMMC_FatFs, SOC_CACHELINE_SIZE);
FATFS g_EMMC_FatFs;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_EMMC
 *
 * Initializes the EMMC device.
 *                                                                            */
/******************************************************************************/
void Init_EMMC(void)
{
	/* set up the pins */
	EMMC_PinMuxSetup();

	/* Initialize the clocks */
	EMMC_HSMMCSDModuleClkConfig();

	/* take EMMC out of reset */
	EMMC_HardwareReset(FALSE);

	memset(EMMC_Buffer, 0, sizeof(EMMC_Buffer));

	EMMC_CardInit();

	Result = f_mount(1, &g_EMMC_FatFs);
	Result = f_open (&fileWrite, "Log5.txt", FA_WRITE | FA_CREATE_NEW | FA_OPEN_ALWAYS);
	sprintf(FileDataBuffer, "This is a test.");
	Result = f_write (&fileWrite, FileDataBuffer, strlen(FileDataBuffer), &BytesWritten);
	Result = f_close (&fileWrite);
}

/******************************************************************************/
/* EMMC_PinMuxSetup
 *
 * Sets up the pins for the EMMC module.
 *                                                                            */
/******************************************************************************/
void EMMC_PinMuxSetup(void)
{
	/* pin U7 as mmc1_dat0 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(0)) =
				   (1 << CONTROL_CONF_GPMC_AD0_CONF_GPMC_AD0_MMODE_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD0_CONF_GPMC_AD0_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD0_CONF_GPMC_AD0_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_GPMC_AD0_CONF_GPMC_AD0_RXACTIVE_SHIFT);

	/* pin V7 as mmc1_dat1 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(1)) =
				   (1 << CONTROL_CONF_GPMC_AD1_CONF_GPMC_AD1_MMODE_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD1_CONF_GPMC_AD1_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD1_CONF_GPMC_AD1_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_GPMC_AD1_CONF_GPMC_AD1_RXACTIVE_SHIFT);

	/* pin R8 as mmc1_dat2 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(2)) =
				   (1 << CONTROL_CONF_GPMC_AD2_CONF_GPMC_AD2_MMODE_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD2_CONF_GPMC_AD2_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD2_CONF_GPMC_AD2_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_GPMC_AD2_CONF_GPMC_AD2_RXACTIVE_SHIFT);

	/* pin T8 as mmc1_dat3 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(3)) =
				   (1 << CONTROL_CONF_GPMC_AD3_CONF_GPMC_AD3_MMODE_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD3_CONF_GPMC_AD3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD3_CONF_GPMC_AD3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_GPMC_AD3_CONF_GPMC_AD3_RXACTIVE_SHIFT);

	/* pin U8 as mmc1_dat4 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(4)) =
				   (1 << CONTROL_CONF_GPMC_AD4_CONF_GPMC_AD4_MMODE_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD4_CONF_GPMC_AD4_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD4_CONF_GPMC_AD4_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_GPMC_AD4_CONF_GPMC_AD4_RXACTIVE_SHIFT);

	/* pin V8 as mmc1_dat5 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(5)) =
				   (1 << CONTROL_CONF_GPMC_AD5_CONF_GPMC_AD5_MMODE_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD5_CONF_GPMC_AD5_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD5_CONF_GPMC_AD5_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_GPMC_AD5_CONF_GPMC_AD5_RXACTIVE_SHIFT);

	/* pin R9 as mmc1_dat6 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(6)) =
				   (1 << CONTROL_CONF_GPMC_AD6_CONF_GPMC_AD6_MMODE_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD6_CONF_GPMC_AD6_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD6_CONF_GPMC_AD6_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_GPMC_AD6_CONF_GPMC_AD6_RXACTIVE_SHIFT);

	/* pin T9 as mmc1_dat7 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(7)) =
				   (1 << CONTROL_CONF_GPMC_AD7_CONF_GPMC_AD7_MMODE_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD7_CONF_GPMC_AD7_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_GPMC_AD7_CONF_GPMC_AD7_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_GPMC_AD7_CONF_GPMC_AD7_RXACTIVE_SHIFT);

	/* pin U9 as mmc1_CLK */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_CSN(1)) =
				   (2 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
				   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

	/* pin V9 as mmc1_CMD */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_CSN(2)) =
				   (2 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
				   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

	/* pin R14 as eMMC_RSTn */
	GPIO_PMUX_OFFADDR_VALUE(1, 20, PAD_FS_RXD_NA_PUPDD(7));  		// pin R14 eMMC_RSTn
	GPIODirModeSet(EMMC_RST_REGS, EMMC_RST_PIN, GPIO_DIR_OUTPUT); 	// eMMC_RSTn
}

/******************************************************************************/
/* EMMC_HSMMCEMMCModuleClkConfig
 *
 * Configure the required clocks for HS MMC/EMMC instance.
 *                                                                            */
/******************************************************************************/
void EMMC_HSMMCSDModuleClkConfig(void)
{
	HWREG(SOC_PRCM_REGS+CM_PER_MMC1_CLKCTRL)|=CM_PER_MMC1_CLKCTRL_MODULEMODE_ENABLE;
	while ((HWREG(SOC_PRCM_REGS+CM_PER_MMC1_CLKCTRL) & CM_PER_MMC1_CLKCTRL_MODULEMODE)!=CM_PER_MMC1_CLKCTRL_MODULEMODE_ENABLE);
}

/******************************************************************************/
/* EMMC_HardwareReset
 *
 * Controls the EMMC reset pin.
 *                                                                            */
/******************************************************************************/
void EMMC_HardwareReset(unsigned char state)
{
	if(state)
	{
		GPIOPinWrite(EMMC_RST_REGS, EMMC_RST_PIN, GPIO_PIN_LOW); // reset EMMC
	}
	else
	{
		GPIOPinWrite(EMMC_RST_REGS, EMMC_RST_PIN, GPIO_PIN_HIGH); // reset EMMC
	}
}

/******************************************************************************/
/* EMMC_SetUpController
 *
 * Sets up the EMMC card controller
 *                                                                            */
/******************************************************************************/
unsigned int EMMC_SetUpController(unsigned int baseAddr)
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
/* EMMC_CardInit
 *
 * Initializes the EMMC card.
 *                                                                            */
/******************************************************************************/
unsigned int EMMC_CardInit(void)
{
	unsigned char status = 0;
	unsigned int speed;
	unsigned int temp;

	if(!EMMC_IsInitialized())
	{

		EMMC_SetUpController(SOC_MMCHS_1_REGS);

		/* CMD0 - reset card */
		status = EMMC_SendCommand(SOC_MMCHS_1_REGS, 0, 0, 0, 512, EMMC_RESPONSE_NONE, response);

		if (status == 0)
		{
			return 0;
		}

		/* CMD1 - SEND_OP_COND */
		RepeatOCR:
		status = EMMC_SendCommand(SOC_MMCHS_1_REGS, 1, 0xC0FF8080, 0, 512, EMMC_RESPONSE_48BITS, response);

		if (status == 0)
		{
			return 0;
		}

		if((response[0] != 0x80FF8080) && (response[0] != 0xC0FF8080))
		{
			goto RepeatOCR;
		}

		OCR = response[0];
		HighCap = (OCR & SD_OCR_HIGH_CAPACITY) ? 1 : 0;

		/* Send CMD2, to get the card identification register */
		status = EMMC_SendCommand(SOC_MMCHS_1_REGS, 2, 0, 0, 512, EMMC_RESPONSE_136BITS, response);

		if (status == 0)
		{
			return 0;
		}

		memcpy(CID, response, 16);

		/* Send CMD3, to get the card relative address */
		RCA = 0x1;
		status = EMMC_SendCommand(SOC_MMCHS_1_REGS, 3, RCA << 16, 0, 512, EMMC_RESPONSE_48BITS, response);

		if (status == 0)
		{
			return 0;
		}

		/* Send CMD9, to get the card specific data */
		status = EMMC_SendCommand(SOC_MMCHS_1_REGS, 9, RCA << 16, 0, 512, EMMC_RESPONSE_136BITS, response);

		memcpy(CSD, response, 16);

		if (status == 0)
		{
			return 0;
		}

		if ((CSD[3] & 0xC0000000) >> 30)
		{
			TransSpeed = SD_CSD1_TRANSPEED(CSD[3], CSD[2], CSD[1], CSD[0]);
			BlockLength = 1 << (SD_CSD1_RDBLKLEN(CSD[3], CSD[2], CSD[1], CSD[0]));
			Size = (SD_CSD0_DEV_SIZE(CSD[3], CSD[2], CSD[1], CSD[0]) + 1) * (BlockLength * 512);
			NumberBlocks = Size / BlockLength;
		}
		else
		{
			TransSpeed = SD_CSD0_TRANSPEED(CSD[3], CSD[2], CSD[1], CSD[0]);
			BlockLength = 1 << (SD_CSD0_RDBLKLEN(CSD[3], CSD[2], CSD[1], CSD[0]));
			Size = (SD_CSD0_DEV_SIZE(CSD[3], CSD[2], CSD[1], CSD[0]) + 1) * (512 * 1024);
			NumberBlocks = Size / BlockLength;
		}

		/* Select the card */
		status = EMMC_SendCommand(SOC_MMCHS_1_REGS, 7, RCA << 16, 0, 512, EMMC_RESPONSE_48BITS, response);

		if (status == 0)
		{
			return 0;
		}

		/* Set data block length to 512 (for byte addressing cards) */
		if(!(HighCap))
		{
			status = EMMC_SendCommand(SOC_MMCHS_1_REGS, 16, 512, 0, 512, EMMC_RESPONSE_48BITS, response);

			if (status == 0)
			{
				return 0;
			}
			else
			{
				BlockLength = 512;
			}
		}

		if(((CSD[3] & (0xFL << 26)) >> 26) == 4)
		{
			/* get EXT_CSD */
			status = EMMC_SendCommand(SOC_MMCHS_1_REGS, 8, 0, 1, 512, EMMC_RESPONSE_READ | EMMC_RESPONSE_DATA, response);

			if (status == 0)
			{
				return 0;
			}

			memset(EMMC_Buffer,2,512);
			EMMC_ReceiveData(SOC_MMCHS_1_REGS, EMMC_Buffer, 512);
		}

		if (TransSpeed == SD_TRANSPEED_50MBPS)
		{
			status = HSMMCSDBusFreqSet(SOC_MMCHS_1_REGS, 96000000, 50000000, 0);
		}
		else
		{
			status = HSMMCSDBusFreqSet(SOC_MMCHS_1_REGS, 96000000, 25000000, 0);
		}

		EMMC_SetInitialized();
		return 1;
	}
	return 1;
}


/******************************************************************************/
/* EMMC_SendCommand
 *
 * Sends a command to the EMMC card.
 *                                                                            */
/******************************************************************************/
unsigned char EMMC_SendCommand(unsigned int baseAddr, unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_EMMC_RESPONSE type, unsigned int* response)
{
		unsigned int cmdType = HS_MMCSD_CMD_TYPE_NORMAL;
	    unsigned int dataPresent;
	    unsigned int status = 0;
	    unsigned int rspType;
	    unsigned int cmdDir;
	    unsigned int cmd;

	    if (type & EMMC_RESPONSE_STOP)
	    {
	        cmdType = HS_MMCSD_CMD_TYPE_SUSPEND;
	    }
	    else if (type & EMMC_RESPONSE_FS)
	    {
	        cmdType = HS_MMCSD_CMD_TYPE_FUNCSEL;
	    }
	    else if (type & EMMC_RESPONSE_ABORT)
	    {
	        cmdType = HS_MMCSD_CMD_TYPE_ABORT;
	    }

	    cmdDir = (type & EMMC_RESPONSE_READ) ? \
	              HS_MMCSD_CMD_DIR_READ : HS_MMCSD_CMD_DIR_WRITE;

	    dataPresent = (type & EMMC_RESPONSE_DATA) ? 1 : 0;
	    nblks = (dataPresent == 1) ? 1 : 0;

	    if (type & EMMC_RESPONSE_NONE)
	    {
	        rspType = HS_MMCSD_NO_RESPONSE;
	    }
	    else if (type & EMMC_RESPONSE_136BITS)
	    {
	        rspType = HS_MMCSD_136BITS_RESPONSE;
	    }
	    else if (type & EMMC_RESPONSE_BUSY)
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
/* EMMC_SendAppCommand
 *
 * Sends an application specific command to the EMMC card.
 *                                                                            */
/******************************************************************************/
unsigned char EMMC_SendAppCommand(unsigned int baseAddr, unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_EMMC_RESPONSE type, unsigned int* response)
{
	if(EMMC_SendCommand(baseAddr, 55, RCA << 16, 0, blocksize, EMMC_RESPONSE_48BITS, response))
	{
		if(EMMC_SendCommand(baseAddr, command, argument, nblks, blocksize, type, response))
		{
			return TRUE;
		}
	}
	return FALSE;
}

/******************************************************************************/
/* EMMC_ReceiveData
 *
 * Sets up the mmc module for receive.
 *
 * len has to be less than 1024 bytes.
 *                                                                            */
/******************************************************************************/
void EMMC_ReceiveData(unsigned int baseAddr, unsigned char* p_buffer, unsigned int length)
{
	while(!(HWREG(baseAddr + MMCHS_PSTATE) & MMCHS_PSTATE_BRE));
	HSMMCSDDataGet(baseAddr, p_buffer, length);
}

/******************************************************************************/
/* EMMC_ReceiveData
 *
 * Sets up the mmc module for receive.
 *
 * len has to be less than 1024 bytes.
 *                                                                            */
/******************************************************************************/
void EMMC_TransmitData(unsigned int baseAddr, unsigned char* p_buffer, unsigned int length)
{
	unsigned int i;

	while(!(HWREG(baseAddr + MMCHS_PSTATE) & MMCHS_PSTATE_BWE));

	for (i = 0; i < length/4; i++)
	{
		 HWREG(baseAddr + MMCHS_DATA) = ((unsigned int*)p_buffer)[i];
	}
}

/******************************************************************************/
/* EMMC_WriteBlocks
 *
 * Writes data to the EMMC card.
 *                                                                            */
/******************************************************************************/
unsigned int EMMC_WriteBlocks(unsigned int baseAddr, unsigned int block, unsigned int nblks, unsigned char *ptr)
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
			status = EMMC_SendCommand(SOC_MMCHS_1_REGS, 24, address, 1, 512, EMMC_RESPONSE_WRITE | EMMC_RESPONSE_DATA, response);

			if (status == 0)
			{
				return 0;
			}

			while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_BWR));

			EMMC_TransmitData(SOC_MMCHS_1_REGS, ptr, 512);

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
    	status = EMMC_SendCommand(SOC_MMCHS_1_REGS, 24, address, 1, 512, EMMC_RESPONSE_WRITE | EMMC_RESPONSE_DATA, response);

    	if (status == 0)
		{
			return 0;
		}

    	while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_BWR));

    	EMMC_TransmitData(SOC_MMCHS_1_REGS, (unsigned char*) ptr, 512);

    	while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_TC));
    }

    return 1;
}

/******************************************************************************/
/* EMMC_ReadBlocks
 *
 * Reads data from the EMMC card.
 *                                                                            */
/******************************************************************************/
unsigned int EMMC_ReadBlocks(unsigned int baseAddr, unsigned int block, unsigned int nblks, unsigned char *ptr)
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
			status = EMMC_SendCommand(baseAddr, 17, address, 1, 512, EMMC_RESPONSE_READ | EMMC_RESPONSE_DATA, response);

			if (status == 0)
			{
				return 0;
			}

			while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_BRR));

			EMMC_ReceiveData(baseAddr, ptr, 512);

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
		status = EMMC_SendCommand(baseAddr, 17, address, 1, 512, EMMC_RESPONSE_READ | EMMC_RESPONSE_DATA, response);

		if (status == 0)
		{
			return 0;
		}

		while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_BRR));

		EMMC_ReceiveData(baseAddr, (unsigned char*) ptr, 512);

		while(!(HWREG(baseAddr + MMCHS_STAT) & MMCHS_STAT_TC));
    }

    return 1;
}

/******************************************************************************/
/* EMMC_GetNumberBlocks
 *
 * Returns the number of blocks on the EMMC card.
 *                                                                            */
/******************************************************************************/
unsigned int EMMC_GetNumberBlocks(void)
{
	return NumberBlocks;
}


/******************************************************************************/
/* EMMC_SetInitialized
 *
 * Sets the EMMC initization flag.
 *                                                                            */
/******************************************************************************/
void EMMC_SetInitialized(void)
{
	EMMC_CardInitialized = TRUE;
}

/******************************************************************************/
/* EMMC_ClearInitialized
 *
 * Clears the EMMC initization flag.
 *                                                                            */
/******************************************************************************/
void EMMC_ClearInitialized(void)
{
	EMMC_CardInitialized = FALSE;
}

/******************************************************************************/
/* EMMC_IsInitialized
 *
 * Returns true if the Card is initialized.
 *                                                                            */
/******************************************************************************/
unsigned char EMMC_IsInitialized(void)
{
	return EMMC_CardInitialized;
}

/******************************* End of file *********************************/
