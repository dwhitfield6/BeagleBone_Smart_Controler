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
/* Contains functions to control the LCD (FT81x).
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include "interrupt.h"
#include "FT_Gpu.h"
#include "gpio_v2.h"
#include "hw_types.h"
#include "mcspi.h"
#include "soc_AM335x.h"

#include "GUI.h"
#include "INTERRUPTS.h"
#include "LCD.h"
#include "MISC.h"
#include "SPI.h"
#include "SYSTEM.h"

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* Private Variable                                                           */
/******************************************************************************/
static unsigned char LCD_Interruptflag = FALSE;

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/
unsigned short RAM_CMD_Offset = 0;
unsigned short FT_DispWidth = 480;
unsigned short FT_DispHeight = 272;
unsigned short FT_DispHCycle =  548;
unsigned short FT_DispHOffset = 43;
unsigned short FT_DispHSync0 = 0;
unsigned short FT_DispHSync1 = 41;
unsigned short FT_DispVCycle = 292;
unsigned short FT_DispVOffset = 12;
unsigned short FT_DispVSync0 = 0;
unsigned short FT_DispVSync1 = 10;
unsigned short FT_DispPCLK = 5;
unsigned short FT_DispSwizzle = 0;
unsigned short FT_DispPCLKPol = 1;
unsigned short FT_DispCSpread = 1;
unsigned short FT_DispDither = 1;
unsigned char SnapshotBuffer[FULL_SCREEN_SNAPSHOT_SIZE];

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/

/******************************************************************************/
/* Init_LCD
 *
 * Initializes the LCD driver IC.
 *                                                                            */
/******************************************************************************/
void Init_LCD(void)
{
	unsigned char temp = 0;
	unsigned char  HardFailCount;
	unsigned char  FailCount;
	unsigned char gpio;
	unsigned char duty;

	/* configure GPIO interrupt pin */
	LCD_InterruptConfigure();

	HardRestartFTDI:
	HardFailCount = 0;

	LCD_Reset(TRUE);
	MSC_DelayUS(500);
	LCD_Reset(FALSE);

	RestartFTDI:
	FailCount = 0;

	/* switch from standby to active mode */
	LCD_host_command(FT810_ACTIVE,0x00,0x00);
	MSC_DelayUS(300);

	/* select the external clock crystal */
	LCD_host_command(FT810_CLKEXT,0x00,0x00);
	MSC_DelayUS(100);

	while(temp != 0x7C)
	{
		temp = LCD_rd8(REG_ID);
		MSC_DelayUS (20);
		FailCount++;
		if(FailCount > 50)
		{
			HardFailCount++;
			if(HardFailCount > 10)
			{
				goto HardRestartFTDI;
			}
			else
			{
				goto RestartFTDI;
			}
		}
	}

	LCD_wr8(REG_PCLK, ZERO);												// Set PCLK to zero - don't clock the LCD until later
	LCD_wr8(REG_PWM_DUTY, ZERO);										// Turn off backlight

	/* Configure the McSPI bus clock depending on clock mode. */
	McSPIClkConfig(LCD_FRAM_SPI_REGS, 48000000, 4000000, LCD_CS, MCSPI_CLK_MODE_0);

	LCD_wr16(REG_HCYCLE, FT_DispHCycle);
	LCD_wr16(REG_HOFFSET, FT_DispHOffset);
	LCD_wr16(REG_HSYNC0, FT_DispHSync0);
	LCD_wr16(REG_HSYNC1, FT_DispHSync1);
	LCD_wr16(REG_HSIZE, FT_DispWidth);
	LCD_wr16(REG_VSIZE, FT_DispHeight);
	LCD_wr16(REG_VCYCLE, FT_DispVCycle);
	LCD_wr16(REG_VOFFSET, FT_DispVOffset);
	LCD_wr16(REG_VSYNC0, FT_DispVSync0);
	LCD_wr16(REG_VSYNC1, FT_DispVSync1);
	LCD_wr16(REG_SWIZZLE, FT_DispSwizzle);
	LCD_wr16(REG_PCLK_POL, FT_DispPCLKPol);
	LCD_wr16(REG_CSPREAD, FT_DispCSpread);
	LCD_wr16(REG_DITHER, FT_DispDither);

	/* Backlight PWM frequency */
	LCD_wr16(REG_PWM_HZ, 10000);

	gpio = LCD_rd8(REG_GPIO);	// Read the FT800 GPIO register for a read/modify/write operation
	gpio = gpio | 0x80;			// set bit 7 of FT800 GPIO register (DISP) - others are inputs
	LCD_wr8(REG_GPIO, gpio);		// Enable the DISP signal to the LCD panel
	LCD_wr8(REG_PCLK, FT_DispPCLK);	// Now start clocking data to the LCD panel

	GUI_DrawInitialScreen();
	for(duty = 0; duty <= 128; duty+=5)
	{
		LCD_wr8(REG_PWM_DUTY, duty);									// Turn on backlight - ramp up slowly to full brighness
		MSC_DelayUS(50);
	}

	LCD_wr16(REG_GPIOX, 0x8020); 									// INT_N is push -pull
	LCD_wr8(REG_INT_MASK, 0);										// disable all interrupts
}

/******************************************************************************/
/* UART_InterruptConfigure0
 *
 * Configures the UART interupt.
 *                                                                            */
/******************************************************************************/
void LCD_InterruptConfigure(void)
{

    GPIOIntTypeSet(LCD_INT_REGS, LCD_INT_PIN, GPIO_INT_TYPE_NO_LEVEL);
    GPIOIntTypeSet(LCD_INT_REGS, LCD_INT_PIN, GPIO_INT_TYPE_FALL_EDGE);

    /* Registering the Interrupt Service Routine(ISR). */
    IntRegister(SYS_INT_GPIOINT3A, GPIO_3A_ISR);

    /* Setting the priority for the system interrupt in AINTC. */
    IntPrioritySet(SYS_INT_GPIOINT3A, 6, AINTC_HOSTINT_ROUTE_IRQ);

    /* Enabling the system interrupt in AINTC. */
    IntSystemEnable(SYS_INT_GPIOINT3A);
}

