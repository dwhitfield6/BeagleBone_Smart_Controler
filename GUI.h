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
#define MAX_BUTTONS_PER_SCREEN 1

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
	SD_CARD 		= 1,
	TV_REMOTE 		= 2,
}ENUM_RAMG_ITEM;

typedef enum e_screen_selection
{
	SCREEN_MAIN 			= 0,
	SCREEN_TV_REMOTE 		= 1,
}ENUM_SCREEN_SELECT;

typedef struct t_screen
{
	pFunction p_CurrentScreen;
	unsigned char ScreenButtons;
	pFunction p_NextScreen[MAX_BUTTONS_PER_SCREEN];
	ENUM_SCREEN_SELECT NextScreen[MAX_BUTTONS_PER_SCREEN];
}TYPE_SCREEN;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern ENUM_SCREEN_SELECT CurrentScreen;
extern TYPE_SCREEN Screens[MAX_SCREENS];

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
void GUI_UpdateScreen(unsigned char tag);

#endif
/******************************* End of file *********************************/
