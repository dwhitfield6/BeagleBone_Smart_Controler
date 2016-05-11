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
 *
 * Note**** the WP pin is pulled high (disabling write) so TP4 must be pulled
 *  low externally to burn a new serial number.
 *                                                                            */
/******************************************************************************/

#ifndef _EEPROM_H_
#define _EEPROM_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/******************************************************************************/
/* I2C_EEPROM_ADDRESS
 *
 * This is the I2C address for the EEPROM.
 * 							                                                  */
/******************************************************************************/
#define I2C_EEPROM_ADDRESS 0x50

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern unsigned char EEPROM_Contents[16];

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_EEPROM(void);
unsigned char EEPROM_ReadAddress(unsigned short address, unsigned char* data);
unsigned char EEPROM_WriteAddress(unsigned short address, unsigned char* data);
unsigned char EEPROM_ReadBuffer(unsigned short address, unsigned char* data, unsigned short bytes);
unsigned char EEPROM_WriteBuffer(unsigned short address, unsigned char* data, unsigned short bytes);

#endif
/******************************* End of file *********************************/
