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
 * ! @brief  HAL API
 *
 * @section Description
 * This file contains the function definition prototypes for HAL
 *
 */

#ifndef _HAL_API_H_
#define _HAL_API_H_


#include "CG_macrodriver.h"
#include "CG_serial.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "CG_userdefine.h"
#include "PinoutDef.h"
#include "rsi_data_types.h"
/**
 * DEFINES
 */
#define MILLISEC_COUNT  1000
#define MICROSEC_COUNT  100



/*! @brief  Power on the module */
#define HAL_MODULE_POWER_ON()  WIFI_PWRON = 1 

/*! @brief  Power off the module */
#define HAL_MODULE_POWER_OFF() WIFI_PWRON = 0 

/*! @brief  Give reset to the module */
#define HAL_MODULE_RESET() WIFI_RESET = 1  

/*! @brief  Clear reset to the module */
#define HAL_MODULE_CLEAR_RESET() WIFI_RESET = 0 

/*! @brief  Selects SPI */
#define HAL_MODULE_SELECT_SPI()

/*! @brief  Selects UART */
#define HAL_MODULE_SELECT_UART()  

#define UART_PutChar(data) RL78_UART_PutChar(data) 
int16 UART_PutChar(uint8 ch);

#define DISABLE_RX_UART_INTERRUPT() uart_disable_rx_done_interrupt()
#define ENABLE_RX_UART_INTERRUPT() uart_enable_rx_done_interrupt()

void uart_enable_rx_done_interrupt(void);
void uart_disable_rx_done_interrupt(void);

#define UART_Start  UART2_Start


#define UART_GetChar UART_GetChar
int  UART_GetChar(uint8 *data_byte);


void rsi_delayMs(uint16 delay);
void rsi_delayUs(uint16 delay);
void rsi_receive (void);
#endif
