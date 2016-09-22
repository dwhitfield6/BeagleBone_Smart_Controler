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
/* Contains functions to configure and control the I2C bus.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "gpio_v2.h"
#include "hsi2c.h"
#include "hw_types.h"
#include "interrupt.h"
#include "soc_AM335x.h"

#include "INTERRUPTS.h"
#include "I2C.h"
#include "PMIC.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
static unsigned char I2C0_FailFlag = FALSE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
TYPE_I2C_DATA I2C_0_Buffers;

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_I2C
 *
 * Initializes the I2C buses.
 *                                                                            */
/******************************************************************************/
void Init_I2C(void)
{
	Init_I2C_Module0();
}

/******************************************************************************/
/* Init_I2C_Module0
 *
 * Initializes the I2C module 0 bus.
 *                                                                            */
/******************************************************************************/
void Init_I2C_Module0(void)
{
	I2C0ModuleClkConfig();

    /* Put I2C in reset/disabled state. */
    I2CMasterDisable(SOC_I2C_0_REGS);

    /* Disable Auto-Idle functionality. */
    I2CAutoIdleDisable(SOC_I2C_0_REGS);

    /* Configure I2C bus frequency. */
    I2CMasterInitExpClk(SOC_I2C_0_REGS, I2C_SYS_CLK_48MHZ, I2C_INTERNAL_CLK_12MHZ, 200000);

    /* Bring I2C out of reset. */
    I2CMasterEnable(SOC_I2C_0_REGS);

    /* set up the interrupts */
    I2C_InterruptConfigure0();
}

/******************************************************************************/
/* I2C_InterruptConfigure0
 *
 * Configures the I2C module 0 interrupt.
 *                                                                            */
/******************************************************************************/
void I2C_InterruptConfigure0(void)
{
    /* Registering the Interrupt Service Routine(ISR). */
    IntRegister(SYS_INT_I2C0INT, I2C_0_ISR);

    /* Setting the priority for the system interrupt in AINTC. */
    IntPrioritySet(SYS_INT_I2C0INT, I2C_INTERRUPT_PRIORITY, AINTC_HOSTINT_ROUTE_IRQ );

    /* Enabling the system interrupt in AINTC. */
    IntSystemEnable(SYS_INT_I2C0INT);
}

/******************************************************************************/
/* I2C_SendReceiveData0
 *
 * Reads data from I2C0 bus.
 *                                                                            */
/******************************************************************************/
unsigned char I2C_SendReceiveData0(unsigned char address, unsigned char* write, unsigned char BytesToWrite, unsigned char* read, unsigned char BytesToRead)
{
	I2C_0_Buffers.p_Transmit = write;
	I2C_0_Buffers.BytesTransmitted = 0;
	I2C_0_Buffers.TransmitBytes = BytesToWrite;
	I2C_0_Buffers.p_Receive = read;
	I2C_0_Buffers.BytesReceived = 0;
	I2C_0_Buffers.ReceiveBytes = BytesToRead;

    /* Set I2C slave address. */
    I2CMasterSlaveAddrSet(SOC_I2C_0_REGS, address);

	I2CSetDataCount(SOC_I2C_0_REGS, I2C_0_Buffers.TransmitBytes);

    /* Clear status of all interrupts */
    I2CMasterIntClearEx(SOC_I2C_0_REGS, 0x7FF);

    /* Configure I2C controller in Master Transmitter mode. */
    I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_TX);

    I2C_ClearFailFlag0();

    /* Transmit and No ACK interrupt is enabled. */
    I2CMasterIntEnableEx(SOC_I2C_0_REGS, I2C_INT_TRANSMIT_READY | I2C_INT_NO_ACK);

    /* Generate Start Condition over I2C bus. */
    I2CMasterStart(SOC_I2C_0_REGS);

    while(I2CMasterBusBusy(SOC_I2C_0_REGS) == 0)
    {
    	if(I2C_GetFailFlag0())
    	{
    		return FALSE;
    	}
    }

    while(I2C_0_Buffers.BytesTransmitted != I2C_0_Buffers.TransmitBytes)
    {
    	if(I2C_GetFailFlag0())
    	{
    		return FALSE;
    	}
    }

    /* Wait until I2C registers are ready to be accessed. */
    while(!(I2CMasterIntRawStatus(SOC_I2C_0_REGS) & (I2C_INT_ADRR_READY_ACESS)));

    if(BytesToRead)
    {
		/* Data Count specifies the number of bytes to be received. */
		I2CSetDataCount(SOC_I2C_0_REGS, I2C_0_Buffers.ReceiveBytes);

		/* Clear status of all interrupts */
		I2CMasterIntClearEx(SOC_I2C_0_REGS, 0x7FF);

		/* Configure I2C controller in Master Receiver mode. */
		I2CMasterControl(SOC_I2C_0_REGS, I2C_CFG_MST_RX);

		/* Receive and Stop Condition Interrupts are enabled. */
		I2CMasterIntEnableEx(SOC_I2C_0_REGS, I2C_INT_RECV_READY | I2C_INT_STOP_CONDITION);

		/* Generate Start Condition over I2C bus */
		I2CMasterStart(SOC_I2C_0_REGS);

		while(I2CMasterBusBusy(SOC_I2C_0_REGS) == 0)
		{
	    	if(I2C_GetFailFlag0())
	    	{
	    		return FALSE;
	    	}
		}

	    while(I2C_0_Buffers.BytesReceived != I2C_0_Buffers.ReceiveBytes)
	    {
	    	if(I2C_GetFailFlag0())
	    	{
	    		return FALSE;
	    	}
	    }
    }
    return TRUE;
}

/******************************************************************************/
/* I2C_SetFailFlag0
 *
 * Sets the I2C 0 Fail flag.
 *                                                                            */
/******************************************************************************/
void I2C_SetFailFlag0(void)
{
	I2C0_FailFlag = TRUE;
}

/******************************************************************************/
/* I2C_ClearFailFlag0
 *
 * Clears the I2C 0 Fail flag.
 *                                                                            */
/******************************************************************************/
void I2C_ClearFailFlag0(void)
{
	I2C0_FailFlag = FALSE;
}

/******************************************************************************/
/* I2C_GetFailFlag0
 *
 * Gets the I2C 0 Fail flag.
 *                                                                            */
/******************************************************************************/
unsigned char I2C_GetFailFlag0(void)
{
	return I2C0_FailFlag;
}

/******************************* End of file *********************************/
