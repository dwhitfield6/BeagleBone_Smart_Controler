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
/* Contains functions to configure and control the I2C bus.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifndef __I2C_H_
#define __I2C_H_

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef struct t_i2c_data
{
	unsigned char* p_Receive;
	unsigned int ReceiveBytes;
	unsigned int BytesReceived;
	unsigned char* p_Transmit;
	unsigned int TransmitBytes;
	unsigned int BytesTransmitted;
}TYPE_I2C_DATA;

/******************************************************************************/
/* Pin Definitions			                                                  */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define I2C_SYS_CLK_48MHZ             (48000000u)
#define I2C_INTERNAL_CLK_12MHZ        (12000000u)

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern TYPE_I2C_DATA I2C_0_Buffers;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_I2C(void);
void Init_I2C_Module0(void);
void I2C_InterruptConfigure0(void);
unsigned char I2C_SendReceiveData0(unsigned char address, unsigned char* write, unsigned char BytesToWrite, unsigned char* read, unsigned char BytesToRead);
void I2C_SetFailFlag0(void);
void I2C_ClearFailFlag0(void);
unsigned char I2C_GetFailFlag0(void);

#endif
/******************************* End of file *********************************/
