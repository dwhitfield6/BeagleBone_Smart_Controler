//*****************************************************************************
//
// usbdmscglue.c - Routines supplied for use by the mass storage class device
// class.
//
//*****************************************************************************
/*
* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
*/
/*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "EMMC.h"
#include "hw_types.h"
#include "debug.h"
#include "interrupt.h"
#include "usblib.h"
#include "usb-ids.h"
#include "usbdevice.h"
#include "usbdmsc.h"
#include "usb_msc_structs.h"
#include "ramdisk.h"
#include "SD.h"
#include "usbhmsc.h"
#include "USB_MSC_DEVICE.h"
#include "USB_MSC_HOST.h"

#define SDCARD_PRESENT          0x00000001
#define SDCARD_IN_USE           0x00000002
struct
{
    unsigned int ulFlags;
}
g_sDriveInformation;

//*****************************************************************************
//
// This function opens the drive number and prepares it for use by the Mass
// storage class device.
//
// /param ulDrive is the driver number to open.
//
// This function is used to initialize and open the physical drive number
// associated with the parameter /e ulDrive.  The function will return zero if
// the drive could not be opened for some reason.  In the case of removable
// device like an SD card this function should return zero if the SD card is
// not present.
//
// /return Returns a pointer to data that should be passed to other APIs or it
// will return 0 if no drive was found.
//
//*****************************************************************************
void *
USBDMSCStorageOpen(unsigned int ulDrive)
{
    //unsigned char ucPower;
    //unsigned int ulTemp;

    ASSERT(ulDrive == 0);

    g_sDriveInformation.ulFlags = SDCARD_PRESENT | SDCARD_IN_USE;

    return((void *)&g_sDriveInformation);
}

//*****************************************************************************
//
// This function close the drive number in use by the mass storage class device.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
//
// This function is used to close the physical drive number associated with the
// parameter /e pvDrive.  This function will return 0 if the drive was closed
// successfully and any other value will indicate a failure.
//
// /return Returns 0 if the drive was successfully closed or non-zero for a
// failure.
//
//*****************************************************************************
void
USBDMSCStorageClose(void * pvDrive)
{
    ASSERT(pvDrive != 0);

    //
    // Clear all flags.
    //
    g_sDriveInformation.ulFlags = 0;

    //
    // Turn off the power to the card.
    //
 }

//*****************************************************************************
//
// This function will read a block from a device opened by the
// USBDMSCStorageOpen() call.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
// /param pucData is the buffer that data will be written into.
// /param ulNumBlocks is the number of blocks to read.
//
// This function is use to read blocks from a physical device and return them
// in the /e pucData buffer.  The data area pointed to by /e pucData should be
// at least /e ulNumBlocks * Block Size bytes to prevent overwriting data.
//
// /return Returns the number of bytes that were read from the device.
//
//*****************************************************************************
unsigned int USBDMSCStorageRead(void * pvDrive,
                                 unsigned char *pucData,
                                 unsigned int ulSector,
                                 unsigned int ulNumBlocks)
{
	unsigned int ulMSCInstance;

    ASSERT(pvDrive != 0);

#ifdef USE_RAM_DISK
    RAM_disk_read(ulSector, pucData, ulNumBlocks);
    ulNumBlocks *= 512;
#else
    if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_SD)
	{
    	SD_ReadBlocks(ulSector, ulNumBlocks, pucData);
    	ulNumBlocks *= 512;
	}
    else if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_EMMC)
	{
		EMMC_ReadBlocks(ulSector, ulNumBlocks, pucData);
		ulNumBlocks *= 512;
	}
    else if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_USB_HOST)
	{
    	ulMSCInstance = (unsigned int)&g_USBHMSCDevice[0];

		/* READ BLOCK */
		if (USBHMSCBlockRead(ulMSCInstance, ulSector, pucData, ulNumBlocks) == 0)
		{
			ulNumBlocks *= 512;
		}
	}
#endif

    return(ulNumBlocks);
}

//*****************************************************************************
//
// This function will write a block to a device opened by the
// USBDMSCStorageOpen() call.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
// /param pucData is the buffer that data will be used for writing.
// /param ulNumBlocks is the number of blocks to write.
//
// This function is use to write blocks to a physical device from the buffer
// pointed to by the /e pucData buffer.  If the number of blocks is greater than
// one then the block address will increment and write to the next block until
// /e ulNumBlocks * Block Size bytes have been written.
//
// /return Returns the number of bytes that were written to the device.
//
//*****************************************************************************
unsigned int USBDMSCStorageWrite(void * pvDrive,
                                  unsigned char *pucData,
                                  unsigned int ulSector,
                                  unsigned int ulNumBlocks)
{
	unsigned int ulMSCInstance;

    ASSERT(pvDrive != 0);

#ifdef USE_RAM_DISK
    RAM_disk_write(ulSector, pucData, ulNumBlocks);
#else
    if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_SD)
	{
		SD_WriteBlocks(ulSector, ulNumBlocks, pucData);
		ulNumBlocks *= 512;
	}
    else if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_EMMC)
	{
		EMMC_WriteBlocks(ulSector, ulNumBlocks, pucData);
		ulNumBlocks *= 512;
	}
	else if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_USB_HOST)
	{
		ulMSCInstance = (unsigned int)&g_USBHMSCDevice[0];

		/* READ BLOCK */
		if (USBHMSCBlockWrite(ulMSCInstance, ulSector, pucData, ulNumBlocks) == 0)
		{
			ulNumBlocks *= 512;
		}
	}
#endif

    return(ulNumBlocks);
}

//*****************************************************************************
//
// This function will return the number of blocks present on a device.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
//
// This function is used to return the total number of blocks on a physical
// device based on the /e pvDrive parameter.
//
// /return Returns the number of blocks that are present in a device.
//
//*****************************************************************************
unsigned int
USBDMSCStorageNumBlocks(void * pvDrive)
{
    unsigned int ulSectorCount = 0;

    //
    // Read the number of sectors.
    //
#ifdef USE_RAM_DISK
    RAM_disk_ioctl(0, GET_SECTOR_COUNT, &ulSectorCount);
#else
    if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_SD)
    {
    	ulSectorCount = (g_SD_FatFs.sects_clust * (g_SD_FatFs.max_clust - 1));
    }
    else if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_EMMC)
    {
    	ulSectorCount = EMMC_GetNumberBlocks();
    }
    else if(USB_GetMSCDevice_EMMC_or_SD() == USB_MSC_USB_HOST)
    {
    	if(fat_devices[DRIVE_NUM_USB].initDone)
    	{
    		ulSectorCount = (g_USB_HOST_FatFs.sects_clust * (g_USB_HOST_FatFs.max_clust - 1));
    	}
    }
#endif

    return(ulSectorCount);
}

//*****************************************************************************
//
// This function will return the current status of a device.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
//
// This function is used to return the current status of the device indicated by
// the /e pvDrive parameter.  This can be used to see if the device is busy,
// or if it is present.
//
// /return Returns the size in bytes of blocks that in a device.
//
//*****************************************************************************
#define USBDMSC_IDLE            0x00000000
#define USBDMSC_NOT_PRESENT     0x00000001
unsigned int USBDMSCStorageStatus(void * pvDrive);