/******************************************************************************/
/* LCD_InteruptEnable
 *
 * Enables an interrupt.
 *                                                                            */
/******************************************************************************/
void LCD_InteruptEnable(ENUM_LCD_INTERRUPT Int)
{
	unsigned char temp;

	temp = LCD_rd8(REG_INT_MASK); // rotate screen
	temp |= Int;
	LCD_wr8(REG_INT_MASK, temp); // rotate screen
}

/******************************************************************************/
/* LCD_InteruptDisable
 *
 * Disables an interrupt.
 *                                                                            */
/******************************************************************************/
void LCD_InteruptDisable(ENUM_LCD_INTERRUPT Int)
{
	unsigned char temp;

	temp = LCD_rd8(REG_INT_MASK); // rotate screen
	temp &= ~Int;
	LCD_wr8(REG_INT_MASK, temp); // rotate screen
}

/******************************************************************************/
/* LCD_Reset
 *
 * Controls the hardware reset line of the FT81x.
 *                                                                            */
/******************************************************************************/
void LCD_Reset(unsigned char state)
{
	if(state)
	{
		GPIOPinWrite(LCD_PD_REGS, LCD_PD_PIN, GPIO_PIN_LOW);
	}
	else
	{
		GPIOPinWrite(LCD_PD_REGS, LCD_PD_PIN, GPIO_PIN_HIGH);
	}
}

/******************************************************************************/
/* LCD_Interrupt
 *
 * Controls the GPIO interrupt associated with the LCD INT line.
 *                                                                            */
/******************************************************************************/
void LCD_Interrupt(unsigned char state)
{
	if(state)
	{
	    GPIOPinIntEnable(LCD_INT_REGS, GPIO_INT_LINE_1, LCD_INT_PIN);
	}
	else
	{
	    GPIOPinIntDisable(LCD_INT_REGS, GPIO_INT_LINE_1, LCD_INT_PIN);
	}
}

/******************************************************************************/
/* LCD_ft810memRead8
 *
 * Reads 8 bits of data at an address.
 *                                                                            */
/******************************************************************************/
unsigned char LCD_ft810memRead8(unsigned long ftAddress)
{
	unsigned char receive[2];
	LCD_SendRAMAddress(ftAddress, MEM_READ);

	receive[0] = SPI_ReadWriteByte1(0x00, LCD_CS);
	receive[1] = SPI_ReadWriteByte1(0x00, LCD_CS);
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);

	return receive[1];
}

/******************************************************************************/
/* LCD_ft810memRead16
 *
 * Reads 16 bits of data at an address.
 *                                                                            */
/******************************************************************************/
unsigned int LCD_ft810memRead16(unsigned long ftAddress)
{
	unsigned char receive[3];
	LCD_SendRAMAddress(ftAddress, MEM_READ);
	receive[0] = SPI_ReadWriteByte1(0x00, LCD_CS);// dummy
	receive[1] = SPI_ReadWriteByte1(0x00, LCD_CS);
	receive[2] = SPI_ReadWriteByte1(0x00, LCD_CS);
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);

	return ((receive[2] << 8) + (receive[1] & 0x00FF));
}

/******************************************************************************/
/* LCD_ft810memRead32
 *
 * Reads 32 bits of data at an address.
 *                                                                            */
/******************************************************************************/
unsigned long LCD_ft810memRead32(unsigned long ftAddress)
{
	unsigned char receive[5];
	LCD_SendRAMAddress(ftAddress, MEM_READ);

	receive[0] = SPI_ReadWriteByte1(0x00, LCD_CS); // dummy
	receive[1] = SPI_ReadWriteByte1(0x00, LCD_CS);
	receive[2] = SPI_ReadWriteByte1(0x00, LCD_CS);
	receive[3] = SPI_ReadWriteByte1(0x00, LCD_CS);
	receive[4] = SPI_ReadWriteByte1(0x00, LCD_CS);
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);

	return ((receive[4] << 24) + (receive[3] << 16) + (receive[2] << 8) + receive[1]);
}

/******************************************************************************/
/* LCD_ft810memWrite8
 *
 * Writes 8 bits of data at an address.
 *                                                                            */
