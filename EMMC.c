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
#include "edma.h"
#include "gpio_v2.h"
#include "hw_control_AM335x.h"
#include "hw_cm_per.h"
#include "hw_hs_mmcsd.h"
#include "hw_types.h"
#include "hs_mmcsd.h"
#include "hs_mmcsdlib.h"
#include "interrupt.h"
#include "mmcsd_proto.h"
#include "soc_AM335x.h"
#include "pin_mux.h"

#include "EMMC.h"
#include "INTERRUPTS.h"
#include "I2C.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
mmcsdCtrlInfo  EMMC_ctrlInfo;
mmcsdCardInfo EMMC_Card;

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

	EMMC_ctrlInfo.memBase = SOC_MMCHS_1_REGS;
	EMMC_ctrlInfo.ctrlInit = HSMMCSDControllerInit;
	EMMC_ctrlInfo.xferSetup = EMMC_HSMMCSDXferSetup;
	EMMC_ctrlInfo.cmdStatusGet = EMMC_HSMMCSDCmdStatusGet;
	EMMC_ctrlInfo.xferStatusGet = EMMC_HSMMCSDXferStatusGet;
	EMMC_ctrlInfo.cardPresent = HSMMCSDCardPresent;
	EMMC_ctrlInfo.cmdSend = HSMMCSDCmdSend;
	EMMC_ctrlInfo.busWidthConfig = HSMMCSDBusWidthConfig;
	EMMC_ctrlInfo.busFreqConfig = HSMMCSDBusFreqConfig;
	EMMC_ctrlInfo.intrMask = (HS_MMCSD_INTR_CMDCOMP | HS_MMCSD_INTR_CMDTIMEOUT |
                            HS_MMCSD_INTR_DATATIMEOUT | HS_MMCSD_INTR_TRNFCOMP);
	EMMC_ctrlInfo.intrEnable = HSMMCSDIntEnable;
	EMMC_ctrlInfo.busWidth = SD_BUS_WIDTH_1BIT;
    EMMC_ctrlInfo.highspeed = 1;
    EMMC_ctrlInfo.ocr = (SD_OCR_VDD_3P0_3P1 | SD_OCR_VDD_3P1_3P2);
    EMMC_ctrlInfo.card = &sdCard;
    EMMC_ctrlInfo.ipClk = EMMC_IN_FREQ;
    EMMC_ctrlInfo.opClk = EMMC_INIT_FREQ;
    EMMC_ctrlInfo.cdPinNum = 0;
    EMMC_Card.ctrl = &EMMC_ctrlInfo;

	/* initialize the controller */
	EMMC_ControllerInit();

	/* initialize the disk */
    EMMC_DiskInitialize();
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
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
				   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

	/* pin V7 as mmc1_dat1 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(1)) =
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
				   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

	/* pin R8 as mmc1_dat2 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(2)) =
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
				   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

	/* pin T8 as mmc1_dat3 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(3)) =
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
				   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

	/* pin U8 as mmc1_dat4 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(4)) =
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
				   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

	/* pin V8 as mmc1_dat5 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(5)) =
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
				   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

	/* pin R9 as mmc1_dat6 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(6)) =
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
				   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

	/* pin T9 as mmc1_dat7 */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_GPMC_AD(7)) =
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
				   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
				   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

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
/* EMMC_HSMMCSDModuleClkConfig
 *
 * Configure the required clocks for HS MMC/SD instance.
 *                                                                            */
