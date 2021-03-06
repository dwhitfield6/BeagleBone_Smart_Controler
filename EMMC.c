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
#include "LEDS.h"
#include "MISC.h"
#include "SD.h"
#include "UART.h"

/******************************************************************************/
/* Pragma                                                                    */
/******************************************************************************/
#pragma diag_suppress 190
#pragma diag_suppress 552

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
static unsigned int BytesWritten;
static unsigned char HighPower = 0;
static unsigned char EXT_CSD[512];
static FRESULT Result;

#pragma DATA_ALIGN(fileWrite, SOC_CACHELINE_SIZE);
static FIL fileWrite;

#pragma DATA_ALIGN(FileDataBuffer, SOC_CACHELINE_SIZE);
static char FileDataBuffer[EMMC_FILE_DATA_BUFFER_SIZE];

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
	fat_devices[DRIVE_NUM_EMMC].initDone = 0;

	/* set up the pins */
	EMMC_PinMuxSetup();

	/* Initialize the clocks */
	EMMC_HSMMCSDModuleClkConfig();

	/* take EMMC out of reset */
	EMMC_HardwareReset(FALSE);

	memset(EMMC_Buffer, 0, sizeof(EMMC_Buffer));

	EMMC_CardInit();

	Result = f_mount(DRIVE_NUM_EMMC, &g_EMMC_FatFs);
	Result = f_open (&fileWrite, "2:/Log11.txt", FA_WRITE | FA_CREATE_NEW | FA_OPEN_ALWAYS);
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
unsigned int EMMC_SetUpController(void)
{
	int status = 0;

	/*Refer to the MMC Host and Bus configuration steps in TRM */
	/* controller Reset */
	status = HSMMCSDSoftReset(EMMC_BASE_ADDRESS);

	if (status != 0)
	{
		UART_PrintString("EMMC Reset failed\n\r");
	}

	/* Lines Reset */
	HSMMCSDLinesReset(EMMC_BASE_ADDRESS, HS_MMCSD_ALL_RESET);

	/* Set supported voltage list */
	HSMMCSDSupportedVoltSet(EMMC_BASE_ADDRESS, HS_MMCSD_SUPPORT_VOLT_1P8 |
												HS_MMCSD_SUPPORT_VOLT_3P3);

	HSMMCSDSystemConfig(EMMC_BASE_ADDRESS, HS_MMCSD_AUTOIDLE_ENABLE);

	/* Set the bus width */
	HSMMCSDBusWidthSet(EMMC_BASE_ADDRESS, HS_MMCSD_BUS_WIDTH_1BIT );

	/* Set the bus voltage */
	HSMMCSDBusVoltSet(EMMC_BASE_ADDRESS, HS_MMCSD_BUS_VOLT_3P3);

	/* Bus power on */
	status = HSMMCSDBusPower(EMMC_BASE_ADDRESS, HS_MMCSD_BUS_POWER_ON);

	if (status != 0)
	{
		UART_PrintString("EMMC Power on failed\n\r");
	}

	/* Set the initialization frequency */
	status = HSMMCSDBusFreqSet(EMMC_BASE_ADDRESS, 96000000, 400000, 0);
	if (status != 0)
	{
		UART_PrintString("EMMC Bus Frequency set failed\n\r");
	}

	status = HSMMCSDInitStreamSend(EMMC_BASE_ADDRESS);

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

	BusWidth = 1;

	if(!EMMC_IsInitialized())
	{

		EMMC_SetUpController();

		/* CMD0 - reset card */
		status = EMMC_SendCommand(0, 0, 0, 512, EMMC_RESPONSE_NONE, response);

		if (status == 0)
		{
			return 0;
		}

		/* CMD1 - SEND_OP_COND */
		RepeatOCR:
		status = EMMC_SendCommand(1, 0xC0FF8080, 0, 512, EMMC_RESPONSE_48BITS, response);

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
		status = EMMC_SendCommand(2, 0, 0, 512, EMMC_RESPONSE_136BITS, response);

		if (status == 0)
		{
			return 0;
		}

		memcpy(CID, response, 16);

		/* Send CMD3, to get the card relative address */
		RCA = 0x1;
		status = EMMC_SendCommand(3, RCA << 16, 0, 512, EMMC_RESPONSE_48BITS, response);

		if (status == 0)
		{
			return 0;
		}

		/* Send CMD9, to get the card specific data */
		status = EMMC_SendCommand(9, RCA << 16, 0, 512, EMMC_RESPONSE_136BITS, response);

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
		status = EMMC_SendCommand(7, RCA << 16, 0, 512, EMMC_RESPONSE_48BITS, response);

		if (status == 0)
		{
			return 0;
		}

		if(((CSD[3] & (0xFL << 26)) >> 26) == 4)
		{
			/* get EXT_CSD */
			status = EMMC_SendCommand(8, 0, 1, 512, EMMC_RESPONSE_READ | EMMC_RESPONSE_DATA, response);

			if (status == 0)
			{
				return 0;
			}

			EMMC_ReceiveData(EMMC_Buffer, 512);
			memcpy(EXT_CSD, EMMC_Buffer, 512);
		}

		/* enable high speed mode */
		if (EMMC_SendCommandSwitch(MMC_SWITCH_MODE_WRITE_BYTE, EXT_CSD_HS_TIMING, 0x1))
		{
			status = HSMMCSDBusFreqSet(EMMC_BASE_ADDRESS, 96000000, 50000000, 0);
		}
		else
		{
			status = HSMMCSDBusFreqSet(EMMC_BASE_ADDRESS, 96000000, 25000000, 0);
		}

		MSC_DelayNOP(10000);

		/* set bus width to 8 */
		if(BusWidth == 1)
		{
			if (EMMC_SendCommandSwitch(MMC_SWITCH_MODE_WRITE_BYTE, EXT_CSD_BUS_WIDTH, EXT_CSD_BUS_WIDTH_8))
			{
				HSMMCSDBusWidthSet(EMMC_BASE_ADDRESS, HS_MMCSD_BUS_WIDTH_8BIT);
				BusWidth = 8;
			}
		}

		MSC_DelayNOP(10000);

		/* set power to high power mode */
		if(!EXT_CSD[EXT_CSD_POWER_CLASS])
		{
			if (EMMC_SendCommandSwitch(MMC_SWITCH_MODE_WRITE_BYTE, EXT_CSD_POWER_CLASS, 8))
			{
				HighPower = 1;
			}
		}

		MSC_DelayNOP(10000);

		if(((CSD[3] & (0xFL << 26)) >> 26) == 4)
		{
			/* get EXT_CSD */
			status = EMMC_SendCommand(8, 0, 1, 512, EMMC_RESPONSE_READ | EMMC_RESPONSE_DATA, response);

			if (status == 0)
			{
				return 0;
			}

			EMMC_ReceiveData(EMMC_Buffer, 512);
			memcpy(EXT_CSD, EMMC_Buffer, 512);
		}

		/* Set data block length to 512 (for byte addressing cards) */
		if(!(HighCap))
		{
			status = EMMC_SendCommand(16, 512, 0, 512, SD_RESPONSE_48BITS, response);

			if (status == 0)
			{
				return 0;
			}
			else
			{
				BlockLength = 512;
			}
		}

		Size = (((unsigned int)EXT_CSD[212] << 0) | ((unsigned int)EXT_CSD[213] << 8) | ((unsigned int)EXT_CSD[214] << 16) | ((unsigned int)EXT_CSD[215] << 24));
		Size *= 512;
		NumberBlocks = Size / BlockLength;

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
unsigned char EMMC_SendCommand(unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_EMMC_RESPONSE type, unsigned int* response)
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

	    HSMMCSDIntrStatusDisable(EMMC_BASE_ADDRESS, MMCHS_ISE_BRR_SIGEN);
	    HSMMCSDIntrStatusDisable(EMMC_BASE_ADDRESS, MMCHS_ISE_BWR_SIGEN);
	    if (dataPresent)
	    {
	    	HSMMCSDIntrStatusEnable(EMMC_BASE_ADDRESS, MMCHS_ISE_BRR_SIGEN);
	    	HSMMCSDIntrStatusEnable(EMMC_BASE_ADDRESS, MMCHS_ISE_BWR_SIGEN);
	    	HSMMCSDIntrStatusEnable(EMMC_BASE_ADDRESS, MMCHS_ISE_TC_SIGEN);
	    	HSMMCSDIntrStatusClear(EMMC_BASE_ADDRESS, MMCHS_ISE_TC_SIGEN);
	        HSMMCSDDataTimeoutSet(EMMC_BASE_ADDRESS, HS_MMCSD_DATA_TIMEOUT(27));
	    }

	    /* enable command event flags */
	    HSMMCSDIntrStatusEnable(EMMC_BASE_ADDRESS, MMCHS_ISE_CTO_SIGEN);
	    HSMMCSDIntrStatusEnable(EMMC_BASE_ADDRESS, MMCHS_ISE_CC_SIGEN);

	    /* clear timeout */
	    HSMMCSDIntrStatusClear(EMMC_BASE_ADDRESS, MMCHS_STAT_CTO);

	    /* clear command complete */
	    HSMMCSDIntrStatusClear(EMMC_BASE_ADDRESS, MMCHS_STAT_CC);

        /* set the block length */
        HSMMCSDBlkLenSet(EMMC_BASE_ADDRESS, blocksize);

	    HSMMCSDCommandSend(EMMC_BASE_ADDRESS, cmd, argument, (void*)dataPresent, nblks, 0);

	    while((!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_CTO)) && (!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_CC)) && (!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_CREM)));

	    if(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_CC)
	    {
	    	status = 1;
	    }

	    if (status == 1)
	    {
	        HSMMCSDResponseGet(EMMC_BASE_ADDRESS, response);
	    }

	    return status;
}