/******************************************************************************/
void LCD_ft810memWrite8(unsigned long ftAddress, unsigned char ftData8)
{
	LCD_SendRAMAddress(ftAddress, MEM_WRITE);
	SPI_WriteByte1(ftData8, LCD_CS);
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_ft810memWrite16
 *
 * Writes 16 bits of data at an address.
 *                                                                            */
/******************************************************************************/
void LCD_ft810memWrite16(unsigned long ftAddress, unsigned int ftData16)
{

	unsigned int data[2];

	data[1] = (ftData16 >> 8);
	data[0] = ftData16 & 0x00FF;

	LCD_SendRAMAddress(ftAddress, MEM_WRITE);
	SPI_WriteByte1((unsigned char)data[0], LCD_CS);
	SPI_WriteByte1((unsigned char)data[1], LCD_CS);
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_ft810memWrite32
 *
 * Writes 32 bits of data at an address.
 *                                                                            */
/******************************************************************************/
void LCD_ft810memWrite32(unsigned long ftAddress, unsigned long ftData32)
{
	unsigned long data[4];

	data[3] = (ftData32 >> 24);
	data[2] = ((ftData32 & 0x00FF0000) >> 16);
	data[1] = ((ftData32 & 0x0000FF00) >> 8);
	data[0] = (ftData32 & 0x0000FF);

	LCD_SendRAMAddress(ftAddress, MEM_WRITE);
	SPI_WriteByte1((unsigned char)data[0], LCD_CS);
	SPI_WriteByte1((unsigned char)data[1], LCD_CS);
	SPI_WriteByte1((unsigned char)data[2], LCD_CS);
	SPI_WriteByte1((unsigned char)data[3], LCD_CS);
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_ft810memWriteBuffer
 *
 * Writes a buffer at an address.
 *                                                                            */
/******************************************************************************/
void LCD_ft810memWriteBuffer(unsigned long ftAddress, unsigned char* ftData, unsigned long bytes)
{
	unsigned long i;									// low byte

	HWREG(LCD_FRAM_SPI_REGS + MCSPI_CHCONF(LCD_CS)) |= (MCSPI_TX_ONLY_MODE & MCSPI_CH1CONF_TRM);
	LCD_SendRAMAddress(ftAddress, MEM_WRITE);
	for(i=0;i<bytes;i++)
	{
		SPI_WriteByteNoRx1(*ftData, LCD_CS);
		ftData++;
	}
	for(i=0;i<1000;i++)
	{
		i++;
		i--;
	}
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
    McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
    HWREG(LCD_FRAM_SPI_REGS + MCSPI_CHCONF(LCD_CS)) &= ~(MCSPI_TX_ONLY_MODE & MCSPI_CH1CONF_TRM);
}

/*************** Programs guide ***********************/

/******************************************************************************/
/* LCD_wr8
 *
 * This function writes 8 bits to intended address location
 *                                                                            */
/******************************************************************************/
void LCD_wr8(unsigned long ftAddress, unsigned char ftData8)
{
	LCD_ft810memWrite8(ftAddress, ftData8);
}

/******************************************************************************/
/* LCD_wr16
 *
 * This function writes 16 bits to intended address location
 *                                                                            */
/******************************************************************************/
void LCD_wr16(unsigned long ftAddress, unsigned int ftData16)
{
	LCD_ft810memWrite16(ftAddress, ftData16);
}

/******************************************************************************/
/* LCD_wr32
 *
 * This function writes 32 bits to intended address location
 *                                                                            */
/******************************************************************************/
void LCD_wr32(unsigned long ftAddress, unsigned int ftData32)
{
	LCD_ft810memWrite32(ftAddress, ftData32);
}

/******************************************************************************/
/* LCD_wr8s
 *
 * This function writes 8 bit string to intended address location
 *                                                                            */
/******************************************************************************/
void LCD_wr8s(unsigned long ftAddress, unsigned char* ftData8)
{
	LCD_SendRAMAddress(ftAddress, MEM_WRITE);
	while(*ftData8)
	{
		SPI_WriteByte1(*ftData8, LCD_CS);
		ftData8++;
	}
	SPI_WriteByte1(*ftData8, LCD_CS);
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_rd8
 *
 * This function read 8 bits from intended address location
 *                                                                            */
/******************************************************************************/
unsigned char LCD_rd8(unsigned long ftAddress)
{
	return LCD_ft810memRead8(ftAddress);
}

/******************************************************************************/
/* LCD_rd16
 *
 * This function read 16 bits from intended address location
 *                                                                            */
/******************************************************************************/
unsigned short LCD_rd16(unsigned long ftAddress)
{
	return LCD_ft810memRead16(ftAddress);
}

/******************************************************************************/
/* LCD_rd32
 *
 * This function read 32 bits from intended address location
 *                                                                            */
/******************************************************************************/
unsigned long LCD_rd32(unsigned long ftAddress)
{
	return LCD_ft810memRead32(ftAddress);
}

/******************************************************************************/
/* LCD_wr_buffer
 *
 * This function writes a buffer to the intended address location
 *                                                                            */
/******************************************************************************/
void LCD_wr_buffer(unsigned long ftAddress, unsigned char* ftData, unsigned long bytes)
{
	unsigned char cTempAddr[3];
	unsigned char temp = ((ftAddress >> 16) & 0x003FU);
	unsigned long i;

	cTempAddr[2] = temp | 0x80;	// Compose the command and address to send
	cTempAddr[1] = (unsigned char) ((ftAddress >> 8) & 0x0000FFU);								// middle byte
	cTempAddr[0] = (unsigned char) (ftAddress & 0x000000FFU);										// low byte

	HWREG(LCD_FRAM_SPI_REGS + MCSPI_CHCONF(LCD_CS)) |= (MCSPI_TX_ONLY_MODE & MCSPI_CH1CONF_TRM);
	McSPIChannelEnable(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPICSAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	SPI_WriteByteNoRx1(cTempAddr[2], LCD_CS);
	SPI_WriteByteNoRx1(cTempAddr[1], LCD_CS);
	SPI_WriteByteNoRx1(cTempAddr[0], LCD_CS);
	for(i=0;i<bytes;i++)
	{
		SPI_WriteByteNoRx1(*ftData, LCD_CS);
		ftData++;
	}
	for(i=0;i<1000;i++)
	{
		i++;
		i--;
	}
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
    McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
    HWREG(LCD_FRAM_SPI_REGS + MCSPI_CHCONF(LCD_CS)) &= ~(MCSPI_TX_ONLY_MODE & MCSPI_CH1CONF_TRM);
}

/******************************************************************************/
/* LCD_rd_buffer
 *
 * This function reads a buffer from the intended address location
 *                                                                            */
/******************************************************************************/
void LCD_rd_buffer(unsigned long ftAddress, unsigned char* ftData, unsigned long bytes)
{
	unsigned char cTempAddr[3];
	unsigned char temp = ((ftAddress >> 16) & 0x003FU);
	unsigned long i;

	cTempAddr[2] = temp;	// Compose the command and address to send
	cTempAddr[1] = (unsigned char) ((ftAddress >> 8) & 0x0000FFU);								// middle byte
	cTempAddr[0] = (unsigned char) (ftAddress & 0x000000FFU);										// low byte

	McSPIChannelEnable(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPICSAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	SPI_ReadWriteByte1(cTempAddr[2], LCD_CS);
	SPI_ReadWriteByte1(cTempAddr[1], LCD_CS);
	SPI_ReadWriteByte1(cTempAddr[0], LCD_CS);
	SPI_ReadWriteByte1(0xFF, LCD_CS);
	for(i=0;i<bytes;i++)
	{
		*ftData = SPI_ReadWriteByte1(0xFF, LCD_CS);
		ftData++;
	}
	for(i=0;i<1000;i++)
	{
		i++;
		i--;
	}
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
    McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd
 *
 * This function writes 32 bits command to co-processor engine FIFO RAM_CMD
 *                                                                            */
/******************************************************************************/
void LCD_cmd(unsigned long command)
{
	LCD_wr32(RAM_CMD + RAM_CMD_Offset, command);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);
}

/******************************************************************************/
/* LCD_host_command
 *
 * This function sends host command in host command protocol.
 *                                                                            */
/******************************************************************************/
void LCD_host_command(unsigned char MSB, unsigned char Middle, unsigned char LSB)
{
	McSPIChannelEnable(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPICSAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	SPI_WriteByte1(MSB, LCD_CS);
	SPI_WriteByte1(Middle, LCD_CS);
	SPI_WriteByte1(LSB, LCD_CS);
	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_IncrementWriteOffset
 *
 * This function incrementes the write offset.
 *                                                                            */
/******************************************************************************/
void LCD_IncrementWriteOffset(unsigned short* offset)
{
	*offset += 4;
	if(*offset > 4095)
	{
		*offset = 0;
	}
}

/******************************************************************************/
/* LCD_WriteCommandParameters
 *
 * This function writes command parameters.
 *                                                                            */
/******************************************************************************/
void LCD_WriteCommandParameters(unsigned long parms)
{
	SPI_WriteByte1((unsigned char)parms, LCD_CS);
	SPI_WriteByte1((unsigned char)(parms >> 8), LCD_CS);
	SPI_WriteByte1((unsigned char)(parms >> 16), LCD_CS);
	SPI_WriteByte1((unsigned char)(parms >> 24), LCD_CS);
}

/******************************************************************************/
/* LCD_SendRAMAddress
 *
 * This function send sthe address in RAM to start a command write.
 *                                                                            */
/******************************************************************************/
void LCD_SendRAMAddress(unsigned long address, ENUM_FTDI_READ_WRITE read_nwrite)
{
	unsigned char cTempAddr[3];
	unsigned char temp = ((address >> 16) & 0x003FU);

	cTempAddr[2] = temp | read_nwrite;	// Compose the command and address to send
	cTempAddr[1] = (unsigned char) ((address >> 8) & 0x0000FFU);								// middle byte
	cTempAddr[0] = (unsigned char) (address & 0x000000FFU);										// low byte

	McSPIChannelEnable(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPICSAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	SPI_WriteByte1((unsigned char)cTempAddr[2], LCD_CS);
	SPI_WriteByte1((unsigned char)cTempAddr[1], LCD_CS);
	SPI_WriteByte1((unsigned char)cTempAddr[0], LCD_CS);
}

/******************************************************************************/
/* LCD_WaitCoprocessorDone
 *
 * This function waits for the coprocessor to finish.
 *                                                                            */
/******************************************************************************/
void LCD_WaitCoprocessorDone(void)
{
   while(LCD_rd16(REG_CMD_READ) != LCD_rd16(REG_CMD_WRITE));
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ commands to draw graphics objects ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/******************************************************************************/
/* LCD_cmd_text
 *
 * draw text
 *
 * LCD_cmd_text(160, 60, 31, OPT_CENTER, "Please Wait!");
 *                                                                            */
/******************************************************************************/
void LCD_cmd_text(unsigned short x, unsigned short y, unsigned short font, unsigned short options, unsigned char* string)
{
	unsigned short characters = 0;
	unsigned long i;

	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_TEXT);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)options<<16)|(font & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	/* write out string */
	while(*string)
	{
		SPI_WriteByte1(*string, LCD_CS);
		characters++;
		string++;
	}
	SPI_WriteByte1(0, LCD_CS); // send null terminator
	characters++;

	/* must write a 'string' that is a multiple of 4 bytes */
	while((characters % 4) != 0)
	{
		SPI_WriteByte1(0, LCD_CS);
		characters++;
	}

	for(i=0;i<characters;i+=4)
	{
		LCD_IncrementWriteOffset(&RAM_CMD_Offset);
	}

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_button
 *
 * draw a button
 *
 * LCD_cmd_button(10, 10, 140, 100, 31, 0, "Press!");
 *                                                                            */
/******************************************************************************/
void LCD_cmd_button(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short font, unsigned short options, unsigned char* string)
{
	unsigned short characters = 0;
	unsigned long i;

	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_BUTTON);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)h<<16)|(w & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)options<<16)|(font & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	/* write out string */
	while(*string)
	{
		SPI_WriteByte1(*string, LCD_CS);
		characters++;
		string++;
	}
	SPI_WriteByte1(0, LCD_CS); // send null terminator
	characters++;

	/* must write a 'string' that is a multiple of 4 bytes */
	while((characters % 4) != 0)
	{
		SPI_WriteByte1(0, LCD_CS);
		characters++;
	}

	for(i=0;i<characters;i+=4)
	{
		LCD_IncrementWriteOffset(&RAM_CMD_Offset);
	}

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_clock
 *
 * draw an analog clock
 *
 * LCD_cmd_clock(80, 60, 50, 0, 8, 15, 0, 0);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_clock(unsigned short x, unsigned short y, unsigned short r, unsigned short options, unsigned short h, unsigned short m, unsigned short s, unsigned short ms)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_CLOCK);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)options<<16)|(r & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)m<<16)|(h & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)ms<<16)|(s & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_bgcolor
 *
 * set the background color
 *
 * LCD_cmd_bgcolor(CREATE_COLOR_RBG(255, 25,10));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_bgcolor(unsigned long C)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_BGCOLOR);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(C);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_fgcolor
 *
 * set the foreground color
 *
 * LCD_cmd_fgcolor(CREATE_COLOR_RBG(255, 25,10));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_fgcolor(unsigned long C)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_FGCOLOR);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(C);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_gradcolor
 *
 * set up the highlight color used in 3D effects for LCD_cmd_BUTTON and LCD_cmd_KEYS
 *
 * LCD_cmd_gradcolor(CREATE_COLOR_RBG(255, 25,10));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_gradcolor(unsigned long C)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_GRADCOLOR);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(C);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_gauge
 *
 * draw a gauge
 *
 * LCD_cmd_gauge(80, 60, 50, 0, 5, 4, 30, 100);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_gauge(unsigned short x, unsigned short y, unsigned short r, unsigned short options, unsigned short major, unsigned short minor, unsigned short val, unsigned short range)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_GAUGE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)options<<16)|(r & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)minor<<16)|(major & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)range<<16)|(val & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_gradient
 *
 * draw a smooth color gradient
 *
 * LCD_cmd_gradient(20, 0, CREATE_COLOR_RBG(255, 25,10), 140, 0, CREATE_COLOR_RBG(0, 25,10));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_gradient(unsigned short x0, unsigned short y0, unsigned long rgb0, unsigned short x1, unsigned short y1, unsigned long rgb1)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_GRADIENT);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y0<<16)|(x0 & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(rgb0);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y1<<16)|(x1 & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(rgb1);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_keys
 *
 * draw a row of keys
 *
 * LCD_cmd_keys(10, 10, 140, 30, 26, 0x32, "12345");
 *                                                                            */
/******************************************************************************/
void LCD_cmd_keys(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short font, unsigned short options, unsigned char* string)
{
	unsigned short characters = 0;
	unsigned long i;

	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_KEYS);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)h<<16)|(w & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)options<<16)|(font & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	/* write out string */
	while(*string)
	{
		SPI_WriteByte1(*string, LCD_CS);
		characters++;
		string++;
	}
	SPI_WriteByte1(0, LCD_CS); // send null terminator
	characters++;

	/* must write a 'string' that is a multiple of 4 bytes */
	while((characters % 4) != 0)
	{
		SPI_WriteByte1(0, LCD_CS);
		characters++;
	}

	for(i=0;i<characters;i+=4)
	{
		LCD_IncrementWriteOffset(&RAM_CMD_Offset);
	}

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_progress
 *
 * draw a progress bar
 *
 * LCD_cmd_progress(50, 120, 400, 100, 0, progress, 100);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_progress(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short options, unsigned short val, unsigned short range)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_PROGRESS);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)h<<16)|(w & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)val<<16)|(options & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)range));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_scrollbar
 *
 * draw a scroll bar
 *
 * LCD_cmd_scrollbar(20, 50, 120, 8, 0, 10, 40, 100);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_scrollbar(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short options, unsigned short value, unsigned short size, unsigned short range)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SCROLLBAR);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)h<<16)|(w & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)value<<16)|(options & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)range<<16)|(size & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_slider
 *
 * draw a slider
 *
 * LCD_cmd_slider(76, 10, 8, 100, 0, 20000, 65535);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_slider(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short options, unsigned short value, unsigned short range)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SLIDER);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)h<<16)|(w & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)value<<16)|(options & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)range));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_dial
 *
 * draw a rotary dial control
 *
 * LCD_cmd_dial(28, 60, 24, 0, 0x0000);
 * LCD_cmd_text(28, 100, 26, OPT_CENTER, “0%”);
 * LCD_cmd_dial(80, 60, 24, 0, 0x5555);
 * LCD_cmd_text(80, 100, 26, OPT_CENTER, “33%”);
 * LCD_cmd_dial(132, 60, 24, 0, 0xaaaa);
 * LCD_cmd_text(132, 100, 26, OPT_CENTER,“66%”);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_dial(unsigned short x, unsigned short y, unsigned short r, unsigned short options, unsigned short value)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_DIAL);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)options<<16)|(r & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)value));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_toggle
 *
 * draw a toggle switch
 *
 * LCD_cmd_bgcolor(0x402000);
 * LCD_cmd_fgcolor(0x703800);
 * LCD_cmd_toggle(60, 20, 33, 27, 0, 0, “no” “\xff” “yes”);
 * LCD_cmd_toggle(60, 60, 33, 27, 0, 65535, “no” “\xff” “yes”);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_toggle(unsigned short x, unsigned short y, unsigned short w, unsigned short font, unsigned short options, unsigned short state, unsigned char* string)
{
	unsigned short characters = 0;
	unsigned long i;

	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_TOGGLE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)font<<16)|(w & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)state<<16)|(options & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	/* write out string */
	while(*string)
	{
		SPI_WriteByte1(*string, LCD_CS);
		characters++;
		string++;
	}
	SPI_WriteByte1(0, LCD_CS); // send null terminator
	characters++;

	/* must write a 'string' that is a multiple of 4 bytes */
	while((characters % 4) != 0)
	{
		SPI_WriteByte1(0, LCD_CS);
		characters++;
	}

	for(i=0;i<characters;i+=4)
	{
		LCD_IncrementWriteOffset(&RAM_CMD_Offset);
	}

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_setbase
 *
 * Set the base for number output
 *
 * LCD_cmd_number(80, 30, 28, OPT_CENTER, 123456);
 * LCD_cmd_setbase(16);
 * LCD_cmd_number(80, 60, 28, OPT_CENTER, 123456);
 * LCD_cmd_setbase(2);
 * LCD_cmd_number(80, 90, 26, OPT_CENTER, 123456);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_setbase(unsigned long b)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SETBASE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(b);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_number
 *
 * draw a decimal number
 *
 * LCD_cmd_number(20, 60, 31, 0, 42);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_number(unsigned short x, unsigned short y, unsigned short font, unsigned short options, unsigned long n)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_NUMBER);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)options<<16)|(font & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(n);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ commands to operate on memory ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/******************************************************************************/
/* LCD_cmd_memcrc
 *
 * compute a CRC-32 for memory
 *
 * LCD_cmd_crc(0, 1024, 0);
 * ...
 * printf("%08x\n", LCD_rd32(RAM_CMD + x + 12));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_memcrc(unsigned long ptr, unsigned long num, unsigned long result)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_MEMCRC);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(num);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(result);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_memzero
 *
 * write zero to a block of memory
 *
 * LCD_cmd_memzero(0, 1024);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_memzero(unsigned long ptr, unsigned long num)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_MEMZERO);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(num);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_memset
 *
 * fill RAM_G with a byte value
 *
 * LCD_cmd_memset(0, 0xff, 1024);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_memset(unsigned long ptr, unsigned long value, unsigned long num)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_MEMSET);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(value);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(num);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_memwrite
 *
 * write bytes into RAM_G
 *
 * LCD_cmd_memwrite(REG_PWM_DUTY, 4, &brightness); // write to the PWM_DUTY register
 *                                                                            */
/******************************************************************************/
void LCD_cmd_memwrite(unsigned long ptr, unsigned long num, unsigned long* buffer)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_MEMWRITE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(num);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	while(num)
	{
		LCD_WriteCommandParameters(*buffer);
		LCD_IncrementWriteOffset(&RAM_CMD_Offset);
		num--;
		buffer++;
	}

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_memcpy
 *
 * copy a block of RAM_G
 *
 * LCD_cmd_memcpy(0x8000, 0, 1024);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_memcpy(unsigned long dest, unsigned long src, unsigned long num)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_MEMCPY);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(dest);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(src);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(num);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_append
 *
 * append more commands to display list
 *
 * LCD_cmd_append(0, 40); // copy 10 commands from main memory address 0
 *                                                                            */
