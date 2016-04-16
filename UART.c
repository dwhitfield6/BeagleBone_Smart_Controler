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
#include "gpio_v2.h"
#include "hw_cm_wkup.h"
#include "hw_cm_per.h"
#include "hw_control_AM335x.h"
#include "hw_types.h"
#include "interrupt.h"
#include "soc_AM335x.h"
#include "uart_irda_cir.h"

#include "CMD.h"
#include "UART.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned char UART_TX_buffer[UART_TX_SIZE];
unsigned char UART_RX_buffer[UART_RX_SIZE];
unsigned long UART_RX_place = 0;
unsigned long UART_TX_add = 0;
unsigned long UART_TX_remove = 0;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_UART
 *
 * Initializes the UART module.
 *                                                                            */
/******************************************************************************/
void Init_UART(void)
{
	UART_Module0ClkConfig();

	/* set pin directions */
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_UART_RXD(0)) = (CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_PUTYPESEL | CONTROL_CONF_UART0_RXD_CONF_UART0_RXD_RXACTIVE); // RXD
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_UART_TXD(0)) = CONTROL_CONF_UART0_TXD_CONF_UART0_TXD_PUTYPESEL; // TXD

    /* Configuring the system clocks for UART0 instance. */
    UART0ModuleClkConfig();

    /* Performing the Pin Multiplexing for UART0 instance. */
    UARTPinMuxSetup(0);

    /* Performing a module reset. */
    UARTModuleReset(SOC_UART_0_REGS);

    UartConfigure(115200, (UART_FRAME_WORD_LENGTH_8 |
                                     UART_FRAME_NUM_STB_1 |
                                     UART_PARITY_NONE));

    UartFIFOConfigure();

    /* Configuring AINTC to receive UART0 interrupts. */
    UART_INTCConfigure0();

    /* Enabling required UART Interrupts. */
    UARTIntEnable(SOC_UART_0_REGS,
                  (UART_INT_LINE_STAT | UART_INT_RHR_CTI));
}

/******************************************************************************/
/* UART_Module0ClkConfig
 *
 * Configures the UART0 module clock.
 *                                                                            */
/******************************************************************************/
void UART_Module0ClkConfig(void)
{

    /* Writing to MODULEMODE field of CM_WKUP_UART0_CLKCTRL register. */
    HWREG(SOC_CM_WKUP_REGS + CM_WKUP_UART0_CLKCTRL) |=
          CM_WKUP_UART0_CLKCTRL_MODULEMODE_ENABLE;

    /* Waiting for MODULEMODE field to reflect the written value. */
    while(CM_WKUP_UART0_CLKCTRL_MODULEMODE_ENABLE !=
          (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_UART0_CLKCTRL) &
           CM_WKUP_UART0_CLKCTRL_MODULEMODE));

    /*
    ** Waiting for CLKACTIVITY_UART0_GFCLK field in CM_WKUP_CLKSTCTRL
    ** register to attain desired value.
    */
    while(CM_WKUP_CLKSTCTRL_CLKACTIVITY_UART0_GFCLK !=
          (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_CLKSTCTRL) &
           CM_WKUP_CLKSTCTRL_CLKACTIVITY_UART0_GFCLK));

    /*
    ** Waiting for IDLEST field in CM_WKUP_UART0_CLKCTRL register to attain
    ** desired value.
    */
    while((CM_WKUP_UART0_CLKCTRL_IDLEST_FUNC <<
           CM_WKUP_UART0_CLKCTRL_IDLEST_SHIFT) !=
          (HWREG(SOC_CM_WKUP_REGS + CM_WKUP_UART0_CLKCTRL) &
           CM_WKUP_UART0_CLKCTRL_IDLEST));
}

/******************************************************************************/
/* UART_PrintChar
 *
 * Prints a character over the UART port.
 *                                                                            */
/******************************************************************************/
void UART_PrintChar(unsigned char data)
{
	UARTCharPut(SOC_UART_0_REGS, data);
}

/******************************************************************************/
/* UART_PrintBuffer
 *
 * Prints a buffer over the UART port.
 *                                                                            */
/******************************************************************************/
void UART_PrintBuffer(unsigned char* data, unsigned long bytes)
{
	while(bytes > 0)
	{
		UARTCharPut(SOC_UART_0_REGS, *data);
		data++;
		bytes--;
	}
}

/******************************************************************************/
/* UART_PrintString
 *
 * Prints a string over the UART port.
 *                                                                            */
/******************************************************************************/
void UART_PrintString(unsigned char* data)
{
	while(*data != 0)
	{
		UART_PrintChar(*data);
		data++;
	}
}

/******************************************************************************/
/* UartFIFOConfigure
 *
 * Sets up the UART FIFO.
 *                                                                            */
