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
 * @brief This file contains the APIs for the following
 *         - UDP/TCP socket interface.
 *         - Power mode control interface of -22 module.
 *         - scan, join interfaces.
 */
#include "rsi_data_types.h"
#include "rsi_uart_api.h"
#include "rsi_at_command_processor.h"


/*! @brief Opens a UDP server socket 
 *
 *  This function is used to open a UDP socket, listening on the given 
 *  port number.
 *
 *  @param  sf            Pointer to the socket frame structure
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_socket_ludp_open (rsi_socketFrame_t * sf)
{
   return rsi_send_at_command (OPEN_LUDP, (void *) sf);
}


/*! @brief Opens a TCP server socket 
 *
 *  This function is used to open a TCP socket, listening on the given 
 *  port number.
 *  
 *  @param sf            Pointer to the socket frame structure
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_socket_ltcp_open (rsi_socketFrame_t * sf)
{
  return rsi_send_at_command (OPEN_LTCP, (void *) sf);
}

/*! @brief Opens a UDP socket
 *
 *  This function is used to open a UDP socket.
 *
 *  @param sf            Pointer to the socket frame structure
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_socket_udp_open (rsi_socketFrame_t * sf)
{
   return rsi_send_at_command (OPEN_UDP, (void *) sf);
}

/*! @brief Opens a TCP socket.
 *
 *  This function is used to open a TCP socket.
 *
 *  @param sf            Pointer to the socket frame structure
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_socket_tcp_open (rsi_socketFrame_t * sf)
{
  return rsi_send_at_command (TCP_CONNECT_REQ, (void *) sf);
}

/*! @brief Opens a multicast socket
 *         
 *  @retval  0 for success 0xff for failure  
 */
int16
rsi_socket_multicast_open(rsi_socketFrame_t * sf)
{
  return rsi_send_at_command (OPEN_MULTICAST, (void*)sf);
}

/*! @brief Sends data on UDP/TCP socket.
 *
 *  This function is used to send data on UDP/TCP socket. 
 *
 *  @param sock     	 Pointer to the sending socket data structure
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_send (rsi_socketFrame_t * sock)
{
  return rsi_send_at_command (TX_DATA, (void *) sock);
}

/*! @brief Gets the connection status of a TCP socket.
 *
 *  This function is used to get the connection status(Opened or not opened)
 *  of a TCP socket.
 *
 *  @param handle	 Socket id of the socket for which the status is to be queried
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_query_ltcp_status (uint8 handle)
{
  return rsi_send_at_command (TCP_CSTATUS, (void *) &handle);
}

/*! @brief Closes UDP/TCP socket.
 *
 *  This function is used to close a UDP/TCP socket.
 *
 *  @param sid           Socket id of UDP/TCP socket.
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_socket_close (int16 sid)
{
  return rsi_send_at_command (CLOSE_SOCKET, (void *) &sid);
}


/*! @brief Sets the sleep timer.
 *
 *  This function is used to set the sleep timer.
 *  -22 module sleeps for the given amount of time and gives the 
 *  "SLEEP" message to the host. 
 *  If host responds with an "ACK" message, the -22 module will continue 
 *  to be in sleep.
 *  This is applicable before the wifi connection is established.
 *
 *  @param sleep_time    sleep_time in mill seconds
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_set_sleep_timer (uint16 sleep_time)
{
  return rsi_send_at_command (SLEEP_TIMER, &sleep_time);
}

/*! @brief Sets the power mode (1/2)
 *
 *  This function is used to set one of 2 power modes (powermode-1, powermode-2)
 *
 *  @param pwr_mode      Power mode
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_powermode (uint8 pwr_mode)
{
  return rsi_send_at_command (POWER_MODE_SEL, (void *) &pwr_mode);
}

/*! @brief Gives deep sleep indication(ACK) to the -22 module.
 *
 *  This function is used to switch off the 
 *  core control block of -22 module in power mode 1
 *
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_send_ack (void)
{
  uint8 ack_msg[] = "ACK\r\n";
  return rsi_send_cmd_to_device (ack_msg, sizeof (ack_msg) - 1);
}

/*! @brief Disables the power mode(1 or 2)
 *
 *  This function is used to disbale the power mode-1 or power mode-2.
 *
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_disable_powermode (void)
{
  uint8 pwr_mode = 0;
  return rsi_send_at_command (POWER_MODE_SEL, &pwr_mode);
}


/*! @brief Configures the band for the Wi-Fi module
 *
 *  This function is used to send the band command to Wi-Fi module
 *
 *  @param band          band value, 0 for 2.4 GHz and 1 for 5 GHz.
 *  @retval 0 for success 0xff for failure  
 */

