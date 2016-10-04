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
/* Contains functions to set up the USB device.
 *                                                                            */
/******************************************************************************/

#ifndef _USB_MSC_DEVICE_H__
#define _USB_MSC_DEVICE_H__

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define NUMBER_OF_ENDPOINTS     2 //Total number of send points(RX +TX) used in this USB configuration
#define USB_MSC_BUFER_SIZE      512
#define USB_INSTANCE 			0
#define FLAG_UPDATE_STATUS      1
#define USBMSC_ACTIVITY_TIMEOUT 30

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef enum e_msc_states
{
	MSC_DEV_DISCONNECTED,
	MSC_DEV_CONNECTED,
	MSC_DEV_IDLE,
	MSC_DEV_READ,
	MSC_DEV_WRITE,
}ENUM_MSC_STATES;

typedef enum e_msc_sd
{
	USB_MSC_SD,
	USB_MSC_EMMC,
}ENUM_USB_MSC_DEVICE;

typedef enum e_msc_status
{
	USB_DISCONNECT,
	USB_CONNECT,
}ENUM_USB_STATUS;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern ENUM_MSC_STATES g_eMSCState;
extern unsigned int g_bufferIndex;
extern unsigned char *dataBuffer;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_USB_Device(void);
void Init_USB0(void);
ENUM_USB_MSC_DEVICE USB_GetMSCDevice_EMMC_or_SD(void);
void USB_SetMSCDevice_EMMC_or_SD(ENUM_USB_MSC_DEVICE device);
void USB_InterruptConfigure0(void);
void USB_InterruptEnable0(unsigned char state);
unsigned int USB_DMSCEventCallback0(void *pvCBData, unsigned int ulEvent, unsigned int ulMsgParam, void *pvMsgData);
void USB_SetMSCState0(ENUM_MSC_STATES state);
ENUM_MSC_STATES USB_GetMSCState0(void);
void USB_SetUSBStatus0(ENUM_USB_STATUS state);
ENUM_USB_STATUS USB_GetUSBStatus0(void);
void USB_SetUSBStatusFlag0(void);
void USB_ClearUSBStatusFlag0(void);
unsigned char USB_GetUSBStatusFlag0(void);
void USB_SetUSBMSCFlag0(void);
void USB_ClearUSBMSCFlag0(void);
unsigned char USB_GetUSBMSCFlag0(void);

#endif /* _USB_MSC_DEVICE_H__ */
/******************************* End of file *********************************/
