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
/* Contains functions to send and receive data over UART.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifndef _UART_H_
#define _UART_H_

/******************************************************************************/
/* UART_TX_SIZE
 *
 * UART transmit buffer size.
 * 							                                                  */
/******************************************************************************/
#define UART_TX_SIZE 2000

/******************************************************************************/
/* UART_RX_SIZE
 *
 * UART receive buffer size.
 * 							                                                  */
/******************************************************************************/
#define UART_RX_SIZE 2000

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/
#define UART_MODULE_INPUT_CLK      (48000000u)
#define CR '\r'
#define LN '\n'
#define CRLN "\r\n"
#define BACKSPACE 0x08
#define DELETE 0x7F

/******************************************************************************/
/* MACRO function declaration                                                 */
/******************************************************************************/
/* Return count in buffer. Basd off linux CIRC_CNT */
#define CIRCULAR_BUFFER_COUNT(head,tail,size) (((head) - (tail)) & ((size)-1U))

/* Return space available, 0..size-1.  We always leave one free char
 *  as a completely full buffer has head == tail, which is the same as
 *  empty. Based off linux CIRC_SPACE */
#define CIRCULAR_BUFFER_SPACE(head,tail,size) CIRCULAR_BUFFER_COUNT((tail),((head)+1U),(size))

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
extern unsigned char UART_TX_buffer[UART_TX_SIZE];
extern unsigned char UART_RX_buffer[UART_RX_SIZE];
extern unsigned long UART_RX_place;
extern unsigned long UART_TX_add;
extern unsigned long UART_TX_remove;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_UART(void);
void UART_Module0ClkConfig(void);
void UART_PrintChar(unsigned char data);
void UART_PrintBuffer(unsigned char* data, unsigned long bytes);
void UART_PrintString(unsigned char* data);
void UART_FIFOConfigure(void);
void UART_BaudRateSet(unsigned int baudRate);
void UART_LineCharacSet(unsigned int lineCharConfig);
void UART_Configure(unsigned int baudRate, unsigned int lineCharConfig);
void UART_0_ISR(void);
void UART_InterruptConfigure0(void);
void UART_AddToRXBuffer(unsigned char data);
void UART_AddToTXBuffer(unsigned char data);
unsigned char UART_GetTXInterruptStatus(void);

#endif
/******************************* End of file *********************************/
