//*****************************************************************************
//
// usbhandler.c - General USB handling routines.
//
// Copyright (c) 2007-2010 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of AM1808 StarterWare USB Library and reused from revision 6288 
// of the  Stellaris USB Library.
//
//*****************************************************************************

#include "hw_usb.h"
#include "interrupt.h"
#include "hw_types.h"
#include "usb.h"
#include "usblib.h"
#include "usbdevice.h"
#include "usbdevicepriv.h"
#include "usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup device_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// USB instance Object
//
//*****************************************************************************
extern tUSBInstanceObject g_USBInstance[];

void
USB1DeviceIntHandler(void)
{
    unsigned int ulStatus = 0;

#if defined (am335x_15x15) || defined(am335x) || defined(c6a811x)

    unsigned int epStatus = 0;

    //
    // Get the controller interrupt status.
    //
    ulStatus = HWREG(g_USBInstance[1].uiSubBaseAddr + USB_0_IRQ_STATUS_1);
    //
    // Get the EP interrupt status.
    //
    epStatus = HWREG(g_USBInstance[1].uiSubBaseAddr + USB_0_IRQ_STATUS_0);
    //
    // Clear the controller interrupt status.
    //
    HWREG(g_USBInstance[1].uiSubBaseAddr + USB_0_IRQ_STATUS_1) = ulStatus;
    //
    // Clear the EP interrupt status.
    //
    HWREG(g_USBInstance[1].uiSubBaseAddr + USB_0_IRQ_STATUS_0) = epStatus;

#ifdef DMA_MODE    
    HWREG(USBSS_BASE + USBSS_IRQ_STATUS) = 
        HWREG(USBSS_BASE + USBSS_IRQ_STATUS);
#endif
    //
    //Call the Interrupt Handler.
    //
    USBDeviceIntHandlerInternal(1, ulStatus, &epStatus);
    //
    //End of Interrupts.
    //
    HWREG(g_USBInstance[1].uiSubBaseAddr + USB_0_IRQ_EOI) = 0;

#ifdef DMA_MODE
    HWREG(USBSS_BASE + USBSS_IRQ_EOI) = 0;
#endif

#else
    //
    // Get the controller interrupt status.
    //
    ulStatus = HWREG(g_USBInstance[1].uiSubBaseAddr + USB_0_INTR_SRC);
    // Clear the Interrupts
    HWREG(g_USBInstance[1].uiSubBaseAddr + USB_0_INTR_SRC_CLEAR) = ulStatus;
#ifdef _TMS320C6X
    IntEventClear(g_USBInstance[1].uiInterruptNum);
#else
    IntSystemStatusClear(g_USBInstance[1].uiInterruptNum);
#endif

    //
    // Call the internal handler.
    //
    USBDeviceIntHandlerInternal(1, ulStatus, NULL);

    // End of Interrupts
    HWREG(g_USBInstance[1].uiSubBaseAddr + USB_0_END_OF_INTR) = 0;
#endif

}


//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
