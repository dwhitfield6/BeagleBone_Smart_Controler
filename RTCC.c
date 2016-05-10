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
/* Contains functions to control the internal RTC.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_cm_wkup.h"
#include "hw_cm_per.h"
#include "hw_cm_rtc.h"
#include "hw_rtc.h"
#include "hw_types.h"
#include "interrupt.h"
#include "soc_AM335x.h"
#include "rtc.h"

#include "INTERRUPTS.h"
#include "MISC.h"
#include "RTCC.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
static unsigned char RTC_flag = FALSE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
TYPE_TIME_DATE CurrentTimeDate;
TYPE_TIME_DATE DefaultTimeDate =
{
		.Time.Second = 12,
		.Time.Minute = 56,
		.Time.Hour = 5,
		.Time.AM_PM = AM,
		.Date.DayOfWeek = WEDNESDAY,
		.Date.Day = 27,
		.Date.Month = APRIL,
		.Date.Year = 2016,
};

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_RTC
 *
 * Initializes the internal RTC.
 *                                                                            */
/******************************************************************************/
void Init_RTC(void)
{
    /* set up RTC clock */
    RTC_ModuleClkConfig();

	/* Disabling Write Protection for RTC registers.*/
	RTCWriteProtectDisable(SOC_RTC_0_REGS);

	/* Selecting External Clock source for RTC. */
	RTC32KClkSourceSelect(SOC_RTC_0_REGS, RTC_EXTERNAL_CLK_SRC_SELECT);

	/* Enabling RTC to receive the Clock inputs. */
	RTC32KClkClockControl(SOC_RTC_0_REGS, RTC_32KCLK_ENABLE);

	HWREG(SOC_RTC_0_REGS + RTC_CTRL) |= RTC_CTRL_HOURMODE; // 12 hour mode

	/* Enable the RTC module. */
	RTCEnable(SOC_RTC_0_REGS);

	RTC_SetTimeDate(&DefaultTimeDate);

	/* Set the 32KHz counter to run. */
	RTCRun(SOC_RTC_0_REGS);

	/* Configure the interrupt */
	RTC_InterruptConfigure();

	/* Enabling RTC interrupts. Configuring RTC to interrupt every minute.*/
	HWREG(SOC_RTC_0_REGS + RTC_INTERRUPT) = 0;
	RTCIntTimerEnable(SOC_RTC_0_REGS, RTC_INT_EVERY_SECOND);
}

/******************************************************************************/
/* RTC_InterruptConfigure
 *
 * Sets up the RTC interrupt.
 *                                                                            */
