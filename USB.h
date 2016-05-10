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

#ifndef ___USB_H__
#define ___USB_H__

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

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

typedef enum e_msc_status
{
	USB_DISCONNECT,
	USB_CONNECT,
}ENUM_USB_STATUS;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern ENUM_MSC_STATES g_eMSCState;
extern unsigned int ulStatus;
extern unsigned int epStatus;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_USB(void);
void Init_USB0(void);
void USB_InterruptConfigure0(void);
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

#endif
/******************************* End of file *********************************/