/******************************************************************************/
void LCD_cmd_append(unsigned long ptr, unsigned long num)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_APPEND);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(num);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Commands for loading image data into FT81X RAM_G ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/******************************************************************************/
/* LCD_cmd_inflate
 *
 * decompress data into memory
 *
 * LCD_cmd_inflate(0x8000, 1000, buffer); // zlib-compressed data follows
 *                                                                            */
/******************************************************************************/
void LCD_cmd_inflate(unsigned long ptr, unsigned long num, unsigned long* buffer)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_INFLATE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	while(num)
	{
		LCD_WriteCommandParameters(*buffer);
		LCD_IncrementWriteOffset(&RAM_CMD_Offset);
		num--;
		buffer++;
	}

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_loadimage
 *
 * load a JPEG/PNG image
 *
 * LCD_cmd_loadimage(0, 0);
 * ... // JPEG file data follows
 * LCD_cmd(BEGIN(BITMAPS))
 * LCD_cmd(VERTEX2II(10, 20, 0, 0)); // draw bitmap at (10,20)
 * LCD_cmd(VERTEX2II(100, 20, 0, 0)); // draw bitmap at (100,20)
 *                                                                            */
/******************************************************************************/
void LCD_cmd_loadimage(unsigned long ptr, unsigned long options)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_LOADIMAGE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(options);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Commands for setting the bitmap transform matrix ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/******************************************************************************/
/* LCD_cmd_loadidentity
 *
 * set the current matrix to identity
 *
 * LCD_cmd_loadidentity();
 *                                                                            */
