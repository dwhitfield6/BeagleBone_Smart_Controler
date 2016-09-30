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
/* Contains functions to set up and control the SD card.
 *                                                                            */
/******************************************************************************/

#ifndef _SD_H_
#define _SD_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "edma.h"
#include "ff.h"
#include "mmcsd_proto.h"

/******************************************************************************/
/* USE_SD_CARD
 *
 * Define this to use an SD card.
 *                                                                            */
/******************************************************************************/
#define USE_SD_CARD

/******************************************************************************/
/* EMMC module base address	                                                  */
/******************************************************************************/
#define SD_BASE_ADDRESS SOC_MMCHS_0_REGS

/******************************************************************************/
/* SD_BUFFER_SIZE
 *
 * This is the size of the SD card buffer in DDR.
 *                                                                            */
/******************************************************************************/
#define SD_BUFFER_SIZE 				16384

/******************************************************************************/
/* FILE_DATA_BUFFER_SIZE
 *
 * This is the size of the file buffer in DDR.
 *                                                                            */
/******************************************************************************/
#define FILE_DATA_BUFFER_SIZE 		1024

/******************************************************************************/
/* SD_CARD_SIZE
 *
 * For SD Version 1.1 cards larger than 4 GB, the size can not be determined
 *  from the card. This is the hardcoded size prior to reading te size from
 *  the file system.
 *                                                                            */
/******************************************************************************/
#define SD_CARD_SIZE (29)

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/

typedef enum e_sd_card_state
{
	CARD_NOTPRESENT,
	CARD_PRESENT,
}ENUM_SD_CARD_STATE;

typedef enum e_sd_response
{
	SD_RESPONSE_48BITS	= 0x0000,
	SD_RESPONSE_NONE 	= 0x0001,
	SD_RESPONSE_STOP 	= 0x0002,
	SD_RESPONSE_FS 		= 0x0004,
	SD_RESPONSE_ABORT 	= 0x0008,
	SD_RESPONSE_BUSY 	= 0x0010,
	SD_RESPONSE_136BITS	= 0x0020,
	SD_RESPONSE_DATA 	= 0x0040,
	SD_RESPONSE_READ 	= 0x0080,
	SD_RESPONSE_WRITE 	= 0x0100,
}ENUM_SD_RESPONSE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern unsigned char SD_Buffer[SD_BUFFER_SIZE];
extern FATFS g_SD_FatFs;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_SD(void);
unsigned int SD_SetUpController(void);
unsigned char SD_IsCardInserted(void);
unsigned long SD_GetFatTime(void);
void SD_SetCardActionFlag(void);
void SD_ClearCardActionFlag(void);
unsigned char SD_GetCardActionFlag(void);
void SD_SetCardStatus(ENUM_SD_CARD_STATE state);
ENUM_SD_CARD_STATE SD_GetCardStatus(void);
void SD_SetInitialized(void);
void SD_ClearInitialized(void);
unsigned char SD_IsInitialized(void);
unsigned int SD_CardInit(void);
unsigned char SD_SendCommand(unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_SD_RESPONSE type, unsigned int* response);
unsigned char SD_SendAppCommand(unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_SD_RESPONSE type, unsigned int* response);
void SD_ReceiveData(unsigned char* p_buffer, unsigned int length);
void SD_TransmitData(unsigned char* p_buffer, unsigned int length);
unsigned int SD_WriteBlocks(unsigned int block, unsigned int nblks, unsigned char *ptr);
unsigned int SD_ReadBlocks(unsigned int block, unsigned int nblks, unsigned char *ptr);
unsigned int SD_GetNumberBlocks(void);

#endif
/******************************* End of file *********************************/
