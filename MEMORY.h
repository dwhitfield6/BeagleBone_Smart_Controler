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
/* Contains functions for controlling the LEDs.
 *                                                                            */
/******************************************************************************/

#ifndef _MMU_CUSTOM_H_
#define _MMU_CUSTOM_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "mmu.h"

/****************************************************************************
**                    MEMORY ADDRESSING
****************************************************************************/
/* Definitions related to MMU Configuration. */
#define START_ADDR_DDR                (0x80000000)
#define START_ADDR_DEV                (0x44000000)
#define START_ADDR_OCMC               (0x40300000)
#define NUM_SECTIONS_DDR              (512)
#define NUM_SECTIONS_DEV              (960)
#define NUM_SECTIONS_OCMC             (1)

static volatile unsigned int pageTable[MMU_PAGETABLE_NUM_ENTRY]
 __attribute__((aligned(MMU_PAGETABLE_ALIGN_SIZE)));

/****************************************************************************
**                    MACRO DEFINITIONS
****************************************************************************/

/****************************************************************************
**                  STRUCTURE DECLARATION
****************************************************************************/

/******************************************************************************
**              GLOBAL VARIABLE DEFINITIONS
******************************************************************************/


/****************************************************************************
**                    FUNCTION PROTOTYPES
****************************************************************************/
void MMU_ConfigAndEnable(void);

#endif
/******************************* End of file *********************************/
