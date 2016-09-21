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

#endif
/******************************* End of file *********************************/