/******************************************************************************/
void LCD_cmd_loadidentity(void)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_LOADIDENTITY);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_translate
 *
 * apply a translation to the current matrix
 *
 * LCD_cmd(BEGIN(BITMAPS));
 * LCD_cmd_loadidentity();
 * LCD_cmd_translate(20 * 65536, 0);
 * LCD_cmd_setmatrix();
 * LCD_cmd(VERTEX2II(68, 28, 0, 0));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_translate(unsigned long tx, unsigned long ty)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_TRANSLATE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(tx);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ty);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_scale
 *
 * apply a scale to the current matrix
 *
 * LCD_cmd(BEGIN(BITMAPS));
 * LCD_cmd_loadidentity();
 * LCD_cmd_scale(2 * 65536, 2 * 65536);
 * LCD_cmd_setmatrix();
 * LCD_cmd(VERTEX2II(68, 28, 0, 0));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_scale(unsigned long sx, unsigned long sy)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SCALE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(sx);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(sy);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_rotate
 *
 * apply a rotation to the current matrix
 *
 * LCD_cmd(BEGIN(BITMAPS));
 * LCD_cmd_loadidentity();
 * LCD_cmd_rotate(10 * 65536 / 360);
 * LCD_cmd_setmatrix();
 * LCD_cmd(VERTEX2II(68, 28, 0, 0));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_rotate(unsigned long a)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_ROTATE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(a);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_setmatrix
 *
 * write the current matrix as a bitmap transform
 *
 * LCD_cmd_setmatrix();
 *                                                                            */