/******************************************************************************/
/* EMMC_SendAppCommand
 *
 * Sends an application specific command to the EMMC card.
 *                                                                            */
/******************************************************************************/
unsigned char EMMC_SendAppCommand(unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_EMMC_RESPONSE type, unsigned int* response)
{
	if(EMMC_SendCommand(55, RCA << 16, 0, blocksize, EMMC_RESPONSE_48BITS, response))
	{
		if(EMMC_SendCommand(command, argument, nblks, blocksize, type, response))
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
void EMMC_ReceiveData(unsigned char* p_buffer, unsigned int length)
{
	while(!(HWREG(EMMC_BASE_ADDRESS + MMCHS_PSTATE) & MMCHS_PSTATE_BRE));
	HSMMCSDDataGet(EMMC_BASE_ADDRESS, p_buffer, length);
}

/******************************************************************************/
/* EMMC_ReceiveData
 *
 * Sets up the mmc module for receive.
 *
 * len has to be less than 1024 bytes.
 *                                                                            */
/******************************************************************************/
void EMMC_TransmitData(unsigned char* p_buffer, unsigned int length)
{
	unsigned int i;

	while(!(HWREG(EMMC_BASE_ADDRESS + MMCHS_PSTATE) & MMCHS_PSTATE_BWE));

	for (i = 0; i < length/4; i++)
	{
		 HWREG(EMMC_BASE_ADDRESS + MMCHS_DATA) = ((unsigned int*)p_buffer)[i];
	}
}

/******************************************************************************/
/* EMMC_WriteBlocks
 *
 * Writes data to the EMMC card.
 *                                                                            */
/******************************************************************************/
unsigned int EMMC_WriteBlocks(unsigned int block, unsigned int nblks, unsigned char *ptr)
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
			status = EMMC_SendCommand(24, address, 1, 512, EMMC_RESPONSE_WRITE | EMMC_RESPONSE_DATA, response);

			if (status == 0)
			{
				return 0;
			}

			while(!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_BWR));

			EMMC_TransmitData(ptr, 512);

			while(!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_TC));

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
    	status = EMMC_SendCommand(24, address, 1, 512, EMMC_RESPONSE_WRITE | EMMC_RESPONSE_DATA, response);

    	if (status == 0)
		{
			return 0;
		}

    	while(!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_BWR));

    	EMMC_TransmitData((unsigned char*) ptr, 512);

    	while(!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_TC));
    }

    return 1;
}

