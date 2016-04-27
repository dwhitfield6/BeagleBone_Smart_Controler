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
/* Contains functions to configure and control the SPI bus.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifndef _SPI_H_
#define _SPI_H_

/******************************************************************************/
/* SPI1_BUFFER_SIZE
 *
 * This is the size of the SPI1 buffer for sending data.
 *                                                                            */
/******************************************************************************/
#define SPI1_BUFFER_SIZE 1000

/******************************************************************************/
/* SPI1_MISC_BUFFER_SIZE
 *
 * This is the size of the SPI1 buffer for holding data. This is pointed to by
 *  the SPI buffer.
 *                                                                            */
/******************************************************************************/
#define SPI1_MISC_BUFFER_SIZE 1000


/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define LCD_FRAM_SPI_REGS SOC_SPI_1_REGS
#define RF_SPI_REGS SOC_SPI_0_REGS

#define RF_CS	0
#define LCD_CS	0
#define FRAM_CS	1

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef enum e_spi_transfer_mode
{
    TX_RX = 0,
	TX_ONLY = 1,
	RX_ONLY = 2,
}ENUM_SPI_TRANSFER_MODE;

typedef struct t_spi_transfer_buffer
{
    unsigned char* TXdata;
    unsigned char* RXdata;
    ENUM_SPI_TRANSFER_MODE TransferType;
    unsigned long BytesRemaining;
    unsigned char Complete;
}TYPE_SPI_TRANSFER;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_SPI(void);
void Init_SPI_Module1(void);
void Init_SPI_Module0(void);
void SPI_INTCConfigure1(void);
void SPI_1_ISR(void);
void SPI_WriteByte0(unsigned char data, unsigned char cs);
void SPI_WriteByte1(unsigned char data, unsigned char cs);
unsigned char SPI_ReadWriteByte0(unsigned char data, unsigned char cs);
unsigned char SPI_ReadWriteByte1(unsigned char data, unsigned char cs);
void SPI_WriteByteNoRx0(unsigned char data, unsigned char cs);
void SPI_WriteByteNoRx1(unsigned char data, unsigned char cs);

#endif
/******************************* End of file *********************************/
