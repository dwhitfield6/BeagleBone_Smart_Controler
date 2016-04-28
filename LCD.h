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
/* Contains functions to control the LCD (FT81x).
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifndef _LCD_H_
#define _LCD_H_

/******************************************************************************/
/* Pin Definitions			                                                  */
/******************************************************************************/
#define LCD_PD_REGS		SOC_GPIO_3_REGS
#define LCD_INT_REGS	SOC_GPIO_3_REGS

#define LCD_PD_PIN	19
#define LCD_INT_PIN	21

#define CENTER_X 240
#define CENTER_Y 136

/******************************************************************************/
/* RAM_G memory allocation
 *
 * This is what sits in RAM_G. The last address of RAM_G is 1048576.
 * 																	          */
/******************************************************************************/
#define BITMAP_CHARLIE_BEACH_RAM_G_LOCATION_START 	RAM_G
#define BITMAP_CHARLIE_BEACH_RAM_G_LOCATION_END 	261119
#define BITMAP_CHARLIE_BEACH_RAM_G_SIZE 			261120

#define BITMAP_TV_REMOTE_RAM_G_LOCATION_START 		261120
#define BITMAP_TV_REMOTE_RAM_G_LOCATION_END 		272319
#define BITMAP_TV_REMOTE_RAM_G_SIZE 				11200

#define MISC_BUFFER_RAM_G_LOCATION_START 			272320
#define MISC_BUFFER_RAM_G_LOCATION_END 				1040319
#define MISC_BUFFER_RAM_G_SIZE 						768000

#define FULL_SCREEN_SNAPSHOT_LOCATION_START 		MISC_BUFFER_RAM_G_LOCATION_START
#define FULL_SCREEN_SNAPSHOT_LOCATION_END 			MISC_BUFFER_RAM_G_LOCATION_END
#define FULL_SCREEN_SNAPSHOT_SIZE 					768000

#define AUDIO_BUFFER_RAM_G_LOCATION_START 			MISC_BUFFER_RAM_G_LOCATION_START
#define AUDIO_BUFFER_RAM_G_LOCATION_END 			(MISC_BUFFER_RAM_G_LOCATION_START + AUDIO_BUFFER_RAM_G_SIZE - 1)
#define AUDIO_BUFFER_RAM_G_SIZE 					4000

/******************************************************************************/
/* MACRO function declaration                                                 */
/******************************************************************************/
#define CREATE_COLOR_RBG(red,green,blue) ((((red)&255UL)<<16)|(((green)&255UL)<<8)|(((blue)&255UL)<<0))

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/
typedef enum e_read_or_write
{
	MEM_READ = 0,
	MEM_WRITE = 0x80,
}ENUM_FTDI_READ_WRITE;

typedef enum e_lcd_interrupt
{
	INTERRUPT_SWAP 			= 0x01,
	INTERRUPT_TOUCH 		= 0x02,
	INTERRUPT_TAG 			= 0x04,
	INTERRUPT_SOUNT 		= 0x08,
	INTERRUPT_PLAYBACK 		= 0x10,
	INTERRUPT_CMDEMPTY 		= 0x20,
	INTERRUPT_CMDFLAG		= 0x40,
	INTERRUPT_CONVCOMPLETE 	= 0x80,
}ENUM_LCD_INTERRUPT;

