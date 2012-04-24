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
 * @brief HAL, Delay Functions
 *
 * @section Description
 * This file implements platform dependent timer delay functions.
 *
 */  
/**
* Include files
*/ 
#include "rsi_data_types.h"
#include "rsi_hal.h"
#include "../gen/r_cg_it.h"
  
/*===============================================================*/ 
/**
 * @fn	        void rsi_delayMs(uint16, delay)
 * @brief	Delays by an integer number of milliseconds
 * @param[in]	delay Number of milliseconds to delay
 * @return      none
 */ 
void
rsi_delayMs (uint16 delay)
{
  delay_ms(delay);
  /*uint32 delay_ms;
  int ii;
  delay_ms = MILLISEC_COUNT * delay;
  for(ii = 0;ii < delay_ms;ii++);
  return; */
}

/*===============================================================*/
/**
 * @fn	        void rsi_delayUs(uint16 delay)
 * @brief       Delays by an integer number of microseconds
 * @param[in]	uint16 delay, Number of microseconds to delay
 * @return      none
 */ 
void
rsi_delayUs (uint16 delay)
{
  uint32 delay_us;
  int ii;
  delay_us = MICROSEC_COUNT * delay;
  for(ii = 0;ii < delay_us;ii++);
  return;
}
