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
/* Contains functions to configure and control the SPI bus.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "gpio_v2.h"
#include "hw_control_AM335x.h"
#include "hw_types.h"
#include "interrupt.h"
#include "mcspi.h"
#include "pin_mux.h"
#include "soc_AM335x.h"

#include "MISC.h"
#include "SPI.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_SPI
 *
 * Initializes the SPI bus.
 *                                                                            */
/******************************************************************************/
void Init_SPI(void)
{
	Init_SPI_Module0();
	Init_SPI_Module1();
}

/******************************************************************************/
/* Init_SPI_Module1
 *
 * Initializes the SPI 1 bus.
 *                                                                            */
/******************************************************************************/
void Init_SPI_Module1(void)
{
    /* Enable the clocks for McSPI1 module. */
    McSPI1ModuleClkConfig();

	/* Reset the McSPI instance.*/
	McSPIReset(LCD_SPI_REGS);

	/* Enable chip select pin.*/
	McSPICSEnable(LCD_SPI_REGS);

	/* Enable master mode of operation.*/
	McSPIMasterModeEnable(LCD_SPI_REGS);

	HWREG(LCD_SPI_REGS + MCSPI_SYSCONFIG) &= ~MCSPI_SYSCONFIG_AUTOIDLE;

	/* D0 is output D1 is input */
	HWREG(LCD_SPI_REGS + MCSPI_SYST) &= ~MCSPI_SYST_SPIDATDIR0;
	HWREG(LCD_SPI_REGS + MCSPI_SYST) |= MCSPI_SYST_SPIDATDIR1;

	/* Chip select time control */
	HWREG(LCD_SPI_REGS + MCSPI_CHCONF(LCD_CS)) |= (MCSPI_CH0CONF_TCS0_3P5 << MCSPI_CH0CONF_TCS0_SHIFT); // 3h = 3.5 clock cycles

    /* Set the Initial SPI delay for first transfer */
    HWREG(LCD_SPI_REGS + MCSPI_MODULCTRL) |= (MCSPI_MODULCTRL_INITDLY_32CLKDLY << MCSPI_MODULCTRL_INITDLY_SHIFT);

    /* Set the MS field with the user sent value. */
    HWREG(LCD_SPI_REGS + MCSPI_MODULCTRL) |= MCSPI_MODULCTRL_SINGLE;

	/* D1 for reception */
	HWREG(LCD_SPI_REGS + MCSPI_CHCONF(LCD_CS)) |= MCSPI_CH0CONF_IS; // 1h = Data line 1 (SPIDAT[1]) selected for reception.

	/* D0 for Transmission */
	HWREG(LCD_SPI_REGS + MCSPI_CHCONF(LCD_CS)) |= MCSPI_CH0CONF_DPE1; // 1h = No transmission on data line 1 (SPIDAT[1])
	HWREG(LCD_SPI_REGS + MCSPI_CHCONF(LCD_CS)) &= ~MCSPI_CH0CONF_DPE0; // 0h = Data line 0 (SPIDAT[0]) selected for transmission

	/* Configure the McSPI bus clock depending on clock mode. */
	McSPIClkConfig(LCD_SPI_REGS, 48000000, 100000, LCD_CS, MCSPI_CLK_MODE_0);

	/* Configure the word length.*/
	McSPIWordLengthSet(LCD_SPI_REGS, MCSPI_WORD_LENGTH(8), LCD_CS);

	/* Set polarity of SPIEN to low.*/
	McSPICSPolarityConfig(LCD_SPI_REGS, MCSPI_CS_POL_LOW, LCD_CS);

    /* Enable the transmitter FIFO of McSPI peripheral.*/
    McSPITxFIFOConfig(LCD_SPI_REGS, MCSPI_TX_FIFO_DISABLE, LCD_CS);

    /* Enable the receiver FIFO of McSPI peripheral.*/
    McSPIRxFIFOConfig(LCD_SPI_REGS, MCSPI_RX_FIFO_DISABLE, LCD_CS);
}

