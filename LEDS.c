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
/* Contains functions to control the LEDs.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "gpio_v2.h"
#include "soc_AM335x.h"

#include "LEDS.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_LED
 *
 * Initializes the LEDs.
 *                                                                            */
/******************************************************************************/
void Init_LED(void)
{

}

/******************************************************************************/
/* LED_Show
 *
 * Sequences the LEDs.
 *                                                                            */
/******************************************************************************/
void LED_Show(void)
{
	unsigned long i;
	unsigned long j;

	for(j=0;j<10;j++)
	{
		/* .... */
		GPIOPinWrite(LED1_REGS, LED1_PIN, GPIO_PIN_LOW); // LED 1
		GPIOPinWrite(LED1_REGS, LED2_PIN, GPIO_PIN_LOW); // LED 2
		GPIOPinWrite(LED1_REGS, LED3_PIN, GPIO_PIN_LOW); // LED 3
		GPIOPinWrite(LED1_REGS, LED4_PIN, GPIO_PIN_LOW); // LED 4
		for(i=0;i<SPEED;i++);

		/* 0... */
		GPIOPinWrite(LED1_REGS, LED1_PIN, GPIO_PIN_HIGH); // LED 1
		GPIOPinWrite(LED1_REGS, LED2_PIN, GPIO_PIN_LOW); // LED 2
		GPIOPinWrite(LED1_REGS, LED3_PIN, GPIO_PIN_LOW); // LED 3
		GPIOPinWrite(LED1_REGS, LED4_PIN, GPIO_PIN_LOW); // LED 4
		for(i=0;i<SPEED;i++);

		/* .0.. */
		GPIOPinWrite(LED1_REGS, LED1_PIN, GPIO_PIN_LOW); // LED 1
		GPIOPinWrite(LED1_REGS, LED2_PIN, GPIO_PIN_HIGH); // LED 2
		GPIOPinWrite(LED1_REGS, LED3_PIN, GPIO_PIN_LOW); // LED 3
		GPIOPinWrite(LED1_REGS, LED4_PIN, GPIO_PIN_LOW); // LED 4
		for(i=0;i<SPEED;i++);

		/* ..0. */
		GPIOPinWrite(LED1_REGS, LED1_PIN, GPIO_PIN_LOW); // LED 1
		GPIOPinWrite(LED1_REGS, LED2_PIN, GPIO_PIN_LOW); // LED 2
		GPIOPinWrite(LED1_REGS, LED3_PIN, GPIO_PIN_HIGH); // LED 3
		GPIOPinWrite(LED1_REGS, LED4_PIN, GPIO_PIN_LOW); // LED 4
		for(i=0;i<SPEED;i++);

		/* ...0 */
		GPIOPinWrite(LED1_REGS, LED1_PIN, GPIO_PIN_LOW); // LED 1
		GPIOPinWrite(LED1_REGS, LED2_PIN, GPIO_PIN_LOW); // LED 2
		GPIOPinWrite(LED1_REGS, LED3_PIN, GPIO_PIN_LOW); // LED 3
		GPIOPinWrite(LED1_REGS, LED4_PIN, GPIO_PIN_HIGH); // LED 4
		for(i=0;i<SPEED;i++);
	}

	GPIOPinWrite(LED1_REGS, LED1_PIN, GPIO_PIN_LOW); // LED 1
	GPIOPinWrite(LED1_REGS, LED2_PIN, GPIO_PIN_LOW); // LED 2
	GPIOPinWrite(LED1_REGS, LED3_PIN, GPIO_PIN_LOW); // LED 3
	GPIOPinWrite(LED1_REGS, LED4_PIN, GPIO_PIN_LOW); // LED 4
}

/******************************* End of file *********************************/
