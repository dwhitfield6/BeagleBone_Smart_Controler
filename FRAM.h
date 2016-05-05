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
/* Pin Definitions			                                                  */
/******************************************************************************/
#define FRAM_WP_REGS	SOC_GPIO_0_REGS
#define FRAM_HOLD_REGS	SOC_GPIO_1_REGS

#define FRAM_WP_PIN		26
#define FRAM_HOLD_PIN 	12

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
/* FRAM_COUNT_TIMEOUT
 *
 * This is the number of loops that we wait for the FRAM.
 *                                                                            */
/******************************************************************************/
#define FRAM_COUNT_TIMEOUT 100

/******************************************************************************/
/* GOOD_TAG
 *
 * This is a tag for good FRAM data values.
 *                                                                            */
/******************************************************************************/
#define GOOD_TAG 0x3D

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef struct __attribute__((packed)) t_system_settings
{
	unsigned char GoodTag;

	/* touch screen calibration */
	TYPE_TOUCH_CALIBRATION TouchCalibration;

	unsigned short CRC16;
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
void FRAM_WriteProtect(unsigned char state);
void FRAM_Hold(unsigned char state);
unsigned long FRAM_CalculateCRC(TYPE_SYSTEM_SETTINGS* settings);
void FRAM_LoadDefaultSettings(TYPE_SYSTEM_SETTINGS* settings);
unsigned char FRAM_Test(void);

#endif
/******************************* End of file *********************************/
