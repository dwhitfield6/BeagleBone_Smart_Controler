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
/* Contains functions monitors and controls the Power flow.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "ADC.h"
#include "GPIO.h"
#include "LEDS.h"
#include "POWER.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
TYPE_VOLTAGE_RAILS Voltage;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_Power
 *
 * Initializes the voltage measurements.
 *                                                                            */
/******************************************************************************/
void Init_Power(void)
{
	PWR_ReadVoltages();
}

/******************************************************************************/
/* PWR_ReadVoltage
 *
 * Takes a voltage measurement.
 *                                                                            */
/******************************************************************************/
void PWR_ReadVoltages(void)
{
	ADC_ReadChannels();
	Voltage.Rail_VDD_5V0 	= ((((R7+R8)/(R8)) * ((POSITIVE_REFERENCE * ((double) Voltage.ADCCounts_VDD_5V0)) / ADC_RESOLUTION)));
	Voltage.Rail_SYS_5V0 	= ((((R5+R6)/(R6)) * ((POSITIVE_REFERENCE * ((double) Voltage.ADCCounts_SYS_5V0)) / ADC_RESOLUTION)));
	Voltage.Rail_VBAT 		= ((((R9+R10)/(R10)) * ((POSITIVE_REFERENCE * ((double) Voltage.ADCCounts_VBAT)) / ADC_RESOLUTION)));
	Voltage.Rail_VDD_3V3 	= ((((R163+R164)/(R164)) * ((POSITIVE_REFERENCE * ((double) Voltage.ADCCounts_VDD_3V3)) / ADC_RESOLUTION)));
}

/******************************* End of file *********************************/
