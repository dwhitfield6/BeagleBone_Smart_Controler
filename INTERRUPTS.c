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
#include "edma.h"
#include "FT_Gpu.h"
#include "gpio_v2.h"
#include "hw_cm_per.h"
#include "hw_cm_wkup.h"
#include "hw_types.h"
#include "hw_usbOtg_AM335x.h"
#include "hs_mmcsd.h"
#include "hsi2c.h"
#include "interrupt.h"
#include "pin_mux.h"
#include "soc_AM335x.h"
#include "tsc_adc.h"
#include "uart_irda_cir.h"
#include "usblib.h"
#include "usbdevicepriv.h"
#include "usblibpriv.h"

#include "ADC.h"
#include "AUDIO.h"
#include "GPIO.h"
#include "GUI.h"
#include "I2C.h"
#include "LCD.h"
#include "INTERRUPTS.h"
#include "LEDS.h"
#include "MISC.h"
#include "POWER.h"
#include "RTCC.h"
#include "SD.h"
#include "SYSTEM.h"
#include "TEST.h"
#include "TIMERS.h"
#include "UART.h"
#include "USB.h"

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

		/* Audio Playback Timer */
		if(AUD_GetPlayingFlag())
		{
			TMR_AudioPlaybackTimer++;
			if(TMR_AudioPlaybackTimer >= AUD_AudioTimeoutCount)
			{
				TMR_ResetAudioPlaybackTimer();
				AUD_SetTimoutFlag();
			}
		}

		/* LCD Backlight Timer */
		if(TMR_BacklightTimer < GUI_BACKLIGHT_TIMER)
		{
			TMR_BacklightTimer++;
			if(TMR_BacklightTimer == GUI_BACKLIGHT_TIMER)
			{
				GUI_SetBacklightTimeout();
			}
		}

	    /* Clear the status of the interrupt flags */
	    DMTimerIntStatusClear(SOC_DMTIMER_2_REGS, DMTIMER_INT_OVF_IT_FLAG);
	}
}

/******************************************************************************/
/* TMR_3_ISR
 *
 * Timer 3 Interrupt service routine.
 *                                                                            */
/******************************************************************************/
void TMR_3_ISR(void)
{
	unsigned char status;

	status = DMTimerIntStatusGet(SOC_DMTIMER_3_REGS);

	if(status == DMTIMER_INT_OVF_IT_FLAG)
	{
		DMTimerDisable(SOC_DMTIMER_3_REGS); // start the timer
		TMR_SetMISCTimerFlag();
		DMTimerIntStatusClear(SOC_DMTIMER_3_REGS, DMTIMER_INT_OVF_IT_FLAG);
	}
}

/******************************************************************************/
/* RTC_ISR
 *
 * RTC interrupt service routine.
 *                                                                            */
/******************************************************************************/
void RTC_ISR(void)
{
	RTC_GetTimeDate(&CurrentTimeDate);
	RTC_SetFlag();
}

/******************************************************************************/
/* I2C_0_ISR
 *
 * I2C module 0 interrupt service routine.
 *                                                                            */
/******************************************************************************/
void I2C_0_ISR(void)
{
	unsigned int status = 0;

	/* interrupt status */
	status = I2CMasterIntStatus(SOC_I2C_0_REGS);

	if(status & I2C_INT_RECV_READY)
	{
		/* Receive data from data receive register */
		*I2C_0_Buffers.p_Receive = I2CMasterDataGet(SOC_I2C_0_REGS);
		I2C_0_Buffers.p_Receive++;
		I2C_0_Buffers.BytesReceived++;

		/* Clear interrupt flag */
		I2CMasterIntClearEx(SOC_I2C_0_REGS,  I2C_INT_RECV_READY);

		if(I2C_0_Buffers.BytesReceived == I2C_0_Buffers.ReceiveBytes)
		{
			/* Disable the receive ready interrupt */
			I2CMasterIntDisableEx(SOC_I2C_0_REGS, I2C_INT_RECV_READY);
			/* Generate a STOP */
			I2CMasterStop(SOC_I2C_0_REGS);
		}
	}

	if (status & I2C_INT_TRANSMIT_READY)
	{
		/* Put data to data transmit register of i2c */
		I2CMasterDataPut(SOC_I2C_0_REGS, *I2C_0_Buffers.p_Transmit);
		I2C_0_Buffers.p_Transmit++;
		I2C_0_Buffers.BytesTransmitted++;

		/* Clear Transmit interrupt status */
		I2CMasterIntClearEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY);

		if(I2C_0_Buffers.BytesTransmitted == I2C_0_Buffers.TransmitBytes)
		{
			/* Disable the transmit ready interrupt */
			I2CMasterIntDisableEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY);

			if(!I2C_0_Buffers.ReceiveBytes)
			{
				/* Generate a STOP */
				I2CMasterStop(SOC_I2C_0_REGS);
			}
		}
	}

	if (status & I2C_INT_STOP_CONDITION)
	{
		/* Disable transmit data ready and receive data read interupt */
		I2CMasterIntDisableEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY | I2C_INT_RECV_READY | I2C_INT_STOP_CONDITION);

		/* Clear interrupt flag */
		I2CMasterIntClearEx(SOC_I2C_0_REGS,  I2C_INT_STOP_CONDITION);

		I2C_SetFailFlag0();
	}

	if(status & I2C_INT_NO_ACK)
	{
		I2CMasterIntDisableEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY  | I2C_INT_RECV_READY | I2C_INT_NO_ACK | I2C_INT_STOP_CONDITION);

		/* Generate a STOP */
		I2CMasterStop(SOC_I2C_0_REGS);

		/* Clear interrupt flag */
		I2CMasterIntClearEx(SOC_I2C_0_REGS,  I2C_INT_NO_ACK);

		I2C_SetFailFlag0();
	}
}