/******************************************************************************/
void RTC_InterruptConfigure(void)
{
    /* Registering the Interrupt Service Routine(ISR). */
    IntRegister(SYS_INT_RTCINT, RTC_ISR);

    /* Setting the priority for the system interrupt in AINTC. */
    IntPrioritySet(SYS_INT_RTCINT, RTC_INTERRUPT_PRIORITY, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the system interrupt in AINTC. */
    IntSystemEnable(SYS_INT_RTCINT);
}

/******************************************************************************/
/* RTC_SetFlag
 *
 * Sets the RTC flag.
 *                                                                            */
/******************************************************************************/
void RTC_SetFlag(void)
{
	RTC_flag = TRUE;
}

/******************************************************************************/
/* RTC_ClearFlag
 *
 * Clears the RTC flag.
 *                                                                            */
/******************************************************************************/
void RTC_ClearFlag(void)
{
	RTC_flag = FALSE;
}

/******************************************************************************/
/* RTC_GetFlag
 *
 * Gets the RTC flag.
 *                                                                            */
/******************************************************************************/
unsigned char RTC_GetFlag(void)
{
	return RTC_flag;
}

/******************************************************************************/
/* RTC_SetTimeDate
 *
 * Sets the RTC time and date.
 *                                                                            */
/******************************************************************************/
void RTC_SetTimeDate(TYPE_TIME_DATE* current)
{
	unsigned int status;

	/* get the RTC run status */
	status = RTCRunStatusGet(SOC_RTC_0_REGS);

    /* Stop the RTC */
	RTCStop(SOC_RTC_0_REGS);

    /* Writing to Second register */
    HWREG(SOC_RTC_0_REGS + RTC_SECOND) = (unsigned long) MSC_HEXtoBCD((unsigned short)current->Time.Second);

    /* Writing to Minute register */
    HWREG(SOC_RTC_0_REGS + RTC_MINUTE) = (unsigned long) MSC_HEXtoBCD((unsigned short)current->Time.Minute);

    /* Writing to Hour register */
    HWREG(SOC_RTC_0_REGS + RTC_HOUR) = (unsigned long) MSC_HEXtoBCD((unsigned short)current->Time.Hour);

    if(current->Time.AM_PM)
    {
    	HWREG(SOC_RTC_0_REGS + RTC_HOUR) |= RTC_HOUR_MERIDIEM;
    }

    /* Writing to Day of week register */
    HWREG(SOC_RTC_0_REGS + RTC_DOTW) = current->Date.DayOfWeek;

    /* Writing to Day register */
    HWREG(SOC_RTC_0_REGS + RTC_DAY) = (unsigned long) MSC_HEXtoBCD((unsigned short)current->Date.Day);

    /* Writing to Month register */
    HWREG(SOC_RTC_0_REGS + RTC_MONTH) = (unsigned long) MSC_HEXtoBCD((unsigned short)current->Date.Month);

    /* Writing to Year register */
    HWREG(SOC_RTC_0_REGS + RTC_YEAR) = (unsigned long) MSC_HEXtoBCD((unsigned short)current->Date.Year - 2000);

    if(status)
    {
    	RTCRun(SOC_RTC_0_REGS);
    }
}

/******************************************************************************/
/* RTC_GetTimeDate
 *
 * Gets the RTC time and date.
 *                                                                            */
/******************************************************************************/
void RTC_GetTimeDate(TYPE_TIME_DATE* current)
{
    /* Read Second register.*/
    current->Time.Second = (unsigned char) MSC_BCDtoHEX((unsigned short) (HWREG(SOC_RTC_0_REGS + RTC_SECOND) & 0x7FL));

    /* Read Minute register.*/
    current->Time.Minute = (unsigned char) MSC_BCDtoHEX((unsigned short) (HWREG(SOC_RTC_0_REGS + RTC_MINUTE) & 0x7FL));

    /* Read Hour register.*/
    current->Time.Hour = (unsigned char) MSC_BCDtoHEX((unsigned short) (HWREG(SOC_RTC_0_REGS + RTC_HOUR) & 0x3FL));

    if(HWREG(SOC_RTC_0_REGS + RTC_HOUR) & RTC_HOUR_MERIDIEM)
    {
    	current->Time.AM_PM = PM;
    }
    else
    {
    	current->Time.AM_PM = AM;
    }

    /* Read Day of week */
    current->Date.DayOfWeek = (ENUM_DAY_OF_WEEK) HWREG(SOC_RTC_0_REGS + RTC_DOTW);

    /* Read Day */
    current->Date.Day = (unsigned char) MSC_BCDtoHEX((unsigned short) (HWREG(SOC_RTC_0_REGS + RTC_DAY) & 0x3FL));

    /* Read Day */
    current->Date.Month = (ENUM_MONTH) MSC_BCDtoHEX((unsigned short) (HWREG(SOC_RTC_0_REGS + RTC_MONTH) & 0x1FL));

    /* Read Year */
    current->Date.Year = MSC_BCDtoHEX((unsigned short) (HWREG(SOC_RTC_0_REGS + RTC_YEAR) & 0xFFL)) + 2000;
}

/******************************************************************************/
/* RTC_ModuleClkConfig
 *
 * Sets up the RTC clock.
 *                                                                            */
/******************************************************************************/
void RTC_ModuleClkConfig(void)
{
    /* Performing System Clock configurations for RTC. */

    /* Writing to MODULEMODE field of CM_RTC_RTC_CLKCTRL register. */
    HWREG(SOC_CM_RTC_REGS + CM_RTC_RTC_CLKCTRL) |=
          CM_RTC_RTC_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_RTC_RTC_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_RTC_REGS + CM_RTC_RTC_CLKCTRL) &
           CM_RTC_RTC_CLKCTRL_MODULEMODE));

    /* Writing to CLKTRCTRL field of CM_RTC_CLKSTCTRL register. */
    HWREG(SOC_CM_RTC_REGS + CM_RTC_CLKSTCTRL) |=
          CM_RTC_CLKSTCTRL_CLKTRCTRL_SW_WKUP;

    /* Waiting for CLKTRCTRL field to reflect the written value. */
    while(CM_RTC_CLKSTCTRL_CLKTRCTRL_SW_WKUP !=
          (HWREG(SOC_CM_RTC_REGS + CM_RTC_CLKSTCTRL) &
           CM_RTC_CLKSTCTRL_CLKTRCTRL));

    /* Checking the RTC related registers for desired values being set. */

    /*
    ** Waiting for IDLEST field in CM_RTC_RTC_CLKCTRL register to attain the
    ** desired value.
    */
    while((CM_RTC_RTC_CLKCTRL_IDLEST_FUNC <<
           CM_RTC_RTC_CLKCTRL_IDLEST_SHIFT) !=
           (HWREG(SOC_CM_RTC_REGS + CM_RTC_RTC_CLKCTRL) &
           CM_RTC_RTC_CLKCTRL_IDLEST));

    /*
    ** Waiting for CLKACTIVITY_L4_RTC_GCLK field in CM_RTC_CLKSTCTRL register
    ** to attain the desired value.
    */
    while(CM_RTC_CLKSTCTRL_CLKACTIVITY_L4_RTC_GCLK !=
          (HWREG(SOC_CM_RTC_REGS + CM_RTC_CLKSTCTRL) &
           CM_RTC_CLKSTCTRL_CLKACTIVITY_L4_RTC_GCLK));

    /*
    ** Waiting for CLKACTIVITY_RTC_32KCLK field in CM_RTC_CLKSTCTRL register
    ** to attain the desired value.
    */
    while(CM_RTC_CLKSTCTRL_CLKACTIVITY_RTC_32KCLK !=
          (HWREG(SOC_CM_RTC_REGS + CM_RTC_CLKSTCTRL) &
           CM_RTC_CLKSTCTRL_CLKACTIVITY_RTC_32KCLK));
}
/******************************* End of file *********************************/
