/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 10/03/16     16.0_DW0a   New project creation.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Contains functions to set up the USB host.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <string.h>

#include "ff.h"
#include "hw_types.h"
#include "interrupt.h"
#include "mmcsd_proto.h"
#include "usbhost.h"
#include "usbhmsc.h"

#include "INTERRUPTS.h"
#include "SD.h"
#include "UART.h"
#include "USB_MSC_HOST.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* USB Events driver interface                                                */
/******************************************************************************/
DECLARE_EVENT_DRIVER(g_sUSBEventDriver, 0, 0, USBHCDEvents);

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";
static unsigned short BytesWritten;
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] =
{
    &g_USBHostMSCClassDriver,
    &g_sUSBEventDriver
};

#pragma DATA_ALIGN(fileWrite, SOC_CACHELINE_SIZE);
static FIL fileWrite;

#pragma DATA_ALIGN(FileDataBuffer, SOC_CACHELINE_SIZE);
static char FileDataBuffer[FILE_DATA_BUFFER_SIZE];
static FRESULT Result;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned int g_ulMSCInstance = 0;
unsigned char g_pHCDPool[HCD_MEMORY_SIZE];
static FATFS g_sFatFs;
static DIR g_sDirObject;
static FILINFO g_sFileInfo;
static FIL g_sFileObject;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_USB_Host
 *
 * Initializes the USB host.
 *                                                                            */
/******************************************************************************/
void Init_USB_Host(void)
{

	//
	// Initially wait for device connection.
	//
	g_eState = STATE_NO_DEVICE;
	g_eUIState = STATE_NO_DEVICE;

	USB_InterruptConfigure1();

	//
	// Register the host class drivers.
	//
	USBHCDRegisterDrivers(1, g_ppHostClassDrivers, NUM_CLASS_DRIVERS);

	//
	// Open an instance of the mass storage class driver.
	//
	g_ulMSCInstance = USBHMSCDriveOpen(1, 0, MSCCallback);

	//
	// Initialize the power configuration.  This sets the power enable signal
	// to be active high and does not enable the power fault.
	//
	USBHCDPowerConfigInit(1, USBHCD_VBUS_AUTO_HIGH);

	//
	// Initialize the host controller.
	//
	USBHCDInit(1, g_pHCDPool, HCD_MEMORY_SIZE);

	//
	// Initialize the file system.
	//
	Result = f_mount(USB_HOST_DRIVE, &g_sFatFs);
}

/******************************************************************************/
/* USB_InterruptConfigure1
 *
 * Sets up USB module 1 interrupts.
 *                                                                            */
/******************************************************************************/
void USB_InterruptConfigure1(void)
{
	/* Registering the Interrupt Service Routine(ISR). */
	IntRegister(SYS_INT_USB1, USB1HostIntHandler);

	/* Setting the priority for the system interrupt in AINTC. */
	IntPrioritySet(SYS_INT_USB1, 0, AINTC_HOSTINT_ROUTE_IRQ);

	/* Enabling the system interrupt in AINTC. */
	IntSystemEnable(SYS_INT_USB1);
}

/******************************************************************************/
/* USB_MSCCallback
 *
 * This is the callback from the host MSC driver.
 *                                                                            */
