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

#ifndef _RTCC_H_
#define _RTCC_H_

/******************************************************************************/
/* Pin Definitions			                                                  */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef enum e_am_pm
{
	AM 		= 0,
	PM 		= 1,
}ENUM_AM_PM;

typedef enum e_day_of_week
{
	SUNDAY 		= 0,
	MONDAY 		= 1,
	TUESDAY 	= 2,
	WEDNESDAY 	= 3,
	THURSDAY 	= 4,
	FRIDAY 		= 5,
	SATURDAY 	= 6,
}ENUM_DAY_OF_WEEK;

typedef enum e_month
{
	JANUARY 	= 1,
	FEBRUARY 	= 2,
	MARCH 		= 3,
	APRIL 		= 4,
	MAY 		= 5,
	JUNE 		= 6,
	JULY 		= 7,
	AUGUST 		= 8,
	SEPTEMBER 	= 9,
	OCTOBER 	= 10,
	NOVEMBER 	= 11,
	DECEMBER 	= 12,
}ENUM_MONTH;

typedef struct t_time
{
	unsigned char Second;
	unsigned char Minute;
	unsigned char Hour;
	ENUM_AM_PM AM_PM;
}TYPE_TIME;

typedef struct t_date
{
	ENUM_DAY_OF_WEEK DayOfWeek;
	unsigned char Day;
	ENUM_MONTH Month;
	unsigned short Year;

}TYPE_DATE;

typedef struct t_time_date
{
	TYPE_TIME Time;
	TYPE_DATE Date;
}TYPE_TIME_DATE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern TYPE_TIME_DATE CurrentTimeDate;
extern TYPE_TIME_DATE DefaultTimeDate;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_RTC(void);
void RTC_InterruptConfigure(void);
void RTC_SetFlag(void);
void RTC_ClearFlag(void);
unsigned char RTC_GetFlag(void);
void RTC_SetTimeDate(TYPE_TIME_DATE* current);
void RTC_GetTimeDate(TYPE_TIME_DATE* current);
void RTC_ModuleClkConfig(void);

#endif
/******************************* End of file *********************************/
