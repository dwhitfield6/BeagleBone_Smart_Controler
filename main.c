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
/* TODOs
 * ---------------------------------------------------------------------------
 * Fix WAV playing.
 * Add PWM (for IR).
 * Add IR decoding.
 * Add RF 315MHZ code.
 * Add BMP file creation from screen shot.
 * Dont do USB MSC device as SD card if one is no present.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Contains main function.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <string.h>
#include "cache.h"
#include "FT_Gpu.h"
#include "hw_control_AM335x.h"
#include "hs_mmcsdlib.h"
#include "hw_types.h"
#include "interrupt.h"
#include "soc_AM335x.h"
#include "usblibpriv.h"

#include "AUDIO.h"
#include "CMD.h"
#include "GUI.h"
#include "LCD.h"
#include "LEDS.h"
#include "MEMORY.h"
#include "MISC.h"
#include "RTCC.h"
#include "SD.h"
#include "SYSTEM.h"
#include "TEST.h"
#include "TIMERS.h"
#include "UART.h"
#include "USB_MSC_DEVICE.h"
#include "USB_MSC_HOST.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main (void)
{
	unsigned char flags;
	unsigned char tags;
	unsigned char mask;
	unsigned long touchX;
	unsigned long touchY;

    /* Configure and enable the MMU. */
	MMU_ConfigAndEnable();

    /* Enable instruction Cache. */
    CacheEnable(CACHE_ICACHE);

    /* Initiate modules */
    Init_Modules();

    /* display the initial status */
    UART_PrintString(CRLN);
    UART_PrintString(PROJECT_NAME);
    UART_PrintString(CRLN);
	UART_PrintString(CRLN);
	UART_PrintString("> ");

    while(1)
    {
    	/* toggle testpoint for timing */
    	TEST_Togglepoint2();

    	/* check for a new UART command */
		if(CMD_GetNewCommand())
		{
			if(!CMD_CheckMatch(Command_String, Commands, LARGEST_COMMAND))
			{
				UART_PrintString(CRLN);
				UART_PrintString("Invalid command");
				UART_PrintString(CRLN);
			}
			UART_PrintString(CRLN);
			UART_PrintString("> ");
			CMD_ClearNewCommand();
		}

		/* check for LCD interrupt flag */
		if(LCD_GetInterruptFlag())
		{
	    	mask = LCD_rd8(REG_INT_MASK); // read interrupt mask
	    	flags = LCD_rd8(REG_INT_FLAGS); // read interrupt flags

	    	/* check for touch flag */
	    	if((flags & INTERRUPT_TOUCH) && (mask & INTERRUPT_TOUCH))
	    	{
	    		/* there was a touch interrupt */
	    		touchY = LCD_rd32(REG_TOUCH_SCREEN_XY);
	    		touchX = touchY >> 16;
	    		touchY &= 0x0000FFFF;
	    		if((touchX < FT_DispWidth) && (touchY < FT_DispHeight))
	    		{
	    			if(LCD_rd8(REG_PWM_DUTY) == 0)
	    			{
	    				/* the screen is off */
	    				GUI_StartNewScreenTagTimer();
		    			GUI_Backlight(TRUE);
	    			}
	    			TMR_ResetBacklightTimer();
	    		}
	    	}

	    	/* check for tag flag */
	    	if((flags & INTERRUPT_TAG) && (mask & INTERRUPT_TAG))
	    	{
	    		/* there was a touch interrupt */
	    		tags = LCD_rd8(REG_TOUCH_TAG); // read interrupt flags
	    		GUI_DrawNextScreen(tags);
	    	}
			LCD_ClearInterruptFlag();
			LCD_Interrupt(TRUE);
		}

    	/* check for Backlight timeout flag */
    	if(GUI_GetBacklightTimeout())
    	{
    		LCD_InteruptDisable(INTERRUPT_TAG);
    		GUI_Backlight(FALSE);
    		GUI_ClearBacklightTimeout();
    	}

		/* check for tag timer timeout */
		if(GUI_GetTagTimoutFlag())
		{
			LCD_InteruptEnable(INTERRUPT_TAG);
			GUI_ClearTagTimoutFlag();
		}

		/* check for an RTC time update */
		if(RTC_GetFlag())
		{
			GUI_ScreenRefresh();
			RTC_ClearFlag();
		}

		/* process a USB event */
		if(USB_GetUSBStatusFlag0())
		{
			GUI_ScreenRefresh();
			USB_ClearUSBStatusFlag0();
		}

		/* process an Audio event */
		if(AUD_GetPlayingFlag())
		{
			/* audio playback is occuring */
			if(AUD_GetTimoutFlag())
			{
				if(!AUD_IsWAVPlaying())
				{
					/* WAV file is not playing */
					if(AUD_GetPlaybackFinishedFlag())
					{
						/* the WAV playback completed */
						AUD_AmpShutdown(TRUE);
						AUD_StopWAV();
						AUD_ClearPlayingFlag();
					}
					else
					{
						/* audio is not currently playing so start playing */
						AUD_TransferWAVToRAMG(AUDIO_BUFFER_RAM_G_SIZE);
						AUD_AmpShutdown(FALSE);
						LCD_wr8(REG_PLAYBACK_PLAY,1);				// start the audio playback
					}
				}
				else
				{
					AUD_TransferWAVToRAMG(AUDIO_BYTES_TO_MOVE_RAM_G);
				}
				AUD_ClearTimoutFlag();
			}
		}

		/* process an SD card event */
#ifdef USE_SD_CARD
		/* check for SD card activity */
		if(((SD_IsCardInserted()) && (SD_GetCardStatus() == CARD_NOTPRESENT)) || (!(SD_IsCardInserted()) && (SD_GetCardStatus() == CARD_PRESENT)))
		{
			/* check for card activity */
			SD_SetCardActionFlag();
		}

		/* check for card Action needed */
		if(SD_GetCardActionFlag())
		{
			/* there was a card removed or inserted */
			if(SD_IsCardInserted())
			{
				SD_CardInit();
#ifndef USE_RAM_DISK
				if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_SD)
				{
					Init_USB0();
				}
#endif
				SD_SetCardStatus(CARD_PRESENT);
				GUI_ScreenRefresh();
			}
			else
			{
				fat_devices[DRIVE_NUM_SD].initDone = 0;
				SD_SetCardStatus(CARD_NOTPRESENT);
				GUI_ScreenRefresh();
			}
			SD_ClearCardActionFlag();
		}
#endif

		/* process a USB host event */
		USB_HOST_Process();
		if(g_eState == STATE_DEVICE_READY)
		{
			USB_Host_Test();
			TEST_Togglepoint3();
		}
    }
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
