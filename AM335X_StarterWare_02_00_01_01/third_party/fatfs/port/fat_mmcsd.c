/*-----------------------------------------------------------------------*/
/* Stellaris USB module                                                  */
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

/*
 * This file was modified from a sample available from the FatFs
 * web site. It was modified to work with the SitaraWare USB stack.
 */
#include "diskio.h"
#include "hw_types.h"
#include "mmcsd_proto.h"
#include "hs_mmcsdlib.h"
#include "ff.h"
#include "EMMC.h"
#include "SD.h"
#include "uartStdio.h"
#include "usbhmsc.h"
#include "UART.h"


extern tUSBHMSCInstance g_USBHMSCDevice[];

static volatile
DSTATUS USBStat = STA_NOINIT;    /* Disk status */

typedef struct _fatDevice
{
    /* Pointer to underlying device/controller */
    void *dev;
    
    /* File system pointer */
    FATFS *fs;

	/* state */
	unsigned int initDone;

}fatDevice;

#define DRIVE_NUM_USB     	0
#define DRIVE_NUM_SD     	1
#define DRIVE_NUM_EMMC     	2
#define DRIVE_NUM_MAX      	3


fatDevice fat_devices[DRIVE_NUM_MAX];


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS
disk_initialize(
    BYTE bValue)                /* Physical drive number (0) */
{
	unsigned int status;
    unsigned int ulMSCInstance;
   
    if (DRIVE_NUM_MAX <= bValue)
    {
        return STA_NODISK;
    }
    
    if ((DRIVE_NUM_SD == bValue) && (fat_devices[bValue].initDone != 1))
    {
    	/* SD Card init */
    	status = SD_CardInit();

        if (status == 0)
        {
        	UART_PrintString("\r\nSD Card Init Failed \r\n");
            
            return STA_NOINIT;
        }
        else
        {
        	UART_PrintString("\r\nSD Card Init Passed \r\n");
        }

		fat_devices[bValue].initDone = 1;
    }

    if ((DRIVE_NUM_EMMC == bValue) && (fat_devices[bValue].initDone != 1))
	{
		/* EMMC Card init */
		status = EMMC_CardInit();

		if (status == 0)
		{
			UART_PrintString("\r\nEMMC Card Init Failed \r\n");

			return STA_NOINIT;
		}
		else
		{
			UART_PrintString("\r\nnEMMC Card Init Passed \r\n");
		}

		fat_devices[bValue].initDone = 1;
	}

    if (DRIVE_NUM_USB == bValue)
	{
    	ulMSCInstance = (unsigned int)&g_USBHMSCDevice[bValue];

    	/* Set the not initialized flag again. If all goes well and the disk is */
        /* present, this will be cleared at the end of the function.            */
        USBStat |= STA_NOINIT;

        /* Find out if drive is ready yet. */
        if (USBHMSCDriveReady(ulMSCInstance))
		{
        	return(FR_NOT_READY);
		}

        /* Clear the not init flag. */
        USBStat &= ~STA_NOINIT;
	}

    return 0;
}



/*-----------------------------------------------------------------------*/
/* Returns the current status of a drive                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE drv)                   /* Physical drive number (0) */
{
	if(drv == DRIVE_NUM_USB)
	{
		return USBStat;
	}
	return 0;
}



/*-----------------------------------------------------------------------*/
/* This function reads sector(s) from the disk drive                     */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE drv,               /* Physical drive number (0) */
    BYTE* buff,             /* Pointer to the data buffer to store read data */
    DWORD sector,           /* Physical drive nmuber (0) */
    BYTE count)             /* Sector count (1..255) */
{
	unsigned int ulMSCInstance;

	if (drv == DRIVE_NUM_SD)
	{
    	/* READ BLOCK */
#ifdef USE_RAM_DISK
    RAM_disk_read(sector, buff, count);
    return RES_OK;
#else
    if (SD_ReadBlocks(sector, count, buff))
	{
		return RES_OK;
	}
#endif
    }
	else if (drv == DRIVE_NUM_EMMC)
	{
		if (EMMC_ReadBlocks(sector, count, buff))
		{
			return RES_OK;
		}
	}
	else if (drv == DRIVE_NUM_USB)
	{
		ulMSCInstance = (unsigned int)&g_USBHMSCDevice[drv];

		if(USBStat & STA_NOINIT)
		{
			return(RES_NOTRDY);
		}

		/* READ BLOCK */
		if (USBHMSCBlockRead(ulMSCInstance, sector, buff, count) == 0)
		{
			return RES_OK;
		}
	}

    return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* This function writes sector(s) to the disk drive                     */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write (
    BYTE ucDrive,           /* Physical drive number (0) */
    const BYTE* buff,       /* Pointer to the data to be written */
    DWORD sector,           /* Start sector number (LBA) */
    BYTE count)             /* Sector count (1..255) */
{
	unsigned int ulMSCInstance;

	if (ucDrive == DRIVE_NUM_SD)
	{
    	/* WRITE BLOCK */
#ifdef USE_RAM_DISK
    RAM_disk_write(sector, buff, count);
    return RES_OK;
#else
		if (SD_WriteBlocks(sector, count, (unsigned char*)buff))
		{
			return RES_OK;
		}
#endif
	}
	else if (ucDrive == DRIVE_NUM_EMMC)
	{
		if (EMMC_WriteBlocks(sector, count, (unsigned char*)buff))
		{
			return RES_OK;
		}
	}
	else if (ucDrive == DRIVE_NUM_USB)
	{
		ulMSCInstance = (unsigned int)&g_USBHMSCDevice[ucDrive];

		if (ucDrive || !count)
		{
			return RES_PARERR;
		}
		if (USBStat & STA_NOINIT)
		{
			return RES_NOTRDY;
		}
		if (USBStat & STA_PROTECT)
		{
			return RES_WRPRT;
		}

		/* WRITE BLOCK */
		if(USBHMSCBlockWrite(ulMSCInstance, sector, (unsigned char *)buff, count) == 0)
		{
			return RES_OK;
		}
	}

    return RES_ERROR;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE drv,               /* Physical drive number (0) */
    BYTE ctrl,              /* Control code */
    void *buff)             /* Buffer to send/receive control data */
{
	if (drv == DRIVE_NUM_USB)
	{
		if(USBStat & STA_NOINIT)
		{
			return(RES_NOTRDY);
		}

		switch(ctrl)
		{
			case CTRL_SYNC:
			{
				return(RES_OK);
			}

			default:
			{
				return(RES_PARERR);
			}
		}
	}

	return RES_OK;
}

