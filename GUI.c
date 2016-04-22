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

#include "BITMAP_CHARLIE_BEACH.h"
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
ENUM_SCREEN_SELECT CurrentScreen = SCREEN_MAIN;
ENUM_SCREEN_SELECT PreviousScreen = SCREEN_MAIN;
TYPE_SCREEN Screens[MAX_SCREENS] =
{
		{
		/* main screen */
		.p_CurrentScreen = GUI_DrawHomeScreen,
		.ScreenButtons = 1,
		.p_NextScreen = {GUI_DrawTVRemoteScreen},
		.NextScreen = {SCREEN_TV_REMOTE},
		}
};

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
	LCD_cmd_text(CENTER_X, CENTER_Y, 31, OPT_CENTER, "Initializing");

	LCD_cmd(END());
	LCD_cmd(DISPLAY());
	LCD_cmd(CMD_SWAP);

	LCD_wr16(REG_CMD_WRITE,RAM_CMD_Offset);
	LCD_WaitCoprocessorDone();
}

/******************************************************************************/
/* GUI_DrawInitialScreenProgress
 *
 * This function draws a progress bar on the loading screen (0-100).
 * 																			  */
/******************************************************************************/
void GUI_DrawInitialScreenProgress(unsigned char progress)
{
	RAM_CMD_Offset = LCD_rd16(REG_CMD_WRITE);

	if(progress > 100)
	{
		progress = 100;
	}

	LCD_cmd(CMD_DLSTART);

	/* draw background black */
	LCD_cmd(CLEAR_COLOR_RGB(0,0,0));
	LCD_cmd(CLEAR(1,1,1));

	/* draw text */
	LCD_cmd(COLOR_RGB(155, 155, 0));
	LCD_cmd_text(CENTER_X, CENTER_Y, 31, OPT_CENTER, "Initializing");

	/* draw line */
	LCD_cmd(COLOR_RGB(7, 123, 133));
	LCD_cmd(BEGIN(LINES));
	LCD_cmd(LINE_WIDTH(274));
	LCD_cmd(VERTEX2F(40 *16, 195 *16));
	LCD_cmd(VERTEX2F(434 *16, 195 *16));
	LCD_cmd(END());

	/* draw progress bar */
	LCD_cmd_bgcolor(CREATE_COLOR_RBG(0, 0, 0));
	LCD_cmd(COLOR_RGB(7, 123, 133));
	LCD_cmd_progress(40, 180, 394, 30, OPT_FLAT, progress, 100);

	LCD_cmd(END());
	LCD_cmd(DISPLAY());
	LCD_cmd(CMD_SWAP);

	LCD_wr16(REG_CMD_WRITE,RAM_CMD_Offset);
	LCD_WaitCoprocessorDone();
}

/******************************************************************************/
/* GUI_DrawHomeScreen
 *
 * This function draws the home screen.
 * 																			  */
/******************************************************************************/
void GUI_DrawHomeScreen(void)
{
	const TYPE_BITMAP_HEADER* header = &HEADER_CharlieBeach;

	RAM_CMD_Offset = LCD_rd16(REG_CMD_WRITE);

	LCD_cmd(CMD_DLSTART);
	LCD_cmd(CLEAR_COLOR_RGB(0,0,0));
	LCD_cmd(CLEAR(1,1,1));

	/* draw bitmap */
	LCD_cmd(BITMAP_HANDLE(0));
	LCD_cmd(BEGIN(BITMAPS));
	LCD_cmd(BITMAP_SOURCE(RAM_G));
	LCD_cmd(BITMAP_LAYOUT(header->Format, header->Stride, header->Height));
	LCD_cmd(BITMAP_LAYOUT_H((header->Stride >> 10), (header->Height >> 9))); 	// for linestrides larger than 1023 and heights larger than 512
	LCD_cmd(BITMAP_SIZE(BILINEAR, BORDER, BORDER, header->Width, header->Height));
	LCD_cmd(BITMAP_SIZE_H((header->Width >> 9), (header->Height >> 9)));		// for widths larger than 512 and heights larger than 512
	LCD_cmd(VERTEX2II(0, 0, 0, 0));
	LCD_cmd(END());

	/* draw the button */
	LCD_cmd(TAG(0));
	LCD_cmd_bgcolor(CREATE_COLOR_RBG(255, 0, 0));
	LCD_cmd(COLOR_RGB(7, 255, 133));
	LCD_cmd_button(10, 10, 100, 75, 31, 0, "TV Remote");

	LCD_cmd(DISPLAY());
	LCD_cmd(CMD_SWAP);

	LCD_wr16(REG_CMD_WRITE,RAM_CMD_Offset);
	LCD_WaitCoprocessorDone();
}

