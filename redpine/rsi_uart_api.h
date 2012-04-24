/**
 * @file
 *
 * Copyright(C) 2011 Redpine Signals Inc.
 * All rights reserved by Redpine Signals.
 *
 * @section License
 *
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief UART API, uart specific constants and values
 *
 * @section Description
 * This file contains the function definition prototypes, as well as the structures and global
 * defines wich are used in the new API which will be
 * used to communicate with the Redpine Signals WiFi modules.
 *
 *
 */

#ifndef _UARTAPI_H_
#define _UARTAPI_H_


#include "rsi_config.h"
/**
 * Include Files
 */

/**
 * Include files
 */

//#define DPRINT(lvl, fmt, args...)  if (lvl & DBG_LVL) printf(fmt, ##args)
#define DPRINT  printf


#define RSI_NOERROR   0
#define RSI_ERROR    -1

#define DBG_LVL 0xFFFF
/**
 * Global Defines
 */
#define ABRD_RETRY_COUNT				10

/**
 *  Error Codes
 *   -1  Waiting for the connection from peer
 *   -2  Unable to allocate socket 
 *   -3  Deauthentication from the Access Point
 *   -4  Illegal IP/Port Parameter
 *   -5  TCP/IP Configuration Failure
 *   -6  Invalid Socket
 *   -7  Association not done
 *   -8  Error in command
 *   -9  Error with byte stuffing for escape characters
 *  -10  IP Lease expired
 *  -11  TCP Connection Closed
 *  -16  DHCP Failure
 * -124  Connection establishment not supported
 * -127	 Socket already exists
 */
/*
 * -12 Pre-Shared Key not sent for connection to a secure access point
 * -13 No Access Points Scanned
 * -14 INIT command already issued. 
 * -15 JOIN command already issued. 
 * -17 Baud Rate Not Supported
 * -18 Encryption mode not supported
 */


/**
 * Defines - top level
 */
#define MAX_STREAM_SIZE      200 //256
#define UART_BUFFER_LEN      MAX_STREAM_SIZE+22 /* size of send/receive buffer */
#define MAX_SLEEPTIME        10000
#define MAC_ADDR_RSP_LEN     10
#define MAX_AP_SCANNED       10

#define NWTYPE_INFRASTRUCTURE          1
#define NWTYPE_IBSS                    0
#define NWTYPE_IBSS_SEC                2

#define LIB_TRUE    1
#define LIB_FALSE   0

/* Error Return */
#define LIB_NOERROR 	0
#define LIB_ERROR      -1

#define RSI_ERROR_NO_RX_PENDING       0x1
#define RSI_ERROR_NO_CMD_RSP_PENDING  0x2
#define RSI_ERROR_JUNK_PKT_RECVD      0x3


/*=================================================*/

#define MAXSOCKETS  4 /* maximum number of open sockets */

// Power Mode Constants
#define POWER_MODE_0  0x0000
#define POWER_MODE_1  0x0001
#define POWER_MODE_2  0x0002

#define IP_ADDR_STR_LEN 15

#define UART_EVENT_RX_DATA       0x0a
#define UART_EVENT_SOCKET_CLOSE  0x0b
#define UART_EVENT_SLEEP         0x0c

#define SSID_LEN 32
#define PSK_LEN  32
#define AP_SCANNED_MAX 10

/**
 * Recv Data Structure
 */
typedef struct rsi_recvSocketFrame_s
{
  /* Rx event type :UART_EVENT_RX_DATA, UART_EVENT_SOCKET_CLOSE, UART_EVENT_SLEEP  */
  /*Library will assign the address of the buffer where the rx packet is filled up,
    this buffer may be overwritten with the new incoming packet. In order to avoid this
	application has to process the received buffer quickly or sufficient no.of rx buffers
	should be supplied to the library during initialisation stage using rsi_set_rx_buffer API*/
  uint8   *buffer;

  /* Info to be filled by the library from received rx packet */
  uint16  buf_len;
  uint8   handle;
  uint8   remote_ip[4];
  uint16  rport;
} rsi_recvSocketFrame_t;

