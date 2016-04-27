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
/* Contains functions to set up the GPIO.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "GPIO.h"
#include "LCD.h"
#include "LEDS.h"
#include "SPI.h"
#include "TEST.h"

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
/* Init_GPIO
 *
 * Initiates the Pins.
 *                                                                            */
/******************************************************************************/
void Init_GPIO(void)
{
	/* initiate the GPIO clocks */
	GPIO_Module0ClkConfig();
	GPIO_Module1ClkConfig();
	GPIO_Module2ClkConfig();
	GPIO_Module3ClkConfig();

	/* reset the GPIO modules */
	GPIOModuleReset(SOC_GPIO_0_REGS);
	GPIOModuleReset(SOC_GPIO_1_REGS);
	GPIOModuleReset(SOC_GPIO_2_REGS);
	GPIOModuleReset(SOC_GPIO_3_REGS);

    /*~~~~~~~~~~~~~~~~~~~ Test Points ~~~~~~~~~~~~~~~~~~~*/
	GPIO_PMUX_OFFADDR_VALUE(1, 13, PAD_FS_RXD_NA_PUPDD(7));  	// TP 1 = GPIO1_13 = pin R12
	GPIO_PMUX_OFFADDR_VALUE(1, 15, PAD_FS_RXD_NA_PUPDD(7));  	// TP 2 = GPIO1_15 = pin U13
	GPIO_PMUX_OFFADDR_VALUE(0, 27, PAD_FS_RXD_NA_PUPDD(7)); 	// TP 3 = GPIO0_27 = pin U12

	/* Setting the GPIO pin direction */
	GPIODirModeSet(TP1_REGS, TP1_PIN, GPIO_DIR_OUTPUT); 		// TP 1
	GPIODirModeSet(TP2_REGS, TP2_PIN, GPIO_DIR_OUTPUT); 		// TP 2
	GPIODirModeSet(TP3_REGS, TP3_PIN, GPIO_DIR_OUTPUT); 		// TP 3

    /*~~~~~~~~~~~~~~~~~~~ LEDS ~~~~~~~~~~~~~~~~~~~*/
	GPIO_PMUX_OFFADDR_VALUE(1, 21, PAD_FS_RXD_NA_PUPDD(7));  	// LED 1 = GPIO1_21 = pin V15
	GPIO_PMUX_OFFADDR_VALUE(1, 22, PAD_FS_RXD_NA_PUPDD(7));  	// LED 2 = GPIO1_22 = pin U15
	GPIO_PMUX_OFFADDR_VALUE(1, 23, PAD_FS_RXD_NA_PUPDD(7)); 	// LED 3 = GPIO1_23 = pin T15
	GPIO_PMUX_OFFADDR_VALUE(1, 24, PAD_FS_RXD_NA_PUPDD(7)); 	// LED 4 = GPIO1_24 = pin V16

	/* Setting the GPIO pin direction */
	GPIODirModeSet(LED1_REGS, LED1_PIN, GPIO_DIR_OUTPUT); 		// LED 1
	GPIODirModeSet(LED2_REGS, LED2_PIN, GPIO_DIR_OUTPUT); 		// LED 2
	GPIODirModeSet(LED3_REGS, LED3_PIN, GPIO_DIR_OUTPUT); 		// LED 3
	GPIODirModeSet(LED4_REGS, LED4_PIN, GPIO_DIR_OUTPUT); 		// LED 4

	/*~~~~~~~~~~~~~~~~~~~ LCD FT81x module ~~~~~~~~~~~~~~~~~~~*/
    /* set up the pins */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_ACLKX) = PAD_FS_RXE_PU_PUPDE(3); 	// SCK 	= pin A13
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_FSX) = PAD_FS_RXE_PU_PUPDE(3); 	// MOSI = pin B13
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AXR0) = PAD_FS_RXE_PU_PUPDE(3);	// MISO = pin D12
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AHCLKR) = PAD_FS_RXE_PU_PUPDE(3);	// CS   = pin C12
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_FSR) = PAD_FS_RXD_NA_PUPDD(7);	    // PD   = GPIO3_19 = pin C13
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MCASP0_AHCLKX) = PAD_FS_RXE_PD_PUPDE(7);	// INT  = GPIO3_21 = pin A14

	LCD_Reset(TRUE);

	/* Setting the GPIO pin direction */
	GPIODirModeSet(LCD_PD_REGS, LCD_PD_PIN, GPIO_DIR_OUTPUT); 		// PD
	GPIODirModeSet(LCD_INT_REGS, LCD_INT_PIN, GPIO_DIR_INPUT); 		// INT

	/*~~~~~~~~~~~~~~~~~~~ RF module ~~~~~~~~~~~~~~~~~~~*/
    /* set up the pins */
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_SCLK) = PAD_FS_RXE_PU_PUPDE(0); 	// SCK 	= pin A17
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_D0) = PAD_FS_RXE_PU_PUPDE(0); 	// MOSI = pin B17
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_D1) = PAD_FS_RXE_PU_PUPDE(0);	// MISO = pin B16
	HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_CS0) = PAD_FS_RXE_PU_PUPDE(0);	// CS   = pin A16

	/* enable the GPIO modules */
	GPIOModuleEnable(SOC_GPIO_0_REGS);
	GPIOModuleEnable(SOC_GPIO_1_REGS);
	GPIOModuleEnable(SOC_GPIO_2_REGS);
	GPIOModuleEnable(SOC_GPIO_3_REGS);
}

