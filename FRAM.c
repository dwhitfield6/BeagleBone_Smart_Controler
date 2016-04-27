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
/* Contains functions to communicate with the FRAM device (FM25W256).
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "mcspi.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "beaglebone.h"
#include "FRAM.h"
#include "GPIO.h"
#include "GUI.h"
#include "MISC.h"
#include "SPI.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
TYPE_SYSTEM_SETTINGS CurrentSystemSettings =
{
	/* touch screen calibration */
	.TouchCalibration.Calibrated = FALSE,
	.TouchCalibration.TransformA = 1.0,
	.TouchCalibration.TransformB = 1.0,
	.TouchCalibration.TransformC = 1.0,
	.TouchCalibration.TransformD = 1.0,
	.TouchCalibration.TransformE = 1.0,
	.TouchCalibration.TransformF = 1.0,
};


/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_FRAM
 *
 * Initializes the FRAM device.
 *                                                                            */
/******************************************************************************/
void Init_FRAM(void)
{
	unsigned char StatusRegister = 0;

	while(!(StatusRegister & 0x02))
	{
		FRAM_WriteStatusRegister(0x02); // enable writes and turn off write protect
		FRAM_WriteEnable();
		StatusRegister = FRAM_ReadStatusRegister();
	}
	FRAM_Hold(FALSE);
	FRAM_WriteProtect(FALSE);
	FRAM_LoadSettings(&CurrentSystemSettings);
}

/******************************************************************************/
/* FRAM_LoadSettings
 *
 * Loads the system settings from FRAM.
 *                                                                            */
/******************************************************************************/
void FRAM_LoadSettings(TYPE_SYSTEM_SETTINGS* settings)
{
	FRAM_ReadMemory(SYSTEM_SETTINGS_ADDRESS_START, (unsigned char*)settings, sizeof(TYPE_SYSTEM_SETTINGS));
}

/******************************************************************************/
/* FRAM_SaveSettings
 *
 * Saves the system settings to FRAM.
 *                                                                            */
/******************************************************************************/
void FRAM_SaveSettings(TYPE_SYSTEM_SETTINGS* settings)
{
	FRAM_WriteMemory(SYSTEM_SETTINGS_ADDRESS_START, (unsigned char*)settings, sizeof(TYPE_SYSTEM_SETTINGS));
}

/******************************************************************************/
/* FRAM_WriteEnable
 *
 * Sets the Write enable bit.
 *                                                                            */
/******************************************************************************/
void FRAM_WriteEnable(void)
{
	unsigned char dummy = 0xFF;
	unsigned char read = 0;
	FRAM_WriteRead(WREN, &dummy, &read, 0, FRAM_WRITE, TRUE);
}

/******************************************************************************/
/* FRAM_WriteDisable
 *
 * Sets the Write disable bit.
 *                                                                            */
/******************************************************************************/
void FRAM_WriteDisable(void)
{
	unsigned char dummy = 0xFF;
	unsigned char read = 0;
	FRAM_WriteRead(WRDI, &dummy, &read, 0, FRAM_READ, TRUE);
}

/******************************************************************************/
/* FRAM_WriteStatusRegister
 *
 * Writes to the FRAM status register.
 *                                                                            */
/******************************************************************************/
void FRAM_WriteStatusRegister(unsigned char StatusRegister)
{
	unsigned char read = 0;
	unsigned char status = StatusRegister;
	FRAM_WriteEnable();
	FRAM_WriteRead(WRSR, &status, &read, 1, FRAM_WRITE, TRUE);
}

/******************************************************************************/
/* FRAM_ReadStatusRegister
 *
 * Reads from the FRAM status register.
 *                                                                            */
/******************************************************************************/
unsigned char FRAM_ReadStatusRegister(void)
{
	unsigned char dummy = 0xFF;
	unsigned char read = 0;
	FRAM_WriteRead(RDSR, &dummy, &read, 1, FRAM_READ, TRUE);
	return read;
}

