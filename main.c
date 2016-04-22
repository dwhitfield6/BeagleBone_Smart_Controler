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
/* Contains main function.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "FT_Gpu.h"
#include "hw_control_AM335x.h"
#include "hw_types.h"
#include "soc_AM335x.h"
#include "cache.h"

#include "CMD.h"
#include "GUI.h"
#include "LCD.h"
#include "LEDS.h"
#include "MEMORY.h"
#include "SYSTEM.h"
#include "UART.h"

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

    /* Enable all levels of Cache. */
    CacheEnable(CACHE_ALL);

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
	    		NOP();
	    	}

	    	/* check for tag flag */
	    	if((flags & INTERRUPT_TAG) && (mask & INTERRUPT_TAG))
	    	{
	    		/* there was a touch interrupt */
	    		LCD_InteruptDisable(INTERRUPT_TAG);
	    		GUI_StartNewScreenTagTimer();
	    		tags = LCD_rd8(REG_TOUCH_TAG); // read interrupt flags
	    		GUI_DrawNextScreen(tags);
	    	}
			LCD_ClearInterruptFlag();
			LCD_Interrupt(ON);
		}
    }
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
