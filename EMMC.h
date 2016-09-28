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
/* Contains functions to configure and communicate with the EMMC device
 *  (MTFC4GACAAAM-1M WT).
 *                                                                            */
/******************************************************************************/
#ifndef _EMMC_H_
#define _EMMC_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/******************************************************************************/
/* Pin Definitions			                                                  */
/******************************************************************************/
#define EMMC_RST_MODULE	1
#define EMMC_RST_REGS	SOC_GPIO_1_REGS
#define EMMC_RST_PIN	20

/******************************************************************************/
/* EMMC_BUFFER_SIZE
 *
 * This is the size of the EMMC card buffer in DDR.
 *                                                                            */
/******************************************************************************/
#define EMMC_BUFFER_SIZE 				16384

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/

typedef enum e_emmc_response
{
	EMMC_RESPONSE_48BITS	= 0x0000,
	EMMC_RESPONSE_NONE 		= 0x0001,
	EMMC_RESPONSE_STOP 		= 0x0002,
	EMMC_RESPONSE_FS 		= 0x0004,
	EMMC_RESPONSE_ABORT 	= 0x0008,
	EMMC_RESPONSE_BUSY 		= 0x0010,
	EMMC_RESPONSE_136BITS	= 0x0020,
	EMMC_RESPONSE_DATA 		= 0x0040,
	EMMC_RESPONSE_READ 		= 0x0080,
	EMMC_RESPONSE_WRITE 	= 0x0100,
}ENUM_EMMC_RESPONSE;

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_EMMC(void);
void EMMC_PinMuxSetup(void);
void EMMC_HSMMCSDModuleClkConfig(void);
void EMMC_HardwareReset(unsigned char state);
unsigned int EMMC_SetUpController(unsigned int baseAddr);
unsigned int EMMC_CardInit(void);
unsigned char EMMC_SendCommand(unsigned int baseAddr, unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_EMMC_RESPONSE type, unsigned int* response);
unsigned char EMMC_SendAppCommand(unsigned int baseAddr, unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_EMMC_RESPONSE type, unsigned int* response);
void EMMC_ReceiveData(unsigned int baseAddr, unsigned char* p_buffer, unsigned int length);
void EMMC_TransmitData(unsigned int baseAddr, unsigned char* p_buffer, unsigned int length);
unsigned int EMMC_WriteBlocks(unsigned int baseAddr, unsigned int block, unsigned int nblks, unsigned char *ptr);
unsigned int EMMC_ReadBlocks(unsigned int baseAddr, unsigned int block, unsigned int nblks, unsigned char *ptr);
unsigned int EMMC_GetNumberBlocks(void);
void EMMC_SetInitialized(void);
void EMMC_ClearInitialized(void);
unsigned char EMMC_IsInitialized(void);

#endif
/******************************* End of file *********************************/
