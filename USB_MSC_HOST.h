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
/* Contains functions to set up the USB host.
 *                                                                            */
/******************************************************************************/

#ifndef _USB_MSC_HOST_H__
#define _USB_MSC_HOST_H__

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "usbhost.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* HCD_MEMORY_SIZE                                                            */
/******************************************************************************/
#define HCD_MEMORY_SIZE         128

/******************************************************************************/
/* NUM_CLASS_DRIVERS
 *
 * Holds the number of class drivers in the g_ppHostClassDrivers list.                                                              */
/******************************************************************************/
#define NUM_CLASS_DRIVERS       (sizeof(g_ppHostClassDrivers) /               \
                                 sizeof(g_ppHostClassDrivers[0]))

/******************************************************************************/
/* USB_HOST_DRIVE
 *
 * This is the drive number.                                                  */
/******************************************************************************/
#define USB_HOST_DRIVE 0

/******************************************************************************/
/* PATH_BUF_SIZE
 *
 * This is the path buffer size.                                              */
/******************************************************************************/
#define PATH_BUF_SIZE   80

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/

typedef enum
{
    //
    // No device is present.
    //
    STATE_NO_DEVICE,

    //
    // Mass storage device is being enumerated.
    //
    STATE_DEVICE_ENUM,

    //
    // Mass storage device is ready.
    //
    STATE_DEVICE_READY,

    //
    // An unsupported device has been attached.
    //
    STATE_UNKNOWN_DEVICE,

    //
    // A power fault has occurred.
    //
    STATE_POWER_FAULT
}
tState;
volatile tState g_eState;
volatile tState g_eUIState;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern FATFS g_USB_HOST_FatFs;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_USB_Host(void);
void USB_InterruptConfigure1(void);
void MSCCallback(unsigned int ulInstance, unsigned int ulEvent, void *pvData);
void USBHCDEvents(void *pvData);
void USB_HOST_Process(void);

#endif /* _USB_MSC_HOST_H__ */
/******************************* End of file *********************************/
