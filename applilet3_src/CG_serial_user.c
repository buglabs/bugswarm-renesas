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
* Filename:	CG_serial_user.c
* Abstract:	This file implements device driver for Serial module.
* APIlib:	Applilet3 for RL78/G13 E1.00b [14 Jan 2011]
* Device:	R5F100LE
* Compiler:	IAR Systems iccrl78
* Creation date:	5/26/2011
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
#include "rsi_hal.h"
/* End user code. Do not edit comment generated here */
#include "CG_userdefine.h"

/*
*******************************************************************************
**  Global define
*******************************************************************************
*/
extern volatile UCHAR	*gpCsi00RxAddress;	/* csi00 receive buffer address */
extern volatile USHORT	gCsi00RxLen;		/* csi00 receive data length */
extern volatile USHORT	gCsi00RxCnt;		/* csi00 receive data count */
extern volatile UCHAR	*gpCsi00TxAddress;	/* csi00 send buffer address */
extern volatile USHORT	gCsi00TxLen;		/* csi00 send data length */
extern volatile USHORT	gCsi00TxCnt;		/* csi00 send data count */
extern volatile UCHAR	*gpCsi10RxAddress;	/* csi10 receive buffer address */
extern volatile USHORT	gCsi10RxLen;		/* csi10 receive data length */
extern volatile USHORT	gCsi10RxCnt;		/* csi10 receive data count */
extern volatile UCHAR	*gpCsi10TxAddress;	/* csi10 send buffer address */
extern volatile USHORT	gCsi10TxLen;		/* csi10 send data length */
extern volatile USHORT	gCsi10TxCnt;		/* csi10 send data count */
extern volatile UCHAR	*gpUart2TxAddress;	/* uart2 transmit buffer address */
extern volatile USHORT	gUart2TxCnt;		/* uart2 transmit data number */
extern volatile UCHAR	*gpUart2RxAddress;	/* uart2 receive buffer address */
extern volatile USHORT	gUart2RxCnt;		/* uart2 receive data number */
extern volatile USHORT	gUart2RxLen;		/* uart2 receive data length */
extern volatile UCHAR	gIica0MasterStatusFlag;	/* iica0 master flag */ 
extern volatile UCHAR	gIica0SlaveStatusFlag;	/* iica0 slave flag */
extern volatile UCHAR	*gpIica0RxAddress;	/* iica0 receive buffer address */
extern volatile USHORT	gIica0RxCnt;		/* iica0 receive data length */
extern volatile USHORT	gIica0RxLen;		/* iica0 receive data count */
extern volatile UCHAR	*gpIica0TxAddress;	/* iica0 send buffer address */
extern volatile USHORT	gIica0TxCnt;		/* iica0 send data count */
/* Start user code for global. Do not edit comment generated here */
volatile char	gCsi00TxComplete;
volatile char	gCsi10TxComplete;
volatile char	gIica0TxComplete;
volatile char	gIica0RxComplete;
//volatile UCHAR  UART_TXst = 0;
//volatile UCHAR  UART_TXed = 0;
//volatile UCHAR  UART_RXst = 0;
//volatile UCHAR  UART_RXed = 0;
//volatile UCHAR  UART_Loop_detect = 0;

volatile UCHAR G_SPI_SendingData; //spi busy 
volatile UCHAR G_SPI_ReceivingData; //spi busy flag

/* End user code. Do not edit comment generated here */



