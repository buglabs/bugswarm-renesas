/** 
 * @file
 * Copyright(C) 2011 Redpine Signals Inc.
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief RSI INIT, Top level system initializations
 * 
 * @section Description
 * Any initializations which are not module or platform specific are done here
 * Any plaform or module specific initializations are called from here
 * *
 */   

/**
 * Includes
 */ 
#include "rsi_data_types.h"
#include "rsi_uart_api.h"
#include "rsi_hal.h"
#include "../gen/r_cg_port.h"

/**
 * @fn	void    rsi_powercycle(uint16 timeout)
 * @brief	MODULE POWER ON, Power cycles the module
 * @param[in]	uint16 timeout, timeout value in MS
 * @return	None
 */
void
rsi_powercycle (void)
{
/**
 * Take care of specific modules types
 */ 
  /* Start off with power off */
  HAL_MODULE_POWER_OFF();

  /* Select UART device */
  HAL_MODULE_SELECT_UART();

  /* Turn on the Power */
  HAL_MODULE_POWER_ON();
  
  /* Let everything stabilize with power off */
  rsi_delayMs (500);              

  /* Start off in reset */
  HAL_MODULE_RESET();
  
  /* Let the power stabilize for */
  rsi_delayMs (100);       

  /* Release reset */
  HAL_MODULE_CLEAR_RESET();
  /* Wait for the module to stabilize before trying to talk to it. */
  rsi_delayMs (200);             
}

/*===========================================================*/ 
/**
 * @fn	        void rsi_init(uint16 timeout)
 * @brief       SYSTEM INIT, Initializes the system specific hardware
 * @param[in]   uint16 timeout, timeout value in MS
 * @return      None
 */ 
void rsi_init(void) 
{
  rsi_lib_init();
  
  /* UART HAL system init */
  rsi_hal_init();
#if (DBG_LVL & PL3)
  DPRINT ("\r\nAfter hsysinit... ");
#endif  
  /* cycle the power to the module */
  rsi_powercycle ();
}
