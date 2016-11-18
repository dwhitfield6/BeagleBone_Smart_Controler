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
#include "usb.h"

#include "INTERRUPTS.h"
#include "MISC.h"
#include "SD.h"
#include "UART.h"
#include "USB_MSC_DEVICE.h"
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
static FRESULT Result;
static unsigned int BytesWritten;
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] =
{
    &g_USBHostMSCClassDriver,
    &g_sUSBEventDriver
};

#pragma DATA_ALIGN(fileWrite, SOC_CACHELINE_SIZE);
static FIL fileWrite;

#pragma DATA_ALIGN(FileDataBuffer, SOC_CACHELINE_SIZE);
static char FileDataBuffer[USB_HOST_FILE_DATA_BUFFER_SIZE];

#pragma DATA_ALIGN(g_sDirObject, SOC_CACHELINE_SIZE);
static DIR g_sDirObject;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned int g_ulMSCInstance = 0;
unsigned char g_pHCDPool[HCD_MEMORY_SIZE];
volatile tState g_eState;
volatile tState g_eUIState;

#pragma DATA_ALIGN(g_USB_HOST_FatFs, SOC_CACHELINE_SIZE);
FATFS g_USB_HOST_FatFs;

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
	unsigned int i;

	//
	// Initially wait for device connection.
	//
	g_eState = STATE_NO_DEVICE;
	g_eUIState = STATE_NO_DEVICE;

	fat_devices[DRIVE_NUM_USB].initDone = 0;

	//
	//USB module clock enable
	//
	USB0ModuleClkConfig();

	USB_InterruptConfigure1();

	//
	// Register the host class drivers.
	//
	USBHCDRegisterDrivers(1, g_ppHostClassDrivers, NUM_CLASS_DRIVERS);

	//
	// Open an instance of the mass storage class driver.
	//
	g_ulMSCInstance = USBHMSCDriveOpen(1, USB_HOST_DRIVE, MSCCallback);

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
	Result = f_mount(USB_HOST_DRIVE, &g_USB_HOST_FatFs);

	if(Result == FR_OK)
	{
		for(i=0;i<60;i++)
		{
			/* process a USB host event */
			USB_HOST_Process();
			if(g_eState == STATE_DEVICE_READY)
			{
				break;
			}
			MSC_DelayUS(50000);
		}
	}
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
	IntPrioritySet(SYS_INT_USB1, USB_HOST_INTERRUPT_PRIORITY, AINTC_HOSTINT_ROUTE_IRQ);

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
            fat_devices[DRIVE_NUM_USB].initDone = 0;

            break;
        }

        default:
        {
            break;
        }
    }
}

/******************************************************************************/
/* USBHCDEvents
 *
 * Generic callback from host stack
 *                                                                            */
/******************************************************************************/
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

/******************************************************************************/
/* USB_HOST_Process
 *
 * Process the USB Host events.
 *                                                                            */
/******************************************************************************/
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
		Result = f_mount(USB_HOST_DRIVE, &g_USB_HOST_FatFs);
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

				if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_USB_HOST)
				{
					Init_USB0();
				}
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

/******************************************************************************/
/* USB_Host_Test
 *
 * Tests the speed at which files are created.
 *                                                                            */
/******************************************************************************/
void USB_Host_Test(void)
{
	char filename[20];
	static unsigned int i = 0;
	sprintf(filename, "0:/Test%d.txt", i);
	Result = f_open (&fileWrite, (const char*)filename, FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS);
	Result = f_write (&fileWrite, FileDataBuffer, sizeof(FileDataBuffer), &BytesWritten);
	Result = f_close (&fileWrite);
	i++;
}

/******************************* End of file *********************************/
