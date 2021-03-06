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
/* Contains functions for initializing the modules. Pin mux values are on page
 *  1356 in TRM.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "FT_Gpu.h"
#include "gpio_v2.h"
#include "interrupt.h"
#include "ramdisk.h"
#include "soc_AM335x.h"

#include "ADC.h"
#include "AUDIO.h"
#include "CMD.h"
#include "EEPROM.h"
#include "EMMC.h"
#include "FRAM.h"
#include "GPIO.h"
#include "GUI.h"
#include "I2C.h"
#include "LCD.h"
#include "LEDS.h"
#include "MISC.h"
#include "POWER.h"
#include "PMIC.h"
#include "RTCC.h"
#include "SD.h"
#include "SPI.h"
#include "SYSTEM.h"
#include "TEST.h"
#include "TIMERS.h"
#include "UART.h"
#include "USB_MSC_DEVICE.h"
#include "USB_MSC_HOST.h"
#include "WAV.h"

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
/* Init_Modules
 *
 * Initializes the AM335x modules.
 *                                                                            */
/******************************************************************************/
void Init_Modules(void)
{
    /* Initialze ARM interrupt controller */
    IntAINTCInit();

    /* Enabling IRQ in CPSR of ARM processor. */
    IntMasterIRQEnable();

	Init_GPIO();
	Init_Timers();
	Init_SPI();
    Init_LCD();
    Init_GUI();

    GUI_DrawInitialScreenProgress(0);
	Init_LED();
	GUI_DrawInitialScreenProgress(5);
    Init_I2C();
    GUI_DrawInitialScreenProgress(10);
    Init_PMIC();
    GUI_DrawInitialScreenProgress(15);
	Init_FRAM();
	GUI_DrawInitialScreenProgress(20);
	Init_RTC();
	GUI_DrawInitialScreenProgress(25);
	Init_ADC();
	GUI_DrawInitialScreenProgress(30);
	Init_Power();
	GUI_DrawInitialScreenProgress(35);
	InitCMD();
	GUI_DrawInitialScreenProgress(40);
	Init_UART();
	GUI_DrawInitialScreenProgress(45);
#ifdef USE_SD_CARD
	Init_SD();
#endif
	GUI_DrawInitialScreenProgress(50);
	Init_EMMC();
	GUI_DrawInitialScreenProgress(55);
	Init_USB_Device();
	GUI_DrawInitialScreenProgress(60);
	Init_USB_Host();
	GUI_DrawInitialScreenProgress(65);
	Init_Test();
	GUI_DrawInitialScreenProgress(70);
	Init_EEPROM();

	/* load bitmaps and other items to RAM_G */
	GUI_LoadItemToRAMG(SD_CARD_SYMBOL);
	GUI_DrawInitialScreenProgress(75);
	GUI_LoadItemToRAMG(CHARLIE_BEACH);
	GUI_DrawInitialScreenProgress(80);
	GUI_LoadItemToRAMG(TV_REMOTE);
	GUI_DrawInitialScreenProgress(85);
	GUI_LoadItemToRAMG(USB_SYMBOL);
	GUI_DrawInitialScreenProgress(90);
	Init_WAV();
	GUI_DrawInitialScreenProgress(92);
	Init_Audio();
	GUI_DrawInitialScreenProgress(93);
	GPIOPinIntClear(SOC_GPIO_3_REGS, GPIO_INT_LINE_1, LCD_INT_PIN);	// clear flag
	LCD_wr8(REG_INT_EN, 1);											// enable interrupts on FT81x
	LCD_Interrupt(TRUE);
	GUI_DrawInitialScreenProgress(95);// enable interrupts on INT pin
	dummy = LCD_rd8(REG_INT_FLAGS); // read interrupt flags
	GUI_DrawInitialScreenProgress(97);
	GUI_TouchConfig();
	GUI_DrawInitialScreenProgress(100);
	GUI_DrawHomeScreen();
	TMR_ResetBacklightTimer();

	if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_EMMC)
	{
		Init_USB0();
	}

#ifdef USE_RAM_DISK
	RAM_disk_initialize();
	Init_USB0();
#endif

}

/******************************* End of file *********************************/
