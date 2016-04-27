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

#include "GPIO.h"
#include "LEDS.h"
#include "MISC.h"
#include "SYSTEM.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned int dummy;
unsigned char MISC_Buffer[MISC_BUFFER_SIZE];

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* MSC_BufferCopy
 *
 * This function copies 'from' buffer to 'To' buffer.
 * 																			  */
/******************************************************************************/
void MSC_BufferCopy(void* From, void* To, unsigned short bytes)
{
    unsigned short i;
    unsigned char* tempFrom = (unsigned char*)From;
    unsigned char* tempTo   = (unsigned char*)To;

    for(i=0;i<bytes;i++)
    {
        *tempTo = *tempFrom;
        tempTo++;
        tempFrom++;
    }
}

/******************************************************************************/
/* MSC_LowercaseChar
 *
 * This function turns the character to lowercase.
 * 																			  */
/******************************************************************************/
unsigned char MSC_LowercaseChar(unsigned char Input)
{
    if((Input >= 65) && (Input <= 90 ))
    {
        return Input + 32;
    }
    else
    {
        return Input;
    }
}

/******************************************************************************/
/* MSC_DelayUS
 *
 * This function waits for the specified number of microseconds.
 * 																			  */
/******************************************************************************/
void MSC_DelayUS(unsigned long US)
{
	unsigned long i;
	unsigned long j;

	for(i=0;i<US;i++)
	{
		for(j=0;j<250;j++)
		{
			NOP();
		}
	}
}

/******************************************************************************/
/* MSC_Round
 *
 * This function rounds to the nearest whole number.						  */
/******************************************************************************/
double MSC_Round(double input)
{
    long temp = (long)(input + 0.5);

    return (double) temp;
}

/******************************************************************************/
/* MSC_HEXtoBCD
 *
 * This function converts from Hex to BCD.									  */
/******************************************************************************/
unsigned short MSC_HEXtoBCD(unsigned short input)
{
    unsigned short temp0;
    unsigned short temp1;
    unsigned short temp2;
    unsigned short temp3;
    unsigned short Value = input;

    temp3 = Value/1000;
    Value -= (temp3 * 1000);
    temp2 = Value / 100;
    Value -= (temp2 * 100);
    temp1 = Value / 10;
    Value -= (temp1 * 10);
    temp0 = Value;

    return (temp0 + (temp1 << 4) + (temp2 << 8) + (temp3 << 12));
}

/******************************************************************************/
/* MSC_BCDtoHEX
 *
 * This function converts from BCD to Hex.									  */
/******************************************************************************/
unsigned short MSC_BCDtoHEX(unsigned short input)
{
    unsigned short temp0;
    unsigned short temp1;
    unsigned short temp2;
    unsigned short temp3;
    unsigned short Value = input;

    temp3 = Value >> 12;
    Value -= (temp3 << 12);
    temp2 = Value >> 8;
    Value -= (temp2 << 8);
    temp1 = Value >> 4;
    Value -= (temp1 << 4);
    temp0 = Value;

    return (temp0 + (temp1 * 10) + (temp2 * 100) + (temp3 * 1000));
}

/******************************* End of file *********************************/
