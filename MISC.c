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
#include "SYSTEM.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned int dummy;

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

/******************************* End of file *********************************/
