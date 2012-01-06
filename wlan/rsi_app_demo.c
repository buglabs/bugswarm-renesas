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
 * @brief
 * UART, INIT, Wifi Initialization
 *
 */

 /**
  * Includes
  */
#include "data_types.h"
#include "uart_api.h"
#include "at_command_processor.h"
#include "hal.h"


uint8 app_rx_buf[MAX_RX_BUFFERS][MAX_BUF_LEN];

int main(void)
{
  rsi_lib_init();

  for (ii = 0; ii < MAX_RX_BUFFERS; ii++)
  {
    rsi_set_rx_buffer(app_rx_buf[ii], MAX_BUF_LEN);
  }


}
