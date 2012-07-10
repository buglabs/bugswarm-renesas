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
 * @brief HEADER, INIT STRUCT, function prototypes & default init values
 *
 */  
  
#ifndef _RSI_GLOBAL_H
#define _RSI_GLOBAL_H
/*==============================================*/ 
/**
 * Global Defines
 */ 
#include "rsi_uart_api.h"
  
/* Band Defines */ 
#define BAND_2P5GHZ   0x00
#define BAND_5GHZ     0x01
  
/* TCPIP Defines */ 
#define DHCP_MODE_YES  0x01
#define DHCP_MODE_NO   0x00
  
/* SOCKET Defines */ 
#define SOCKET_TCP_CLIENT  0x0000
#define SOCKET_UDP_CLIENT  0x0001
#define SOCKET_TCP_SERVER  0x0002
/* SECURITY Type Defines */ 
#define SECURITY_OPEN  0
#define SECURITY_WPA1  1
#define SECURITY_WPA2  2
#define SECURITY_WEP   3
  
/* NETWORK Type */ 
#define IBSS_ADHOC_MODE       0
#define INFRASTRUCTURE_MODE   1
#define IBSS_ADHOC_SEC_MODE   2
#define IBSS_JOINER           0
#define IBSS_CREATOR          1
  
/* DATA Rates */ 
#define DATA_RATE_AUTO        0
#define DATA_RATE_1           1
#define DATA_RATE_2           2
#define DATA_RATE_5P5         3
#define DATA_RATE_11          4
#define DATA_RATE_6           5
#define DTAT_RATE_9           6
#define DATA_RATE_12           7
#define DATA_RATE_18           8
#define DATA_RATE_24           9
#define DATA_RATE_36           10
#define DATA_RATE_48           11
#define DATA_RATE_54           12
#define DATA_RATE_MCS0        13
#define DATA_RATE_MCS1        14
#define DATA_RATE_MCS2        15
#define DATA_RATE_MCS3        16
#define DATA_RATE_MCS4        17
#define DATA_RATE_MCS5        18
#define DATA_RATE_MCS6        19
#define DATA_RATE_MCS7        20
/* POWER Levels */ 
#define POWER_LEVEL_LOW       0
#define POWER_LEVEL_MEDIUM    1
#define POWER_LEVEL_HIGH      2
  
#define MACADDRLEN            6


//************************************** RSI **********************************************************//
  
/* RF parameters */ 
#define BAND  BAND_2P5GHZ

/* Module IP Parameters */
#define DHCP_MODE             DHCP_MODE_YES         /* DHCP_MODE_NO or DHCP_MODE_YES */

#define	MODULE_IP_ADDRESS     "192.168.11.250"       /* IP Address of the WiFi Module */

#define GATEWAY               "192.168.11.230"         /* Default Gateway */

#define NETMASK               "255.255.0.0"       /* Netmask */

/* Remote IP PC IP address to which WiFi module comminicats */
#define REMOTE_IP             "192.168.0.60"       /* IP Address we will connect to */

#define NUM_AP_SCAN           3                     /*Number of APs returned from WiFi 
                                                      in scan list for each iteration */

/*security info */
  
#define SECURITY_TYPE         SECURITY_WPA2	

#define PSK                   "renesaspsk"

#define AUTH_MODE             0                     /* 0- OPEN , 1 - SHARED */
  
/*scan info */
  
/* SCAN_SSID Macro must be enabled to scan an hidden SSID */ 
#define SCAN_SSID             "renesas" 

#define SCAN_CHANNEL          0                     /* 0 scans all channels */
  
/*join info */
#define JOIN_SSID 	      "renesas"              /* SSID We will join to */

#define DATA_RATE             DATA_RATE_5P5

#define POWER_LEVEL           POWER_LEVEL_HIGH

