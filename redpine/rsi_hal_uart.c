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
#include "../gen/r_cg_macrodriver.h"
#include "../gen/r_cg_serial.h"
#include "../gen/r_cg_userdefine.h"
#include "rsi_data_types.h"
#include "rsi_uart_api.h"
#include "rsi_hal.h"

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


//int16 UART_PutChar(UCHAR ch)
uint16_t UART_PutChar(uint8_t ch)
{
        //if ( (SSR10 & 0x20) == 0)
	if ( (SSR10 & (1<<6)) == 0)
	{
	  TXD2 = ch;
	  //printf(" P%02x ",ch);
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

int UART_GetChar(uint8_t *data_byte)
{ 
    int err_type ;     
    *data_byte = RXD2;   
    //printf(" G%02x ",*data_byte);
    err_type = (int)(SSR10 & 0x0007U);     
    return err_type;
}