/******************************************************************************/
/* ADC_0_ISR
 *
 * ADC module 0 interrupt service routine.
 *                                                                            */
/******************************************************************************/
void ADC_0_ISR(void)
{
	unsigned int status;

    status = TSCADCIntStatus(SOC_ADC_TSC_0_REGS);

    if(status & TSCADC_END_OF_SEQUENCE_INT)
    {
         /* Read data from fifo 0 */
         Voltage.ADCCounts_VDD_5V0 	= TSCADCFIFOADCDataRead(SOC_ADC_TSC_0_REGS, TSCADC_FIFO_0);
         Voltage.ADCCounts_SYS_5V0 	= TSCADCFIFOADCDataRead(SOC_ADC_TSC_0_REGS, TSCADC_FIFO_0);
         Voltage.ADCCounts_VBAT 		= TSCADCFIFOADCDataRead(SOC_ADC_TSC_0_REGS, TSCADC_FIFO_0);
         Voltage.ADCCounts_VDD_3V3 	= TSCADCFIFOADCDataRead(SOC_ADC_TSC_0_REGS, TSCADC_FIFO_0);
         ADC_SetFinishedFlag();
    }

    TSCADCIntStatusClear(SOC_ADC_TSC_0_REGS, status);
}

/******************************************************************************/
/* SPI_1_ISR
 *
 * SPI module 1 Interrupt service routine.
 *                                                                            */
/******************************************************************************/
void SPI_1_ISR(void)
{

}

/******************************************************************************/
/* USB_0_ISR
 *
 * USB module 0 Interrupt service routine.
 *                                                                            */
/******************************************************************************/
void USB_0_ISR(void)
{
    extern tUSBInstanceObject g_USBInstance[];
    unsigned int epStatus = 0;
    unsigned int ulStatus = 0;
    //
    // Get the controller interrupt status.
    //
    ulStatus = HWREG(g_USBInstance[0].uiSubBaseAddr + USB_0_IRQ_STATUS_1);
    //
    // Get the EP interrupt status.
    //
    epStatus = HWREG(g_USBInstance[0].uiSubBaseAddr + USB_0_IRQ_STATUS_0);
    //
    // Clear the controller interrupt status.
    //
    HWREG(g_USBInstance[0].uiSubBaseAddr + USB_0_IRQ_STATUS_1) = ulStatus;
    //
    // Clear the EP interrupt status.
    //
    HWREG(g_USBInstance[0].uiSubBaseAddr + USB_0_IRQ_STATUS_0) = epStatus;

#ifdef DMA_MODE
    HWREG(USBSS_BASE + USBSS_IRQ_STATUS) =
        HWREG(USBSS_BASE + USBSS_IRQ_STATUS);
#endif
    //
    //Call the Interrupt Handler.
    //
    USBDeviceIntHandlerInternal(0, ulStatus, &epStatus);
    //
    //End of Interrupts.
    //
    HWREG(g_USBInstance[0].uiSubBaseAddr + USB_0_IRQ_EOI) = 0;

#ifdef DMA_MODE
    HWREG(USBSS_BASE + USBSS_IRQ_EOI) = 0;
#endif

}

/******************************* End of file *********************************/