/* Set the mac address of the module to this value */
#define MAC_ADDRESS           "00:01:02:03:04:05" 


  
/* SOCKET_UDP_CLIENT or SOCKET_TCP_CLIENT or SOCKET_TCP_SERVER */
#define SOCKET_TYPE           SOCKET_TCP_CLIENT	

#define LOCAL_PORT            14046                 /* Module Socket/Port Number */

#define REMOTE_PORT           14046                 /* Socket/Port Number we will connect to */


/* Network Type */
/* INFRASTRUCTURE_MODE or IBSS_ADHOC_MODE or IBSS_ADHOC_SEC_MODE */
#define NETWORK_TYPE          INFRASTRUCTURE_MODE 

/* Only valid if in IBSS (AdHoc) mode IBSS_JOINER  or IBSS_CREATOR */
#define IBSS_MODE             IBSS_JOINER 

/* if IBSS creator, then set channel, 2.5GHz channels are 1-14 */
#define IBSS_CHANNEL          6	

/* Misc Parameters */
/* 0=Default SPI CS, 1=Alternate SPI CS, others possible in the future */
#define SPI_CHANNEL           SPISSWIFI 

/* 0=Power Saving Disabled, 1=RF+Baseband+Control disabled, 2=RF+Baseband disabled */
#define POWER_MODE            POWER_MODE_0 

/**Application specific structure
 * The struct is initialized by a set of #defines for default values
 * or for simple use cases
 */ 
typedef struct 
{
  uint8 band;                  /* uint8, frequency band to use */
  /* uint16, power mode, 0=power mode0, 1=power mode 1, 2=power mode 2 */
  uint8 powerMode;             
  rsi_scanFrameSnd_t ScanFrame;     /* frame sent as the scan command */
  rsi_joinFrameSnd_t JoinFrame;     /* frame sent as the join command */
  rsi_ipparamFrameSnd_t IPparamFrame;       /* frame sent to set the IP parameters */
  rsi_socketFrame_t SocketFrame;
} api;



enum app_cmd_e
{
  CMD_BAND_SEL = 0, CMD_INIT, CMD_QRY_FWVERSION,CMD_SCAN_NUM, CMD_SCAN, CMD_QRY_NUMSCAN,CMD_SCAN_NEXT, CMD_NETWORK_TYPE_SEL, CMD_AUTH_MODE_SEL, 
  CMD_PRE_SHARED_KEY, CMD_JOIN, CMD_IP_CONFIG, 
  CMD_SCAN_PASSIVE, CMD_DISCONNECT,CMD_POWER_MODE_SEL, CMD_SLEEP_TIMER, 
  CMD_TCP_CONNECT_REQ, CMD_OPEN_LUDP, CMD_OPEN_UDP, CMD_OPEN_LTCP, CMD_TCP_CSTATUS, CMD_CLOSE_SOCKET,
  CMD_TX_DATA, CMD_MAC_REQ,  CMD_QRY_RSSI, CMD_QRY_NET_PARAMS, CMD_QRY_BSSID, 
  CMD_QRY_NWTYPE
};


int16  rsi_wifi_init(uint8 band, rsi_scanFrameSnd_t *sf, 
                     rsi_joinFrameSnd_t *jf, rsi_ipparamFrameSnd_t *ip_f);

/**
 * Global Variables
 */ 
extern api strApi;

#define DECLARE_AND_INIT_GLOBAL_STRUCT(type, id) type id =  \
              {  BAND, POWER_MODE,  \
                {SCAN_CHANNEL, SCAN_SSID},  \
                {NETWORK_TYPE,SECURITY_TYPE, DATA_RATE,  POWER_LEVEL,  \
                 PSK, JOIN_SSID, IBSS_MODE, IBSS_CHANNEL,  AUTH_MODE }, \
                {DHCP_MODE, MODULE_IP_ADDRESS, NETMASK, GATEWAY }, \
                { 0,REMOTE_IP, REMOTE_PORT, LOCAL_PORT,NULL, 0} \
              } 


/**
 * Includes
 */ 
#endif	
