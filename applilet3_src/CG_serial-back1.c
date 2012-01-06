/*
*******************************************************************************
* Copyright(C) 2011 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation. 
*
* This device driver was created by Applilet3 for RL78/G13
* 16-Bit Single-Chip Microcontrollers
* Filename:	CG_serial.c
* Abstract:	This file implements device driver for Serial module.
* APIlib:	Applilet3 for RL78/G13 E1.00b [14 Jan 2011]
* Device:	R5F100LE
* Compiler:	IAR Systems iccrl78
* Creation date:	8/26/2011
*******************************************************************************
*/

/*
*******************************************************************************
** Include files
*******************************************************************************
*/
#include "CG_macrodriver.h"
#include "CG_serial.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "CG_userdefine.h"

/*
*******************************************************************************
**  Global define
*******************************************************************************
*/
volatile UCHAR  *gpUart0TxAddress;	/* uart0 transmit buffer address */
volatile USHORT gUart0TxCnt;		/* uart0 transmit data number */
volatile UCHAR  *gpUart0RxAddress;	/* uart0 receive buffer address */
volatile USHORT gUart0RxCnt;		/* uart0 receive data number */
volatile USHORT gUart0RxLen;		/* uart0 receive data length */
volatile UCHAR  *gpCsi10RxAddress;	/* csi10 receive buffer address */
volatile USHORT gCsi10RxLen;		/* csi10 receive data length */
volatile USHORT gCsi10RxCnt;		/* csi10 receive data count */
volatile UCHAR  *gpCsi10TxAddress;	/* csi10 send buffer address */
volatile USHORT gCsi10TxLen;		/* csi10 send data length */
volatile USHORT gCsi10TxCnt;		/* csi10 send data count */
volatile UCHAR  *gpUart2TxAddress;	/* uart2 transmit buffer address */
volatile USHORT gUart2TxCnt;		/* uart2 transmit data number */
volatile UCHAR  *gpUart2RxAddress;	/* uart2 receive buffer address */
volatile USHORT gUart2RxCnt;		/* uart2 receive data number */
volatile USHORT gUart2RxLen;		/* uart2 receive data length */
volatile UCHAR	gIica0MasterStatusFlag;	/* iica0 master flag */
volatile UCHAR	gIica0SlaveStatusFlag;	/* iica0 slave flag */
volatile UCHAR*	gpIica0RxAddress;	/* iica0 receive buffer address */
volatile USHORT	gIica0RxLen;		/* iica0 receive data length */
volatile USHORT	gIica0RxCnt;		/* iica0 receive data count */
volatile UCHAR*	gpIica0TxAddress;	/* iica0 send buffer address */
volatile USHORT	gIica0TxCnt;		/* iica0 send data count */
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes the SAU0 module.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void SAU0_Init(void)
{
	SAU0EN = 1U;	/* supply SAU0 clock */
	NOP();
	NOP();
	NOP();
	NOP();
	SPS0 = _0002_SAU_CK00_FCLK_2 | _0000_SAU_CK01_FCLK_0;
	UART0_Init();
	CSI10_Init();
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes the UART0 module.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UART0_Init(void)
{
	ST0 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;	/* disable UART0 receive and transmit */
	STMK0 = 1U;	/* disable INTST0 interrupt */
	STIF0 = 0U;	/* clear INTST0 interrupt flag */
	SRMK0 = 1U;	/* disable INTSR0 interrupt */
	SRIF0 = 0U;	/* clear INTSR0 interrupt flag */
	SREMK0 = 1U;	/* disable INTSRE0 interrupt */
	SREIF0 = 0U;	/* clear INTSRE0 interrupt flag */
	/* Set INTST0 low priority */
	STPR10 = 1U;
	STPR00 = 1U;
	/* Set INTSR0 low priority */
	SRPR10 = 1U;
	SRPR00 = 1U;
	/* Set INTSRE0 low priority */
	SREPR10 = 1U;
	SREPR00 = 1U;
	SMR00 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR00 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SDR00 = _CE00_UART0_TRANSMIT_DIVISOR;
	NFEN0 |= _01_SAU_RXD0_FILTER_ON;
	SIR01 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;	/* clear error flag */
	SMR01 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR01 = _4000_SAU_RECEPTION | _0400_SAU_INTSRE_ENABLE | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SDR01 = _CE00_UART0_RECEIVE_DIVISOR;
	SO0 |= _0001_SAU_CH0_DATA_OUTPUT_1;
	SOL0 |= _0000_SAU_CHANNEL0_NORMAL;	/* output level normal */
	SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;	/* enable UART0 output */
	/* Set RxD0 pin */
	PM1 |= 0x02U;
	/* Set TxD0 pin */
	P1 |= 0x04U;
	PM1 &= 0xFBU;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function starts the UART0 module operation.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UART0_Start(void)
{
	STIF0 = 0U;	/* clear INTST0 interrupt flag */
	STMK0 = 0U;	/* enable INTST0 interrupt */
	SRIF0 = 0U;	/* clear INTSR0 interrupt flag */
	SRMK0 = 0U;	/* enable INTSR0 interrupt */
	SREIF0 = 0U;	/* clear INTSRE0 interrupt flag */
	SREMK0 = 0U;	/* enable INTSRE0 interrupt */
	SO0 |= _0001_SAU_CH0_DATA_OUTPUT_1;	/* output level normal */
	SOE0 |= _0001_SAU_CH0_OUTPUT_ENABLE;	/* enable UART0 output */
	SS0 |= _0002_SAU_CH1_START_TRG_ON | _0001_SAU_CH0_START_TRG_ON;	/* enable UART0 receive and transmit */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function stops the UART0 module operation.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UART0_Stop(void)
{
	ST0 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;	/* disable UART0 receive and transmit */
	SOE0 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;	/* disable UART0 output */
	STMK0 = 1U;	/* disable INTST0 interrupt */
	STIF0 = 0U;	/* clear INTST0 interrupt flag */
	SRMK0 = 1U;	/* disable INTSR0 interrupt */
	SRIF0 = 0U;	/* clear INTSR0 interrupt flag */
	SREMK0 = 1U;	/* disable INTSRE0 interrupt */
	SREIF0 = 0U;	/* clear INTSRE0 interrupt flag */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function receives UART0 data.
**
**  Parameters:
**	rxbuf: receive buffer pointer
**	rxnum: buffer size
**
**  Returns:
**	MD_OK
**	MD_ARGERROR
**
**-----------------------------------------------------------------------------
*/
MD_STATUS UART0_ReceiveData(UCHAR *rxbuf, USHORT rxnum)
{
	MD_STATUS status = MD_OK;

	if (rxnum < 1U)
	{
		status = MD_ARGERROR;
	}
	else
	{
		gUart0RxCnt = 0U;
		gUart0RxLen = rxnum;
		gpUart0RxAddress = rxbuf;
	}

	return (status);
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function sends UART0 data.
**
**  Parameters:
**	txbuf: transfer buffer pointer
**	txnum: buffer size
**
**  Returns:
**	MD_OK
**	MD_ARGERROR
**
**-----------------------------------------------------------------------------
*/
MD_STATUS UART0_SendData(UCHAR* txbuf, USHORT txnum)
{
	MD_STATUS status = MD_OK;

	if (txnum < 1U)
	{
		status = MD_ARGERROR;
	}
	else
	{
		gpUart0TxAddress = txbuf;
		gUart0TxCnt = txnum;
		STMK0 = 1U;	/* disable INTST0 interrupt */
		TXD0 = *gpUart0TxAddress;
		gpUart0TxAddress++;
		gUart0TxCnt--;
		STMK0 = 0U;	/* enable INTST0 interrupt */
	}

	return (status);
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes the CSI10 module.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void CSI10_Init(void)
{
	ST0 |= _0004_SAU_CH2_STOP_TRG_ON;	/* disable CSI10 */
	CSIMK10 = 1U;	/* disable INTCSI10 interrupt */
	CSIIF10 = 0U;	/* clear INTCSI10 interrupt flag */
	/* Set INTCSI10 low priority */
	CSIPR110 = 1U;
	CSIPR010 = 1U;
	SIR02 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;	/* clear error flag */
	SMR02 = _0020_SAU_SMRMN_INITIALVALUE | _8000_SAU_CLOCK_SELECT_CK01 | _0000_SAU_CLOCK_MODE_CKS | _0000_SAU_TRIGGER_SOFTWARE | _0000_SAU_MODE_CSI | _0000_SAU_TRANSFER_END;
	SCR02 = _C000_SAU_RECEPTION_TRANSMISSION | _0000_SAU_TIMING_1 | _0000_SAU_MSB | _0007_SAU_LENGTH_8;
	SDR02 = _6400_CSI10_DIVISOR;
	SO0 |= _0400_SAU_CH2_CLOCK_OUTPUT_1;	/* CSI10 clock initial level */
	SO0 &= ~_0004_SAU_CH2_DATA_OUTPUT_1;	/* CSI10 SO initial level */
	SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;	/* enable CSI10 output */
	/* Set SI10 pin */
	PMC0 &= 0xF7U;
	PM0 |= 0x08U;
	/* Set SO10 pin */
	P0 |= 0x04U;
	PMC0 &= 0xFBU;
	PM0 &= 0xFBU;
	/* Set SCK10 pin */
	P0 |= 0x10U;
	PM0 &= 0xEFU;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function starts the CSI10 module operation.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void CSI10_Start(void)
{
	CSIIF10 = 0U;	/* clear INTCSI10 interrupt flag */
	CSIMK10 = 0U;	/* enable INTCSI10 */
	SO0 |= _0400_SAU_CH2_CLOCK_OUTPUT_1;	/* CSI10 clock initial level */
	SO0 &= ~_0004_SAU_CH2_DATA_OUTPUT_1;	/* CSI10 SO initial level */
	SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;	/* enable CSI10 output */
	SS0 |= _0004_SAU_CH2_START_TRG_ON;	/* enable CSI10 */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function stops the CSI10 module operation.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void CSI10_Stop(void)
{
	ST0 |= _0004_SAU_CH2_STOP_TRG_ON;	/* disable CSI10 */
	SOE0 &= ~_0004_SAU_CH2_OUTPUT_ENABLE;	/* disable CSI10 output */
	CSIMK10 = 1U;	/* disable INTCSI10 interrupt */
	CSIIF10 = 0U;	/* clear INTCSI10 interrupt flag */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function sends and receives CSI10 data.
**
**  Parameters:
**	txbuf: transfer buffer pointer
**	txnum: buffer size
**	rxbuf: receive buffer pointer
**
**  Returns:
**	MD_OK
**	MD_ARGERROR
**
**-----------------------------------------------------------------------------
*/
MD_STATUS CSI10_SendReceiveData(UCHAR *txbuf, USHORT txnum, UCHAR *rxbuf)
{
	MD_STATUS status = MD_OK;

	if (txnum < 1U)
	{
		status = MD_ARGERROR;
	}
	else
	{
		gCsi10TxCnt = txnum;	/* send data count */
		gpCsi10TxAddress = txbuf;	/* send buffer pointer */
		gpCsi10RxAddress = rxbuf;	/* receive buffer pointer */
		CSIMK10 = 1U;	/* disable INTCSI10 interrupt */
		SIO10 = *gpCsi10TxAddress;	/* started by writing data to SDR[7:0] */
		gpCsi10TxAddress++;
		gCsi10TxCnt--;
		CSIMK10 = 0U;	/* enable INTCSI10 interrupt */
	}

	return (status);
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes the SAU1 module.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void SAU1_Init(void)
{
	SAU1EN = 1U;	/* supply SAU1 clock */
	NOP();
	NOP();
	NOP();
	NOP();
	SPS1 = _0002_SAU_CK00_FCLK_2 | _0020_SAU_CK01_FCLK_2;
	UART2_Init();
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes the UART2 module.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UART2_Init(void)
{
	ST1 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;	/* disable UART2 receive and transmit */
	STMK2 = 1U;	/* disable INTST2 interrupt */
	STIF2 = 0U;	/* clear INTST2 interrupt flag */
	SRMK2 = 1U;	/* disable INTSR2 interrupt */
	SRIF2 = 0U;	/* clear INTSR2 interrupt flag */
	SREMK2 = 1U;	/* disable INTSRE2 interrupt */
	SREIF2 = 0U;	/* clear INTSRE2 interrupt flag */
	/* Set INTST2 low priority */
	STPR12 = 1U;
	STPR02 = 1U;
	/* Set INTSR2 low priority */
	SRPR12 = 1U;
	SRPR02 = 1U;
	/* Set INTSRE2 low priority */
	SREPR12 = 1U;
	SREPR02 = 1U;
	SMR10 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_TRIGGER_SOFTWARE | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR10 = _8000_SAU_TRANSMISSION | _0000_SAU_INTSRE_MASK | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SDR10 = _CE00_UART2_TRANSMIT_DIVISOR;
	NFEN0 |= _10_SAU_RXD2_FILTER_ON;
	SIR11 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;	/* clear error flag */
	SMR11 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0100_SAU_TRIGGER_RXD | _0000_SAU_EDGE_FALL | _0002_SAU_MODE_UART | _0000_SAU_TRANSFER_END;
	SCR11 = _4000_SAU_RECEPTION | _0400_SAU_INTSRE_ENABLE | _0000_SAU_PARITY_NONE | _0080_SAU_LSB | _0010_SAU_STOP_1 | _0007_SAU_LENGTH_8;
	SDR11 = _CE00_UART2_RECEIVE_DIVISOR;
	SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;
	SOL1 |= _0000_SAU_CHANNEL0_NORMAL;	/* output level normal */
	SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;	/* enable UART2 output */
	/* Set RxD2 pin */
	PM1 |= 0x10U;
	/* Set TxD2 pin */
	P1 |= 0x08U;
	PM1 &= 0xF7U;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function starts the UART2 module operation.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UART2_Start(void)
{
	STIF2 = 0U;	/* clear INTST2 interrupt flag */
	STMK2 = 0U;	/* enable INTST2 interrupt */
	SRIF2 = 0U;	/* clear INTSR2 interrupt flag */
	SRMK2 = 0U;	/* enable INTSR2 interrupt */
	SREIF2 = 0U;	/* clear INTSRE2 interrupt flag */
	SREMK2 = 0U;	/* enable INTSRE2 interrupt */
	SO1 |= _0001_SAU_CH0_DATA_OUTPUT_1;	/* output level normal */
	SOE1 |= _0001_SAU_CH0_OUTPUT_ENABLE;	/* enable UART2 output */
	SS1 |= _0002_SAU_CH1_START_TRG_ON | _0001_SAU_CH0_START_TRG_ON;	/* enable UART2 receive and transmit */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function stops the UART2 module operation.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UART2_Stop(void)
{
	ST1 |= _0002_SAU_CH1_STOP_TRG_ON | _0001_SAU_CH0_STOP_TRG_ON;	/* disable UART2 receive and transmit */
	SOE1 &= ~_0001_SAU_CH0_OUTPUT_ENABLE;	/* disable UART2 output */
	STMK2 = 1U;	/* disable INTST2 interrupt */
	STIF2 = 0U;	/* clear INTST2 interrupt flag */
	SRMK2 = 1U;	/* disable INTSR2 interrupt */
	SRIF2 = 0U;	/* clear INTSR2 interrupt flag */
	SREMK2 = 1U;	/* disable INTSRE2 interrupt */
	SREIF2 = 0U;	/* clear INTSRE2 interrupt flag */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function receives UART2 data.
**
**  Parameters:
**	rxbuf: receive buffer pointer
**	rxnum: buffer size
**
**  Returns:
**	MD_OK
**	MD_ARGERROR
**
**-----------------------------------------------------------------------------
*/
MD_STATUS UART2_ReceiveData(UCHAR *rxbuf, USHORT rxnum)
{
	MD_STATUS status = MD_OK;

	if (rxnum < 1U)
	{
		status = MD_ARGERROR;
	}
	else
	{
		gUart2RxCnt = 0U;
		gUart2RxLen = rxnum;
		gpUart2RxAddress = rxbuf;
	}

	return (status);
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function sends UART2 data.
**
**  Parameters:
**	txbuf: transfer buffer pointer
**	txnum: buffer size
**
**  Returns:
**	MD_OK
**	MD_ARGERROR
**
**-----------------------------------------------------------------------------
*/
MD_STATUS UART2_SendData(UCHAR* txbuf, USHORT txnum)
{
	MD_STATUS status = MD_OK;

	if (txnum < 1U)
	{
		status = MD_ARGERROR;
	}
	else
	{
		gpUart2TxAddress = txbuf;
		gUart2TxCnt = txnum;
		STMK2 = 1U;	/* disable INTST2 interrupt */
		TXD2 = *gpUart2TxAddress;
		gpUart2TxAddress++;
		gUart2TxCnt--;
		STMK2 = 0U;	/* enable INTST2 interrupt */
	}

	return (status);
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes the IICA0 module.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void IICA0_Init(void)
{
	IICA0EN = 1U;	/* supply IICA0 clock */
	IICE0 = 0U;	/* disable IICA0 operation */
	IICAMK0 = 1U;	/* disable INTIICA0 interrupt */
	IICAIF0 = 0U;	/* clear INTIICA0 interrupt flag */
	/* Set INTIICA0 low priority */
	IICAPR10 = 1U;
	IICAPR00 = 1U;	
	/* Set SCLA0, SDAA0 pin */
	P6 &= 0xFCU;
	PM6 |= 0x03U;
	SMC0 = 0U;
	IICWL0 = _97_IICA0_IICWL_VALUE;
	IICWH0 = _AA_IICA0_IICWH_VALUE;
	IICCTL01 &= (UCHAR)~_01_IICA_fCLK_HALF;
	SVA0 = _10_IICA0_MASTERADDRESS;
	STCEN0 = 1U;
	IICRSV0 = 1U;
	SPIE0 = 0U;
	WTIM0 = 1U;
	ACKE0 = 1U;
	IICAMK0 = 0U;
	IICE0 = 1U;
	LREL0 = 1U;
	/* Set SCLA0, SDAA0 pin */
	PM6 &= 0xFCU;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function stops IICA0 module operation.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void IICA0_Stop(void)
{
	IICE0 = 0U;	/* disable IICA0 operation */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function set IICA0 stop condition flag.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void IICA0_StopCondition(void)
{
	SPT0 = 1U;	/* set stop condition flag */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function starts to send data as master mode.
**
**  Parameters:
**	adr: set address for select slave
**	txbuf: transfer buffer pointer
**	txnum: buffer size
**	wait: wait for start condition
**
**  Returns:
**	MD_OK
**	MD_ERROR1
**	MD_ERROR2
**
**-----------------------------------------------------------------------------
*/
MD_STATUS IICA0_MasterSendStart(UCHAR adr, UCHAR *txbuf, USHORT txnum, UCHAR wait)
{
	MD_STATUS status = MD_OK;

	IICAMK0 = 1U;	/* disable INTIICA0 interrupt */
	if (IICBSY0 == 1U)
	{
		/* Check bus busy */
		IICAMK0 = 0U;	/* enable INTIICA0 interrupt */
		status = MD_ERROR1;
	}	
	else if ((SPT0 == 1U) || (STT0 == 1U)) 
	{
		/* Check trigger */
		IICAMK0 = 0U;	/* enable INTIICA0 interrupt */	
		status = MD_ERROR2;
	}
	else
	{
		STT0 = 1U;	/* send IICA0 start condition */
		IICAMK0 = 0U;	/* enable INTIICA0 interrupt */
		/* Wait */
		while (wait--)
		{
			;
		}
		/* Set parameter */
		gIica0TxCnt = txnum;
		gpIica0TxAddress = txbuf;
		gIica0MasterStatusFlag = _00_IICA_MASTER_FLAG_CLEAR;
		adr &= (UCHAR)~0x01U;	/* set send mode */
		IICA0 = adr;	/* send address */
	}

  	return (status);
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function starts to receive IICA0 data as master mode.
**
**  Parameters:
**	adr: set address for select slave
**	rxbuf: receive buffer pointer
**	rxnum: buffer size
**	wait: wait for start condition
**
**  Returns:
**	MD_OK
**	MD_ERROR1
**	MD_ERROR2
**
**-----------------------------------------------------------------------------
*/
MD_STATUS IICA0_MasterReceiveStart(UCHAR adr, UCHAR *rxbuf, USHORT rxnum, UCHAR wait)
{
	MD_STATUS status = MD_OK;

	IICAMK0 = 1U;	/* disable INTIIA0 interrupt */
	if (IICBSY0 == 1U)
	{
		/* Check bus busy */
		IICAMK0 = 0U;	/* enable INTIIA0 interrupt */
		status = MD_ERROR1;
	}	
	else if ((SPT0 == 1U) || (STT0 == 1U))
	{
		/* Check trigger */
		IICAMK0 = 0U;	/* enable INTIICA0 interrupt */
		status = MD_ERROR2;
	}
	else
	{
		STT0 = 1U;	/* set IICA0 start condition */
		IICAMK0 = 0U;	/* enable INTIIA0 interrupt */
		/* Wait */
		while (wait--)
		{
			;
		}
		/* Set parameter */
		gIica0RxLen = rxnum;
		gIica0RxCnt = 0U;
		gpIica0RxAddress = rxbuf;
  		gIica0MasterStatusFlag  = _00_IICA_MASTER_FLAG_CLEAR;
		adr |= 0x01U;	/* set receive mode */
		IICA0 = adr;	/* receive address */
	}

	return (status);
}

/* Start user code for adding. Do not edit comment generated here */

void CSI10_TXOnly_Init(void)
{
	ST0 |= _0004_SAU_CH2_STOP_TRG_ON;	/* disable CSI10 */
	CSIMK10 = 1U;	/* disable INTCSI10 interrupt */
	CSIIF10 = 0U;	/* clear INTCSI10 interrupt flag */
	/* Set INTCSI10 low priority */
	CSIPR110 = 1U;
	CSIPR010 = 1U;
	SIR02 = _0004_SAU_SIRMN_FECTMN | _0002_SAU_SIRMN_PECTMN | _0001_SAU_SIRMN_OVCTMN;	/* clear error flag */
	SMR02 = _0020_SAU_SMRMN_INITIALVALUE | _0000_SAU_CLOCK_SELECT_CK00 | _0000_SAU_CLOCK_MODE_CKS | _0000_SAU_TRIGGER_SOFTWARE | _0000_SAU_MODE_CSI | _0000_SAU_TRANSFER_END;
	SCR02 = _8000_SAU_TRANSMISSION | _0000_SAU_TIMING_1 | _0000_SAU_MSB | _0007_SAU_LENGTH_8;
	SDR02 = _FE00_CSI10_DIVISOR;
	SO0 |= _0400_SAU_CH2_CLOCK_OUTPUT_1;	/* CSI10 clock initial level */
	SO0 &= ~_0004_SAU_CH2_DATA_OUTPUT_1;	/* CSI10 SO initial level */
	SOE0 |= _0004_SAU_CH2_OUTPUT_ENABLE;	/* enable CSI10 output */
	/* Set SO10 pin */
	P0 |= 0x04U;
	PMC0 &= 0xFBU;
	PM0 &= 0xFBU;
	/* Set SCK10 pin */
	P0 |= 0x10U;
	PM0 &= 0xEFU;
}

MD_STATUS CSI10_TXOnly_SendData(UCHAR *txbuf, USHORT txnum)
{
	MD_STATUS status = MD_OK;

	if (txnum < 1U)
	{
		status = MD_ARGERROR;
	}
	else
	{
		gCsi10TxCnt = txnum;	/* send data count */
		gpCsi10TxAddress = txbuf;	/* send buffer pointer */
		CSIMK10 = 1U;	/* disable INTCSI10 interrupt */
		SIO10 = *gpCsi10TxAddress;	/* started by writing data to SDR[7:0] */
		gpCsi10TxAddress++;
		gCsi10TxCnt--;
		CSIMK10 = 0U;	/* enable INTCSI10 interrupt */
	}

	return (status);
}

/* End user code. Do not edit comment generated here */
