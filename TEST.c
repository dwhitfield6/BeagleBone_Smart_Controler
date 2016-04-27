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
/* Contains functions to control the test points.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "gpio_v2.h"
#include "hw_types.h"
#include "soc_AM335x.h"

#include "TEST.h"

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
/* Init_Test
 *
 * Initializes the LEDs.
 *                                                                            */
/******************************************************************************/
void Init_Test(void)
{
	TEST_Setpoint1(OFF);
	TEST_Setpoint1(ON);
	TEST_Setpoint2(OFF);
	TEST_Setpoint2(ON);
	TEST_Setpoint3(OFF);
	TEST_Setpoint3(ON);
	TEST_Togglepoint1();
	TEST_Togglepoint2();
	TEST_Togglepoint3();
	TEST_Togglepoint1();
	TEST_Togglepoint2();
	TEST_Togglepoint3();
}

/******************************************************************************/
/* TEST_Setpoint1
 *
 * Controls Test point 1.
 *                                                                            */
/******************************************************************************/
void TEST_Setpoint1(unsigned char state)
{
	if(state)
	{
		GPIOPinWrite(TP1_REGS, TP1_PIN, GPIO_PIN_HIGH); // TP 1
	}
	else
	{
		GPIOPinWrite(TP1_REGS, TP1_PIN, GPIO_PIN_LOW); // TP 1
	}
}

/******************************************************************************/
/* TEST_Setpoint2
 *
 * Controls Test point 2.
 *                                                                            */
/******************************************************************************/
void TEST_Setpoint2(unsigned char state)
{
	if(state)
	{
		GPIOPinWrite(TP2_REGS, TP2_PIN, GPIO_PIN_HIGH); // TP 2
	}
	else
	{
		GPIOPinWrite(TP2_REGS, TP2_PIN, GPIO_PIN_LOW); // TP 2
	}
}

/******************************************************************************/
/* TEST_Setpoint3
 *
 * Controls Test point 3.
 *                                                                            */
/******************************************************************************/
void TEST_Setpoint3(unsigned char state)
{
	if(state)
	{
		GPIOPinWrite(TP3_REGS, TP3_PIN, GPIO_PIN_HIGH); // TP 3
	}
	else
	{
		GPIOPinWrite(TP3_REGS, TP3_PIN, GPIO_PIN_LOW); // TP 3
	}
}

/******************************************************************************/
/* TEST_Togglepoint1
 *
 * Toggles Test point 1.
 *                                                                            */
/******************************************************************************/
void TEST_Togglepoint1(void)
{
	unsigned int state;

	state = HWREG(TP1_REGS + GPIO_DATAOUT);
	state &= (1L << TP1_PIN);

	if(state)
	{
		GPIOPinWrite(TP1_REGS, TP1_PIN, GPIO_PIN_LOW); // TP 1
	}
	else
	{
		GPIOPinWrite(TP1_REGS, TP1_PIN, GPIO_PIN_HIGH); // TP 1
	}
}

/******************************************************************************/
/* TEST_Togglepoint2
 *
 * Toggles Test point 2.
 *                                                                            */
/******************************************************************************/
void TEST_Togglepoint2(void)
{
	unsigned int state;

	state = HWREG(TP2_REGS + GPIO_DATAOUT);
	state &= (1L << TP2_PIN);

	if(state)
	{
		GPIOPinWrite(TP2_REGS, TP2_PIN, GPIO_PIN_LOW); // TP 2
	}
	else
	{
		GPIOPinWrite(TP2_REGS, TP2_PIN, GPIO_PIN_HIGH); // TP 2
	}
}

/******************************************************************************/
/* TEST_Togglepoint3
 *
 * Toggles Test point 3.
 *                                                                            */
/******************************************************************************/
void TEST_Togglepoint3(void)
{
	unsigned int state;

	state = HWREG(TP3_REGS + GPIO_DATAOUT);
	state &= (1L << TP3_PIN);

	if(state)
	{
		GPIOPinWrite(TP3_REGS, TP3_PIN, GPIO_PIN_LOW); // TP 3
	}
	else
	{
		GPIOPinWrite(TP3_REGS, TP3_PIN, GPIO_PIN_HIGH); // TP 3
	}
}

/******************************* End of file *********************************/
