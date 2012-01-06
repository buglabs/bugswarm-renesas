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

/* temperatur sensor */
#define TEMP_ADDR	0x90
#define TEMP_REG	0x00
#define DEGREE_CHAR	186

/* Light Sensor */
#define LS_ADDR         0x72u
#define LS_INSTR        0x51

#define  POT_CNTRL            0

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


typedef struct thrput_calc_s
{
	uint16     rx_size_of_pkt;
	uint16     Ttime;
	uint32     no_of_pkts_rcvd;	
	uint32     no_of_bytes_rcvd;
	uint32     tx_no_ofpkts;
	uint32     rx_datasize;
	uint32     tx_datasize;
}thrput_calc_t;



#define REMOTE_UDP	0
#define REMOTE_TCP  1


  
void ReadIIC(unsigned char aAddr, unsigned char *aRXData, int aNumBytes);
void WriteIIC(uint8_t aAddr, uint8_t *aTXData, int aNumBytes);
void ReadIICReg(unsigned char aAddr, unsigned char aReg, unsigned char *aRXData, int aNumBytes);
void processdata(api *ptrStr,rsi_socketFrame_t *usock, rsi_socketFrame_t *ltsock,rsi_socketFrame_t *dltsock);
void send_sensor_reading(rsi_socketFrame_t *ltcp_sock);
void process_led_data(rsi_socketFrame_t *ltcp_sock);
void send_lightsensor_reading(rsi_socketFrame_t *ltcp_sock);
void send_pushbutton_status(uint8 button_no,rsi_socketFrame_t *ltcp_sock);
uint16 rsi_convert_ip_to_string(uint8 *num_buff, uint8 *ip_buff);

#endif	

