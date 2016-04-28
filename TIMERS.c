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
#include "beaglebone.h"
#include "dmtimer.h"
#include "gpio_v2.h"
#include "interrupt.h"
#include "soc_AM335x.h"

#include "GUI.h"
#include "INTERRUPTS.h"
#include "I2C.h"
#include "MISC.h"
#include "TIMERS.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                            */
/******************************************************************************/
static unsigned char TMR_TagTimerEnabled = FALSE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned long TMR_NewScreenTagTimer = 0;
unsigned long TMR_AudioPlaybackTimer = 0;
unsigned long TMR_BacklightTimer = GUI_BACKLIGHT_TIMER;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_Timers
 *
 * Initializes the Timers.
 *                                                                            */
/******************************************************************************/
void Init_Timers(void)
{
	Init_Timer2();
}

/******************************************************************************/
/* Init_Timer2
 *
 * Initialize timer 2.
 *                                                                            */
/******************************************************************************/
void Init_Timer2(void)
{
	unsigned int InitialTimerCount;

	InitialTimerCount = TMR_CalculateReload(TIMER_MODULE_INPUT_CLK, 1000);	// 1 milisecond timer

    /* Reset the DMTimer module */
    HWREG(SOC_DMTIMER_2_REGS + DMTIMER_TIOCP_CFG) |= DMTIMER_TIOCP_CFG_SOFTRESET;
    while(DMTIMER_TIOCP_CFG_SOFTRESET == (HWREG(SOC_DMTIMER_2_REGS + DMTIMER_TIOCP_CFG) & DMTIMER_TIOCP_CFG_SOFTRESET));

	DMTimer2ModuleClkConfig();

    /* Load the counter with the initial count value */
    DMTimerCounterSet(SOC_DMTIMER_2_REGS, InitialTimerCount);

    /* Load the load register with the reload count value */
    DMTimerReloadSet(SOC_DMTIMER_2_REGS, InitialTimerCount);

    /* Configure the DMTimer for Auto-reload */
    DMTimerModeConfigure(SOC_DMTIMER_2_REGS, DMTIMER_AUTORLD_NOCMP_ENABLE);

    /* Enable the DMTimer interrupts */
    DMTimerIntEnable(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_EN_FLAG);

    /* Start the DMTimer */
    DMTimerEnable(SOC_DMTIMER_2_REGS);

    TMR_InterruptConfigure2();
}



/******************************************************************************/
/* TMR_InterruptConfigure2
 *
 * Configure the timer 2 interrupt.
 *                                                                            */
/******************************************************************************/
unsigned int TMR_CalculateReload(unsigned int timerinput, unsigned int timeroutput)
{
	unsigned int value;
	value = (unsigned int) MSC_Round((double) (timerinput / timeroutput));
	value = 0xFFFFFFFF - value;
	value -= 1;
	return value;
}

/******************************************************************************/
/* TMR_InterruptConfigure2
 *
 * Configure the tiemr 2 interrupt.
 *                                                                            */
/******************************************************************************/
void TMR_InterruptConfigure2(void)
{

    /* Registering DMTimerIsr */
    IntRegister(SYS_INT_TINT2, TMR_2_ISR);

    /* Set the priority */
    IntPrioritySet(SYS_INT_TINT2, 2, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enable the system interrupt */
    IntSystemEnable(SYS_INT_TINT2);
}

/******************************************************************************/
/* TMR_GetNewScreenTagTimerEnabled
 *
 * This gets the GUI tag timer enabled status.
 * 																			  */
/******************************************************************************/
unsigned char TMR_GetNewScreenTagTimerEnabled(void)
{
	return TMR_TagTimerEnabled;
}

/******************************************************************************/
/* TMR_SetNewScreenTagTimerEnabled
 *
 * This sets the GUI tag timer enabled status.
 * 																			  */
/******************************************************************************/
void TMR_SetNewScreenTagTimerEnabled(unsigned char state)
{
	if(state)
	{
		TMR_TagTimerEnabled = TRUE;
	}
	else
	{
		TMR_TagTimerEnabled = FALSE;
	}
}

/******************************************************************************/
/* TMR_ResetNewScreenTagTimer
 *
 * This resets the GUI tag timer count.
 * 																			  */
/******************************************************************************/
void TMR_ResetNewScreenTagTimer(void)
{
	TMR_NewScreenTagTimer = 0;
}

/******************************************************************************/
/* TMR_ResetNewScreenTagTimer
 *
 * This resets the GUI tag timer count.
 * 																			  */
/******************************************************************************/
void TMR_ResetAudioPlaybackTimer(void)
{
	TMR_AudioPlaybackTimer = 0;
}

/******************************************************************************/
/* TMR_ResetBacklightTimer
 *
 * This resets the backlight timer.
 * 																			  */
/******************************************************************************/
void TMR_ResetBacklightTimer(void)
{
	TMR_BacklightTimer = 0;
}

/******************************* End of file *********************************/