int16
rsi_band (uint8 band)
{
  return rsi_send_at_command (BAND_SEL, &band);
}

/*! @brief Intialises the baseband and RF components of the Wi-Fi module
 *
 *  This function is used to send the init command to Wi-Fi module
 *
 *  @param timeout      timeout period in milli seconds.
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_init_baseband (void)
{
  return rsi_send_at_command (INIT, NULL);
}

/*! @brief Disconnects the Wi-Fi connection
 *
 *  This function is used to disassociate the Wi-Fi connection 
 *
 *  @retval 0 for success 0xff for failure  
 */

int16
rsi_disconnect (void)
{
  return rsi_send_at_command (DISCONNECT, NULL);
}

/*! @brief Performs the scanning process.
 *
 *   This function is used to perform the scanning based on the parameters.
 *   - To scan the hidden AP, SSID should be given; 
 *     Ex: linksys_AP, 10, 0, pointer to the scan rsp;
 *   - If the channel number is provided, scanning is done in the given channel, 
 *     otherwise all channels are scanned.
 *
 *  @param  scan_frame   Pointer to the sending scan command frame.
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_scan (struct rsi_scanFrameSnd_s * scan_frame)
{
  return rsi_send_at_command (SCAN, scan_frame);
}

/*! @brief Establishes the Wi-Fi connection 
 *
 *  This function is used to join to an AP. 
 *
 *  @param jf            Pointer to the join frame structure.
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_join (struct rsi_joinFrameSnd_s * jf)
{
  return rsi_send_at_command (JOIN, jf);
}

/*! @brief Configures the IP credentials.
 *   This function is used to configure the IP details, 
 *
 *  @param ip_param      Pointer to the ip_param data structure.
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_ipparam_set (rsi_ipparamFrameSnd_t * ip_param)
{
  return rsi_send_at_command (IP_CONFIG, ip_param);
}

/*! @brief Queries the RSSI value of the AP.
 * 
 *   This function is used to query the RSSI value. 
 *
 *  @retval 0 for success 0xff for failure  
 */

int16
rsi_query_rssi (void)
{
  return rsi_send_at_command (QRY_RSSI, NULL);
}

/*! @brief Queries the network parameters for the Wi-Fi connection.
 * 
 *   This function is used to query the network parameters for the Wi-Fi connection. 
 *
 *  @retval 0 for success 0xff for failure  
 */

int16
rsi_query_net_parms (void)
{
  return rsi_send_at_command (QRY_NET_PARAMS, NULL);
}


/*! @brief Queries the MAC address of the Wi-Fi module.
 * 
 *  This function is used to query the MAC address. 
 *
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_query_mac_addr (void)
{
  return rsi_send_at_command (MAC_REQ, NULL);
}

/*! @brief Queries the BSSIDs of the scanned networks.
 * 
 *   This function is used to query the BSSIDs for the scanned networks.
 *   This command has to be given immediately after scan command
 *
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_query_bssid (void)
{
  return rsi_send_at_command (QRY_BSSID, NULL);
}


/*! @brief Queries the network type of the scanned networks.
 * 
 *  This function is used to query the network type of the scanned networks.
 *  This command has to be given immediately after scan command
 *
 *  @retval 0 for success 0xff for failure  
 */

int16
rsi_query_nwtype (void)
{
  return rsi_send_at_command (QRY_NWTYPE, NULL);
}

/*! @brief Gets the FW version
 *   This function is used to retrieve the firmware version of the connection module.
 *
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_query_fwversion (void)
{
  return rsi_send_at_command (FWVERSION, NULL);
}

/*! @brief Gets the number of APs scanned
 * 
 *  @retval 0 for success 0xff for failure  
 */

