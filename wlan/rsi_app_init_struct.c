/**
 * @file
 *
 * Copyright(C) Redpine Signals 2011
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief INIT STRUCT, Initializes the global parameter structure to the values in the #define statements in main.h
 *
 *
 *
 */
/**
 * Includes
 */
#include "../Include/data_types.h"
#include "../Include/api_init_struct.h"
#include "../Include/uart_api.h"
#include <stdio.h>
/**
 * Initialize the Global Structure values from the #defines
 */

api strApi;


api strApi =
{
  .band = BAND,
  /* Scan frame */
  .ScanFrame =
  {
   .channel = SCAN_CHANNEL
#ifdef SCAN_SSID
     , .ssid = SCAN_SSID
#endif 
  }
  ,
  /* Join frame */
  .JoinFrame =
  {
      .nwType = NETWORK_TYPE, 
      .securityType = SECURITY_TYPE, 
      .dataRate = DATA_RATE, 
      .powerLevel = POWER_LEVEL, 
      .psk = PSK, 
      .ssid = JOIN_SSID, 
      .ibssMode = IBSS_MODE, 
      .ibssChannel = IBSS_CHANNEL, 
      .auth_mode = AUTH_MODE 
  }
  , 
  /* IP configuration */
  .IPparamFrame =
  {
     .dhcpMode = DHCP_MODE, 
     .ipaddr = MODULE_IP_ADDRESS, 
     .netmask = NETMASK, 
     .gateway = GATEWAY 
  } , 
  .SocketFrame =
  {
     .remote_ip = REMOTE_IP, 
     .rport = REMOTE_PORT, 
     .lport = LOCAL_PORT 
  }, 
};



