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


#include "../gen/r_cg_macrodriver.h"
#include "../gen/r_cg_serial.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "../gen/r_cg_userdefine.h"
//#include "PinoutDef.h"
#include "rsi_data_types.h"
/**
 * DEFINES
 */
#define MILLISEC_COUNT  1000
#define MICROSEC_COUNT  100



/*! @brief  Power on the module */
#define HAL_MODULE_POWER_ON()  set_gpio(&P4, 3, 1) 

/*! @brief  Power off the module */
#define HAL_MODULE_POWER_OFF() set_gpio(&P4, 3, 0)

/*! @brief  Give reset to the module */
#define HAL_MODULE_RESET() set_gpio(&P13, 0, 1)

/*! @brief  Clear reset to the module */
#define HAL_MODULE_CLEAR_RESET() set_gpio(&P13, 0, 0)

/*! @brief  Selects SPI */
#define HAL_MODULE_SELECT_SPI()

/*! @brief  Selects UART */
#define HAL_MODULE_SELECT_UART()  

//#define UART_PutChar(data) RL78_UART_PutChar(data) 
//uint16 UART_PutChar(uint8 ch);
uint16_t UART_PutChar(uint8_t ch);

#define DISABLE_RX_UART_INTERRUPT() uart_disable_rx_done_interrupt()
#define ENABLE_RX_UART_INTERRUPT() uart_enable_rx_done_interrupt()

void uart_enable_rx_done_interrupt(void);
void uart_disable_rx_done_interrupt(void);

#define UART_Start  R_UART2_Start


//#define UART_GetChar UART_GetChar
int UART_GetChar(uint8_t *data_byte);


void rsi_delayMs(uint16 delay);
void rsi_delayUs(uint16 delay);
void rsi_receive (void);
#endif