int16
rsi_query_scan_num (void)
{
  return rsi_send_at_command (QRY_NUMSCAN, NULL);
}

/*! @brief  Returns the scanned access points.
 *   This command is used in conjunction with the rsi_scan_num and 
 *   rsi_scan commands. 
 *   It is valid only if the rsi_scan_num command has been used to configure the 
 *   number of scan results to be returned by the module. The module responds by 
 *   returning the scanned Access Points. 
 *
 *  @retval 0 for success 0xff for failure  
 */
int16
rsi_scan_next (void)
{
  return rsi_send_at_command (SCAN_NEXT, NULL);
}

/*! @brief  To configure the number of scan results returned by the module.
 *   This function is used to set the number of scan results to be return by module.
 *
 *  @param   num  	 Number of scan results to be return by module. 
 *                       For ex: num=2 means the number of scan results that the
 *                       module returns in scan response are 2.
 *  @retval  0 for success 0xff for failure  
 */

int16
rsi_scan_num (uint8 num)
{
  return rsi_send_at_command (SCAN_NUM, &num);
}

/*! @brief  Enables the passive scan of channels.
 *   This function is used to scan the channels passive.
 *
 *  @param   value  	 Pointer to the bitmap value of passive scan.
 *                       For ex: bitmap value 255 whose binary equivalent is 11111111 
 *                       scans first 8 channels passively.
 *  @retval  0 for success 0xff for failure  
 */

int16
rsi_scan_passive (uint16 value)
{
  return rsi_send_at_command (SCAN_PASSIVE, &value);
}


/*! @brief Sets the network type 
 *   This function is used to set the network type 
 *
 *  @param   nwtype  	 Network type value, 0 for IBSS , 1 for INFRASTRUCTURE 
 *                       and 2 for IBSS_SEC.
 *  @param   ibss_type   IBSS type, 0 for joiner and 1 for creator
 *  @param   channel_num channel number in which IBSS network is to be created, 
 *                       O in case of ibss joiner.
 *  @retval  0 for success 0xff for failure  
 */
int16
rsi_setNetworkType (uint8 nwType, uint16 ibss_type, uint16 channel_num)
{
  struct rsi_nwType_s nw;
  nw.nwType = nwType;
  nw.ibss_type = ibss_type;
  nw.channel_num = channel_num;

  return rsi_send_at_command (NETWORK_TYPE_SEL, &nw);
}

/*! @brief Sets the preshared key 
 *   This function is used to set the preshared key
 *
 *  @param   psk         A character pointer to pre shared key.
 *  @retval  0 for success 0xff for failure  
 */

int16
rsi_set_psk (uint8 * psk)
{
  return rsi_send_at_command (PRE_SHARED_KEY, psk);
}

/*! @brief Sets the authentication mode 
 *   This function is used to set the authentication mode 
 *
 *  @param   auth_mode   auth_mode value, 
 *                       0 for Open authentication and 1 for Shared authentication.
 *  @retval  0 for success 0xff for failure  
 */

int16
rsi_set_auth_mode (uint8 auth_mode)
{
  return rsi_send_at_command (AUTH_MODE_SEL, &auth_mode);
}

/*! @brief Resets the RS2X module. 
 *         
 *  This command acts as a software reset to the module. 
 *  The module will reset all information regarding the WLAN connection 
 *  and IP configuration after receiving this command. 
 *  The baud rate is the only setting which is not changed. 
 *  The Host has to start from the rsi_band command after issuing this command
 *  @retval  0 for success 0xff for failure  
 */ 
int16
rsi_soft_reset(void)
{
  return rsi_send_at_command (SOFT_RESET, NULL);
}

/*! @brief Configures new baud rate
 *         
 *  This command is used to configure the baud rate of the UART interface 
 *  from the one detected during the ABRD process. 
 *  The module will use the new baud rate from the next command. 
 *  The response to this command will be at the same baud rate at 
 *  which the command is issued.
 *  @param   baudrate Ex: 9600
 *  @retval  0 for success 0xff for failure  
 */
int16
rsi_baudrate(uint32 baudrate)
{
  return rsi_send_at_command (BAUD_RATE, &baudrate);
}