/******************************************************************************/
/* GPIO_Module0ClkConfig
 *
 * Initiates the GPIO module 0 clocks.
 *                                                                            */
/******************************************************************************/
void GPIO_Module0ClkConfig(void)
{
    /* Writing to MODULEMODE field of CM_WKUP_GPIO0_CLKCTRL register. */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL) |=
        CM_WKUP_GPIO0_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_WKUP_GPIO0_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL) &
           CM_WKUP_GPIO0_CLKCTRL_MODULEMODE));

    /*
    ** Writing to OPTFCLKEN_GPIO0_GDBCLK field of CM_WKUP_GPIO0_CLKCTRL
    ** register.
    */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL) |=
        CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK;

    /* Waiting for OPTFCLKEN_GPIO0_GDBCLK field to reflect the written value. */
    while(CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK !=
          (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL) &
           CM_WKUP_GPIO0_CLKCTRL_OPTFCLKEN_GPIO0_GDBCLK));

    /* Writing to IDLEST field in CM_WKUP_GPIO0_CLKCTRL register. */
    while((CM_WKUP_GPIO0_CLKCTRL_IDLEST_FUNC <<
           CM_WKUP_GPIO0_CLKCTRL_IDLEST_SHIFT) !=
          (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_GPIO0_CLKCTRL) &
           CM_WKUP_GPIO0_CLKCTRL_IDLEST));

    /*
    ** Waiting for CLKACTIVITY_GPIO0_GDBCLK field in CM_WKUP_GPIO0_CLKCTRL
    ** register to attain desired value.
    */
    while(CM_WKUP_CLKSTCTRL_CLKACTIVITY_GPIO0_GDBCLK !=
          (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CLKSTCTRL) &
           CM_WKUP_CLKSTCTRL_CLKACTIVITY_GPIO0_GDBCLK));
}

/******************************************************************************/
/* GPIO_Module1ClkConfig
 *
 * Initiates the GPIO module 1 clocks.
 *                                                                            */
