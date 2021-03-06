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
#include <stdio.h>
#include <USB_MSC_DEVICE.h>
#include "beaglebone.h"
#include "FT_Gpu.h"
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "BITMAP_CHARLIE_BEACH.h"
#include "BITMAP_SD_Card_SYMBOL.h"
#include "BITMAP_TV_REMOTE.h"
#include "BITMAP_USB_SYMBOL.h"
#include "FRAM.h"
#include "GPIO.h"
#include "GUI.h"
#include "LCD.h"
#include "LEDS.h"
#include "MISC.h"
#include "RTCC.h"
#include "SD.h"
#include "TIMERS.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
static unsigned char GUI_CurrentTag = 0;
static unsigned char GUI_TagTimoutFlag = FALSE;
static unsigned char GUI_BacklightFlag = 0;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
ENUM_SCREEN_SELECT CurrentScreen;
ENUM_SCREEN_SELECT PreviousScreen;
ENUM_SCREEN_SELECT PreviousPreviousScreen;
pFunction p_PreviousScreen;
pFunction p_CurrentScreen;
TYPE_SCREEN Screens[MAX_SCREENS] =
{
	{
	/* main screen */
	.p_CurrentScreen = GUI_DrawHomeScreen,
	.NextScreen.p_NextScreen = {GUI_DrawTVRemoteScreen},
	.NextScreen.ScreenName = {SCREEN_TV_REMOTE},
	.NextScreen.TagButtons = {MAIN_SCREEN_TV_REMOTE},
	},

	{
	/* TV screen */
	.p_CurrentScreen = GUI_DrawTVRemoteScreen,
	.NextScreen.p_NextScreen =
		{
				GUI_DrawTVRemoteScreen, // TV_REMOTE_SCREEN_1
				GUI_DrawTVRemoteScreen, // TV_REMOTE_SCREEN_2
				GUI_DrawTVRemoteScreen, // TV_REMOTE_SCREEN_3
				GUI_DrawPreviousScreen, // TV_REMOTE_SCREEN_BACK
		},
	.NextScreen.ScreenName =
		{
				SCREEN_TV_REMOTE, 	// TV_REMOTE_SCREEN_1
				SCREEN_TV_REMOTE, 	// TV_REMOTE_SCREEN_2
				SCREEN_TV_REMOTE, 	// TV_REMOTE_SCREEN_3
				SCREEN_PREVIOUS, 	// TV_REMOTE_SCREEN_BACK
		},
	.NextScreen.TagButtons =
		{
				TV_REMOTE_SCREEN_1,		// TV_REMOTE_SCREEN_1
				TV_REMOTE_SCREEN_2,		// TV_REMOTE_SCREEN_2
				TV_REMOTE_SCREEN_3,		// TV_REMOTE_SCREEN_3
				TV_REMOTE_SCREEN_BACK,	// TV_REMOTE_SCREEN_BACK
		},
	},
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
	CurrentScreen = SCREEN_MAIN;
	PreviousScreen = SCREEN_MAIN;
	PreviousPreviousScreen = SCREEN_MAIN;
	p_PreviousScreen = GUI_DrawHomeScreen;
	p_CurrentScreen = GUI_DrawHomeScreen;
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

	/* rotate screen */
	LCD_cmd_setrotate(1);

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
	const TYPE_BITMAP_HEADER* header;

	RAM_CMD_Offset = LCD_rd16(REG_CMD_WRITE);

	LCD_cmd(CMD_DLSTART);
	LCD_cmd(CLEAR_COLOR_RGB(0,0,0));
	LCD_cmd(CLEAR(1,1,1));

	/* draw bitmap */
	header = &HEADER_CharlieBeach;
	LCD_cmd(BITMAP_HANDLE(0));
	LCD_cmd(BEGIN(BITMAPS));
	LCD_cmd(BITMAP_SOURCE(RAM_G));
	LCD_cmd(BITMAP_LAYOUT(header->Format, header->Stride, header->Height));
	LCD_cmd(BITMAP_LAYOUT_H((header->Stride >> 10), (header->Height >> 9))); 	// for linestrides larger than 1023 and heights larger than 512
	LCD_cmd(BITMAP_SIZE(BILINEAR, BORDER, BORDER, header->Width, header->Height));
	LCD_cmd(BITMAP_SIZE_H((header->Width >> 9), (header->Height >> 9)));		// for widths larger than 512 and heights larger than 512
	LCD_cmd(VERTEX2II(0, 0, 0, 0));
	LCD_cmd(END());

	/* draw the TV button */
	LCD_cmd( COLOR_RGB(128, 0, 0) );
	LCD_cmd( LINE_WIDTH(1 * 16) );
	LCD_cmd( BEGIN(RECTS) );
	LCD_cmd( TAG(MAIN_SCREEN_TV_REMOTE) );
	LCD_cmd( VERTEX2F(15 * 16,25 * 16) );
	LCD_cmd( VERTEX2F(95 * 16,115 * 16) );
	LCD_cmd(END());

	header = &HEADER_TVRemote;
	LCD_cmd( COLOR_RGB(255, 255, 255) );
	LCD_cmd(COLOR_A(255));
	LCD_cmd(BITMAP_HANDLE(1));
	LCD_cmd(BEGIN(BITMAPS));
	LCD_cmd(BITMAP_SOURCE(BITMAP_TV_REMOTE_RAM_G_LOCATION_START));
	LCD_cmd(BITMAP_LAYOUT(header->Format, header->Stride, header->Height));
	LCD_cmd(BITMAP_LAYOUT_H((header->Stride >> 10), (header->Height >> 9))); 	// for linestrides larger than 1023 and heights larger than 512
	LCD_cmd(BITMAP_SIZE(BILINEAR, BORDER, BORDER, header->Width, header->Height));
	LCD_cmd(BITMAP_SIZE_H((header->Width >> 9), (header->Height >> 9)));		// for widths larger than 512 and heights larger than 512
	LCD_cmd(VERTEX2II(20, 30, 1, 0));
	LCD_cmd(END());

	/* draw the time */
	LCD_cmd(COLOR_RGB(255, 132, 21));
	LCD_cmd(COLOR_A(255));
	GUI_DrawTime(330,10, 28);

	/* draw the USB status */
	if(USB_GetUSBStatus0() == USB_CONNECT)
	{
		header = &HEADER_USB_Symbol;
		LCD_cmd( COLOR_RGB(0, 0, 0) );
		LCD_cmd(COLOR_A(255));
		LCD_cmd(BITMAP_HANDLE(2));
		LCD_cmd(BEGIN(BITMAPS));
		LCD_cmd(BITMAP_SOURCE(BITMAP_USB_SYMBOL_RAM_G_LOCATION_START));
		LCD_cmd(BITMAP_LAYOUT(header->Format, header->Stride, header->Height));
		LCD_cmd(BITMAP_LAYOUT_H((header->Stride >> 10), (header->Height >> 9))); 	// for linestrides larger than 1023 and heights larger than 512
		LCD_cmd(BITMAP_SIZE(BILINEAR, BORDER, BORDER, header->Width, header->Height));
		LCD_cmd(BITMAP_SIZE_H((header->Width >> 9), (header->Height >> 9)));		// for widths larger than 512 and heights larger than 512
		LCD_cmd(VERTEX2II(446, 2, 2, 0));
		LCD_cmd(END());
	}

	/* draw the SD card status */
	if(SD_GetCardStatus() == CARD_PRESENT)
	{
		header = &HEADER_SD_Card_Symbol;
		LCD_cmd( COLOR_RGB(0, 0, 0) );
		LCD_cmd(COLOR_A(255));
		LCD_cmd(BITMAP_HANDLE(3));
		LCD_cmd(BEGIN(BITMAPS));
		LCD_cmd(BITMAP_SOURCE(BITMAP_SD_CARD_SYMBOL_RAM_G_LOCATION_START));
		LCD_cmd(BITMAP_LAYOUT(header->Format, header->Stride, header->Height));
		LCD_cmd(BITMAP_LAYOUT_H((header->Stride >> 10), (header->Height >> 9))); 	// for linestrides larger than 1023 and heights larger than 512
		LCD_cmd(BITMAP_SIZE(BILINEAR, BORDER, BORDER, header->Width, header->Height));
		LCD_cmd(BITMAP_SIZE_H((header->Width >> 9), (header->Height >> 9)));		// for widths larger than 512 and heights larger than 512
		LCD_cmd(VERTEX2II(390, 2, 3, 0));
		LCD_cmd(END());
	}

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
	LCD_cmd(BEGIN(BITMAPS));
	LCD_cmd(COLOR_RGB(7, 255, 133));
	LCD_cmd( POINT_SIZE(40 * 16) );
	LCD_cmd( BEGIN(POINTS) );
	LCD_cmd( TAG(TV_REMOTE_SCREEN_BACK) );
	LCD_cmd( VERTEX2II(80, 60, 0, 0) );
	LCD_cmd(END());

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
			LCD_wr_buffer(BITMAP_CHARLIE_BEACH_RAM_G_LOCATION_START, (unsigned char*)BITMAP_CharlieBeach, 0, header->Stride*header->Height); // load image to RAM_G
			break;
		case TV_REMOTE:
			header =  (TYPE_BITMAP_HEADER *)&HEADER_TVRemote;
			LCD_wr_buffer(BITMAP_TV_REMOTE_RAM_G_LOCATION_START, (unsigned char*)BITMAP_TVRemote, 0, header->Stride*header->Height); // load image to RAM_G
			break;
		case USB_SYMBOL:
			header =  (TYPE_BITMAP_HEADER *)&HEADER_USB_Symbol;
			LCD_wr_buffer(BITMAP_USB_SYMBOL_RAM_G_LOCATION_START, (unsigned char*)BITMAP_USB_Symbol, 0, header->Stride*header->Height); // load image to RAM_G
			break;
		case SD_CARD_SYMBOL:
			header =  (TYPE_BITMAP_HEADER *)&HEADER_SD_Card_Symbol;
			LCD_wr_buffer(BITMAP_SD_CARD_SYMBOL_RAM_G_LOCATION_START, (unsigned char*)BITMAP_SD_Card_Symbol, 0, header->Stride*header->Height); // load image to RAM_G
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

	if(CurrentSystemSettings.TouchCalibration.Calibrated)
	{
		GUI_LoadTouchCalibration(&CurrentSystemSettings.TouchCalibration);
	}
	else
	{
		GUI_CreateTouchCalibration(&CurrentSystemSettings.TouchCalibration);
		FRAM_SaveSettings(&CurrentSystemSettings);
	}

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
void GUI_DrawNextScreen(unsigned char tag)
{
	unsigned short i;

	if(tag != 0)
	{
		for(i=0;i<MAX_BUTTONS_PER_SCREEN;i++)
		{
			if( tag == Screens[CurrentScreen].NextScreen.TagButtons[i])
			{
	    		LCD_InteruptDisable(INTERRUPT_TAG);
	    		GUI_StartNewScreenTagTimer();
	    		GUI_SetCurrentTag(tag);
				PreviousScreen = CurrentScreen;
				PreviousPreviousScreen = PreviousScreen;
				p_CurrentScreen = Screens[CurrentScreen].NextScreen.p_NextScreen[i];
				p_CurrentScreen();
				if(Screens[CurrentScreen].NextScreen.ScreenName[i] != SCREEN_PREVIOUS)
				{
					if(PreviousPreviousScreen != CurrentScreen)
					{
						p_PreviousScreen = Screens[CurrentScreen].p_CurrentScreen;
					}
					CurrentScreen = Screens[CurrentScreen].NextScreen.ScreenName[i];
				}
				break;
			}
		}
	}
}

/******************************************************************************/
/* GUI_DrawPreviousScreen
 *
 * This function navigates to the previous screen.
 * 																			  */
/******************************************************************************/
void GUI_DrawPreviousScreen(void)
{
	p_PreviousScreen();
	CurrentScreen = PreviousScreen;
	PreviousScreen = SCREEN_MAIN;
	PreviousPreviousScreen = PreviousScreen;
	p_PreviousScreen = GUI_DrawHomeScreen;
}

/******************************************************************************/
/* GUI_LoadTouchCalibration
 *
 * This function loads the screen calibration.
 * 																			  */
/******************************************************************************/
void GUI_LoadTouchCalibration(TYPE_TOUCH_CALIBRATION* calibration)
{
	if(calibration->Calibrated)
	{
		LCD_wr32(REG_TOUCH_TRANSFORM_A, (unsigned int)calibration->TransformA);
		LCD_wr32(REG_TOUCH_TRANSFORM_B, (unsigned int)calibration->TransformB);
		LCD_wr32(REG_TOUCH_TRANSFORM_C, (unsigned int)calibration->TransformC);
		LCD_wr32(REG_TOUCH_TRANSFORM_D, (unsigned int)calibration->TransformD);
		LCD_wr32(REG_TOUCH_TRANSFORM_E, (unsigned int)calibration->TransformE);
		LCD_wr32(REG_TOUCH_TRANSFORM_F, (unsigned int)calibration->TransformF);
	}
}

/******************************************************************************/
/* GUI_CreateTouchCalibration
 *
 * This function does a screen calibration.
 * 																			  */
/******************************************************************************/
void GUI_CreateTouchCalibration(TYPE_TOUCH_CALIBRATION* calibration)
{
	unsigned long temp;

	RAM_CMD_Offset = LCD_rd16(REG_CMD_WRITE);

	LCD_cmd(CMD_DLSTART);
	LCD_cmd(CLEAR_COLOR_RGB(0,0,0));
	LCD_cmd(CLEAR(1,1,1));

	LCD_cmd_text(240, 50, 27, OPT_CENTER, "Please tap on the dot");
	LCD_cmd_calibrate(3);

	LCD_cmd(DISPLAY());
	LCD_cmd(CMD_SWAP);

	LCD_wr16(REG_CMD_WRITE,RAM_CMD_Offset);
	LCD_WaitCoprocessorDone();

	calibration->Calibrated = TRUE;
	temp = LCD_rd32(REG_TOUCH_TRANSFORM_A);
	calibration->TransformA = temp;

	temp = LCD_rd32(REG_TOUCH_TRANSFORM_B);
	calibration->TransformB = temp;

	temp = LCD_rd32(REG_TOUCH_TRANSFORM_C);
	calibration->TransformC = temp;

	temp = LCD_rd32(REG_TOUCH_TRANSFORM_D);
	calibration->TransformD = temp;

	temp = LCD_rd32(REG_TOUCH_TRANSFORM_E);
	calibration->TransformE = temp;

	temp = LCD_rd32(REG_TOUCH_TRANSFORM_F);
	calibration->TransformF = temp;

}

/******************************************************************************/
/* GUI_StartNewScreenTagTimer
 *
 * This starts the timer controlling the touch access.
 * 																			  */
/******************************************************************************/
void GUI_StartNewScreenTagTimer(void)
{
	TMR_SetNewScreenTagTimerEnabled(FALSE);
	TMR_ResetNewScreenTagTimer();
	TMR_SetNewScreenTagTimerEnabled(TRUE);
}

/******************************************************************************/
/* GUI_SetTagTimoutFlag
 *
 * This sets the flag indicating the tag timer has ended and a new tag can
 *  be touched.
 * 																			  */
/******************************************************************************/
void GUI_SetTagTimoutFlag(void)
{
	GUI_TagTimoutFlag = TRUE;
}

/******************************************************************************/
/* GUI_ClearTagTimoutFlag
 *
 * This clears the flag indicating the tag timer has ended and a new tag can
 *  be touched.
 * 																			  */
/******************************************************************************/
void GUI_ClearTagTimoutFlag(void)
{
	GUI_TagTimoutFlag = FALSE;
}

/******************************************************************************/
/* GUI_GetTagTimoutFlag
 *
 * This gets the flag indicating the tag timer has ended and a new tag can
 *  be touched.
 * 																			  */
/******************************************************************************/
unsigned char GUI_GetTagTimoutFlag(void)
{
	return GUI_TagTimoutFlag;
}

/******************************************************************************/
/* GUI_ScreenRefresh
 *
 * Refresh the screen in the case of a time update...etc.
 * 																			  */
/******************************************************************************/
void GUI_ScreenRefresh(void)
{
	p_CurrentScreen();
}

/******************************************************************************/
/* GUI_DrawTime
 *
 * Refresh the screen in the case of a time update...etc.
 * 																			  */
/******************************************************************************/
void GUI_DrawTime(unsigned short x, unsigned short y, unsigned short font)
{
	RTC_GetTimeDate(&CurrentTimeDate);
	if(CurrentTimeDate.Time.AM_PM == AM)
	{
		sprintf((char*)MISC_Buffer, "%d:%02d:%02d AM", CurrentTimeDate.Time.Hour, CurrentTimeDate.Time.Minute, CurrentTimeDate.Time.Second);
	}
	else
	{
		sprintf((char*)MISC_Buffer, "%d:%02d:%02d PM", CurrentTimeDate.Time.Hour, CurrentTimeDate.Time.Minute, CurrentTimeDate.Time.Second);
	}
	LCD_cmd_text(x, y, font, OPT_CENTER, MISC_Buffer);
}

/******************************************************************************/
/* GUI_Backlight
 *
 * Controls the Backlight.
 * 																			  */
/******************************************************************************/
void GUI_Backlight(unsigned char state)
{
	if(state)
	{
		LCD_wr8(REG_PWM_DUTY, BackLightSetting);
	}
	else
	{
		LCD_wr8(REG_PWM_DUTY, 0);
	}
}

/******************************************************************************/
/* GUI_SetBacklightTimeout
 *
 * This sets the Backlight timeout flag indicating turn off the backlight.
 * 																			  */
/******************************************************************************/
void GUI_SetBacklightTimeout(void)
{
	GUI_BacklightFlag = TRUE;
}

/******************************************************************************/
/* GUI_ClearBacklightTimeout
 *
 * This clears the Backlight timeout flag indicating turn off the backlight.
 * 																			  */
/******************************************************************************/
void GUI_ClearBacklightTimeout(void)
{
	GUI_BacklightFlag = FALSE;
}

/******************************************************************************/
/* GUI_GetBacklightTimeout
 *
 * This gets the Backlight timeout flag indicating turn off the backlight.
 * 																			  */
/******************************************************************************/
unsigned char GUI_GetBacklightTimeout(void)
{
	return GUI_BacklightFlag;
}

/******************************************************************************/
/* GUI_SetCurrentTag
 *
 * This sets the current tag value.
 * 																			  */
/******************************************************************************/
void GUI_SetCurrentTag(unsigned char tag)
{
	GUI_CurrentTag = tag;
}

/******************************************************************************/
/* GUI_GetCurrentTag
 *
 * This gets the current tag value.
 * 																			  */
/******************************************************************************/
unsigned char GUI_GetCurrentTag(void)
{
	return GUI_CurrentTag;
}

/******************************* End of file *********************************/
