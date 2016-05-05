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
/* Contains functions to set up the USB host/device.
 *                                                                            */
/******************************************************************************/

#ifndef _USB_H_
#define _USB_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define FLAG_UPDATE_STATUS      1
 //*****************************************************************************
 //
 // The number of ticks to wait before falling back to the idle state.  Since
 // the tick rate is 100Hz this is approximately 3 seconds.
 //
 //*****************************************************************************
#define USBMSC_ACTIVITY_TIMEOUT 30

 //*****************************************************************************
 //
 // DMA Configuration.
 //
 //*****************************************************************************
 #define NUMBER_OF_ENDPOINTS     2 //Total number of send points(RX +TX) used in this USB configuration
 #define USB_MSC_BUFER_SIZE      512

 //*****************************************************************************
 //
 // This enumeration holds the various states that the device can be in during
 // normal operation.
 //
 //*****************************************************************************
 volatile enum
 {
     //
     // Unconfigured.
     //
     MSC_DEV_DISCONNECTED,

     //
     // Connected but not yet fully enumerated.
     //
     MSC_DEV_CONNECTED,

     //
     // Connected and fully enumerated but not currently handling a command.
     //
     MSC_DEV_IDLE,

     //
     // Currently reading the SD card.
     //
     MSC_DEV_READ,

     //
     // Currently writing the SD card.
     //
     MSC_DEV_WRITE,
 }
 g_eMSCState;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
 void Init_USB(void);

#endif
/******************************* End of file *********************************/
