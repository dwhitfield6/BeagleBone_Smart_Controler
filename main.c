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
 * Add SD card code.
 * Add EMMC code.
 * Add FAT32 library and fix it up.
 * Add PWM (for IR).
 * Add IR decoding.
 * Add RF 315MHZ code.
 * Add BMP file creation from screen shot.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Contains main function.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "FT_Gpu.h"
#include "hw_control_AM335x.h"
#include "hs_mmcsdlib.h"
#include "hw_types.h"
#include "soc_AM335x.h"
#include "cache.h"

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
#include "USB.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
#pragma DATA_ALIGN(fileWrite, SOC_CACHELINE_SIZE);
static FIL fileWrite;

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main (void)
{
	volatile unsigned int initFlg = 1;
	volatile unsigned int i = 0;
	unsigned char result;
    unsigned int byteswritten = 0;
    unsigned char data1[256] = "David was here for testing.";


	unsigned char flags;
	unsigned char tags;
	unsigned char mask;
	unsigned long touchX;
	unsigned long touchY;

    /* Configure and enable the MMU. */
	MMU_ConfigAndEnable();

    /* Enable all levels of Cache. */
    //CacheEnable(CACHE_ALL);

    /* Initiate modules */
    Init_Modules();

    /* display the initial status */
    LED_Show();
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
		    			GUI_Backlight(ON);
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
			LCD_Interrupt(ON);
		}

    	/* check for Backlight timeout flag */
    	if(GUI_GetBacklightTimeout())
    	{
    		LCD_InteruptDisable(INTERRUPT_TAG);
    		GUI_Backlight(OFF);
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

		if(USB_GetUSBStatusFlag0())
		{
			GUI_ScreenRefresh();
			USB_ClearUSBStatusFlag0();
		}

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

		while(1)
		{
			if((HSMMCSDCardPresent(&ctrlInfo)) == 1)
			{
				if(initFlg)
				{
					HSMMCSDFsMount(0, &sdCard);
					result = f_open (&fileWrite, "file.txt", FA_WRITE | FA_CREATE_ALWAYS);
					result = f_write (&fileWrite, data1, 256, &byteswritten);
					result = f_close (&fileWrite);
					initFlg = 0;
					Init_USB();
				}
			}
			else
			{

				i = (i + 1) & 0xFFF;

				if(i == 1)
				{
					 //ConsoleUtilsPrintf("Please insert the card \n\r");
				}

				if(initFlg != 1)
				{
					 /* Reinitialize all the state variables */
					 callbackOccured = 0;
					 xferCompFlag = 0;
					 dataTimeout = 0;
					 cmdCompFlag = 0;
					 cmdTimeout = 0;

					 /* Initialize the MMCSD controller */
					 MMCSDCtrlInit(&ctrlInfo);

					 MMCSDIntEnable(&ctrlInfo);
				}

				initFlg = 1;
			}
			MSC_DelayUS(100);
		}
    }
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
