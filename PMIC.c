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
/* Contains functions to configure and control the PMIC (TPS65217C).
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "gpio_v2.h"
#include "hw_types.h"
#include "soc_AM335x.h"

#include "I2C.h"
#include "MISC.h"
#include "PMIC.h"

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
/* Init_PMIC
 *
 * Initializes the PMIC registers.
 *                                                                            */
/******************************************************************************/
void Init_PMIC(void)
{
	unsigned char temp;

	PMIC_ReadRegister(CHIPID, &temp);
	PMIC_ReadRegister(PPATH, &temp);
}

/******************************************************************************/
/* PMIC_ReadRegister
 *
 * The function reads a byte from the PMIC at the register.
 * 																			  */
/******************************************************************************/
unsigned char PMIC_ReadRegister(ENUM_PMIC_ADDRESS reg, unsigned char* data)
{
	unsigned char write[1];

	write[0] = reg;
	if(I2C_SendReceiveData0(write, 1, data, 1))
	{
		return TRUE;
	}
	return FALSE;
}

/******************************************************************************/
/* PMIC_WriteRegister
 *
 * The function writes a byte to the PMIC at the register.
 * 																			  */
/******************************************************************************/
unsigned char PMIC_WriteRegister(ENUM_PMIC_ADDRESS reg, unsigned char* data)
{
	unsigned char write[1];

	write[0] = reg;
	write[1] = *data;
	if(I2C_SendReceiveData0(write, 2, (unsigned char *) &dummy, 0))
	{
		return TRUE;
	}
	return FALSE;
}

/******************************* End of file *********************************/