/******************************************************************************/
void GPIO_Module1ClkConfig(void)
{
    /* Writing to MODULEMODE field of CM_PER_GPIO1_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) |=
          CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_GPIO1_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) &
           CM_PER_GPIO1_CLKCTRL_MODULEMODE));
    /*
    ** Writing to OPTFCLKEN_GPIO_1_GDBCLK bit in CM_PER_GPIO1_CLKCTRL
    ** register.
    */
    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) |=
          CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK;

    /*
    ** Waiting for OPTFCLKEN_GPIO_1_GDBCLK bit to reflect the desired
    ** value.
    */
    while(CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) &
           CM_PER_GPIO1_CLKCTRL_OPTFCLKEN_GPIO_1_GDBCLK));

    /*
    ** Waiting for IDLEST field in CM_PER_GPIO1_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_GPIO1_CLKCTRL_IDLEST_FUNC <<
           CM_PER_GPIO1_CLKCTRL_IDLEST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO1_CLKCTRL) &
            CM_PER_GPIO1_CLKCTRL_IDLEST));

    /*
    ** Waiting for CLKACTIVITY_GPIO_1_GDBCLK bit in CM_PER_L4LS_CLKSTCTRL
    ** register to attain desired value.
    */
    while(CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
           CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_1_GDBCLK));
}

/******************************************************************************/
/* GPIO_Module2ClkConfig
 *
 * Initiates the GPIO module 2 clocks.
 *                                                                            */
/******************************************************************************/
void GPIO_Module2ClkConfig(void)
{
    /* Writing to MODULEMODE field of CM_PER_GPIO1_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) |=
          CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_GPIO2_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
           CM_PER_GPIO2_CLKCTRL_MODULEMODE));
    /*
    ** Writing to OPTFCLKEN_GPIO_1_GDBCLK bit in CM_PER_GPIO1_CLKCTRL
    ** register.
    */
    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) |=
          CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK;

    /*
    ** Waiting for OPTFCLKEN_GPIO_1_GDBCLK bit to reflect the desired
    ** value.
    */
    while(CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
           CM_PER_GPIO2_CLKCTRL_OPTFCLKEN_GPIO_2_GDBCLK));

    /*
    ** Waiting for IDLEST field in CM_PER_GPIO1_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_PER_GPIO2_CLKCTRL_IDLEST_FUNC <<
           CM_PER_GPIO2_CLKCTRL_IDLEST_SHIFT) !=
           (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO2_CLKCTRL) &
            CM_PER_GPIO2_CLKCTRL_IDLEST));

    /*
    ** Waiting for CLKACTIVITY_GPIO_1_GDBCLK bit in CM_PER_L4LS_CLKSTCTRL
    ** register to attain desired value.
    */
    while(CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_2_GDBCLK !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_L4LS_CLKSTCTRL) &
           CM_PER_L4LS_CLKSTCTRL_CLKACTIVITY_GPIO_2_GDBCLK));
}

/******************************************************************************/
/* GPIO_Module3ClkConfig
 *
 * Initiates the GPIO module 3 clocks.
 *                                                                            */
/******************************************************************************/
void GPIO_Module3ClkConfig(void)
{
    /* Writing to MODULEMODE field of CM_PER_GPIO3_CLKCTRL register. */
    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO3_CLKCTRL) =
          CM_PER_GPIO3_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_PER_GPIO3_CLKCTRL_MODULEMODE_ENABLE !=
         (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO3_CLKCTRL) &
          CM_PER_GPIO3_CLKCTRL_MODULEMODE));

    /*
    ** Writing to OPTFCLKEN_GPIO_3_GDBCLK bit in CM_PER_GPIO3_CLKCTRL
    ** register.
    */
    HWREG(SOC_CM_PER_REGS + CM_PER_GPIO3_CLKCTRL) |=
          CM_PER_GPIO3_CLKCTRL_OPTFCLKEN_GPIO_3_GDBCLK;

    /*
    ** Waiting for OPTFCLKEN_GPIO_3_GDBCLK bit to reflect the desired
    ** value.
    */
    while(CM_PER_GPIO3_CLKCTRL_OPTFCLKEN_GPIO_3_GDBCLK !=
          (HWREG(SOC_CM_PER_REGS + CM_PER_GPIO3_CLKCTRL) &
           CM_PER_GPIO3_CLKCTRL_OPTFCLKEN_GPIO_3_GDBCLK));

}

/******************************* End of file *********************************/
