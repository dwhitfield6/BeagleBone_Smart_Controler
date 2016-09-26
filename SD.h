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
#include "mmcsd_proto.h"

/******************************************************************************/
/* USE_SD_CARD
 *
 * Define this to use an SD card.
 *                                                                            */
/******************************************************************************/
#define USE_SD_CARD

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define HSMMCSD_BLK_SIZE			512
#define DRIVE_NUM_MMCSD     		0
#define DRIVE_NUM_MAX      			2
#define MMCSD_INST_BASE    			(SOC_MMCHS_0_REGS)
#define MMCSD_INT_NUM       		(SYS_INT_MMCSD0INT)
#define EDMA_INST_BASE  			(SOC_EDMA30CC_0_REGS)
#define EDMA_COMPLTN_INT_NUM 		(SYS_INT_EDMACOMPINT)
#define EDMA_ERROR_INT_NUM  		(SYS_INT_EDMAERRINT)
#define MMCSD_TX_EDMA_CHAN			(EDMA3_CHA_MMCSD0_TX)
#define MMCSD_RX_EDMA_CHAN   		(EDMA3_CHA_MMCSD0_RX)
#define EVT_QUEUE_NUM      			0
#define HSMMCSD_IN_FREQ         	96000000 /* 96MHz */
#define HSMMCSD_INIT_FREQ    		400000   /* 400kHz */
#define HSMMCSD_CARD_DETECT_PINNUM 	6
#define PATH_BUF_SIZE   			512
#define DATA_BUF_SIZE   			64 * (2 * 512)
#define CMD_BUF_SIZE    			512
#define FILE_DATA_BUFFER_SIZE 		1024
#define SD_BUFFER_SIZE 				16384

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

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_SD(void);
unsigned int SD_SetUpController(unsigned int baseAddr);
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
unsigned char SD_SendCommand(unsigned int baseAddr, unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_SD_RESPONSE type, unsigned int* response);
unsigned char SD_SendAppCommand(unsigned int baseAddr, unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_SD_RESPONSE type, unsigned int* response);
void SD_ReceiveData(unsigned int baseAddr, unsigned char* p_buffer, unsigned int length);

#endif
/******************************* End of file *********************************/
