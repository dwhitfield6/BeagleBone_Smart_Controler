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
/* Contains functions to set up the audio portion of the FT81x.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "beaglebone.h"
#include "FT_Gpu.h"
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "AUDIO.h"
#include "GPIO.h"
#include "LCD.h"
#include "LEDS.h"
#include "MISC.h"
#include "TIMERS.h"
#include "WAV.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
static unsigned char WAVPlayingFlag = FALSE;
static unsigned char WAVPlaybackDoneFlag = TRUE;
static unsigned char WAVTimeoutFlag = FALSE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned long AudioBytesTransferred = 0;
unsigned long AUD_AudioTimeoutCount = 0;
unsigned long LastWritePointerAddress = 0;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_Audio
 *
 * Initializes the Audio functionailty of the FT81x.
 *                                                                            */
/******************************************************************************/
void Init_Audio(void)
{
	AUD_TestAudio();
	AUD_PlayWAV(&WAV_Files[0], 100);
}

/******************************************************************************/
/* AUD_PlayWAV
 *
 * Plays a WAV file.
 *                                                                            */
/******************************************************************************/
void AUD_PlayWAV(TYPE_WAV* file, unsigned char volume)
{
	if(!AUD_GetPlayingFlag())
	{
		/* make sure that a previous file isnt already playing */
		if(file->Valid && file->Calculated)
		{
			AUD_AudioTimeoutCount = (file->SampleRate / AUDIO_BYTES_TO_MOVE_RAM_G) / 2;
			TMR_ResetAudioPlaybackTimer();
			AUD_ClearPlaybackFinishedFlag();
			p_CurrentWAVFile = file;
			LastWritePointerAddress = AUDIO_BUFFER_RAM_G_LOCATION_START;
			LCD_wr8(REG_VOL_PB,volume);									// configure audio playback volume
			LCD_wr32(REG_PLAYBACK_START,AUDIO_BUFFER_RAM_G_LOCATION_START);	// configure audio buffer starting address
			LCD_wr32(REG_PLAYBACK_LENGTH, AUDIO_BUFFER_RAM_G_SIZE);						// configure audio buffer length
			LCD_wr16(REG_PLAYBACK_FREQ,file->SampleRate);				// configure audio sampling frequency
			LCD_wr8(REG_PLAYBACK_FORMAT,LINEAR_SAMPLES);				// configure audio format
			LCD_wr8(REG_PLAYBACK_LOOP,1);								// configure loop mode
			AUD_SetPlayingFlag();
		}
	}
}

/******************************************************************************/
/* AUD_StopWAV
 *
 * Stops a WAV file.
 *                                                                            */
/******************************************************************************/
void AUD_StopWAV(void)
{
	LCD_wr8(REG_VOL_PB,0);				// mute
	LCD_wr32(REG_PLAYBACK_LENGTH,0);	// configure the playback length to 0
	LCD_wr8(REG_PLAYBACK_PLAY,1);		// start audio playback
}

/******************************************************************************/
/* AUD_SetPlayingFlag
 *
 * Sets the playing flag that indicated that a WAV file is currently being
 *  transfered and playing.
 *                                                                            */
/******************************************************************************/
void AUD_SetPlayingFlag(void)
{
	WAVPlayingFlag = TRUE;
}

/******************************************************************************/
/* AUD_ClearPlayingFlag
 *
 * Sets the playing flag that indicated that a WAV file is currently being
 *  transfered and playing.
 *                                                                            */
/******************************************************************************/
void AUD_ClearPlayingFlag(void)
{
	WAVPlayingFlag = FALSE;
}

/******************************************************************************/
/* AUD_GetPlayingFlag
 *
 * Returns the playing flag that indicated that a WAV file is currently being
 *  transfered and playing.
 *                                                                            */
/******************************************************************************/
unsigned char AUD_GetPlayingFlag(void)
{
	return WAVPlayingFlag;
}

/******************************************************************************/
/* AUD_GetPlayingFlag
 *
 * Returns the playing flag that indicated that a WAV file is currently being
 *  transfered and playing.
 *                                                                            */