/******************************************************************************/
/* GUI_DrawTVRemoteScreen
 *
 * This function draws the TV remote screen.
 * 																			  */
/******************************************************************************/
void GUI_DrawTVRemoteScreen(void)
{
	RAM_CMD_Offset = LCD_rd16(REG_CMD_WRITE);

	LCD_cmd(CMD_DLSTART);
	LCD_cmd(CLEAR_COLOR_RGB(0,0,0));
	LCD_cmd(CLEAR(1,1,1));

	/*~~~~~~~~~~~ draw the buttons ~~~~~~~~~~~ */

	/* draw 1 */
	LCD_cmd(TAG(1));
	LCD_cmd_bgcolor(CREATE_COLOR_RBG(255, 0, 0));
	LCD_cmd(COLOR_RGB(7, 255, 133));
	LCD_cmd_button(10, 10, 100, 75, 31, 0, "1");

	/* draw 2 */
	LCD_cmd(TAG(2));
	LCD_cmd_bgcolor(CREATE_COLOR_RBG(255, 0, 0));
	LCD_cmd(COLOR_RGB(7, 255, 133));
	LCD_cmd_button(120, 10, 100, 75, 31, 0, "2");

	/* draw 3 */
	LCD_cmd(TAG(3));
	LCD_cmd_bgcolor(CREATE_COLOR_RBG(255, 0, 0));
	LCD_cmd(COLOR_RGB(7, 255, 133));
	LCD_cmd_button(230, 10, 100, 75, 31, 0, "3");

	LCD_cmd(DISPLAY());
	LCD_cmd(CMD_SWAP);

	LCD_wr16(REG_CMD_WRITE,RAM_CMD_Offset);
	LCD_WaitCoprocessorDone();
}

/******************************************************************************/
/* GUI_LoadItemToRAMG
 *
 * This function loads the items to RAM_G.
 * 																			  */
/******************************************************************************/
void GUI_LoadItemToRAMG(ENUM_RAMG_ITEM item)
{
	TYPE_BITMAP_HEADER* header;
	switch(item)
	{
		case CHARLIE_BEACH:
			header =  (TYPE_BITMAP_HEADER *)&HEADER_CharlieBeach;
			LCD_wr_buffer(RAM_G, (unsigned char*)BITMAP_CharlieBeach, header->Stride*header->Height); // load image to RAM_G
			break;
	}
}

/******************************************************************************/
/* GUI_TouchConfig
 *
 * This function enables, configures, and calibrates the Touch Screen.
 * 																			  */
/******************************************************************************/
void GUI_TouchConfig(void)
{
	unsigned long temp;

	LCD_wr32(CLEAR_TAG(0), 0);

	/* turn on the touch screen */
	temp = LCD_rd32(REG_TOUCH_MODE);
	if(!(temp &0x3))
	{
		LCD_wr32(REG_TOUCH_MODE, 0x3);
	}

	/* test the touch pressure */
	temp = LCD_rd32(REG_TOUCH_RZ);
	temp = LCD_rd32(REG_TOUCH_RZ);
	LCD_wr32(REG_TOUCH_RZTHRESH, 0x1000);

	LCD_InteruptEnable(INTERRUPT_TOUCH);
	LCD_InteruptEnable(INTERRUPT_TAG);
}

/******************************************************************************/
/* GUI_UpdateScreen
 *
 * This function navigates to the next screen based on the tag value (the
 *  button that was pressed).
 * 																			  */
/******************************************************************************/
void GUI_UpdateScreen(unsigned char tag)
{
	if(tag <= Screens[CurrentScreen].ScreenButtons)
	{
		PreviousScreen = CurrentScreen;
		Screens[CurrentScreen].p_NextScreen[tag]();
		CurrentScreen = Screens[CurrentScreen].NextScreen[tag];
	}
}

/******************************* End of file *********************************/
