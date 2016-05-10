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
/* Contains the ISRs.
 *                                                                            */
/******************************************************************************/

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Interrupt Priorities                                                       */
/******************************************************************************/
#define ADC_INTERRUPT_PRIORITY 				8
#define I2C_INTERRUPT_PRIORITY 				7
#define LCD_INTERRUPT_PRIORITY 				6
#define RTC_INTERRUPT_PRIORITY 				5
#define DMA_COMPLETE_INTERRUPT_PRIORITY 	2
#define DMA_ERROR_INTERRUPT_PRIORITY 		2
#define SD_INTERRUPT_PRIORITY 				2
#define SPI_INTERRUPT_PRIORITY 				4
#define TIMER_2_INTERRUPT_PRIORITY			1
#define TIMER_3_INTERRUPT_PRIORITY 			0
#define UART_INTERRUPT_PRIORITY 			9
#define USB_INTERRUPT_PRIORITY 				3

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* ISR Declarations                                                           */
/******************************************************************************/
void GPIO_3A_ISR(void);
void UART_0_ISR(void);
void TMR_2_ISR(void);
void TMR_3_ISR(void);
void RTC_ISR(void);
void I2C_0_ISR(void);
void ADC_0_ISR(void);
void SPI_1_ISR(void);
void USB_0_ISR(void);

#endif
/******************************* End of file *********************************/