/******************************************************************************/
void EMMC_HSMMCSDModuleClkConfig(void)
{
    HWREG(SOC_PRCM_REGS + CM_PER_MMC1_CLKCTRL) |=
                             CM_PER_MMC1_CLKCTRL_MODULEMODE_ENABLE;

    while((HWREG(SOC_PRCM_REGS + CM_PER_MMC1_CLKCTRL) &
      CM_PER_MMC1_CLKCTRL_MODULEMODE) != CM_PER_MMC1_CLKCTRL_MODULEMODE_ENABLE);
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
/* EMMC_ControllerInit
 *
 * Initializes the MMC controller.
 *                                                                            */
/******************************************************************************/
void EMMC_ControllerInit(void)
{
    /*Refer to the MMC Host and Bus configuration steps in TRM */
    /* controller Reset */
    HSMMCSDSoftReset(SOC_MMCHS_1_REGS);

    /* Lines Reset */
    HSMMCSDLinesReset(SOC_MMCHS_1_REGS, HS_MMCSD_ALL_RESET);

    /* Set supported voltage list */
    HSMMCSDSupportedVoltSet(SOC_MMCHS_1_REGS, HS_MMCSD_SUPPORT_VOLT_1P8 |
                                                HS_MMCSD_SUPPORT_VOLT_3P0);

    HSMMCSDSystemConfig(SOC_MMCHS_1_REGS, HS_MMCSD_AUTOIDLE_ENABLE);

    /* Set the bus width */
    HSMMCSDBusWidthSet(SOC_MMCHS_1_REGS, HS_MMCSD_BUS_WIDTH_1BIT );

    /* Set the bus voltage */
    HSMMCSDBusVoltSet(SOC_MMCHS_1_REGS, HS_MMCSD_BUS_VOLT_3P0);

    /* Bus power on */
    HSMMCSDBusPower(SOC_MMCHS_1_REGS, HS_MMCSD_BUS_POWER_ON);

    /* Set the initialization frequency */
    HSMMCSDBusFreqSet(SOC_MMCHS_1_REGS, EMMC_Card.ctrl->ipClk, EMMC_Card.ctrl->opClk, 0);

    HSMMCSDInitStreamSend(SOC_MMCHS_1_REGS);
}

/******************************************************************************/
/* EMMC_DiskInitialize
 *
 * Initializes the EMMC disk
 *                                                                            */
/******************************************************************************/
void EMMC_DiskInitialize(void)
{
	unsigned int status;

	/* SD Card init */
	status = MMCSDCardInit(EMMC_Card.ctrl);

	if(status)
	{
		/* Set bus width */
		MMCSDBusWidthSet(EMMC_Card.ctrl);

		/* Transfer speed */
		MMCSDTranSpeedSet(EMMC_Card.ctrl);
	}
}

/******************************************************************************/
/* EMMC_HSMMCSDXferSetup
 *
 * Configures the DMA for SD card for transmit and receive.
 *                                                                            */
/******************************************************************************/
void EMMC_HSMMCSDXferSetup(mmcsdCtrlInfo *ctrl, unsigned char rwFlag, void *ptr, unsigned int blkSize, unsigned int nBlks)
{
    HSMMCSDBlkLenSet(ctrl->memBase, blkSize);
}

/******************************************************************************/
/* EMMC_HSMMCSDCmdStatusGet
 *
 * Checks the command status.
 *                                                                            */
/******************************************************************************/
unsigned int EMMC_HSMMCSDCmdStatusGet(mmcsdCtrlInfo *ctrl)
{
	while(!(HWREG(SOC_MMCHS_1_REGS + MMCHS_STAT) & MMCHS_STAT_CC))
	{
		if(HWREG(SOC_MMCHS_1_REGS + MMCHS_STAT) & MMCHS_STAT_CTO)
		{
			HWREG(SOC_MMCHS_1_REGS + MMCHS_STAT) |= MMCHS_STAT_CTO; // clear
			return FAIL;
		}
	}
	return PASS;
}


/******************************************************************************/
/* EMMC_HSMMCSDXferStatusGet
 *
 * Gets the EMMC transfer status.
 *                                                                            */
/******************************************************************************/
unsigned int EMMC_HSMMCSDXferStatusGet(mmcsdCtrlInfo *ctrl)
{
	while(!(HWREG(SOC_MMCHS_1_REGS + MMCHS_STAT) & MMCHS_STAT_TC));

	return PASS;
}


/******************************* End of file *********************************/
