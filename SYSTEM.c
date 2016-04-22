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
#include "gpio_v2.h"
#include "interrupt.h"
#include "soc_AM335x.h"

#include "CMD.h"
#include "GPIO.h"
#include "GUI.h"
#include "LCD.h"
#include "LEDS.h"
#include "SPI.h"
#include "SYSTEM.h"
#include "UART.h"

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

	Init_GPIO();
	Init_SPI();
    Init_LCD();
    Init_GUI();

    /* Enabling IRQ in CPSR of ARM processor. */
    IntMasterIRQEnable();

    GUI_DrawInitialScreenProgress(0);
	Init_LED();
	GUI_DrawInitialScreenProgress(5);
	InitCMD();
	GUI_DrawInitialScreenProgress(10);
	Init_UART();
	GUI_DrawInitialScreenProgress(15);

	/* load bitmaps and other items to RAM_G */
	GUI_LoadItemToRAMG(CHARLIE_BEACH);
	GUI_DrawInitialScreenProgress(20);
	GUI_LoadItemToRAMG(SD_CARD);
	GUI_DrawInitialScreenProgress(40);
	GUI_LoadItemToRAMG(TV_REMOTE);
	GUI_DrawInitialScreenProgress(60);
	GUI_TouchConfig();
	GUI_DrawInitialScreenProgress(100);
	GUI_DrawHomeScreen();
}

/******************************* End of file *********************************/
