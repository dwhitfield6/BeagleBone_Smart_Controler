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

#ifndef _GUI_H_
#define _GUI_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "SYSTEM.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* MAX_BUTTONS_PER_SCREEN
 *
 * This is the maximum buttons (tags) per screen.
 *                                                                            */
/******************************************************************************/
#define MAX_BUTTONS_PER_SCREEN 10

/******************************************************************************/
/* MAX_SCREENS
 *
 * This is the maximum number of screens.
 *                                                                            */
/******************************************************************************/
#define MAX_SCREENS 20

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef enum e_ramg_item
{
	CHARLIE_BEACH 	= 0,
	TV_REMOTE 		= 1,
}ENUM_RAMG_ITEM;

typedef enum e_screen_selection
{
	SCREEN_MAIN 			= 0,
	SCREEN_TV_REMOTE 		= 1,
	SCREEN_PREVIOUS 		= 255,
}ENUM_SCREEN_SELECT;

typedef struct t_next_screen
{
	pFunction p_NextScreen[MAX_BUTTONS_PER_SCREEN];
	ENUM_SCREEN_SELECT ScreenName[MAX_BUTTONS_PER_SCREEN];
	unsigned char TagButtons[MAX_BUTTONS_PER_SCREEN];
}TYPE_NEXT_SCREEN;

typedef struct t_screen
{
	pFunction p_CurrentScreen;
	TYPE_NEXT_SCREEN NextScreen;
}TYPE_SCREEN;

typedef enum e_main_screen_tags
{
	MAIN_SCREEN_TV_REMOTE 	= 1,
}ENUM_MAIN_SCREEN_TAGS;

typedef enum e_tv_screen_tags
{
	TV_REMOTE_SCREEN_1 		= 1,
	TV_REMOTE_SCREEN_2 		= 2,
	TV_REMOTE_SCREEN_3 		= 3,
	TV_REMOTE_SCREEN_BACK 	= 4,
}ENUM_TV_REMOTE_SCREEN_TAGS;

typedef struct t_touch_calibration
{
	unsigned char Calibrated;
	double TransformA;
	double TransformB;
	double TransformC;
	double TransformD;
	double TransformE;
	double TransformF;
}TYPE_TOUCH_CALIBRATION;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern ENUM_SCREEN_SELECT CurrentScreen;
extern ENUM_SCREEN_SELECT PreviousScreen;
extern ENUM_SCREEN_SELECT PreviousPreviousScreen;
extern TYPE_SCREEN Screens[MAX_SCREENS];
extern pFunction p_PreviousScreen;
extern pFunction p_CurrentScreen;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_GUI(void);
void GUI_DrawInitialScreen(void);
void GUI_DrawInitialScreenProgress(unsigned char progress);
void GUI_DrawHomeScreen(void);
void GUI_DrawTVRemoteScreen(void);
void GUI_LoadItemToRAMG(ENUM_RAMG_ITEM item);
void GUI_TouchConfig(void);
void GUI_DrawNextScreen(unsigned char tag);
void GUI_DrawPreviousScreen(void);
void GUI_LoadTouchCalibration(TYPE_TOUCH_CALIBRATION* calibration);
void GUI_CreateTouchCalibration(TYPE_TOUCH_CALIBRATION* calibration);
void GUI_StartNewScreenTagTimer(void);
void GUI_SetTagTimoutFlag(void);
void GUI_ClearTagTimoutFlag(void);
unsigned char GUI_GetTagTimoutFlag(void);
void GUI_ScreenRefresh(void);
void GUI_DrawTime(unsigned short x, unsigned short y, unsigned short font);

#endif
/******************************* End of file *********************************/