/* Response for rsi_ipparam_set command */
struct rsi_IpparamRsp_s 
{
  uint8  macAddr[6];	/* uint8[6], hex, MAC Address */
  uint8  ipaddr[4];   /* uint8[4], hex, IP Address */
  uint8  subnetMask[4]; /* uint8[4], hex, Subnet Mask */
  uint8  gatewayAddr[4];/* uint8[4], hex, Gateway Address */
};	

/* Response for rsi_uart_query_ltcp_status command */
struct rsi_SocketLtcpQueryRsp_s
{
  int8 socketHandle; /* uint8, hex, socket handle */
  uint8 ipAddr[4];  /* uint8[4], hex, IP Address */
  uint8	port[2]; /*uint8[2], hex, port the socket is bound to */
};	
/* Response for rsi_query_net_parms comamnd */
struct rsi_QueryNetworkParmsRsp_s 
{
  uint8	ssid[32];
  uint8	securityType; /* 0=Open, 1=WPA, 2=WPA2, 3=WEP */
  uint8	psk[32];  /* ascii, pre-shared key*/
  uint8	channel;  /* channel number */
  uint8	macAddr[6]; /*Hex mac address of the module */
  uint8	dhcpMode;/*  ascii, '0'=Manual, '1'=DHCP,  */
  uint8	ipAddr[4];/* hex, IP Address of the module */
  uint8	subnetMask[4]; /*hex, Subnet Mask of the module */
  uint8	gatewayAddr[4];/*hex, Gateway Address of the module*/
  uint8	nOpenSockets; /* hex, Number of open  sockets, range from 0 to 8 */
  };

/* Response for rsi_query_bssid command */
/**
 * Query BSSID
 */
  struct rsi_queryBssidRsp_s
  {
    uint8  ssid[32];
    uint8  bssid[6];
  };

/*  Response for rsi_query_nwtype command */
  /**
   * Query NwType 
   */
  struct rsi_queryNwTypeRsp_s
  {
    uint8  ssid[32];
    uint8  nwType;
  };

/* Response for rsi_query_fwversion command */
/** 
  * Query FW Version 
  */
  struct rsi_queryFWVersionRsp_s
  {
    uint8 fwVersion[10];
  };
/* Response for rsi_query_mac_addr command */
  /** 
   * Query MAC Address
   */
  struct rsi_queryMACRsp_s
  {
    uint8 mac_addr[10];
  };
/* Response for rsi_scan, rsi_scan_next commands */
struct rsi_scanInfo_s
{
   uint8  ssid[SSID_LEN]; /* ssid of scanned access point */
   /* security mode, 0=open, 1=wpa1, 2=wpa2, 3=wep */
    uint8  securityMode; 
    uint8  rssiVal;	/* absolute value of RSSI */
}; /* AP_SCANNED_MAX is 15 */

  /* DNS get */
struct rsi_dns_info_s
{
    uint8  ip_address_count; /* Number of IP address */
#define MAX_DNS_IP_ADDRESSES 10
    uint8  ip_addr[MAX_DNS_IP_ADDRESSES][4];
};

