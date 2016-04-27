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
/* Contains miscellaneous functions.
 *                                                                            */
/******************************************************************************/

#ifndef _MISC_H_
#define _MISC_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern unsigned int dummy;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void MSC_BufferCopy(void* From, void* To, unsigned short bytes);
unsigned char MSC_LowercaseChar(unsigned char Input);
void MSC_DelayUS(unsigned long US);
double MSC_Round(double input);

#endif
/******************************* End of file *********************************/
