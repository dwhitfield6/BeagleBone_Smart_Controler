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
/* Contains functions to configure and control the internal timers.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifndef _TIMERS_H_
#define _TIMERS_H_

/******************************************************************************/
/* Pin Definitions			                                                  */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define TIMER_MODULE_INPUT_CLK      (24000000u)

/******************************************************************************/
/* GUI_NEW_SCREEN_TAG_TIMER
 *
 * This is the number of milliseconds to delay between a screen change and
 *  the next touch activation.
 *                                                                            */
/******************************************************************************/
#define GUI_NEW_SCREEN_TAG_TIMER 200

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern unsigned long TMR_NewScreenTagTimer;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_Timers(void);
void Init_Timer2(void);
unsigned int TMR_CalculateReload(unsigned int timerinput, unsigned int timeroutput);
void TMR_InterruptConfigure2(void);
unsigned char TMR_GetNewScreenTagTimerEnabled(void);
void TMR_SetNewScreenTagTimerEnabled(unsigned char state);
void TMR_ResetNewScreenTagTimer(void);

#endif
/******************************* End of file *********************************/
