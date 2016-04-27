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
/* Contains functions to control the test points.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#ifndef _TEST_H_
#define _TEST_H_

/******************************************************************************/
/* Pin Definitions			                                                  */
/******************************************************************************/
#define TP1_MODULE	1
#define TP2_MODULE	1
#define TP3_MODULE	0

#define TP1_REGS	SOC_GPIO_1_REGS
#define TP2_REGS	SOC_GPIO_1_REGS
#define TP3_REGS	SOC_GPIO_0_REGS

#define TP1_PIN	13
#define TP2_PIN	15
#define TP3_PIN	27

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_Test(void);
void TEST_Setpoint1(unsigned char state);
void TEST_Setpoint2(unsigned char state);
void TEST_Setpoint3(unsigned char state);
void TEST_Togglepoint1(void);
void TEST_Togglepoint2(void);
void TEST_Togglepoint3(void);

#endif
/******************************* End of file *********************************/