/*! @brief Sets DNS server IP address
 *  This command is used to configure the DNS server's IP address 
 *  when the DNS is used in DHCP disable mode. 
 *  This command should be used before DNS query command. 
 *  This command should be sent before sending rsi_ipparam_set command.
 *  @param   dns_server_ip DNS server IP as ASCII string
 *  @retval  0 for success 0xff for failure  
 */
int16
rsi_set_dns_server(uint8 *dns_server_ip)
{
  return rsi_send_at_command (DNS_SERVER, dns_server_ip);
}

/*! @brief Resolves IP of a DNS domain.
 *   This command is used to send the domain name of a website to 
 *   the module to resolve the IP address. 
 *   The domain name should be maximum of 134 bytes.
 *  @param   domain_name Domain name 
 *  @retval  0 for success 0xff for failure  
 */
int16
rsi_set_dns_get(uint8 *domain_name)
{
  return rsi_send_at_command (DNS_GET, domain_name);
}

/*! @brief Saves configuration
 *  This command is used to save the parameters of an access point to internal memory. 
 *  This command should be given after rsi_scan, rsi_set_psk, rsi_join and 
 *  rsi_ipparam_set commands are given to save the configuration of the access point 
 *  to which the module joined. This will enable the module to join immediately to 
 *  this access point the next time the module powers up, 
 *  provided the automatic join feature is enabled by the command “rsi_cfg_enable”.
 * 
 *  @retval  0 for success 0xff for failure  
 */
int16
rsi_cfg_save(void)
{
  return rsi_send_at_command (CFG_SAVE, NULL);
}

/*! @brief Enables store configuration feature
 *  This command is used to enable or disable the feature of 
 *  automatic joining to a pre-configured Access Point on power up.
 * 
 *  @param   enable
 *           1-Enable, 0-Disable
 *  @retval  0 for success 0xff for failure 
 */
int16
rsi_cfg_enable(uint8 enable)
{
  return rsi_send_at_command (CFG_ENABLE, &enable);
}

/*! @brief Gets stored configuration information
 *  This command is used to get the configuration values that have been 
 *  stored in the module's memory. The parameters returned in this command 
 *  are the parameters used in automatically joining to a pre-configured network 
 *  when the module boots up in the auto-join mode.
 *  @retval  0 for success 0xff for failure 
 */
int16
rsi_cfg_get(void)
{
  return rsi_send_at_command (CFG_GET, NULL);
}

#if 0
/*=============================================================================*/
/**
 * @fn			char * bytes4ToAsciiDotAddr(uint8 *hexAddr)
 * @brief		Convert an uint8 4-Byte array to  . notation network address
 * @param[in]		uint8 *hexAddress, Address to convert
 * @param[out]		none
 * @return		char * destString, pointer to the string with the data
 */
int8 * bytes4ToAsciiDotAddr(uint8 *hexAddr,uint8 *strBuf)
{
  uint8			i;															// loop counter
  uint8			ii;															// loop counter
  int8			cBuf[4];													// character buffer
  int8			*destString;

  strBuf[0] = 0;																// make  strcat think the array is empty
  for (i = 0; i < 3; i++) {													// we are assuming IPV4, so 4 bytes
    // This will take care of the first 3 bytes
    for(ii = 0; ii < sizeof(cBuf); ii++) { cBuf[ii] = 0; }					// zero out the character buffer since we don't know how long the string will be
    sprintf ((char *)cBuf, "%d", (unsigned int)(((uint8*)hexAddr)[i]));
    destString =(int8 *) strcat((char *)strBuf,(char *) cBuf);
    destString = (int8 *)strcat((char *)strBuf,(char *) ".");
  }
  //take care of the last entry outside the loop, there is no . after the last octet
  for(ii = 0; ii < sizeof(cBuf); ii++) { cBuf[ii] = 0; }					// zero out the character buffer since we don't know how long the string will be
  sprintf ((char *)cBuf, "%d", (unsigned int)(((uint8*)hexAddr)[i]));
  destString = (int8 *) strcat((char *)strBuf,(char *) cBuf);
  return destString;
}
#endif
/* EOF */
