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
#ifndef _I2C_H_
#define _I2C_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "mmcsd_proto.h"

/******************************************************************************/
/* Pin Definitions			                                                  */
/******************************************************************************/
#define EMMC_RST_MODULE	1
#define EMMC_RST_REGS	SOC_GPIO_1_REGS
#define EMMC_RST_PIN	20

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define EMMC_IN_FREQ         	96000000 /* 96MHz */
#define EMMC_INIT_FREQ    		400000   /* 400kHz */

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
void EMMC_ControllerInit(void);
void EMMC_DiskInitialize(void);
void EMMC_HSMMCSDXferSetup(mmcsdCtrlInfo *ctrl, unsigned char rwFlag, void *ptr, unsigned int blkSize, unsigned int nBlks);
unsigned int EMMC_HSMMCSDCmdStatusGet(mmcsdCtrlInfo *ctrl);
unsigned int EMMC_HSMMCSDXferStatusGet(mmcsdCtrlInfo *ctrl);

#endif
/******************************* End of file *********************************/