/******************************************************************************/
void LCD_cmd_setmatrix(void)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SETMATRIX);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_getmatrix
 *
 * retrieves the current matrix coefficients
 *                                                                            */
/******************************************************************************/
void LCD_cmd_getmatrix(unsigned long a, unsigned long b, unsigned long c, unsigned long d, unsigned long e, unsigned long f)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_GETMATRIX);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(a);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(b);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(c);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(d);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(e);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(f);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_getptr
 *
 * get the end memory address of data inflated by LCD_cmd_INFLATE
 *
 * LCD_cmd_inflate(1000); //Decompress the data into RAM_G + 1000
 * ...... //Following the zlib compressed data
 * While(LCD_rd16(REG_LCD_cmd_WRITE) != LCD_rd16(REG_LCD_cmd_READ)); //Wait till the compression was done
 * uint16_t x = LCD_rd16(REG_LCD_cmd_WRITE);
 * uint32_t ending_address = 0;
 * LCD_cmd_getptr(0);
 * ending_address = LCD_rd32(RAM_CMD + x + 4);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_getptr(unsigned long result)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_GETPTR);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(result);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_getprops
 *
 * get the image properties decompressed by LCD_cmd_LOADIMAGE
 *                                                                            */
/******************************************************************************/
void LCD_cmd_getprops(unsigned long ptr, unsigned long width, unsigned long height)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_GETPROPS);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(width);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(height);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ media/video commands ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/******************************************************************************/
/* LCD_cmd_mediafifo
 *
 * set up a streaming media FIFO in RAM_G
 *
 * LCD_cmd_mediafifo(0x100000 - 65536, 65536); //0x100000 is the top of RAM_G
 *                                                                            */
