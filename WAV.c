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
/* Contains functions to decode a WAV audio file.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "beaglebone.h"
#include "GPIO.h"
#include "MISC.h"
#include "WAV.h"
#include "WAV_POST_COMPLETE.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
TYPE_WAV WAV_Files[] =
{
		{
				.p_File = WAV_PostComplete,
				.Calculated = FALSE,
				.Name = "WAV_POST_COMPLETE.wav",
				.Valid = FALSE,
		},
};

TYPE_WAV* p_CurrentWAVFile;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_WAV
 *
 * Initializes the functions for decoding a WAV file.
 *                                                                            */
/******************************************************************************/
void Init_WAV(void)
{
	unsigned int NumberOfWAV;
	unsigned char i;

	NumberOfWAV = sizeof(WAV_Files) / sizeof(TYPE_WAV);
	for(i=0; i < NumberOfWAV; i++)
	{
		WAV_CalculateHeader(&WAV_Files[i]);
	}
}

/******************************************************************************/
/* WAV_CalculateHeader
 *
 * Calculate the WAV file headers.
 *                                                                            */
/******************************************************************************/
void WAV_CalculateHeader(TYPE_WAV* file)
{
	unsigned char *pointer;
	unsigned char status = PASS;

	pointer = file->p_File;
	MSC_BufferCopy(pointer,&file->ChunkID[0], 44);

	file->Valid = FALSE;

	pointer = file->p_File + 44;
	file->p_Start = pointer;

	file->NumSamples = file->Subchunk2Size / (file->NumChannels * (file->BitsPerSample >> 3));

	file->Calculated = TRUE;

        /* chaeck the Chunk ID */
    if(file->ChunkID[0] != 'R' || file->ChunkID[1] != 'I' || file->ChunkID[2] != 'F' || file->ChunkID[3] != 'F')
    {
    	status = FAIL;
    }

    if(status)
    {
    	if(file->Format[0] != 'W' || file->Format[1] != 'A' || file->Format[2] != 'V' || file->Format[3] != 'E')
    	{
    		status = FAIL;
    	}
    }

    if(status)
    {
    	if(file->Subchunk1ID[0] != 'f' || file->Subchunk1ID[1] != 'm' || file->Subchunk1ID[2] != 't' || file->Subchunk1ID[3] != ' ')
    	{
    		status = FAIL;
    	}
    }

    if(status)
    {
    	if(file->Subchunk2ID[0] != 'd' || file->Subchunk2ID[1] != 'a' || file->Subchunk2ID[2] != 't' || file->Subchunk2ID[3] != 'a')
    	{
    		status = FAIL;
    	}
    }

    if(status)
    {
    	if(file->Subchunk1Size != 16)
    	{
    		status = FAIL;
    	}
    }

    if(status)
    {
    	if(file->AudioFormat != 1)
    	{
    		status = FAIL;
    	}
    }

    if(status)
    {
    	if((file->NumChannels != 1) && (file->NumChannels != 2))
    	{
    		status = FAIL;
    	}
    }

    if(status)
    {
    	file->Valid = TRUE;
    }

}

/******************************* End of file *********************************/
