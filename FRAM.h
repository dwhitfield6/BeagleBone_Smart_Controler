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

#ifndef _FRAM_H_
#define _FRAM_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "GUI.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* SYSTEM_SETTINGS_ADDRESS_START
 *
 * Address where we start the structure burn in FRAM.
 *                                                                            */
/******************************************************************************/
#define SYSTEM_SETTINGS_ADDRESS_START 0

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef struct t_system_settings
{
	/* touch screen calibration */
	TYPE_TOUCH_CALIBRATION TouchCalibration;
}TYPE_SYSTEM_SETTINGS;

typedef enum e_optcode
{
	WREN  	= 0x06, // Set write enable latch
	WRDI  	= 0x04, // Write disable
	RDSR  	= 0x05, // Read Status Register
	WRSR  	= 0x01, // Write Status Register
	READ  	= 0x03, // Read memory data
	WRITE  	= 0x02, // Write memory data
}ENUM_OPCODE;

typedef enum e_Fram_write_read
{
	FRAM_READ,			// read to FRAM
	FRAM_WRITE,			// write to FRAM
	FRAM_WRITE_READ,	// read and write to FRAM
}ENUM_FRAM_READ_WRITE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern TYPE_SYSTEM_SETTINGS CurrentSystemSettings;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_FRAM(void);
void FRAM_LoadSettings(TYPE_SYSTEM_SETTINGS* settings);
void FRAM_SaveSettings(TYPE_SYSTEM_SETTINGS* settings);
void FRAM_WriteEnable(void);
void FRAM_WriteDisable(void);
void FRAM_WriteStatusRegister(unsigned char StatusRegister);
unsigned char FRAM_ReadStatusRegister(void);
void FRAM_WriteMemory(unsigned long address, unsigned char* write, unsigned long bytes);
void FRAM_ReadMemory(unsigned long address, unsigned char* read, unsigned long bytes);
void FRAM_WriteRead(ENUM_OPCODE optcode, unsigned char* write, unsigned char* read,
					unsigned long bytes, ENUM_FRAM_READ_WRITE type, unsigned char ChipSelect);


#endif
/******************************* End of file *********************************/
