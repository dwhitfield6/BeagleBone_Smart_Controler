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

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "beaglebone.h"
#include "dmtimer.h"
#include "FT_Gpu.h"
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "pin_mux.h"
#include "soc_AM335x.h"
#include "uart_irda_cir.h"

#include "GPIO.h"
#include "GUI.h"
#include "LCD.h"
#include "INTERRUPTS.h"
#include "LEDS.h"
#include "MISC.h"
#include "SYSTEM.h"
#include "TEST.h"
#include "TIMERS.h"
#include "UART.h"

/******************************************************************************/
/* Interrupt Service Routines                                                 */
/******************************************************************************/

/******************************************************************************/
/* GPIO_3A_ISR
 *
 * Interrupt Service routine for GPIO module 3A.
 *                                                                            */
/******************************************************************************/
void GPIO_3A_ISR(void)
{
    if(GPIOPinIntStatus(SOC_GPIO_3_REGS, GPIO_INT_LINE_1, LCD_INT_PIN))
    {
    	LCD_Interrupt(OFF);
    	LCD_SetInterruptFlag();
        GPIOPinIntClear(SOC_GPIO_3_REGS, GPIO_INT_LINE_1, LCD_INT_PIN);
    }
}

/******************************************************************************/
/* UART_0_ISR
 *
 * UART 0 Interrupt service routine.
 *                                                                            */
/******************************************************************************/
void UART_0_ISR(void)
{
    unsigned int rxErrorType = 0;
    unsigned char rxByte = 0;
    unsigned int intId = 0;
    unsigned int idx = 0;

    /* Checking ths source of UART interrupt. */
    intId = UARTIntIdentityGet(SOC_UART_0_REGS);

    switch(intId)
    {
        case UART_INTID_RX_THRES_REACH:
            rxByte = UARTCharGetNonBlocking(SOC_UART_0_REGS);
            UART_AddToRXBuffer(rxByte);
            break;

        case UART_INTID_RX_LINE_STAT_ERROR:
            rxErrorType = UARTRxErrorGet(SOC_UART_0_REGS);

            /* Check if Overrun Error has occured. */
            if(rxErrorType & UART_LSR_RX_OE)
            {
                /* Read the entire RX FIFO and the data in RX Shift register. */
                for(idx = 0; idx < (RX_FIFO_SIZE + 1); idx++)
                {
                    rxByte = UARTFIFOCharGet(SOC_UART_0_REGS);
                    UART_AddToRXBuffer(rxByte);
                }

                break;
            }
            rxByte = UARTFIFOCharGet(SOC_UART_0_REGS);
            break;

        case UART_INTID_CHAR_TIMEOUT:
            /* Read all the data in RX FIFO. */
            while(TRUE == UARTCharsAvail(SOC_UART_0_REGS))
            {
                rxByte = UARTFIFOCharGet(SOC_UART_0_REGS);
                UART_AddToRXBuffer(rxByte);
            }

            break;

        case (UART_INTID_TX_THRES_REACH):
			/* UART transfer register and FIFO is empty */
			if(UART_TX_remove != UART_TX_add)
			{
				while(UARTTxFIFOFullStatusGet(SOC_UART_0_REGS) == UART_TX_FIFO_NOT_FULL)
				{
					if(UART_TX_remove == UART_TX_add)
					{
						break;
					}
					UARTFIFOCharPut(SOC_UART_0_REGS, UART_TX_buffer[UART_TX_remove]);
					UART_TX_remove = (UART_TX_remove + 1UL) % UART_TX_SIZE;
				}
			}
			else
			{
				/* Disabling required TX Interrupts. */
				UARTIntDisable(SOC_UART_0_REGS, UART_INT_THR);
			}
			break;

        default:
        	break;
    }
}

/******************************************************************************/
/* TMR_2_ISR
 *
 * Timer 2 Interrupt service routine.
 *                                                                            */
/******************************************************************************/
void TMR_2_ISR(void)
{
	unsigned char status;

	status = DMTimerIntStatusGet(SOC_DMTIMER_2_REGS);

	if(status == DMTIMER_INT_OVF_IT_FLAG)
	{
		TEST_Togglepoint1();

		/* GUI  New Screen Tag Timer */
		if(TMR_GetNewScreenTagTimerEnabled())
		{
			TMR_NewScreenTagTimer++;
			if(TMR_NewScreenTagTimer >= GUI_NEW_SCREEN_TAG_TIMER)
			{
				TMR_SetNewScreenTagTimerEnabled(OFF);
				TMR_ResetNewScreenTagTimer();
				GUI_SetTagTimoutFlag();
			}
		}


	    /* Clear the status of the interrupt flags */
	    DMTimerIntStatusClear(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_IT_FLAG);
	}
}

/******************************* End of file *********************************/
