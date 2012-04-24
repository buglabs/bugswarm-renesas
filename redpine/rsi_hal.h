/**
 * @file
 *
 * Copyright(C) 2011 Redpine Signals Inc.
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief HEADER, HAL
 *
 * @section Description
 * This file contains the function definition prototypes for HAL
 *
 */


#ifndef _HAL_H_
#define _HAL_H_

/**
* INCLUDES
*/

#include "rsi_hal_api.h"

enum RSI_UART_ERROR 
{
  UART_NOERROR	= 0x0000,
  UART_TIMEOUT_ERROR = 0x0ff,
};

int16  rsi_getLine(uint8 *buffer, uint16 *maxLen);
uint16 rsi_rgetchar(uint8 *dataByte);
int16  rsi_rputchar(uint8 c);

/* Sysinit */
void rsi_hal_init(void);
void rsi_hal_shutdown(void);
void rsi_powercycle(void);
#endif