/******************************************************************************/
void AUD_TransferWAVToRAMG(unsigned long NumberOfBytes)
{
	unsigned long SpaceAvailable;
	unsigned long CurrentReadPointer;

	CurrentReadPointer = (LCD_rd32(REG_PLAYBACK_READPTR) & 0x000FFFFF);
	if(CurrentReadPointer == 0)
	{
		CurrentReadPointer = AUDIO_BUFFER_RAM_G_LOCATION_START;
	}

	SpaceAvailable = CIRCULAR_BUFFER_SPACE(LastWritePointerAddress, CurrentReadPointer, AUDIO_BUFFER_RAM_G_SIZE);

	if(SpaceAvailable >= NumberOfBytes)
	{
		SpaceAvailable = NumberOfBytes;
	}

	if((p_CurrentWAVFile->NumSamples - AudioBytesTransferred) < SpaceAvailable)
	{
		SpaceAvailable = (p_CurrentWAVFile->NumSamples - AudioBytesTransferred);
	}

	if(CIRCULAR_BUFFER_SPACE_TILL_END(LastWritePointerAddress, AUDIO_BUFFER_RAM_G_LOCATION_END) < SpaceAvailable)
	{
		LCD_wr_buffer(LastWritePointerAddress, p_CurrentWAVFile->p_Start + (AudioBytesTransferred * p_CurrentWAVFile->BlockAlign), (p_CurrentWAVFile->BlockAlign - 1), CIRCULAR_BUFFER_SPACE_TILL_END(LastWritePointerAddress, AUDIO_BUFFER_RAM_G_LOCATION_END));
		SpaceAvailable -=  CIRCULAR_BUFFER_SPACE_TILL_END(LastWritePointerAddress, AUDIO_BUFFER_RAM_G_LOCATION_END);
		AudioBytesTransferred += CIRCULAR_BUFFER_SPACE_TILL_END(LastWritePointerAddress, AUDIO_BUFFER_RAM_G_LOCATION_END);
		LastWritePointerAddress = AUDIO_BUFFER_RAM_G_LOCATION_START;

		/* transfer behind the buffer */
		LCD_wr_buffer(LastWritePointerAddress, p_CurrentWAVFile->p_Start + AudioBytesTransferred, (p_CurrentWAVFile->BlockAlign - 1), SpaceAvailable);
		AudioBytesTransferred += SpaceAvailable;
		LastWritePointerAddress += SpaceAvailable;
	}
	else
	{
		LCD_wr_buffer(LastWritePointerAddress, p_CurrentWAVFile->p_Start + (AudioBytesTransferred * p_CurrentWAVFile->BlockAlign), (p_CurrentWAVFile->BlockAlign - 1), SpaceAvailable);
		AudioBytesTransferred += SpaceAvailable;
		LastWritePointerAddress += SpaceAvailable;
	}

	if(AudioBytesTransferred >= p_CurrentWAVFile->NumSamples)
	{
		LCD_wr8(REG_PLAYBACK_LOOP, 0);
		AUD_SetPlaybackFinishedFlag();
	}
}

/******************************************************************************/
/* AUD_SetPlaybackFinishedFlag
 *
 * Sets the playing done flag.
 *                                                                            */
/******************************************************************************/
void AUD_SetPlaybackFinishedFlag(void)
{
	WAVPlaybackDoneFlag = TRUE;
}

/******************************************************************************/
/* AUD_ClearPlaybackFinishedFlag
 *
 * Sets the playing done flag.
 *                                                                            */
/******************************************************************************/
void AUD_ClearPlaybackFinishedFlag(void)
{
	AudioBytesTransferred = 0;
	WAVPlaybackDoneFlag = FALSE;
}

/******************************************************************************/
/* AUD_GetPlaybackFinishedFlag
 *
 * Gets the playing done flag.
 *                                                                            */
/******************************************************************************/
unsigned char AUD_GetPlaybackFinishedFlag(void)
{
	return WAVPlaybackDoneFlag;
}

/******************************************************************************/
/* AUD_SetTimoutFlag
 *
 * Sets the timeout flag which causes a transfer.
 *                                                                            */
/******************************************************************************/
void AUD_SetTimoutFlag(void)
{
	WAVTimeoutFlag = TRUE;
}

/******************************************************************************/
/* AUD_ClearTimoutFlag
 *
 * Clears the timeout flag which causes a transfer.
 *                                                                            */
/******************************************************************************/
void AUD_ClearTimoutFlag(void)
{
	WAVTimeoutFlag = FALSE;
}

/******************************************************************************/
/* AUD_GetTimoutFlag
 *
 * Gets the timeout flag which causes a transfer.
 *                                                                            */
/******************************************************************************/
unsigned char AUD_GetTimoutFlag(void)
{
	return WAVTimeoutFlag;
}

/******************************************************************************/
/* AUD_IsWAVPlaying
 *
 * Gets the status of a Playback.
 *                                                                            */
/******************************************************************************/
unsigned char AUD_IsWAVPlaying(void)
{
	unsigned char temp;

	temp = LCD_rd8(REG_PLAYBACK_PLAY);
	return temp;
}

/******************************************************************************/
/* AUD_TestAudio
 *
 * Tests the audio sound effects.
 *                                                                            */
/******************************************************************************/
void AUD_TestAudio(void)
{
	AUD_AmpShutdown(FALSE);

	LCD_wr8(REG_VOL_SOUND,5); //set the volume to maximum
	LCD_wr16(REG_SOUND, (0x6C<< 8) | 0x41); // C8 MIDI note on xylophone
	LCD_wr8(REG_PLAY, 1); // play the sound

	MSC_DelayUS(10000);

	LCD_wr16(REG_SOUND,0x0);//configure silence as sound to be played
	LCD_wr8(REG_PLAY,1);//play sound
	AUD_AmpShutdown(TRUE);
}

/******************************************************************************/
/* AUD_AmpShutdown
 *
 * Controls the audio amps shutdown pin.
 *                                                                            */
/******************************************************************************/
void AUD_AmpShutdown(unsigned char state)
{
	unsigned char temp;

	temp = LCD_rd8(REG_GPIO);	// Read the FT800 GPIO register for a read/modify/write operation
	if(state)
	{
		temp &= ~(0x02);
	}
	else
	{
		temp |= 0x02;
	}
	LCD_wr8(REG_GPIO, temp);		// Enable the DISP signal to the LCD panel
}

/******************************* End of file *********************************/
