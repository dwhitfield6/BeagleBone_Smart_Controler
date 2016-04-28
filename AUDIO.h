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
/* Contains functions to set up the GPIO.
 *                                                                            */
/******************************************************************************/

#ifndef _AUDIO_H_
#define _AUDIO_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "WAV.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* AUDIO_BYTES_TO_MOVE_RAM_G
 *
 * This is the number of bytes to move to RAM_G per timeout.
 *                                                                            */
/******************************************************************************/
#define AUDIO_BYTES_TO_MOVE_RAM_G 1000

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern unsigned long AudioBytesTransferred;
extern unsigned long AUD_AudioTimeoutCount;
extern unsigned long LastWritePointerAddress;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_Audio(void);
void AUD_PlayWAV(TYPE_WAV* file, unsigned char volume);
void AUD_StopWAV(void);
void AUD_SetPlayingFlag(void);
void AUD_ClearPlayingFlag(void);
unsigned char AUD_GetPlayingFlag(void);
void AUD_TransferWAVToRAMG(unsigned long NumberOfBytes);
void AUD_SetPlaybackFinishedFlag(void);
void AUD_ClearPlaybackFinishedFlag(void);
unsigned char AUD_GetPlaybackFinishedFlag(void);
void AUD_SetTimoutFlag(void);
void AUD_ClearTimoutFlag(void);
unsigned char AUD_GetTimoutFlag(void);
unsigned char AUD_IsWAVPlaying(void);

#endif
/******************************* End of file *********************************/