/******************************************************************************/
void MSCCallback(unsigned int ulInstance, unsigned int ulEvent, void *pvData)
{
    //
    // Determine the event.
    //
    switch(ulEvent)
    {
        //
        // Called when the device driver has successfully enumerated an MSC
        // device.
        //
        case MSC_EVENT_OPEN:
        {
            //
            // Proceed to the enumeration state.
            //
            g_eState = STATE_DEVICE_ENUM;
            break;
        }

        //
        // Called when the device driver has been unloaded due to error or
        // the device is no longer present.
        //
        case MSC_EVENT_CLOSE:
        {
            //
            // Go back to the "no device" state and wait for a new connection.
            //
            g_eState = STATE_NO_DEVICE;

            break;
        }

        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This is the generic callback from host stack.
//
// \param pvData is actually a pointer to a tEventInfo structure.
//
// This function will be called to inform the application when a USB event has
// occurred that is outside those related to the mass storage device.  At this
// point this is used to detect unsupported devices being inserted and removed.
// It is also used to inform the application when a power fault has occurred.
// This function is required when the g_USBGenericEventDriver is included in
// the host controller driver array that is passed in to the
// USBHCDRegisterDrivers() function.
//
// \return None.
//
//*****************************************************************************
void USBHCDEvents(void *pvData)
{
    tEventInfo *pEventInfo;

    //
    // Cast this pointer to its actual type.
    //
    pEventInfo = (tEventInfo *)pvData;

    switch(pEventInfo->ulEvent)
    {
        //
        // New keyboard detected.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // An unknown device was detected.
            //
            g_eState = STATE_UNKNOWN_DEVICE;

            break;
        }

        //
        // Keyboard has been unplugged.
        //
        case USB_EVENT_DISCONNECTED:
        {
            //
            // Unknown device has been removed.
            //
            g_eState = STATE_NO_DEVICE;

            break;
        }

        case USB_EVENT_POWER_FAULT:
        {
            //
            // No power means no device is present.
            //
            g_eState = STATE_POWER_FAULT;

            break;
        }

        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This function reads a line of text from the console.  The USB host main
// function is called throughout this process to keep USB alive and well.
//
//*****************************************************************************
void USB_HOST_Process(void)
{
	tState eStateCopy;

	//
	// See if a mass storage device has been enumerated.
	//
	if(g_eState == STATE_DEVICE_ENUM)
	{
		//
		// Take it easy on the Mass storage device if it is slow to
		// start up after connecting.
		//
		if(USBHMSCDriveReady(g_ulMSCInstance) != 0)
		{
			UART_PrintString("USB Host not ready");
		}

		//
		// Reset the working directory to the root.
		//
		g_cCwdBuf[0] = '/';
		g_cCwdBuf[1] = '\0';

		//
		// Attempt to open the directory.  Some drives take longer to
		// start up than others, and this may fail (even though the USB
		// device has enumerated) if it is still initializing.
		//
		Result = f_mount(USB_HOST_DRIVE, &g_sFatFs);
		 if(f_opendir(&g_sDirObject, g_cCwdBuf) == FR_OK)
		{
			//
			// The drive is fully ready, so move to that state.
			//
			g_eState = STATE_DEVICE_READY;
		}
	}

	//
	// See if the state has changed.  We make a copy of g_eUIState to
	// prevent a compiler warning about undefined order of volatile
	// accesses.
	//
	eStateCopy = g_eUIState;
	if(g_eState != eStateCopy)
	{
		//
		// Determine the new state.
		//
		switch(g_eState)
		{
			//
			// A previously connected device has been disconnected.
			//
			case STATE_NO_DEVICE:
			{
				if(g_eUIState == STATE_UNKNOWN_DEVICE)
				{
					UART_PrintString("\nUnknown device disconnected.\n");
				}
				else
				{
					UART_PrintString("\nMass storage device disconnected.\n");
				}
				break;
			}

			//
			// A mass storage device is being enumerated.
			//
			case STATE_DEVICE_ENUM:
			{
				break;
			}

			//
			// A mass storage device has been enumerated and initialized.
			//
			case STATE_DEVICE_READY:
			{
				UART_PrintString("\nMass storage device connected.\n");
				Result = f_open (&fileWrite, "0:/Log10.txt", FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS);
				sprintf(FileDataBuffer, "This is a test.");
				Result = f_write (&fileWrite, FileDataBuffer, strlen(FileDataBuffer), &BytesWritten);
				Result = f_close (&fileWrite);
				break;
			}

			//
			// An unknown device has been connected.
			//
			case STATE_UNKNOWN_DEVICE:
			{
				UART_PrintString("\nUnknown device connected.\n");
				break;
			}

			//
			// A power fault has occurred.
			//
			case STATE_POWER_FAULT:
			{
				UART_PrintString("\nPower fault.\n");
				break;
			}
		}

		//
		// Save the current state.
		//
		g_eUIState = g_eState;
	}
    USBHCDMain(1, g_ulMSCInstance);
}


/******************************* End of file *********************************/
