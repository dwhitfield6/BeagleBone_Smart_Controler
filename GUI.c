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

	/* draw square */
	LCD_cmd(COLOR_RGB(50, 50, 50));
	LCD_cmd(BEGIN(RECTS));
	LCD_cmd(LINE_WIDTH(10 *16));
	LCD_cmd(VERTEX2F(20 *16, 40 *16));
	LCD_cmd(VERTEX2F(780 *16, 430 *16));
	LCD_cmd(END());

	LCD_cmd(COLOR_RGB(0,0,0));
	LCD_cmd(BEGIN(RECTS));
	LCD_cmd(LINE_WIDTH(10 *16));
	LCD_cmd(VERTEX2F(25 *16, 45 *16));
	LCD_cmd(VERTEX2F(775 *16, 425 *16));
	LCD_cmd(END());

	/* draw line */
	LCD_cmd(COLOR_RGB(50,50,50));
	LCD_cmd(BEGIN(RECTS));
	LCD_cmd(LINE_WIDTH(1 *16));
	LCD_cmd(VERTEX2F(15 *16, 90 *16));
	LCD_cmd(VERTEX2F(200 *16, 150 *16));
	LCD_cmd(END());

	LCD_cmd(BEGIN(RECTS));
	LCD_cmd(LINE_WIDTH(21 *16));
	LCD_cmd(VERTEX2F(80 *16, 110 *16));
	LCD_cmd(VERTEX2F(200 *16, 130 *16));
	LCD_cmd(END());

	/* draw text */
	LCD_cmd(COLOR_RGB(255, 255, 255));
	LCD_cmd_text(35, 97, 31, 0, "Voyant 2");

	LCD_cmd_romfont(1, 32);
	LCD_cmd_text(400, 240, 1, OPT_CENTER, "Connect a Device");

	LCD_cmd_text(140, 455, 28, 0, "Menu");
	LCD_cmd_text(605, 455, 28, 0, "Volume");

	LCD_cmd(DISPLAY());
	LCD_cmd(CMD_SWAP);

	LCD_wr16(REG_CMD_WRITE,RAM_CMD_Offset);
	LCD_WaitCoprocessorDone();
}

/******************************* End of file *********************************/
