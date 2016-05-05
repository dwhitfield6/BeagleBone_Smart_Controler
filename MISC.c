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
#include "dmtimer.h"
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
#include "TIMERS.h"

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

	double prescalerD;
	unsigned long prescalerL;

	prescalerD = MSC_Round(((double)TIMER_MODULE_INPUT_CLK * (double) US) / (16.0 * 1000000.0));
	prescalerL = (unsigned long) prescalerD;
	prescalerL = 0xFFFFFFFF - prescalerL;

	DMTimerCounterSet(SOC_DMTIMER_3_REGS,prescalerL);
	TMR_ClearMISCTimerFlag();
	DMTimerEnable(SOC_DMTIMER_3_REGS); // start the timer

	while(!TMR_GetMISCTimerFlag());
}

/******************************************************************************/
/* MSC_DelayNOP
 *
 * This function waits for the specified number of NOPs.
 * 																			  */
/******************************************************************************/
void MSC_DelayNOP(unsigned long nop)
{
	while(nop)
	{
		NOP();
		nop--;
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

/******************************************************************************/
/* MSC_EndianShort
 *
 * This function converts a 2 byte number from big endian to little endian or
 * vice versa.																  */
/******************************************************************************/
unsigned short MSC_EndianShort(unsigned short number)
{
    unsigned short temp1, temp2;

    temp1 = (unsigned short) (number & 0x00FF);
    temp2 = (unsigned short) (number & 0xFF00) >> 8;

    return (temp2 | (temp1 << 8));
}

/******************************************************************************/
/* MSC_EndianShortArray
 *
 * This function converts an array from big endian to little endian or
 * vice versa.																  */
/******************************************************************************/
unsigned short MSC_EndianShortArray(unsigned char* buffer)
{
    unsigned char temp1, temp2;

    temp1 = *buffer;
    buffer++;
    temp2 = *buffer;

    return (temp1 | ((unsigned int) temp2 << 8));
}

/******************************************************************************/
/* MSC_EndianLong
 *
 * This function converts a 4 byte number from big endian to little endian or
 * vice versa.																  */
/******************************************************************************/
unsigned long MSC_EndianLong(unsigned long number)
{
    unsigned long temp1, temp2, temp3, temp4;

    temp1 = (number & 0x000000FF);
    temp2 = (number & 0x0000FF00) >> 8;
    temp3 = (number & 0x00FF0000) >> 16;
    temp4 = (number & 0xFF000000) >> 24;

    return ((temp1 << 24) | (temp2 << 16) | (temp3 << 8) | temp4);
}

/******************************************************************************/
/* MSC_EndianLongArray
 *
 * This function converts an array from big endian to little endian or
 * vice versa.																  */
/******************************************************************************/
unsigned long MSC_EndianLongArray(unsigned char* buffer)
{
    unsigned char temp1, temp2, temp3, temp4;

    temp1 = *buffer;
    buffer++;
    temp2 = *buffer;
    buffer++;
    temp3 = *buffer;
    buffer++;
    temp4 = *buffer;

    return ((unsigned long)temp1 | ((unsigned long) temp2 << 8) | ((unsigned long) temp3 << 16) | ((unsigned long) temp4 << 24));
}

/******************************************************************************/
/* MSC_ReverseByte
 *
 * The function reads the value of 'This' and returns the reverse of the data.*/
/******************************************************************************/
unsigned char MSC_ReverseByte(unsigned char This)
{
    unsigned char temp=0;

    temp += (This & 0x01) << 7;
    temp += (This & 0x02) << 5;
    temp += (This & 0x04) << 3;
    temp += (This & 0x08) << 1;
    temp += (This & 0x10) >> 1;
    temp += (This & 0x20) >> 3;
    temp += (This & 0x40) >> 5;
    temp += (This & 0x80) >> 7;

    return temp;
}

/******************************************************************************/
/* MISC_CRC_CRC16
 *
 * The function calcualtes a single CRC-16-CCITT.                             */
/******************************************************************************/
unsigned short MISC_CRC_CRC16 (unsigned short data, unsigned short previous, unsigned char size)
{
	short i;
	unsigned long long Polynomial = (0x80000000 | (0x1021U << 15));
	unsigned long long temp = (unsigned long long) previous << 16;
	unsigned long long Shift = 0x8000000000000000;
	temp += data;
	temp <<= 32;
	Polynomial <<= 32;

	/* calculate CRC */
	if(size == FULL)
	{
		for(i=63;i >= 48;i--)
		{
			if(temp & Shift)
			{
				temp ^= Polynomial >> (63 - i);
			}
			Shift >>= 1;
		}
		temp >>= 32;
	}
	else
	{
		for(i=63;i >= 56;i--)
		{
			if(temp & Shift)
			{
				temp ^= Polynomial >> (63 - i);
			}
			Shift >>= 1;
		}
		temp >>= 40;
	}

	return (unsigned short) temp;
}

/******************************************************************************/
/* MISC_Calculate_CRC16
 *
 * The function calcualtes a full CRC-16-CCITT on a buffer of arbitrary size. */
/******************************************************************************/
unsigned short MISC_Calculate_CRC16(unsigned char* buffer, unsigned short bytes)
{
	unsigned long TempNewData;
	unsigned long TempOldData = 0;
	unsigned long i;

	if(bytes >= 2)
	{
		TempNewData = ((unsigned long)*buffer) << 8;
		buffer++;
		TempNewData += *buffer;
		buffer++;

		TempOldData = MISC_CRC_CRC16(TempNewData, 0, FULL);

		for(i=2; i<(bytes - 2);i+=2)
		{
			TempNewData = ((unsigned long)*buffer) << 8;
			buffer++;
			TempNewData += *buffer;
			buffer++;

			TempOldData = MISC_CRC_CRC16(TempNewData, TempOldData, FULL);
		}
	}
	if(bytes % 2)
	{
		/* Odd number of bytes */
		TempNewData = *buffer;
		TempNewData <<= 8;
		buffer++;
		TempOldData = MISC_CRC_CRC16(TempNewData, TempOldData, HALF);
	}
	TempOldData = MISC_CRC_CRC16(0, TempOldData, FULL);
	return TempOldData;
}

/******************************* End of file *********************************/