typedef struct t_bitmap_header
{
	unsigned char 	Format;
	unsigned short 	Width;
	unsigned short 	Height;
	unsigned short 	Stride;
	unsigned long 	Totalsize;
}TYPE_BITMAP_HEADER;

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern unsigned short RAM_CMD_Offset;
extern unsigned short FT_DispWidth;
extern unsigned short FT_DispHeight;
extern unsigned short FT_DispHCycle;
extern unsigned short FT_DispHOffset;
extern unsigned short FT_DispHSync0;
extern unsigned short FT_DispHSync1;
extern unsigned short FT_DispVCycle;
extern unsigned short FT_DispVOffset;
extern unsigned short FT_DispVSync0;
extern unsigned short FT_DispVSync1;
extern unsigned short FT_DispPCLK;
extern unsigned short FT_DispSwizzle;
extern unsigned short FT_DispPCLKPol;
extern unsigned short FT_DispCSpread;
extern unsigned short FT_DispDither;
extern unsigned char SnapshotBuffer[FULL_SCREEN_SNAPSHOT_SIZE];
extern unsigned char BackLightSetting;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_LCD(void);
void LCD_InterruptConfigure(void);
void LCD_InteruptEnable(ENUM_LCD_INTERRUPT Int);
void LCD_InteruptDisable(ENUM_LCD_INTERRUPT Int);
void LCD_Reset(unsigned char state);
void LCD_Interrupt(unsigned char state);
unsigned char LCD_ft810memRead8(unsigned long ftAddress);
unsigned int LCD_ft810memRead16(unsigned long ftAddress);
unsigned long LCD_ft810memRead32(unsigned long ftAddress);
void LCD_ft810memWrite8(unsigned long ftAddress, unsigned char ftData8);
void LCD_ft810memWrite16(unsigned long ftAddress, unsigned int ftData16);
void LCD_ft810memWrite32(unsigned long ftAddress, unsigned long ftData32);
void LCD_ft810memWriteBuffer(unsigned long ftAddress, unsigned char* ftData, unsigned long bytes);
void LCD_wr8(unsigned long ftAddress, unsigned char ftData8);
void LCD_wr16(unsigned long ftAddress, unsigned int ftData16);
void LCD_wr32(unsigned long ftAddress, unsigned int ftData32);
void LCD_wr8s(unsigned long ftAddress, unsigned char* ftData8);
unsigned char LCD_rd8(unsigned long ftAddress);
unsigned short LCD_rd16(unsigned long ftAddress);
unsigned long LCD_rd32(unsigned long ftAddress);
void LCD_wr_buffer(unsigned long ftAddress, unsigned char* ftData, unsigned char dataskip, unsigned long bytes);
void LCD_rd_buffer(unsigned long ftAddress, unsigned char* ftData, unsigned long bytes);
void LCD_cmd(unsigned long command);
void LCD_host_command(unsigned char MSB, unsigned char Middle, unsigned char LSB);
void LCD_IncrementWriteOffset(unsigned short* offset);
void LCD_WriteCommandParameters(unsigned long parms);
void LCD_SendRAMAddress(unsigned long address, ENUM_FTDI_READ_WRITE read_nwrite);
void LCD_WaitCoprocessorDone(void);
void LCD_cmd_text(unsigned short x, unsigned short y, unsigned short font, unsigned short options, unsigned char* string);
void LCD_cmd_button(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short font, unsigned short options, unsigned char* string);
void LCD_cmd_clock(unsigned short x, unsigned short y, unsigned short r, unsigned short options, unsigned short h, unsigned short m, unsigned short s, unsigned short ms);
void LCD_cmd_bgcolor(unsigned long C);
void LCD_cmd_fgcolor(unsigned long C);
void LCD_cmd_gradcolor(unsigned long C);
void LCD_cmd_gauge(unsigned short x, unsigned short y, unsigned short r, unsigned short options, unsigned short major, unsigned short minor, unsigned short val, unsigned short range);
void LCD_cmd_gradient(unsigned short x0, unsigned short y0, unsigned long rgb0, unsigned short x1, unsigned short y1, unsigned long rgb1);
void LCD_cmd_keys(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short font, unsigned short options, unsigned char* string);
void LCD_cmd_progress(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short options, unsigned short val, unsigned short range);
void LCD_cmd_scrollbar(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short options, unsigned short value, unsigned short size, unsigned short range);
void LCD_cmd_slider(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short options, unsigned short value, unsigned short range);
void LCD_cmd_dial(unsigned short x, unsigned short y, unsigned short r, unsigned short options, unsigned short value);
void LCD_cmd_toggle(unsigned short x, unsigned short y, unsigned short w, unsigned short font, unsigned short options, unsigned short state, unsigned char* string);
void LCD_cmd_setbase(unsigned long b);
void LCD_cmd_number(unsigned short x, unsigned short y, unsigned short font, unsigned short options, unsigned long n);
void LCD_cmd_memcrc(unsigned long ptr, unsigned long num, unsigned long result);
void LCD_cmd_memzero(unsigned long ptr, unsigned long num);
void LCD_cmd_memset(unsigned long ptr, unsigned long value, unsigned long num);
void LCD_cmd_memwrite(unsigned long ptr, unsigned long num, unsigned long* buffer);
void LCD_cmd_memcpy(unsigned long dest, unsigned long src, unsigned long num);
void LCD_cmd_append(unsigned long ptr, unsigned long num);
void LCD_cmd_inflate(unsigned long ptr, unsigned long num, unsigned long* buffer);
void LCD_cmd_loadimage(unsigned long ptr, unsigned long options);
void LCD_cmd_loadidentity(void);
void LCD_cmd_translate(unsigned long tx, unsigned long ty);
void LCD_cmd_scale(unsigned long sx, unsigned long sy);
void LCD_cmd_rotate(unsigned long a);
void LCD_cmd_setmatrix(void);
void LCD_cmd_getmatrix(unsigned long a, unsigned long b, unsigned long c, unsigned long d, unsigned long e, unsigned long f);
void LCD_cmd_getptr(unsigned long result);
void LCD_cmd_getprops(unsigned long ptr, unsigned long width, unsigned long height);
void LCD_cmd_mediafifo(unsigned long ptr, unsigned long size);
void LCD_cmd_playvideo(unsigned long opts);
void LCD_cmd_videostart(void);
void LCD_cmd_videoframe(unsigned long dst, unsigned long ptr);
void LCD_cmd_track(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short tag);
void LCD_cmd_coldstart(void);
void LCD_cmd_interrupt(unsigned long ms);
void LCD_cmd_regread(unsigned long ptr, unsigned long result);
void LCD_cmd_calibrate(unsigned long result);
void LCD_cmd_romfont(unsigned long font, unsigned long romslot);
void LCD_cmd_setfont(unsigned long font, unsigned long ptr);
void LCD_cmd_setfont2(unsigned long font, unsigned long ptr, unsigned long firstchar);
void LCD_cmd_setscratch(unsigned long handle);
void LCD_cmd_setrotate(unsigned long r);
void LCD_cmd_setbitmap(unsigned long addr, unsigned short fmt, unsigned short width, unsigned short height);
void LCD_cmd_spinner(unsigned short x, unsigned short y, unsigned short style, unsigned short scale);
void LCD_cmd_stop(void);
void LCD_cmd_screensaver(void);
void LCD_cmd_sketch(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned long ptr, unsigned short format);
void LCD_cmd_snapshot(unsigned long ptr);
void LCD_cmd_snapshot2(unsigned long fmt, unsigned long ptr, unsigned short x, unsigned short y, unsigned short w, unsigned short h);
void LCD_cmd_logo(void);
void LCD_SetInterruptFlag(void);
void LCD_ClearInterruptFlag(void);
unsigned char LCD_GetInterruptFlag(void);

#endif
/******************************* End of file *********************************/