/******************************************************************************/
/* Init_SPI_Module0
 *
 * Initializes the SPI 0 bus.
 *                                                                            */
/******************************************************************************/
void Init_SPI_Module0(void)
{
    /* Enable the clocks for McSPI1 module. */
    McSPI0ModuleClkConfig();

	/* Reset the McSPI instance.*/
	McSPIReset(RF_SPI_REGS);

	/* Enable chip select pin.*/
	McSPICSEnable(RF_SPI_REGS);

	/* Enable master mode of operation.*/
	McSPIMasterModeEnable(RF_SPI_REGS);

	HWREG(RF_SPI_REGS + MCSPI_SYSCONFIG) &= ~MCSPI_SYSCONFIG_AUTOIDLE;

	/* D0 is output D1 is input */
	HWREG(RF_SPI_REGS + MCSPI_SYST) &= ~MCSPI_SYST_SPIDATDIR0;
	HWREG(RF_SPI_REGS + MCSPI_SYST) |= MCSPI_SYST_SPIDATDIR1;

	/* Chip select time control */
	HWREG(RF_SPI_REGS + MCSPI_CHCONF(RF_CS)) |= (MCSPI_CH0CONF_TCS0_3P5 << MCSPI_CH0CONF_TCS0_SHIFT); // 3h = 3.5 clock cycles

    /* Set the Initial SPI delay for first transfer */
    HWREG(RF_SPI_REGS + MCSPI_MODULCTRL) |= (MCSPI_MODULCTRL_INITDLY_32CLKDLY << MCSPI_MODULCTRL_INITDLY_SHIFT);

    /* Set the MS field with the user sent value. */
    HWREG(RF_SPI_REGS + MCSPI_MODULCTRL) |= MCSPI_MODULCTRL_SINGLE;

	/* D1 for reception */
	HWREG(RF_SPI_REGS + MCSPI_CHCONF(RF_CS)) |= MCSPI_CH0CONF_IS; // 1h = Data line 1 (SPIDAT[1]) selected for reception.

	/* D0 for Transmission */
	HWREG(RF_SPI_REGS + MCSPI_CHCONF(RF_CS)) |= MCSPI_CH0CONF_DPE1; // 1h = No transmission on data line 1 (SPIDAT[1])
	HWREG(RF_SPI_REGS + MCSPI_CHCONF(RF_CS)) &= ~MCSPI_CH0CONF_DPE0; // 0h = Data line 0 (SPIDAT[0]) selected for transmission

	/* Configure the McSPI bus clock depending on clock mode. */
	McSPIClkConfig(RF_SPI_REGS, 48000000, 100000, RF_CS, MCSPI_CLK_MODE_0);

	/* Configure the word length.*/
	McSPIWordLengthSet(RF_SPI_REGS, MCSPI_WORD_LENGTH(8), RF_CS);

	/* Set polarity of SPIEN to low.*/
	McSPICSPolarityConfig(RF_SPI_REGS, MCSPI_CS_POL_LOW, RF_CS);

    /* Enable the transmitter FIFO of McSPI peripheral.*/
    McSPITxFIFOConfig(RF_SPI_REGS, MCSPI_TX_FIFO_DISABLE, RF_CS);

    /* Enable the receiver FIFO of McSPI peripheral.*/
    McSPIRxFIFOConfig(RF_SPI_REGS, MCSPI_RX_FIFO_DISABLE, RF_CS);
}

/******************************************************************************/
/* UART_INTCConfigure0
 *
 * Configures the SPI 1 interrupt.
 *                                                                            */