/******************************************************************************/
void LCD_cmd_mediafifo(unsigned long ptr, unsigned long size)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_MEDIAFIFO);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(size);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_playvideo
 *
 * Plays back MJPEG-encoded AVI video
 *
 * LCD_cmd_playvideo(OPT_FULLSCREEN | OPT_NOTEAR);
 * //... append AVI data ...
 *                                                                            */
/******************************************************************************/
void LCD_cmd_playvideo(unsigned long opts)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_PLAYVIDEO);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(opts);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	/* send video data */

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_videostart
 *
 * Initializes the AVI video decoder.
 *
 * videostart();
 * videoframe(4, 0);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_videostart(void)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_VIDEOSTART);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_videoframe
 *
 * load the next frame of video
 *
 * videostart();
 * videoframe(4, 0);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_videoframe(unsigned long dst, unsigned long ptr)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_VIDEOFRAME);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(dst);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Other commands ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/******************************************************************************/
/* LCD_cmd_track
 *
 * track touches for a graphics object
 *
 * dl( CLEAR_COLOR_RGB(5, 45, 110) );
 * dl( COLOR_RGB(255, 168, 64) );
 * dl( CLEAR(1 ,1 ,1) );
 * dl( BEGIN(RECTS) );
 * dl( VERTEX2F(70 * 16,40 * 16) );
 * dl( VERTEX2F(82 * 16,80 * 16) );
 * dl( COLOR_RGB(255, 0, 0) );
 * dl( VERTEX2F(70 * 16,40 * 16) );
 * dl( VERTEX2F(82 * 16,60 * 16) );
 * dl( COLOR_MASK(0 ,0 ,0 ,0) );
 * dl( TAG(1) );
 * dl( VERTEX2F(70 * 16,40 * 16) );
 * dl( VERTEX2F(82 * 16,80 * 16) );
 * LCD_cmd_track(70 * 16, 40 * 16, 12, 40, 1);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_track(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned short tag)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_TRACK);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)h<<16)|(w & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters((unsigned long) tag);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_coldstart
 *
 * set co-processor engine state to default values
 *
 * LCD_cmd_coldstart();
 *                                                                            */
/******************************************************************************/
void LCD_cmd_coldstart(void)
{
	LCD_cmd(CMD_COLDSTART);
}

/******************************************************************************/
/* LCD_cmd_interrupt
 *
 * trigger interrupt INT_LCD_cmdFLAG
 *
 * LCD_cmd_interrupt(500);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_interrupt(unsigned long ms)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_INTERRUPT);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ms);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_regread
 *
 * read a register value
 *
 * //To capture the exact time when a command completes:
 * uint16_t x = LCD_rd16(REG_LCD_cmd_WRITE);
 * LCD_cmd_regread(REG_CLOCK, 0); //...
 * printf("%08x\n", LCD_rd32(RAM_CMD + x + 8));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_regread(unsigned long ptr, unsigned long result)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_REGREAD);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(result);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_calibrate
 *
 * execute the touch screen calibration routine
 *
 * LCD_cmd_dlstart();
 * LCD_cmd(CLEAR(1,1,1));
 * LCD_cmd_text(80, 30, 27, OPT_CENTER, "Please tap on the dot");
 * LCD_cmd_calibrate();
 *                                                                            */
/******************************************************************************/
void LCD_cmd_calibrate(unsigned long result)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_CALIBRATE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(result);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_romfont
 *
 * load a ROM font into bitmap handle
 *
 * LCD_cmd_romfont(1, 34);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_romfont(unsigned long font, unsigned long romslot)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_ROMFONT);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(font);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(romslot);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_setfont
 *
 * set up a custom font
 *
 * LCD_cmd_setfont(7, 1000);
 * LCD_cmd_button(20, 20, 	// x,y
 * 			  120, 40,	// width,height in pixels
 * 			  7, 		// font 7, just loaded
 * 			  0, 		// default options,3D style
 * 			  "custom font!");
 *                                                                            */
/******************************************************************************/
void LCD_cmd_setfont(unsigned long font, unsigned long ptr)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SETFONT);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(font);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_setfont2
 *
 * set up a custom font
 *
 * LCD_cmd_setfont2(20, 100000, 32);
 * LCD_cmd_button(15, 30, 130, 20, 18, 0, "This is font 18");
 * LCD_cmd_button(15, 60, 130, 20, 20, 0, "This is font 20");
 *                                                                            */
/******************************************************************************/
void LCD_cmd_setfont2(unsigned long font, unsigned long ptr, unsigned long firstchar)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SETFONT2);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(font);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(firstchar);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_setscratch
 *
 * set the scratch bitmap for widget use
 *
 * LCD_cmd_setscratch(31);
 * LCD_cmd_setfont2(15, 100000, 32);
 * LCD_cmd_button(15, 30, 130, 20, 15, 0, "This is font 15");
 * //Restore bitmap handle 31 to ROM Font number 31.
 * LCD_cmd_romfont(31, 31);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_setscratch(unsigned long handle)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SETSCRATCH);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(handle);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_setrotate
 *
 * Rotate the screen and set up transform matrix accordingly
 *
 * LCD_cmd_setrotate(2); //Put the display in portrait mode
 *                                                                            */
/******************************************************************************/
void LCD_cmd_setrotate(unsigned long r)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SETROTATE);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(r);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_setbitmap
 *
 * Set up display list commands for specified bitmap
 *                                                                            */
