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
 * @brief AT command processer header file
 * 
 */  
#ifndef HOST_APP_H
#define HOST_APP_H


/*! @brief  Converts 1 digit integer to ascii */
#define ITOA_8(v)  ( (v) + '0')

/*! @brief  Assigns a byte to the command */
#define PUT_PARAM_8(m, val)  *m++ = (val) 


/*! @brief  Put string ; used in prepration of  at command */
#define PUT_STR(m, str)  do {  \
	                      uint16 tmp_len; \
	                      tmp_len = strlen((char *)str);\
	                      memcpy(m, str, tmp_len);\
                              m += tmp_len;\
		            } while(0)

/*! @brief  AT command tail size \r\n  */
#define AT_MSG_TAIL_SZ  2 /* \r\n */


/*! @brief  Max sockets that can be accomidated in the library */
#define MAX_SOCKET_ID 20

/*! @brief  Used in library for UDP protocol */
#define LIB_PROTO_UDP 1
/*! @brief  Used in library for TCP protocol */
#define LIB_PROTO_TCP 2


#define LIB_PROTO_CLOSED 0


/*! @brief  Enum to represent the AT commands */
enum rsi_at_cmd_e 
{
  BAND_SEL = 0, INIT, SCAN_PASSIVE, SCAN, SCAN_NEXT,SCAN_NUM, NETWORK_TYPE_SEL, 
  AUTH_MODE_SEL, PRE_SHARED_KEY, JOIN, DISCONNECT,POWER_MODE_SEL, SLEEP_TIMER, IP_CONFIG, 
  TCP_CONNECT_REQ, OPEN_LUDP, OPEN_UDP, OPEN_LTCP, TCP_CSTATUS, CLOSE_SOCKET,
  TX_DATA, FWVERSION, MAC_REQ,  QRY_RSSI, QRY_NET_PARAMS, QRY_BSSID, 
  QRY_NWTYPE, QRY_NUMSCAN, SOFT_RESET, BAUD_RATE, OPEN_MULTICAST, DNS_SERVER, DNS_GET,
  CFG_SAVE, CFG_ENABLE, CFG_GET
};

/*! @brief  MAX AT commands supported */
#define MAX_AT_COMMANDS  CFG_GET 

/*! @brief  Enum to represent the AT command  sent states */
enum rsi_at_cmd_state_e {
	BAND_SEL_SENT = 0, INIT_SENT, SCAN_PASSIVE_SENT, SCAN_SENT, SCAN_NEXT_SENT, 
	SCAN_NUM_SENT,NETWORK_TYPE_SENT, AUTH_MODE_SEL_SENT,PRE_SHARED_KEY_SENT, 
	JOIN_SENT, DISCONNECT_SENT, POWER_MODE_SENT, SLEEP_TIMER_SENT, IP_CONFIG_SENT, 
	TCP_CONNECT_REQ_SENT, OPEN_LUDP_SENT, OPEN_UDP_SENT, 
	OPEN_LTCP_SENT, TCP_CSTATUS_SENT, CLOSE_SOCKET_SENT, TX_DATA_SENT, 
	FWVERSION_SENT, MAC_REQ_SENT, QRY_RSSI_SENT, QRY_NET_PARAMS_SENT, 
	QRY_BSSID_SENT, QRY_NWTYPE_SENT, QRY_NUMSCAN_SENT, SOFT_RESET_SENT, BAUD_RATE_SENT, 
        OPEN_MULTICAST_SENT, DNS_SERVER_SENT, DNS_GET_SENT, 
        CFG_SAVE_SENT, CFG_ENABLE_SENT, CFG_GET_SENT
};

/*! @brief  AT command constantstrings  */
#define  BAND_STR                 "band=" 
#define  INIT_STR                 "init"
#define  SCAN_PASSIVE_STR         "passscan="
#define  SCAN_STR                 "scan="
#define  SCAN_NEXT_STR            "nextscan"
#define  SCAN_NUM_STR	          "numscan="
#define  NETWORK_TYPE_STR	  "network="
#define  AUTH_MODE_STR            "authmode="
#define  PRE_SHARED_KEY_STR       "psk="
#define  JOIN_STR                 "join=" 
#define  DISCONNECT_STR	          "disassoc"
#define  POWER_MODE_STR           "pwmode="
#define  SLEEP_TIMER_STR          "sleeptimer="
#define  IP_CONFIG_STR 	          "ipconf="
#define  TCP_CONNECT_REQ_STR      "tcp="
#define  OPEN_LUDP_STR            "ludp="
#define  OPEN_UDP_STR             "udp="
#define  OPEN_LTCP_STR            "ltcp="
#define  TCP_CSTATUS_STR          "ctcp=" 
#define  CLOSE_SOCKET_STR         "cls="
#define  TX_DATA_STR              "snd="

#define  FW_VERSION_STR           "fwversion?"
#define  MAC_STR                  "mac?"
#define  QRY_RSSI_STR             "rssi?"
#define  QRY_NET_PARAMS_STR       "nwparams?"
#define  QRY_BSSID_STR            "bssid?"
#define  QRY_NWTYPE_STR           "nwtype?"
#define  QRY_NUMSCAN_STR          "numscan?"

#define  SOFT_RESET_STR           "reset"
#define  BAUD_RATE_STR            "baudrate="
#define  OPEN_MULTICAST_STR       "multicast="
#define  DNS_SERVER_STR           "dnsserver="
#define  DNS_GET_STR              "dnsget="

#define  CFG_SAVE_STR             "cfgsave"
#define  CFG_ENABLE_STR           "cfgenable="
#define  CFG_GET_STR              "cfgget?"


/*! @brief  Security types */
#define SEC_TYPE_OPEN  0
#define SEC_TYPE_WPA   1
#define SEC_TYPE_WPA2  2

/*! @brief  Used to parse scan response */
#define SCAN_RSP_RECORD_SZ  10 //34
#define SSID_SZ 32


/*! \brief Structure for the driver's control block */
struct rsi_host_app_cb_s 
{
  uint8 state;
#define MAX_AT_CMD_SIZE 60 //100
  uint8 at_cmd[MAX_AT_CMD_SIZE];
};

struct rsi_recv_s
{
  uint8 *rx_buf;
  uint16 rx_buf_len;
  uint16 rx_buf_max_size;
  uint8  pending;
  struct rsi_recv_s *next;
};

/*! @brief  Internal function prototypes */
int16 rsi_send_at_command(uint16 cmd_type, void *args);
int16 rsi_send_cmd_to_device(uint8 *tx_buf, uint16 cmd_len);
int16 rsi_process_cmd_resp (uint8 *rsp, uint16 len, uint8 *drsp);
int16 rsi_process_at_cmd(rsi_recvSocketFrame_t *rcv, uint16 timeout);
void  rsi_update_read_buf(void);
void  rsi_process_recv_data(uint8 *at_cmd, void *rsp);
void  rsi_process_raw_data(uint8 *at_cmd, void *rsp);

#endif 

/* EOF */

