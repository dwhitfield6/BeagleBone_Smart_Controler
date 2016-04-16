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
/* Contains functions for the command structure.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "hw_types.h"

#include "CMD.h"
#include "MISC.h"
#include "UART.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                            */
/******************************************************************************/
static unsigned char Process_Command = FALSE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

const COMMANDTYPE Commands[] =
{
    {"?", CMD_Help,},
};


unsigned char NumCommand = 0;
unsigned char Misc_Buffer[MISC_BUFFER_SIZE];
unsigned char Command_String[LARGEST_COMMAND];

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* InitCMD
 *
 * Initializes the command structure.
 *                                                                            */
/******************************************************************************/
void InitCMD(void)
{
    NumCommand = sizeof(Commands) / sizeof(COMMANDTYPE);
}

/******************************************************************************/
/* CMD_CheckMatch
 *
 * Checks for a command match.
 *                                                                            */
/******************************************************************************/
unsigned char CMD_CheckMatch(unsigned char* received, const COMMANDTYPE* commands, unsigned char size)
{
    unsigned char i,j;
    unsigned char* buffer;

    for(j=0;j<NumCommand;j++)
    {
        buffer = received;
        for(i=0;i<size;i++)
        {
            if(MSC_LowercaseChar(*buffer) == commands[j].Command[i] || commands[j].Command[i] == 0 || commands[j].Command[i] == '~')
            {
                if(commands[j].Command[i] == 0 || commands[j].Command[i] == '~')
                {
                    /* Found a match, now check to make sure that a CR LN follows */
					if(*buffer == 0 || commands[j].Command[i] == '~')
					{
						/* we have a match */
						(*commands[j].Function) ();
						return PASS;
					}
					else
					{
						break;
					}
                }
                buffer++;
            }
            else
            {
                break;
            }
        }
    }
    return FAIL;
}

/******************************************************************************/
/* CMD_HelpOpen
 *
 * This function prints the open mode help menu.
 *																			  */
/******************************************************************************/
void CMD_Help(void)
{
	UART_PrintString("\r\nCommand Help Menu\r\n\r\n");
}

/******************************************************************************/
/* CMD_SetNewCommand
 *
 * Sets the new command flag.
 *																			  */
/******************************************************************************/
void CMD_SetNewCommand(void)
{
	Process_Command = TRUE;
}

/******************************************************************************/
/* CMD_ClearNewCommand
 *
 * Clears the new command flag.
 *																			  */
/******************************************************************************/
void CMD_ClearNewCommand(void)
{
	Process_Command = FALSE;
}

/******************************************************************************/
/* CMD_GetNewCommand
 *
 * Gets the new command flag.
 *																			  */
/******************************************************************************/
unsigned char CMD_GetNewCommand(void)
{
	return Process_Command;
}

/******************************* End of file *********************************/