/******************************************************************************/
/* FRAM_WriteMemory
 *
 * Writes data to memory.
 *                                                                            */
/******************************************************************************/
void FRAM_WriteMemory(unsigned long address, unsigned char* write, unsigned long bytes)
{
	unsigned char read = 0;
	unsigned long temp;
	unsigned long i;

	FRAM_WriteEnable();

	McSPICSAssert(LCD_FRAM_SPI_REGS, FRAM_CS);

	/* write the 15 bit starting address */
	temp = (address >> 8) & 0x7F;
	FRAM_WriteRead(WRITE, (unsigned char*) &temp , &read, 1, FRAM_WRITE, FALSE);
	temp = address;
	SPI_ReadWriteByte1((unsigned char)temp);

	/* write the buffer */
	for(i=0;i<bytes;i++)
	{
		SPI_ReadWriteByte1(*write);
		write++;
	}
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, FRAM_CS);
}


/******************************************************************************/
/* FRAM_ReadMemory
 *
 * Reads data from memory.
 *                                                                            */
/******************************************************************************/
void FRAM_ReadMemory(unsigned long address, unsigned char* read, unsigned long bytes)
{
	unsigned char dummy = 0;
	unsigned char temp;
	unsigned long i;

	McSPICSAssert(LCD_FRAM_SPI_REGS, FRAM_CS);

	/* write the 15 bit starting address */
	temp = (address >> 8) & 0x7F;
	FRAM_WriteRead(READ, &temp , &dummy, 1, FRAM_WRITE, FALSE);
	temp = address;
	SPI_ReadWriteByte1(temp);

	/* read the buffer */
	for(i=0;i<bytes;i++)
	{
		*read = SPI_ReadWriteByte1(0xFF);
		read++;
	}

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, FRAM_CS);
}

/******************************************************************************/
/* FRAM_WriteRead
 *
 * Performs a special read/write.
 *                                                                            */
/******************************************************************************/
void FRAM_WriteRead(ENUM_OPCODE optcode, unsigned char* write, unsigned char* read,
					unsigned long bytes, ENUM_FRAM_READ_WRITE type, unsigned char ChipSelect)
{
	unsigned long i;

	if(ChipSelect)
	{
		McSPICSAssert(LCD_FRAM_SPI_REGS, FRAM_CS);
	}
	dummy = SPI_ReadWriteByte1(optcode);
	for(i=0;i<bytes;i++)
	{
		*read = SPI_ReadWriteByte1(*write);
		if(type == FRAM_READ)
		{
			read++;
		}
		else if(FRAM_WRITE)
		{
			write++;
		}
		else
		{
			read++;
			write++;
		}
	}
	if(ChipSelect)
	{
		McSPICSDeAssert(LCD_FRAM_SPI_REGS, FRAM_CS);
	}
}

/******************************************************************************/
/* FRAM_WriteProtect
 *
 * Controls the write protect pin.
 *                                                                            */
/******************************************************************************/
void FRAM_WriteProtect(unsigned char state)
{
	if(state)
	{
		GPIOPinWrite(FRAM_WP_REGS, FRAM_WP_PIN, GPIO_PIN_LOW);
	}
	else
	{
		GPIOPinWrite(FRAM_WP_REGS, FRAM_WP_PIN, GPIO_PIN_HIGH);
	}
}

/******************************************************************************/
/* FRAM_Hold
 *
 * Controls the hold pin.
 *                                                                            */
/******************************************************************************/
void FRAM_Hold(unsigned char state)
{
	if(state)
	{
		GPIOPinWrite(FRAM_HOLD_REGS, FRAM_HOLD_PIN, GPIO_PIN_LOW);
	}
	else
	{
		GPIOPinWrite(FRAM_HOLD_REGS, FRAM_HOLD_PIN, GPIO_PIN_HIGH);
	}
}

/******************************* End of file *********************************/
