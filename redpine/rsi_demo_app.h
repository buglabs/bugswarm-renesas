/* *
 * @file
 * Copyright(C) Redpine Signals 2011
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief HEADER, INIT STRUCT, function prototypes & default init values
 *
 */  
  
#ifndef _RSI_DEMO_APP_H_
#define _RSI_DEMO_APP_H_
/*==============================================*/ 
/**
 * Global Defines
 */


#define  RX_THRU_START        2
#define  RX_THRU_START_ACK    3
#define  RX_THRU_STOP         4
#define  RX_THRU_INFO         5
#define  RX_THRU_INFO_ACK     6
#define  TX_THRU_START        7
#define  TX_THRU_START_ACK    8
#define  TX_THRU_STOP         9
#define  TX_TRHU_STOP_ACK     10
#define  CNFG_CNTRL           11
#define  LED_CNTRL            13
#define  PUSHBUTON_CNTRL      14 
#define	 TEMPERATURE_CNTRL    0x10
#define  LIGHTSENSOR_CNTRL    0x11

#define MODULE_SOCKET_TWO     55500
#define MODULE_SOCKET_THREE   50000
#define TARGET_SOCKET_TWO     55000

uint16 rsi_convert_ip_to_string(uint8 *num_buff, uint8 *ip_buff);

#endif	