/******************************************************************************/
void SPI_INTCConfigure1(void)
{
    /* Register McSPIIsr interrupt handler */
    IntRegister(SYS_INT_SPI1INT, SPI_1_ISR);

    /* Set Interrupt Priority */
    IntPrioritySet(SYS_INT_SPI1INT, 5, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enable system interrupt in AINTC */
    IntSystemEnable(SYS_INT_SPI1INT);
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
/* SPI_WriteByte0
 *
 * Sends a byte over the SPI0 port and waites for reception.
 *                                                                            */
/******************************************************************************/
void SPI_WriteByte0(unsigned char data)
{
	while(!(McSPIChannelStatusGet(RF_SPI_REGS, RF_CS) & MCSPI_CH_STAT_TXS_EMPTY));
    McSPITransmitData(RF_SPI_REGS, data, RF_CS);
    while(!(McSPIChannelStatusGet(RF_SPI_REGS, RF_CS) & MCSPI_CH_STAT_RXS_FULL));
    dummy = McSPIReceiveData(RF_SPI_REGS, RF_CS);
}

/******************************************************************************/
/* SPI_WriteByte1
 *
 * Sends a byte over the SPI1 port and waites for reception.
 *                                                                            */
/******************************************************************************/
void SPI_WriteByte1(unsigned char data)
{
	while(!(McSPIChannelStatusGet(LCD_SPI_REGS, LCD_CS) & MCSPI_CH_STAT_TXS_EMPTY));
    McSPITransmitData(LCD_SPI_REGS, data, LCD_CS);
    while(!(McSPIChannelStatusGet(LCD_SPI_REGS, LCD_CS) & MCSPI_CH_STAT_RXS_FULL));
    dummy = McSPIReceiveData(LCD_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* SPI_ReadWriteByte0
 *
 * Sends/receives a byte over the SPI0 port.
 *                                                                            */
/******************************************************************************/
unsigned char SPI_ReadWriteByte0(unsigned char data)
{
	unsigned int dummy;

	while(!(McSPIChannelStatusGet(RF_SPI_REGS, RF_CS) & MCSPI_CH_STAT_TXS_EMPTY));
    McSPITransmitData(RF_SPI_REGS, data, RF_CS);
    while(!(McSPIChannelStatusGet(RF_SPI_REGS, RF_CS) & MCSPI_CH_STAT_RXS_FULL));
    dummy = McSPIReceiveData(RF_SPI_REGS, RF_CS);
    return (unsigned char)dummy;
}

/******************************************************************************/
/* SPI_ReadWriteByte1
 *
 * Sends/receives a byte over the SPI1 port.
 *                                                                            */
/******************************************************************************/
unsigned char SPI_ReadWriteByte1(unsigned char data)
{
	unsigned int dummy;

	while(!(McSPIChannelStatusGet(LCD_SPI_REGS, LCD_CS) & MCSPI_CH_STAT_TXS_EMPTY));
    McSPITransmitData(LCD_SPI_REGS, data, LCD_CS);
    while(!(McSPIChannelStatusGet(LCD_SPI_REGS, LCD_CS) & MCSPI_CH_STAT_RXS_FULL));
    dummy = McSPIReceiveData(LCD_SPI_REGS, LCD_CS);
    return (unsigned char)dummy;
}

/******************************************************************************/
/* SPI_WriteByteNoRx0
 *
 * Sends a byte over the SPI0 port.
 *                                                                            */
/******************************************************************************/
void SPI_WriteByteNoRx0(unsigned char data)
{
	while(!(McSPIChannelStatusGet(RF_SPI_REGS, RF_CS) & MCSPI_CH_STAT_TXS_EMPTY));
    McSPITransmitData(RF_SPI_REGS, data, RF_CS);
}

/******************************************************************************/
/* SPI_WriteByteNoRx1
 *
 * Sends a byte over the SPI1 port.
 *                                                                            */
/******************************************************************************/
void SPI_WriteByteNoRx1(unsigned char data)
{
	while(!(McSPIChannelStatusGet(LCD_SPI_REGS, LCD_CS) & MCSPI_CH_STAT_TXS_EMPTY));
    McSPITransmitData(LCD_SPI_REGS, data, LCD_CS);
}


/******************************* End of file *********************************/
