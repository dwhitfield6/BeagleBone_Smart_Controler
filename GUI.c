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
/* Contains functions to set up the Touchscreen and GUI portion of the FT81x.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "beaglebone.h"
#include "FT_Gpu.h"
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "GPIO.h"
#include "GUI.h"
#include "LCD.h"
#include "LEDS.h"

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
/* Init_GUI
 *
 * Initializes the FT81x touchscreen portion.
 *                                                                            */
/******************************************************************************/
void Init_GUI(void)
{
	GUI_DrawInitialScreen();
}

/******************************************************************************/
/* GUI_DrawInitialScreen
 *
 * This function draws the initial screen.
 * 																			  */
/******************************************************************************/
void GUI_DrawInitialScreen(void)
{
	RAM_CMD_Offset = LCD_rd16(REG_CMD_WRITE);

	LCD_cmd(CMD_DLSTART);

	/* draw background black */
	LCD_cmd(CLEAR_COLOR_RGB(0,0,0));
	LCD_cmd(CLEAR(1,1,1));

	/* draw text */
	LCD_cmd(COLOR_RGB(155, 155, 0));
	LCD_cmd_text(CENTER_X, CENTER_Y, 31, OPT_CENTER, "LOADING");


	LCD_cmd(DISPLAY());
	LCD_cmd(CMD_SWAP);

	LCD_wr16(REG_CMD_WRITE,RAM_CMD_Offset);
	LCD_WaitCoprocessorDone();
}

/******************************* End of file *********************************/
