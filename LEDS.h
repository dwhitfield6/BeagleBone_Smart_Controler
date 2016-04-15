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
/* Contains functions for controlling the LEDs.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifndef _LEDS_H_
#define _LEDS_H_

/******************************************************************************/
/* Pin Definitions			                                                  */
/******************************************************************************/
#define LED1_MODULE	1
#define LED2_MODULE	1
#define LED3_MODULE	1
#define LED4_MODULE	1

#define LED1_REGS	SOC_GPIO_1_REGS
#define LED2_REGS	SOC_GPIO_1_REGS
#define LED3_REGS	SOC_GPIO_1_REGS
#define LED4_REGS	SOC_GPIO_1_REGS

#define LED1_PIN	21
#define LED2_PIN	22
#define LED3_PIN	23
#define LED4_PIN	24

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* SPEED
 *
 * COntrols the SPeed of the LED seqeunce in LED_Show().
 * 							                                                  */
/******************************************************************************/
#define SPEED 10000

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_LED(void);
void LED_Show(void);

#endif
/******************************* End of file *********************************/
