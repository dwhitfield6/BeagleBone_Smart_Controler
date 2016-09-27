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
/* Contains functions to set up and control the Memory Management Unit.
 *                                                                            */
/******************************************************************************/

#ifndef _MMU_CUSTOM_H_
#define _MMU_CUSTOM_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "mmu.h"

/******************************************************************************/
/* Memory Addresses                                                           */
/******************************************************************************/
/* Definitions related to MMU Configuration. */
#define START_ADDR_DDR                     (0x80000000)
#define START_ADDR_DDR_CACHE               (0x90000000)
#define START_ADDR_DEV                     (0x44000000)
#define START_ADDR_OCMC                    (0x40300000)
#define NUM_SECTIONS_DDR                   (256)
#define NUM_SECTIONS_DDR_CACHE             (256)
#define NUM_SECTIONS_DEV                   (960)
#define NUM_SECTIONS_OCMC                  (1)

static volatile unsigned int pageTable[4*1024]__attribute__((aligned(16*1024)));

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void MMU_ConfigAndEnable(void);

#endif
/******************************* End of file *********************************/
