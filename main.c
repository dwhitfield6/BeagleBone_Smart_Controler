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
#include "hw_control_AM335x.h"
#include "soc_AM335x.h"
#include "cache.h"

#include "CMD.h"
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
    }
}

/*-----------------------------------------------------------------------------/
 End of File
/-----------------------------------------------------------------------------*/