/******************************************************************************/
/* EMMC_ReadBlocks
 *
 * Reads data from the EMMC card.
 *                                                                            */
/******************************************************************************/
unsigned int EMMC_ReadBlocks(unsigned int block, unsigned int nblks, unsigned char *ptr)
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
			status = EMMC_SendCommand(17, address, 1, 512, EMMC_RESPONSE_READ | EMMC_RESPONSE_DATA, response);

			if (status == 0)
			{
				return 0;
			}

			while(!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_BRR));

			EMMC_ReceiveData(ptr, 512);

			while(!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_TC));

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
		status = EMMC_SendCommand(17, address, 1, 512, EMMC_RESPONSE_READ | EMMC_RESPONSE_DATA, response);

		if (status == 0)
		{
			return 0;
		}

		while(!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_BRR));

		EMMC_ReceiveData((unsigned char*) ptr, 512);

		while(!(HWREG(EMMC_BASE_ADDRESS + MMCHS_STAT) & MMCHS_STAT_TC));
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

/******************************************************************************/
/* EMMC_SendCommandSwitch
 *
 * Sends the switch command.
 *                                                                            */
/******************************************************************************/
unsigned char EMMC_SendCommandSwitch(unsigned int set, unsigned int index, unsigned int value)
{
	unsigned int status;
	unsigned char err;

	err = EMMC_SendCommand(6, (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (index << 16) | (value << 8) | set, 1, 512, EMMC_RESPONSE_BUSY, response);

	if (err == 0)
	{
		return 0;
	}


	/* Must check status to be sure of no errors */
	do
	{
		err = EMMC_SendStatus(&status);
		if (err == 0)
		{
			return 0;
		}

		if(status & (BIT(7)))
		{
			/* switch error */
			return 0;
		}

		if(status & (BIT(22)))
	   {
			return 0;
	   }

	} while (!(status & BIT(8)) & (!err)); // run while the card is not ready

	return 1;
}

/******************************************************************************/
/* EMMC_SendStatus
 *
 * Sends the status command.
 *                                                                            */
/******************************************************************************/
unsigned char EMMC_SendStatus(unsigned int* status)
{
    unsigned char err;

    err = EMMC_SendCommand(13, RCA << 16, 0, 512, EMMC_RESPONSE_48BITS, response);

    if (err == 0)
	{
		return 0;
	}

    *status = response[0];

    return 1;
}

/******************************************************************************/
/* EMMC_TestWrite
 *
 * Tests write and read.
 *                                                                            */
/******************************************************************************/
unsigned char EMMC_TestWrite(void)
{
	unsigned char status;

	memset(EMMC_Buffer, 0, 512);
	status = EMMC_WriteBlocks(0, 1, EMMC_Buffer);
	if(status)
	{
		memset(EMMC_Buffer, 0, 512);
		status = EMMC_ReadBlocks(0, 1, EMMC_Buffer);
	}

	if(status)
	{
		status = 0;
		if((EMMC_Buffer[0] == 0) && (EMMC_Buffer[511] == 0))
		{
			status = 1;
		}
	}
	return status;
}

/******************************* End of file *********************************/