/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is INTCSI00 interrupt service routine.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
#pragma vector = INTCSI00_vect
__interrupt void MD_INTCSI00(void)
{
	UCHAR err_type;

	err_type = (UCHAR)(SSR00 & _0001_SAU_OVERRUN_ERROR);
	SIR00 = (USHORT)err_type;
	if (err_type == 1U)
	{
		CSI00_ErrorCallback(err_type);	/* overrun error occurs */
	}
	else
	{
		if (gCsi00TxCnt > 0U)
		{
			*gpCsi00RxAddress = SIO00;
			gpCsi00RxAddress++;
			SIO00 = *gpCsi00TxAddress;
			gpCsi00TxAddress++;
			gCsi00TxCnt--;		
		}
		else 
		{
			if (gCsi00TxCnt == 0U)
			{
				*gpCsi00RxAddress = SIO00;
			}
			CSI00_SendEndCallback();	/* complete send */
			CSI00_ReceiveEndCallback();	/* complete receive */
		}
	}
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when CSI00 finishes reception.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void CSI00_ReceiveEndCallback(void)
{
	/* Start user code. Do not edit comment generated here */
	/* End user code. Do not edit comment generated here */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when CSI00 reception error occurs.
**
**  Parameters:
**	err_type: error type value
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void CSI00_ErrorCallback(UCHAR err_type)
{
	/* Start user code. Do not edit comment generated here */
	/* End user code. Do not edit comment generated here */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when CSI00 finishes transmission.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void CSI00_SendEndCallback(void)
{
	/* Start user code. Do not edit comment generated here */
	/* End user code. Do not edit comment generated here */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is INTCSI10 interrupt service routine.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
#pragma vector = INTCSI10_vect
__interrupt void MD_INTCSI10(void)
{
#if 0  
  
	UCHAR err_type;

	err_type = (UCHAR)(SSR02 & _0001_SAU_OVERRUN_ERROR);
	SIR02 = (USHORT)err_type;
	if (err_type != 1U)
	{
		if (gCsi10TxCnt > 0U)
		{
			SIO10 = *gpCsi10TxAddress;
			gpCsi10TxAddress++;
			gCsi10TxCnt--;
		}
		else
		{
			CSI10_SendEndCallback();	/* complete send */
		}
	}
#endif
        
    	UCHAR err_type;

	err_type = (UCHAR)(SSR02 & _0001_SAU_OVERRUN_ERROR);
	SIR02 = (USHORT)err_type;
	if (err_type == 1U)
	{
		CSI10_ErrorCallback(err_type);	/* overrun error occurs */
	}
	else
	{
		if (gCsi10TxCnt > 0U)
		{
			*gpCsi10RxAddress = SIO10;
			gpCsi10RxAddress++;
			SIO10 = *gpCsi10TxAddress;
			gpCsi10TxAddress++;
			gCsi10TxCnt--;		
		}
		else 
		{
			if (gCsi10TxCnt == 0U)
			{
				*gpCsi10RxAddress = SIO10;
			}
			CSI10_SendEndCallback();	/* complete send */
			CSI10_ReceiveEndCallback();	/* complete receive */
		}
	}    
}



/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when CSI10 finishes reception.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void CSI10_ReceiveEndCallback(void)
{
	/* Start user code. Do not edit comment generated here */
    G_SPI_ReceivingData = 0;
	/* End user code. Do not edit comment generated here */
}



/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when CSI10 finishes transmission.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void CSI10_SendEndCallback(void)
{
	/* Start user code. Do not edit comment generated here */
  
        G_SPI_SendingData = 0; //spi busy flag
	/* End user code. Do not edit comment generated here */
}




void CSI10_ErrorCallback(UCHAR err_type)
{
	/* Start user code. Do not edit comment generated here */
	/* End user code. Do not edit comment generated here */
}


/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is INTSR2 interrupt service routine.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/



#pragma vector = INTSR2_vect
__interrupt void UART2_RxInt(void) {
	volatile UCHAR		c;
	    
#if 0
        UCHAR			next;
	c = RXD2;   // Read data
	// put read data to buffer
	next = (UART_RXed + 1) & UART_BUF_MASK;
	if (UART_RXst != next) {
		UART_RXbuf[UART_RXed] = c;
		UART_RXed = next;
              
	}
        
        if(UART_RXed == UART_RXst)
        {
          UART_Loop_detect = 1;
        }
#endif  
        
            rsi_receive();
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is INTSRE2 interrupt service routine.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
#pragma vector = INTSRE2_vect
__interrupt void MD_INTSRE2(void)
{
	UCHAR err_type;

	*gpUart2RxAddress = RXD2;
	err_type = (UCHAR)(SSR11 & 0x0007U);
	SIR11 = (USHORT)err_type;
	UART2_ErrorCallback(err_type);
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is INTST2 interrupt service routine.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
#pragma vector = INTST2_vect
__interrupt void UART2_TxInt(void) {
#if 0
	if (UART_TXst == UART_TXed) {  // Empty
		STMK2 = 1;
		while((SSR10 & (0x20))!=0) {}; // Wait for SDR to clear
		return;
	}
	
	while((SSR10 & (0x20))!=0) {
		NOP();   // Wait for SDR to clear
	};
	TXD2 = UART_TXbuf[UART_TXst];
	UART_TXst = (UART_TXst + 1) & UART_BUF_MASK;
#endif
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when UART2 finishes reception.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UART2_ReceiveEndCallback(void)
{
	/* Start user code. Do not edit comment generated here */
	/* End user code. Do not edit comment generated here */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when UART2 receives an overflow data.
**
**  Parameters:
**	rx_data: receive data
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UART2_SoftOverRunCallback(UCHAR rx_data)
{
	/* Start user code. Do not edit comment generated here */
	/* End user code. Do not edit comment generated here */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when UART2 finishes transmission.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UART2_SendEndCallback(void)
{
	/* Start user code. Do not edit comment generated here */
	/* End user code. Do not edit comment generated here */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when UART2 reception error occurs.
**
**  Parameters:
**	err_type: error type value
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UART2_ErrorCallback(UCHAR err_type)
{
	/* Start user code. Do not edit comment generated here */
      LED2 = 0;
	/* End user code. Do not edit comment generated here */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is INTIICA0 interrupt service routine.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
#pragma vector = INTIICA0_vect
__interrupt void MD_INTIICA0(void)
{
	if ((IICS0 & _80_IICA_STATUS_MASTER) == 0x80U)
	{
		IICA0_MasterHandler();
	}
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is IICA0 master handler.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/

void IICA0_MasterHandler(void)
{
	/* Control for communication */
	if ((IICBSY0 == 0U) && (gIica0TxCnt != 0U))
	{
		IICA0_MasterErrorCallback(MD_SPT);
	}
	/* Control for sended address */
	else
	{
		if ((gIica0MasterStatusFlag & _80_IICA_ADDRESS_COMPLETE) == 0U)
		{
			if (ACKD0 == 1U)
			{
				gIica0MasterStatusFlag |= _80_IICA_ADDRESS_COMPLETE;
				if (TRC0 == 1U)
				{
					WTIM0 = 1U;
					if (gIica0TxCnt > 0U)
					{
						IICA0 = *gpIica0TxAddress;
						gpIica0TxAddress++;
						gIica0TxCnt--;
					}
					else
					{
						IICA0_MasterSendEndCallback();
					}
				}
				else
				{
					ACKE0 = 1U;
					WTIM0 = 0U;
					WREL0 = 1U;
				}
			}
			else
			{
				IICA0_MasterErrorCallback(MD_NACK);
			}
		}
		else
		{
			/* Master send control */
			if (TRC0 == 1U)
			{
				if ((ACKD0 == 0U) && (gIica0TxCnt != 0U))
				{
					IICA0_MasterErrorCallback(MD_NACK);
				}
				else
				{
					if (gIica0TxCnt > 0U)
					{
						IICA0 = *gpIica0TxAddress;
						gpIica0TxAddress++;
						gIica0TxCnt--;
					}
					else
					{
						IICA0_MasterSendEndCallback();
					}
				}
			}
			/* Master receive control */
			else
			{
				if (gIica0RxCnt < gIica0RxLen)
				{
					*gpIica0RxAddress = IICA0;
					gpIica0RxAddress++;
					gIica0RxCnt++;
					if (gIica0RxCnt == gIica0RxLen)
					{
						ACKE0 = 0U;
						WREL0 = 1U;
						WTIM0 = 1U;
					}
					else
					{
						WREL0 = 1U;
					}
				}
				else
				{
					IICA0_MasterReceiveEndCallback();
				}
			}
		}
	}
}


/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when IICA0 master error occurs.
**
**  Parameters:
**	flag: status flag
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void IICA0_MasterErrorCallback(MD_STATUS flag)
{
	/* Start user code. Do not edit comment generated here */
	/* End user code. Do not edit comment generated here */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when IICA0 finishes master reception.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void IICA0_MasterReceiveEndCallback(void)
{
	SPT0 = 1U;
	/* Start user code. Do not edit comment generated here */
        gIica0RxComplete = 0;
	/* End user code. Do not edit comment generated here */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is a callback function when IICA0 finishes master transmission.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void IICA0_MasterSendEndCallback(void)
{
	SPT0 = 1U;
	/* Start user code. Do not edit comment generated here */
        gIica0TxComplete = 0;
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */

/* End user code. Do not edit comment generated here */
