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
#include "beaglebone.h"
#include "hw_control_AM335x.h"
#include "hw_cm_per.h"
#include "hw_types.h"
#include "gpio_v2.h"
#include "gpio.h"
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

/******************************* End of file *********************************/
