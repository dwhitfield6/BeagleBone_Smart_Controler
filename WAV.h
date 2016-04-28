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

#ifndef _WAV_H_
#define _WAV_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* WAV_NAME_SIZE
 *
 * This is the size of the aray holding the WAV file name.
 *                                                                            */
/******************************************************************************/
#define WAV_NAME_SIZE 40

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef struct __attribute__((packed)) t_wav
{
	unsigned char *p_File;		// pointer to the file

	/* Header */
	unsigned char ChunkID[4];
    unsigned long ChunkSize;
    unsigned char Format[4];
    unsigned char Subchunk1ID[4];
    unsigned long Subchunk1Size;
    unsigned short  AudioFormat;
    unsigned short  NumChannels;
    unsigned long SampleRate;
    unsigned long ByteRate;
    unsigned short BlockAlign;
    unsigned short BitsPerSample;
    unsigned char Subchunk2ID[4];
    unsigned long Subchunk2Size;

    /* Extra */
    unsigned char Calculated;
    unsigned char *p_Start;
    unsigned long NumSamples;
    unsigned char Name[WAV_NAME_SIZE];
    unsigned char Valid;
}TYPE_WAV;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern TYPE_WAV WAV_Files[];
extern TYPE_WAV* p_CurrentWAVFile;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_WAV(void);
void WAV_CalculateHeader(TYPE_WAV* file);

#endif
/******************************* End of file *********************************/