/* CFG GET */
struct rsi_cfg_info_s
{
    uint16 valid_flag;
    uint8  channel_num;
    uint8  nw_type;
    uint8  sec_mode;
    uint8  data_rate;
    uint8  powerLevel;
    uint8  psk[SSID_LEN];
    uint8  ssid[PSK_LEN];
    uint8  reserved[3];
    uint8  dhcp_enable;
    uint8  ip_address[4];
    uint8  subnetMask[4];
    uint8  gateway[4]; /* Default gateway */
};
typedef union 
{
  struct rsi_IpparamRsp_s IpparamRsp;
  struct rsi_SocketLtcpQueryRsp_s SocketLtcpQueryRsp;
  /* Response for all socket open APIs */
  uint8 socketHandle; /* uint8, hex, socket handle */
  /* Response for RSSI Query */
  uint8 rssi_val;
  uint8 scanned_AP_count;
  struct rsi_QueryNetworkParmsRsp_s QueryNetworkParmsRsp;
  struct rsi_queryBssidRsp_s queryBssidRsp[AP_SCANNED_MAX];
  struct rsi_queryNwTypeRsp_s queryNwTypeRsp[AP_SCANNED_MAX];
  struct rsi_queryFWVersionRsp_s queryFWVersionRsp;
  struct rsi_queryMACRsp_s queryMACRsp;
  struct rsi_scanInfo_s scanInfo[AP_SCANNED_MAX];
  rsi_recvSocketFrame_t  rcv_data; 
  struct rsi_dns_info_s dns_info;
  struct rsi_cfg_info_s cfg_info;
}rsi_uUartRsp;

/**
 * Scan Structures
 */

/* The scan command argument variables */
typedef struct rsi_scanFrameSnd_s
{
  uint16 channel;/* RF channel to scan, 0=All, 1-14 for 2.5GHz channels 1-14 */
  uint8  ssid[SSID_LEN]; /* uint8[32], ssid to scan for */
} rsi_scanFrameSnd_t;

typedef struct rsi_nwType_s
{
  uint8 nwType;
  uint8 ibss_type;
  uint8 channel_num;
}rsi_nwType_t;

/*===============================================*/
/**
 * Join Data Frame Structure
 */
typedef struct rsi_joinFrameSnd_s
{
  uint8	nwType;	/* network type, 0=Ad-Hoc (IBSS), 1=Infrastructure */
  /* security type, 0=Open, 1=WPA1, 2=WPA2, 3=WEP */
  uint8	securityType;
  /* data rate, 0=auto, 1=1Mbps, 2=2Mbps, 3=5.5Mbps, 4=11Mbps, 12=54Mbps */
  uint8	dataRate;  
  /* xmit power level, 0=low (6-9dBm), 1=medium (10-14dBm, 2=high (15-17dBm) */
  uint8	powerLevel;
  uint8	psk[PSK_LEN];/* pre-shared key, 32-byte string */
  uint8	ssid[SSID_LEN];	/* ssid of access point to join to, 32-byte string */
  uint8	ibssMode;	/* Ad-Hoc Mode (IBSS), 0=Joiner, 1=Creator */
  uint8	ibssChannel;	/* rf channel number for Ad-Hoc (IBSS) mode */
  uint8 auth_mode;
} rsi_joinFrameSnd_t;



/**
 * TCP/IP Configure
 */
/* Command */
typedef struct rsi_ipparamFrameSnd_s 
{
  uint8	dhcpMode;/* 0=Manual, 1=Use DHCP */
  uint8	ipaddr[IP_ADDR_STR_LEN];/* IP address of this module if in manual mode */
  uint8	netmask[IP_ADDR_STR_LEN];/* Netmask used if in manual mode */
  uint8	gateway[IP_ADDR_STR_LEN];/* IP address of default gateway if in manual mode */
} rsi_ipparamFrameSnd_t;

/**
 * Socket Configure
 */
/* Commad */
typedef struct rsi_socketFrame_s 
{
  int8   handle;
  uint8	 remote_ip[IP_ADDR_STR_LEN];/* remote host IP address  */
  uint16 rport; /* Remote port */
  uint16 lport; /* Local port */
  uint8  *buf; /* Send buffer pointer */
  uint16 buf_len; /* send buffer length */
}rsi_socketFrame_t;

 
/**
 * Function Prototypes
 */
