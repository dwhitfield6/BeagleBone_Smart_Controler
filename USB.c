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

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "beaglebone.h"
#include "cppi41dma.h"
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "interrupt.h"
#include "pin_mux.h"
#include "soc_AM335x.h"
#include "usbdevice.h"
#include "usbdmsc.h"
#include "usb_msc_structs.h"

#include "GPIO.h"
#include "INTERRUPTS.h"
#include "LEDS.h"
#include "USB.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
static ENUM_MSC_STATES USB0_MSCState = MSC_DEV_IDLE;
static ENUM_USB_STATUS USB0_Status = USB_DISCONNECT;
static unsigned char USBChangeStatusFlag0 = FALSE;
static unsigned char USBMSCFlag0 = FALSE;
static unsigned int g_bufferIndex = 0;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned char *USBdataBuffer;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_USB
 *
 * Initializes both USB modules.
 *                                                                            */
/******************************************************************************/
void Init_USB(void)
{

}

/******************************************************************************/
/* Init_USB0
 *
 * Initializes USB module 0.
 *                                                                            */
/******************************************************************************/
void Init_USB0(void)
{
	/* initialize the USB0 clock */
    USB0ModuleClkConfig();

    USB_InterruptConfigure0();

    USB_SetMSCState0(MSC_DEV_IDLE);
    USB_SetUSBStatus0(USB_DISCONNECT);
    USBDMSCInit(0, (tUSBDMSCDevice *)&g_sMSCDevice);

#ifdef DMA_MODE
    Cppi41DmaInit(0, epInfo, NUMBER_OF_ENDPOINTS);

    for(;g_bufferIndex < NUM_OF_RX_BDs; g_bufferIndex++)
    {
    	USBdataBuffer = (unsigned char *)cppiDmaAllocBuffer();
        doDmaRxTransfer(0, USB_MSC_BUFER_SIZE, USBdataBuffer,
                            g_sMSCDevice.psPrivateData->ucOUTEndpoint);
    }
#endif

}

/******************************************************************************/
/* USB_InterruptConfigure0
 *
 * Sets up USB module 0 interrupts.
 *                                                                            */
/******************************************************************************/
void USB_InterruptConfigure0(void)
{
    /* Registering the Interrupt Service Routine(ISR). */
    IntRegister(SYS_INT_USB0, USB_0_ISR);

    /* Setting the priority for the system interrupt in AINTC. */
    IntPrioritySet(SYS_INT_USB0, USB_INTERRUPT_PRIORITY, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the system interrupt in AINTC. */
    IntSystemEnable(SYS_INT_USB0);

    /* Registering the Interrupt Service Routine(ISR). */
    IntRegister(SYS_INT_USBSSINT, USB_0_ISR);

    /* Setting the priority for the system interrupt in AINTC. */
    IntPrioritySet(SYS_INT_USBSSINT, USB_INTERRUPT_PRIORITY, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the system interrupt in AINTC. */
    IntSystemEnable(SYS_INT_USBSSINT);
}

/******************************************************************************/
/* USB_InterruptEnable0
 *
 * Controls the USB 0 interupts.
 *                                                                            */
/******************************************************************************/
void USB_InterruptEnable0(unsigned char state)
{
	if(state)
	{
		IntSystemEnable(SYS_INT_USB0);
	}
	else
	{
		IntSystemDisable(SYS_INT_USB0);
	}
}

/******************************************************************************/
/* USB_DMSCEventCallback0
 *
 * This function is the call back notification function provided to the USB
 *  library's mass storage class.
 *                                                                            */
/******************************************************************************/
unsigned int USB_DMSCEventCallback0(void *pvCBData, unsigned int ulEvent, unsigned int ulMsgParam, void *pvMsgData)
{
    switch(ulEvent)
    {
        /* Writing to the device. */
        case USBD_MSC_EVENT_WRITING:
        {
            /* Only update if this is a change. */
            if(USB_GetMSCState0() != MSC_DEV_WRITE)
            {
                /* Go to the write state. */
            	USB_SetMSCState0(MSC_DEV_WRITE);
            }
            break;
        }

        /* Reading from the device. */
        case USBD_MSC_EVENT_READING:
        {
            /* Only update if this is a change. */
            if(USB_GetMSCState0() != MSC_DEV_READ)
            {
                /* Go to the write state. */
            	USB_SetMSCState0(MSC_DEV_READ);
            }
            break;
        }
        case USBD_MSC_EVENT_IDLE:
        default:
        {
            break;
        }
    }

    return(0);
}

/******************************************************************************/
/* USB_SetMSCState0
 *
 * Sets the USB 0 MSC state.
 *                                                                            */
/******************************************************************************/
void USB_SetMSCState0(ENUM_MSC_STATES state)
{
	USB0_MSCState = state;
}

/******************************************************************************/
/* USB_GetMSCState0
 *
 * Gets the USB 0 MSC state.
 *                                                                            */
/******************************************************************************/
ENUM_MSC_STATES USB_GetMSCState0(void)
{
	return USB0_MSCState;
}

/******************************************************************************/
/* USB_SetUSBStatus0
 *
 * Sets the USB 0 status.
 *                                                                            */
/******************************************************************************/
void USB_SetUSBStatus0(ENUM_USB_STATUS state)
{
	if(state != USB_GetUSBStatus0())
	{
		USB0_Status = state;
		USB_SetUSBStatusFlag0();
	}
}

/******************************************************************************/
/* USB_GetUSBStatus0
 *
 * Gets the USB 0 status.
 *                                                                            */
/******************************************************************************/
ENUM_USB_STATUS USB_GetUSBStatus0(void)
{
	return USB0_Status;
}

/******************************************************************************/
/* USB_SetUSBStatusFlag0
 *
 * Sets the USB 0 change status flag.
 *                                                                            */
/******************************************************************************/
void USB_SetUSBStatusFlag0(void)
{
	USBChangeStatusFlag0 = TRUE;
}

/******************************************************************************/
/* USB_ClearUSBStatusFlag0
 *
 * Clears the USB 0 change status flag.
 *                                                                            */
/******************************************************************************/
void USB_ClearUSBStatusFlag0(void)
{
	USBChangeStatusFlag0 = FALSE;
}

/******************************************************************************/
/* USB_GetUSBStatusFlag0
 *
 * Gets the USB 0 change status flag.
 *                                                                            */
/******************************************************************************/
unsigned char USB_GetUSBStatusFlag0(void)
{
	return USBChangeStatusFlag0;
}

/******************************************************************************/
/* USB_SetUSBMSCFlag0
 *
 * Sets the USB 0 MSC flag.
 *                                                                            */
/******************************************************************************/
void USB_SetUSBMSCFlag0(void)
{
	USBMSCFlag0 = TRUE;
}

/******************************************************************************/
/* USB_ClearUSBMSCFlag0
 *
 * Clears the USB 0 MSC flag.
 *                                                                            */
/******************************************************************************/
void USB_ClearUSBMSCFlag0(void)
{
	USBMSCFlag0 = FALSE;
}

/******************************************************************************/
/* USB_GetUSBMSCFlag0
 *
 * Gets the USB 0 MSC flag.
 *                                                                            */
/******************************************************************************/
unsigned char USB_GetUSBMSCFlag0(void)
{
	return USBMSCFlag0;
}

/******************************* End of file *********************************/