/******************************************************************************/
void LCD_cmd_setbitmap(unsigned long addr, unsigned short fmt, unsigned short width, unsigned short height)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SETBITMAP);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(addr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)width<<16)|(fmt & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters((unsigned long)height);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_spinner
 *
 * start an animated spinner
 *
 * LCD_cmd_dlstart();
 * LCD_cmd(CLEAR(1,1,1));
 * LCD_cmd_text(80, 30, 27, OPT_CENTER, "Please wait...");
 * LCD_cmd_spinner(80, 60, 0, 0);
 *                                                                            */
/******************************************************************************/
void LCD_cmd_spinner(unsigned short x, unsigned short y, unsigned short style, unsigned short scale)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SPINNER);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)scale<<16)|(style & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_stop
 *
 * stop any spinner, screensaver or sketch
 *
 * LCD_cmd_stop();
 *                                                                            */
/******************************************************************************/
void LCD_cmd_stop(void)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_STOP);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_screensaver
 *
 * start an animated screensaver
 *
 * LCD_cmd_screensaver();
 * LCD_cmd(BITMAP_SOURCE(0));
 * LCD_cmd(BITMAP_LAYOUT(RGB565, 128, 64));
 * LCD_cmd(BITMAP_SIZE(NEAREST,BORDER,BORDER, 40, 30));
 * LCD_cmd(BEGIN(BITMAPS));
 * LCD_cmd(MACRO(0));
 * LCD_cmd(DISPLAY());
 *                                                                            */
/******************************************************************************/
void LCD_cmd_screensaver(void)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SCREENSAVER);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_sketch
 *
 * start a continuous sketch update
 *
 * LCD_cmd_memzero(0, 480 * 272 / 8);
 * LCD_cmd_sketch(0, 0, 480, 272, 0, L1);
 * //Then to display the bitmap
 * LCD_cmd(BITMAP_SOURCE(0));
 * LCD_cmd(BITMAP_LAYOUT(L1, 60, 272));
 * LCD_cmd(BITMAP_SIZE(NEAREST, BORDER, BORDER, 480, 272));
 * LCD_cmd(BEGIN(BITMAPS));
 * LCD_cmd(VERTEX2II(0, 0, 0, 0));
 *
 * //Finally, to stop sketch updates
 * LCD_cmd_stop();
 *                                                                            */
/******************************************************************************/
void LCD_cmd_sketch(unsigned short x, unsigned short y, unsigned short w, unsigned short h, unsigned long ptr, unsigned short format)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SKETCH);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)h<<16)|(w & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters((unsigned long)format);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_snapshot
 *
 * take a snapshot of the current screen
 *
 * wr(REG_PCLK,0);//Turn off the PCLK
 * LCD_wr16(REG_HSIZE,120);
 * LCD_wr16(REG_WSIZE,160);
 *
 * LCD_cmd_snapshot(0);//Taking snapshot.
 *
 * wr(REG_PCLK,5);//Turn on the PCLK
 * LCD_wr16(REG_HSIZE,272);
 * LCD_wr16(REG_WSIZE,480);
 *
 * LCD_cmd_dlstart();
 * LCD_cmd(CLEAR(1,1,1));
 * LCD_cmd(BITMAP_SOURCE(0));
 * LCD_cmd(BITMAP_LAYOUT(ARGB4, 2 * 160, 120));
 * LCD_cmd(BITMAP_SIZE(NEAREST, BORDER, BORDER, 160, 120));
 * LCD_cmd(BEGIN(BITMAPS));
 * LCD_cmd(VERTEX2II(10, 10, 0, 0));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_snapshot(unsigned long ptr)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SNAPSHOT);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_snapshot2
 *
 * take a snapshot of part of the current screen with more format option
 *
 * LCD_cmd_snapshot2(RGB565, 0, 0, 0, 32, 32);
 * LCD_cmd_dlstart(); LCD_cmd_setbitmap(0, RGB565, 32, 32);
 * LCD_cmd(CLEAR(1,1,1));
 * LCD_cmd(BEGIN(BITMAPS));
 * LCD_cmd(VERTEX2II(10, 10, 0, 0));
 *                                                                            */
/******************************************************************************/
void LCD_cmd_snapshot2(unsigned long fmt, unsigned long ptr, unsigned short x, unsigned short y, unsigned short w, unsigned short h)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_SNAPSHOT2);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(fmt);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(ptr);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)y<<16)|(x & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	LCD_WriteCommandParameters(((unsigned long)h<<16)|(w & 0xffff));
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_cmd_logo
 *
 * play device logo animation
 *
 * LCD_cmd_logo();
 * delay(3000); // Optional to wait
 * While( (0 != LCD_rd16(REG_LCD_cmd_WRITE)) && (LCD_rd16(REG_LCD_cmd_WRITE) != LCD_rd16(REG_LCD_cmd_READ) ));//Wait till both read & write pointer register are equal to zero
 *                                                                            */
/******************************************************************************/
void LCD_cmd_logo(void)
{
	LCD_SendRAMAddress(RAM_CMD + RAM_CMD_Offset, MEM_WRITE);

	LCD_WriteCommandParameters(CMD_LOGO);
	LCD_IncrementWriteOffset(&RAM_CMD_Offset);

	McSPICSDeAssert(LCD_FRAM_SPI_REGS, LCD_CS);
	McSPIChannelDisable(LCD_FRAM_SPI_REGS, LCD_CS);
}

/******************************************************************************/
/* LCD_SetInterruptFlag
 *
 * Sets the flag interrupt activity flag status.
 *                                                                            */
/******************************************************************************/
void LCD_SetInterruptFlag(void)
{
	LCD_Interruptflag = TRUE;
}

/******************************************************************************/
/* LCD_ClearInterruptFlag
 *
 * Clears the flag interrupt activity flag status.
 *                                                                            */
/******************************************************************************/
void LCD_ClearInterruptFlag(void)
{
	LCD_Interruptflag = FALSE;
}

/******************************************************************************/
/* LCD_GetInterruptFlag
 *
 * Gets the flag interrupt activity flag status.
 *                                                                            */
/******************************************************************************/
unsigned char LCD_GetInterruptFlag(void)
{
	return LCD_Interruptflag;
}

/******************************* End of file *********************************/
