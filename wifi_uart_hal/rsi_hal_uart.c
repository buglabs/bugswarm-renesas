/**
 * @file hal_uart.c
 *
 * Copyright(C) 2011 Redpine Signals Inc.
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief HAL, UART
 *
 * @section Description
 * This file contains the low level UART functions
 *
 */
/**
 * Includes
 */
#include "rsi_data_types.h"
#include "rsi_uart_api.h"
#include "rsi_hal.h"
#include "CG_macrodriver.h"
#include "CG_serial.h"
#include "CG_userdefine.h"

/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function transmits the character
**
**  Parameters:
**	None
**
**  Returns:
**	none
**
**-----------------------------------------------------------------------------
*/


SHORT RL78_UART_PutChar(UCHAR ch) 
{
        if ( (SSR10 & 0x20) == 0)
	{
	  TXD2 = ch;
          return UART_NOERROR;
	}

	return UART_TIMEOUT_ERROR;

}


/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function is used to read the character
**
**  Parameters:
**	None
**
**  Returns:
**	none
**
**-----------------------------------------------------------------------------
*/

int  UART_GetChar(UCHAR *data_byte)
{ 
    UCHAR err_type ;     
    *data_byte = RXD2;   
    err_type = (UCHAR)(SSR11 & 0x0007U);     
    return err_type;
}