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
/* Contains functions to comunicate with the onboard EEPROM (24LC32A).
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <string.h>

#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "EEPROM.h"
#include "GPIO.h"
#include "I2C.h"
#include "MISC.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned char EEPROM_Contents[16];

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_EEPROM
 *
 * Initializes the EEPROM.
 *                                                                            */
/******************************************************************************/
void Init_EEPROM(void)
{
	EEPROM_ReadBuffer(0, EEPROM_Contents, sizeof(EEPROM_Contents));
}

/******************************************************************************/
/* EEPROM_ReadAddress
 *
 * The function reads a byte from the EEPROM address.
 * 																			  */
/******************************************************************************/
unsigned char EEPROM_ReadAddress(unsigned short address, unsigned char* data)
{
	unsigned char write[2];

	write[0] = address >> 8;
	write[1] = (unsigned char) address;
	if(I2C_SendReceiveData0(I2C_EEPROM_ADDRESS, write, 2, data, 1))
	{
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************/
/* EEPROM_WriteAddress
 *
 * The function writes a byte from the EEPROM address.
 * 																			  */
/******************************************************************************/
unsigned char EEPROM_WriteAddress(unsigned short address, unsigned char* data)
{
	unsigned char write[3];

	write[0] = address >> 8;
	write[1] = (unsigned char) address;
	write[2] = *data;
	if(I2C_SendReceiveData0(I2C_EEPROM_ADDRESS, write, 3, (unsigned char *) &dummy, 0))
	{
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************/
/* EEPROM_ReadBuffer
 *
 * The function reads a buffer from the EEPROM address.
 * 																			  */
/******************************************************************************/
unsigned char EEPROM_ReadBuffer(unsigned short address, unsigned char* data, unsigned short bytes)
{
	unsigned short temp_address = address;
	while(bytes)
	{
		if(!EEPROM_ReadAddress(temp_address, data))
		{
			return FALSE;
		}
		temp_address++;
		data++;
		bytes--;
	}
	return TRUE;
}

/******************************************************************************/
/* EEPROM_WriteBuffer
 *
 * The function writes a buffer to the EEPROM address.
 * 																			  */
/******************************************************************************/
unsigned char EEPROM_WriteBuffer(unsigned short address, unsigned char* data, unsigned short bytes)
{
	unsigned short temp_address = address;
	while(bytes)
	{
		if(!EEPROM_WriteAddress(temp_address, data))
		{
			return FALSE;
		}
		temp_address++;
		data++;
		bytes--;
	}
	return TRUE;
}

/******************************* End of file *********************************/
