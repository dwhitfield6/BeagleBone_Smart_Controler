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
/* Contains functions to control the internal Analog to digital converter.
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
#include "interrupt.h"
#include "pin_mux.h"
#include "soc_AM335x.h"
#include "tsc_Adc.h"

#include "ADC.h"
#include "GPIO.h"
#include "INTERRUPTS.h"
#include "LEDS.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
static unsigned char ADCReadingDoneFlag = FALSE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_ADC
 *
 * Initializes the ADC module.
 *                                                                            */
/******************************************************************************/
void Init_ADC(void)
{

    /* Enable the clock for ADC */
    TSCADCModuleClkConfig();

    /* set up the ADC interrupts */
    ADC_ConfigureInterrupt();

    /* Configures ADC to 3Mhz */
    TSCADCConfigureAFEClock(SOC_ADC_TSC_0_REGS, 24000000, 3000000);

    /* Disable Transistor bias */
    TSCADCTSTransistorConfig(SOC_ADC_TSC_0_REGS, TSCADC_TRANSISTOR_DISABLE);

    /* Disable Write Protection of Step Configuration regs*/
    TSCADCStepConfigProtectionDisable(SOC_ADC_TSC_0_REGS);

    /* Configure ADC to Single ended operation mode */
    TSCADCTSStepOperationModeControl(SOC_ADC_TSC_0_REGS, TSCADC_SINGLE_ENDED_OPER_MODE, 0);
    TSCADCTSStepOperationModeControl(SOC_ADC_TSC_0_REGS, TSCADC_SINGLE_ENDED_OPER_MODE, 1);
    TSCADCTSStepOperationModeControl(SOC_ADC_TSC_0_REGS, TSCADC_SINGLE_ENDED_OPER_MODE, 2);
    TSCADCTSStepOperationModeControl(SOC_ADC_TSC_0_REGS, TSCADC_SINGLE_ENDED_OPER_MODE, 3);

    /* Configure step to select Channel, refernce voltages */
    TSCADCTSStepConfig(SOC_ADC_TSC_0_REGS, 0, TSCADC_NEGATIVE_REF_ADCREFM, (unsigned int)VDD_5V0, (unsigned int)VDD_5V0, TSCADC_POSITIVE_REF_ADCREFP);
    TSCADCTSStepConfig(SOC_ADC_TSC_0_REGS, 1, TSCADC_NEGATIVE_REF_ADCREFM, (unsigned int)SYS_5V0, (unsigned int)SYS_5V0, TSCADC_POSITIVE_REF_ADCREFP);
    TSCADCTSStepConfig(SOC_ADC_TSC_0_REGS, 2, TSCADC_NEGATIVE_REF_ADCREFM, (unsigned int)VBAT, (unsigned int)VBAT, TSCADC_POSITIVE_REF_ADCREFP);
    TSCADCTSStepConfig(SOC_ADC_TSC_0_REGS, 3, TSCADC_NEGATIVE_REF_ADCREFM, (unsigned int)VDD_3V3, (unsigned int)VDD_3V3, TSCADC_POSITIVE_REF_ADCREFP);

    /* select fifo 0 or 1*/
    TSCADCTSStepFIFOSelConfig(SOC_ADC_TSC_0_REGS, 0, TSCADC_FIFO_0);
    TSCADCTSStepFIFOSelConfig(SOC_ADC_TSC_0_REGS, 1, TSCADC_FIFO_0);
    TSCADCTSStepFIFOSelConfig(SOC_ADC_TSC_0_REGS, 2, TSCADC_FIFO_0);
    TSCADCTSStepFIFOSelConfig(SOC_ADC_TSC_0_REGS, 3, TSCADC_FIFO_0);

    /* Configure ADC to one short mode */
    TSCADCTSStepModeConfig(SOC_ADC_TSC_0_REGS, 0, TSCADC_ONE_SHOT_SOFTWARE_ENABLED);
    TSCADCTSStepModeConfig(SOC_ADC_TSC_0_REGS, 1, TSCADC_ONE_SHOT_SOFTWARE_ENABLED);
    TSCADCTSStepModeConfig(SOC_ADC_TSC_0_REGS, 2, TSCADC_ONE_SHOT_SOFTWARE_ENABLED);
    TSCADCTSStepModeConfig(SOC_ADC_TSC_0_REGS, 3, TSCADC_ONE_SHOT_SOFTWARE_ENABLED);

    /* General purpose inputs */
    TSCADCTSModeConfig(SOC_ADC_TSC_0_REGS, TSCADC_GENERAL_PURPOSE_MODE);

    /* disable the step id saving */
    TSCADCStepIDTagConfig(SOC_ADC_TSC_0_REGS, 0);

    /* Clear the status of all interrupts */
    TSCADCIntStatusClear(SOC_ADC_TSC_0_REGS, 0x7FF);

    /* End of sequence interrupt is enable */
    TSCADCEventInterruptEnable(SOC_ADC_TSC_0_REGS, TSCADC_END_OF_SEQUENCE_INT);
}

/******************************************************************************/
/* ADC_ConfigureInterrupt
 *
 * Sets up the ADC interrupt.
 *                                                                            */
/******************************************************************************/
void ADC_ConfigureInterrupt(void)
{
	IntRegister(SYS_INT_ADC_TSC_GENINT, ADC_0_ISR);

	IntPrioritySet(SYS_INT_ADC_TSC_GENINT, 20, AINTC_HOSTINT_ROUTE_IRQ);

	IntSystemEnable(SYS_INT_ADC_TSC_GENINT);
}

/******************************************************************************/
/* ADC_ReadChannels
 *
 * Reads the ADC channels.
 *                                                                            */
/******************************************************************************/
void ADC_ReadChannels(void)
{
	ADC_ClearFinishedFlag();
    TSCADCIntStatusClear(SOC_ADC_TSC_0_REGS, 0x7FF);

    /* Enable steps */
    TSCADCConfigureStepEnable(SOC_ADC_TSC_0_REGS, 1, 1);
    TSCADCConfigureStepEnable(SOC_ADC_TSC_0_REGS, 2, 1);
    TSCADCConfigureStepEnable(SOC_ADC_TSC_0_REGS, 3, 1);
    TSCADCConfigureStepEnable(SOC_ADC_TSC_0_REGS, 4, 1);

    /* Enable the TSC_ADC_SS module*/
    TSCADCModuleStateSet(SOC_ADC_TSC_0_REGS, TSCADC_MODULE_ENABLE);

	while(!ADC_GetFinishedFlag());

    /* Disable the TSC_ADC_SS module*/
    TSCADCModuleStateSet(SOC_ADC_TSC_0_REGS, TSCADC_MODULE_DISABLE);
}

/******************************************************************************/
/* ADC_SetFinishedFlag
 *
 * Sets the ADC reading complete flag.
 *                                                                            */
/******************************************************************************/
void ADC_SetFinishedFlag(void)
{
	ADCReadingDoneFlag = TRUE;
}

/******************************************************************************/
/* ADC_ClearFinishedFlag
 *
 * Clears the ADC reading complete flag.
 *                                                                            */
/******************************************************************************/
void ADC_ClearFinishedFlag(void)
{
	ADCReadingDoneFlag = FALSE;
}

/******************************************************************************/
/* ADC_GetFinishedFlag
 *
 * Gets the ADC reading complete flag.
 *                                                                            */
/******************************************************************************/
unsigned char ADC_GetFinishedFlag(void)
{
	return ADCReadingDoneFlag;
}

/******************************* End of file *********************************/
