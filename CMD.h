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

#ifndef _CMD_H_
#define _CMD_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include <stdarg.h>
#include <stdio.h>

/******************************************************************************/
/* LARGEST_COMMAND
 *
 * This is greatest number of command characters.
 * 			          				                                          */
/******************************************************************************/
#define LARGEST_COMMAND 10

/******************************************************************************/
/* MISC_BUFFER_SIZE
 *
 * This is greatest number of characters in the misc buffer.
 * 			          				                                          */
/******************************************************************************/
#define MISC_BUFFER_SIZE 1000


/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef void (*pFunction)(void); // function pointer

typedef struct Type_command
{
    char Command[LARGEST_COMMAND];      // User Command
    pFunction Function;                 // Function to point to
}COMMANDTYPE;


/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern unsigned char NumCommand;
extern unsigned char Misc_Buffer[MISC_BUFFER_SIZE];
extern const COMMANDTYPE Commands[];
extern unsigned char Command_String[LARGEST_COMMAND];

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void InitCMD(void);
unsigned char CMD_CheckMatch(unsigned char* received, const COMMANDTYPE* commands, unsigned char size);
void CMD_Help(void);
void CMD_SetNewCommand(void);
void CMD_ClearNewCommand(void);
unsigned char CMD_GetNewCommand(void);

#endif
/******************************* End of file *********************************/
