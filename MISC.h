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
/* Contains miscellaneous functions.
 *                                                                            */
/******************************************************************************/

#ifndef _MISC_H_
#define _MISC_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define FULL 1
#define HALF 0

/*
** Round-off division value to upper integer. The parameters passed for this
** macro should be greater than 1.
*/
#define DIV_CEIL(a,b)       (((a) + (b) - 1) / (b))

/*  Deprecated function - Compiler message */
#ifdef __GNUC__
    #define DEPRECATED(func) func __attribute__ ((deprecated))
#else
    #define DEPRECATED(func) func
#endif

/* WFI intstruction */
#define wfi()                                   asm("   WFI");

/******************************************************************************/
/* MACRO function declaration                                                 */
/******************************************************************************/
/* Return count in buffer. Basd off linux CIRC_CNT */
#define CIRCULAR_BUFFER_COUNT(head,tail,size) (((head) - (tail)) & ((size)-1U))

/* Return space available, 0..size-1.  We always leave one free char
 *  as a completely full buffer has head == tail, which is the same as
 *  empty. Based off linux CIRC_SPACE */
#define CIRCULAR_BUFFER_SPACE(head,tail,size) CIRCULAR_BUFFER_COUNT((tail),((head)+1U),(size))

/* Return space until the wrap */
#define CIRCULAR_BUFFER_SPACE_TILL_END(place,end) (end - place)

/******************************************************************************/
/* MISC_BUFFER_SIZE
 *
 * This is the size of the Misc buffer.
 *                                                                            */
/******************************************************************************/
#define MISC_BUFFER_SIZE 1000

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern unsigned int dummy;
extern unsigned char MISC_Buffer[MISC_BUFFER_SIZE];

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void MSC_BufferCopy(void* From, void* To, unsigned short bytes);
unsigned char MSC_LowercaseChar(unsigned char Input);
void MSC_DelayUS(unsigned long US);
void MSC_DelayNOP(unsigned long nop);
double MSC_Round(double input);
unsigned short MSC_HEXtoBCD(unsigned short input);
unsigned short MSC_BCDtoHEX(unsigned short input);
unsigned short MSC_EndianShort(unsigned short number);
unsigned short MSC_EndianShortArray(unsigned char* buffer);
unsigned long MSC_EndianLong(unsigned long number);
unsigned long MSC_EndianLongArray(unsigned char* buffer);
unsigned char MSC_ReverseByte(unsigned char This);
unsigned short MISC_CRC_CRC16 (unsigned short data, unsigned short previous, unsigned char size);
unsigned short MISC_Calculate_CRC16(unsigned char* buffer, unsigned short bytes);

#endif
/******************************* End of file *********************************/