/******************************************************************************/
void UartFIFOConfigure(void)
{
    unsigned int fifoConfig = 0;

    /*
    ** - Transmit Trigger Level Granularity is 4
    ** - Receiver Trigger Level Granularity is 1
    ** - Transmit FIFO Space Setting is 56. Hence TX Trigger level
    **   is 8 (64 - 56). The TX FIFO size is 64 bytes.
    ** - The Receiver Trigger Level is 1.
    ** - Clear the Transmit FIFO.
    ** - Clear the Receiver FIFO.
    ** - DMA Mode enabling shall happen through SCR register.
    ** - DMA Mode 0 is enabled. DMA Mode 0 corresponds to No
    **   DMA Mode. Effectively DMA Mode is disabled.
    */
    fifoConfig = UART_FIFO_CONFIG(UART_TRIG_LVL_GRANULARITY_4,
                                  UART_TRIG_LVL_GRANULARITY_1,
                                  UART_FCR_TX_TRIG_LVL_56,
                                  1,
                                  1,
                                  1,
                                  UART_DMA_EN_PATH_SCR,
                                  UART_DMA_MODE_0_ENABLE);

    /* Configuring the FIFO settings. */
    UARTFIFOConfig(SOC_UART_0_REGS, fifoConfig);
}

/******************************************************************************/
/* UartBaudRateSet
 *
 * Sets up the UART baud rate.
 *                                                                            */
/******************************************************************************/
void UartBaudRateSet(unsigned int baudRate)
{
    unsigned int divisorValue = 0;

    /* Computing the Divisor Value. */
    divisorValue = UARTDivisorValCompute(UART_MODULE_INPUT_CLK,
                                         baudRate,
                                         UART16x_OPER_MODE,
                                         UART_MIR_OVERSAMPLING_RATE_42);

    /* Programming the Divisor Latches. */
    UARTDivisorLatchWrite(SOC_UART_0_REGS, divisorValue);
}

/******************************************************************************/
/* UartLineCharacSet
 *
 * Sets up the parity and character length
 *                                                                            */
/******************************************************************************/
void UartLineCharacSet(unsigned int lineCharConfig)
{
    unsigned int wLenStbFlag = 0;
    unsigned int parityFlag = 0;

    wLenStbFlag = (lineCharConfig & (UART_LCR_NB_STOP | UART_LCR_CHAR_LENGTH));
    parityFlag = (lineCharConfig & (UART_LCR_PARITY_TYPE2 |
                                    UART_LCR_PARITY_TYPE1 |
                                    UART_LCR_PARITY_EN));

    UARTLineCharacConfig(SOC_UART_0_REGS, wLenStbFlag, parityFlag);
}

/******************************************************************************/
/* UartConfigure
 *
 * Configures the UART.
 *                                                                            */
/******************************************************************************/
void UartConfigure(unsigned int baudRate, unsigned int lineCharConfig)
{
    UartBaudRateSet(baudRate);

    /* Switching to Configuration Mode B. */
    UARTRegConfigModeEnable(SOC_UART_0_REGS, UART_REG_CONFIG_MODE_B);

    UartLineCharacSet(lineCharConfig);

    /* Disabling write access to Divisor Latches. */
    UARTDivisorLatchDisable(SOC_UART_0_REGS);

    /* Disabling Break Control. */
    UARTBreakCtl(SOC_UART_0_REGS, UART_BREAK_COND_DISABLE);

    /* Switching to UART16x operating mode. */
    UARTOperatingModeSelect(SOC_UART_0_REGS, UART16x_OPER_MODE);
}

/******************************************************************************/
/* UART_ISR
 *
 * UART Interrupt service routine.
 *                                                                            */
/******************************************************************************/
void UART_ISR(void)
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
            UART_AddToBuffer(rxByte);
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
                    UART_AddToBuffer(rxByte);
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
                UART_AddToBuffer(rxByte);
            }

        break;

        default:
        break;
    }
}

/******************************************************************************/
/* UART_INTCConfigure0
 *
 * Configures the UART interupt.
 *                                                                            */
/******************************************************************************/
void UART_INTCConfigure0(void)
{
    /* Registering the Interrupt Service Routine(ISR). */
    IntRegister(SYS_INT_UART0INT, UART_ISR);

    /* Setting the priority for the system interrupt in AINTC. */
    IntPrioritySet(SYS_INT_UART0INT, 10, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the system interrupt in AINTC. */
    IntSystemEnable(SYS_INT_UART0INT);
}

/******************************************************************************/
/* UART_AddToBuffer
 *
 * Adds a character to the UART buffer.
 *                                                                            */
/******************************************************************************/
void UART_AddToBuffer (unsigned char data)
{
	if(data == CR)
	{
		if(UART_RX_place > 0)
		{
			UART_RX_buffer[UART_RX_place] = 0;
			CMD_SetNewCommand();
			MSC_BufferCopy(UART_RX_buffer, Command_String, LARGEST_COMMAND);
			Command_String[LARGEST_COMMAND - 1] = 0;
			UART_RX_place = 0;
		}
		else
		{
			UART_PrintString(CRLN);
			UART_PrintString("> ");
		}
	}
	else if(data == BACKSPACE || data == DELETE)
	{
		if(UART_RX_place > 0)
		{
			UART_RX_place--;
			UART_RX_buffer[UART_RX_place] = 0;
			UART_PrintChar(BACKSPACE);
			UART_PrintChar(' ');
			UART_PrintChar(BACKSPACE);
		}
	}
	else if(data == LN)
	{
		/* do nothing */
	}
	else
	{
		UART_PrintChar(data);
		if(UART_RX_place < UART_RX_SIZE)
		{
			UART_RX_buffer[UART_RX_place] = data;
			UART_RX_place++;
		}
	}
}

/******************************* End of file *********************************/
