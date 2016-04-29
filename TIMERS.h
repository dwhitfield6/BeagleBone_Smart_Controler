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
/* Global Variable                                                            */
/******************************************************************************/
extern unsigned long TMR_NewScreenTagTimer;
extern unsigned long TMR_AudioPlaybackTimer;
extern unsigned long TMR_BacklightTimer;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_Timers(void);
void Init_Timer2(void);
void Init_Timer3(void);
unsigned int TMR_CalculateReload(unsigned int timerinput, unsigned int timeroutput);
void TMR_InterruptConfigure2(void);
void TMR_InterruptConfigure3(void);
unsigned char TMR_GetNewScreenTagTimerEnabled(void);
void TMR_SetNewScreenTagTimerEnabled(unsigned char state);
void TMR_ResetNewScreenTagTimer(void);
void TMR_ResetAudioPlaybackTimer(void);
void TMR_ResetBacklightTimer(void);
void TMR_SetMISCTimerFlag(void);
void TMR_ClearMISCTimerFlag(void);
unsigned char TMR_GetMISCTimerFlag(void);

#endif
/******************************* End of file *********************************/