int16  rsi_set_auth_mode(uint8 auth_mode);
int16  rsi_autobaud(void);
int16  rsi_band(uint8 band);
int16  rsi_disconnect(void);
int16  rsi_init_baseband(void);
int16  rsi_ipparam_set(rsi_ipparamFrameSnd_t *ip_param);
int16  rsi_join(struct rsi_joinFrameSnd_s *jf);
int16  rsi_powermode(uint8 pwr_mode);
int16  rsi_query_fwversion(void);
int16  rsi_query_mac_addr(void);
int16  rsi_query_net_parms(void);
int16  rsi_query_rssi(void);
int16  rsi_query_mac_addr(void);
int16  rsi_query_nwtype(void);
int16  rsi_query_bssid (void);
int16  rsi_query_scan_num(void);
int16  rsi_scan(struct rsi_scanFrameSnd_s *scan_frame);
int16  rsi_scan_next(void);
int16  rsi_scan_num(uint8 num);
int16  rsi_scan_passive(uint16 value);
int16  rsi_setNetworkType(uint8 nwType, uint16 ibss_type, uint16 channel_num);
int16  rsi_send(rsi_socketFrame_t *sock);
int16  rsi_recv (rsi_uUartRsp *rcv);
int16  rsi_set_sleep_timer(uint16 sleep_time);
int16  rsi_socket_close(int16 sid);
int16  rsi_deepSleep(void);
void   rsi_init (void);
int16  rsi_boot_device(void);
int16  rsi_set_psk(uint8 *psk);
void   rsi_lib_init(void);
int16  rsi_socket_tcp_open(rsi_socketFrame_t *sf);
int16  rsi_socket_ltcp_open(rsi_socketFrame_t *sf);
int16  rsi_query_ltcp_status(uint8 handle);
int16  rsi_socket_udp_open(rsi_socketFrame_t *sf);
int16  rsi_socket_ludp_open(rsi_socketFrame_t *sf);
int16  rsi_socket_multicast_open(rsi_socketFrame_t * sf);
int16  rsi_send_ack(void);

int16 rsi_soft_reset(void);
int16 rsi_baudrate(uint32 baudrate);
int16 rsi_set_dns_server(uint8 *dns_server_ip);
int16 rsi_set_dns_get(uint8 *domain_name);
int16 rsi_cfg_save(void);
int16 rsi_cfg_enable(uint8 enable);
int16 rsi_cfg_get(void);

int16  rsi_read_cmd_rsp (void *rsp);
int16  rsi_read_data(void *rsp, uint16 *event);
void   rsi_set_rx_buffer (uint8 *buffer, uint16 buf_size);

int8 *bytes4ToAsciiDotAddr(uint8 *hexAddr, uint8 *strBuf);



/* @brief Debug Print Levels */
/* These bit values may be ored to all different combinations of debug printing */
#define PL0  0xffff
#define PL1  0x0001
#define PL2  0x0002
#define PL3  0x0004
#define PL4  0x0008
#define PL5  0x0010
#define PL6  0x0020
#define PL7  0x0040
#define PL8  0x0080
#define PL9  0x0100
#define PL10 0x0200
#define PL11 0x0400
#define PL12 0x0800
#define PL13 0x1000
#define PL14 0x2000
#define PL15 0x4000
#define PL16 0x8000



enum RSI_PROTOCOL {
 PROTOCOL_TCP  = 0x00,
 PROTOCOL_UDP  = 0x01,
 PROTOCOL_LTCP  = 0x02,
 PROTOCOL_MULTICAST = 0x03,
 PROTOCOL_LUDP	 = 0x04,
 PROTOCOL_UNDEFINED = 0x05
};

enum RSI_MODULE_MODE 
{
  MODE_SPI = LIB_FALSE,
  MODE_UART = LIB_TRUE
};


/* User interaction */
#define  RSI_EVENT_CMD_RESPONSE     0x01
#define  RSI_EVENT_RX_DATA          0x02
#define  RSI_EVENT_SOCKET_CLOSE     0x04
#define  RSI_EVENT_SLEEP            0x08

#endif


