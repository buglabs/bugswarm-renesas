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
 * @brief HAL Initialization and shutdown functions
 *
 * @section Description
 * This file contains the platform specific initialization and shutdown functions
 *
 */  
/**
* Include files
*/ 
#include "rsi_data_types.h"
#include "rsi_hal.h"

/*! @brief      HAL Initialization
 * @fn          void rsi_hal_init(uint16 timeout)
 * @brief       HAL,  Initializes the platform specifc hardware
 * @param[in]	uint16 timeout, timeout for the operation in MS
 */ 
void rsi_hal_init(void)
{
  
  /* Platform specific code comes here */
	  
  /* UART1 Initialization */
  UART_Start ();
}

/*! @brief      HAL shutdown
 * @fn          void rsi_hal_shutdown(void)
 * @brief       HAL,  Shuts down the platform specifc hardware
 */ 
void rsi_hal_shutdown (void)
{
  return;
}
