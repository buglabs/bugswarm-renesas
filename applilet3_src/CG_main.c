/*
*******************************************************************************
* Copyright(C) 2011 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation. 
*
* This device driver was created by Applilet3 for RL78/G13
* 16-Bit Single-Chip Microcontrollers
* Filename:	CG_main.c
* Abstract:	This file implements main function.
* APIlib:	Applilet3 for RL78/G13 E1.00b [14 Jan 2011]
* Device:	R5F100LE
* Compiler:	IAR Systems iccrl78
* Creation date:	8/26/2011
*******************************************************************************
*/

/*
*******************************************************************************
** Include files
*******************************************************************************
*/
#include "CG_macrodriver.h"
#include "CG_system.h"
#include "CG_port.h"
#include "CG_int.h"
#include "CG_serial.h"
#include "CG_ad.h"
#include "CG_timer.h"
#include "CG_it.h"

/* Start user code for include. Do not edit comment generated here */
#include "rsi_data_types.h"
#include "rsi_uart_api.h"
#include "rsi_hal.h"
#include "rsi_global.h"
#include "rsi_hal_api.h"
#include "lcd.h"
#include "rsi_demo_app.h"

api strApi;
DECLARE_AND_INIT_GLOBAL_STRUCT(api, strApi);
#define LIB_RX_BUF_SIZE       100
#define LIB_NETWORK_HDR_LEN   50

rsi_socketFrame_t      insock_obj;
rsi_socketFrame_t      outsock_obj;
extern rsi_recvSocketFrame_t  pd_rcv;

extern  rsi_uUartRsp   lib_rspBuf;
extern uint8           calc_thruput;
uint8                  sock_desc[3]={0};
uint8 		       socket0 = 0;
uint8                  socket1 = 0;
uint8                  socket2 = 0;
uint8 lib_rx_buffer1[LIB_RX_BUF_SIZE+LIB_NETWORK_HDR_LEN]; 
uint8 lib_rx_buffer2[LIB_RX_BUF_SIZE+LIB_NETWORK_HDR_LEN];


#if 0 // Skip Applilet3 code here to set custom option bytes for LVD

/* End user code. Do not edit comment generated here */
#include "CG_userdefine.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/*
*******************************************************************************
**  Global define
*******************************************************************************
*/
/* Set option bytes */
#pragma location = "OPTBYTE"
__root const UCHAR opbyte0 = 0xEFU;
#pragma location = "OPTBYTE"
__root const UCHAR opbyte1 = 0x73U;
#pragma location = "OPTBYTE"
__root const UCHAR opbyte2 = 0xE8U;
#pragma location = "OPTBYTE"
__root const UCHAR opbyte3 = 0x04U;

/* Set security ID */
#pragma location = "SECUID"
__root const UCHAR secuid[10] = 
	{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};
/* Start user code for global. Do not edit comment generated here */
#else

/* Set option bytes */
#pragma location = "OPTBYTE"
__root const UCHAR opbyte0 = 0xEFU;
#pragma location = "OPTBYTE"
__root const UCHAR opbyte1 = 0x35U;
#pragma location = "OPTBYTE"
__root const UCHAR opbyte2 = 0xE8U;   
#pragma location = "OPTBYTE"
__root const UCHAR opbyte3 = 0x84U;

/* Set security ID */
#pragma location = "SECUID"
__root const UCHAR secuid[10] = 
	{0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};

#endif
/* End user code. Do not edit comment generated here */


/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function implements main function.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/

void  main(void)
{
   /* Start user code. Do not edit comment generated here */     
      int16     status = 0;
      uint32    loopCount = 0;
      int16     retryCount = 0;
      int       retval;
      uint16    app_event = 0;   
      char     tempbuff[30];
         
     
      struct rsi_socketFrame_s *insock = &insock_obj;
      struct rsi_socketFrame_s *outsock = &outsock_obj;

      /* Initialize UART and enable Module power and reset pins */  
      rsi_init();
     
      /* Register UART HAL buffers */
      rsi_set_rx_buffer(lib_rx_buffer1, (LIB_RX_BUF_SIZE + LIB_NETWORK_HDR_LEN));
      rsi_set_rx_buffer(lib_rx_buffer2, (LIB_RX_BUF_SIZE + LIB_NETWORK_HDR_LEN));
      
      /* Boot WLAN module */
      if(rsi_boot_device () != RSI_NOERROR)
      {
	  ;	
      }
      
      /* Initialize LCD module */
      LCDInit();
      /* Set LCD display Font to small */
      LCDFont(FONT_SMALL);      
      /* Initialize WLAN module, Scan APs, Associate (Join) with AP, configure IP Parameters */
      rsi_wifi_init (strApi.band, &strApi.ScanFrame, &strApi.JoinFrame, &strApi.IPparamFrame);
     
      /* Put LEDs in off state */
      LED1 = TRUE;
      LED2 = TRUE;
      LED3 = TRUE;
      
      /* Open listening socket first... */
      insock->lport = 80;
      insock->handle = 0; 
      
      /* Now open our outbound socket... */
      outsock->rport = 80;
      outsock->lport = 80;
      outsock->handle = 0; 
      /* Remote address is IP address for api.bugswarm.net 
      dltcp_sock->remote_ip[0] = 107U;
      dltcp_sock->remote_ip[1] = 20U;
      dltcp_sock->remote_ip[2] = 250U;
      dltcp_sock->remote_ip[3] = 52U; */
      outsock->remote_ip[0] = 0xC0;
      outsock->remote_ip[1] = 0xA8;
      outsock->remote_ip[2] = 0x01;
      outsock->remote_ip[3] = 0x1D;
      
      //TODO - We need to open a listening socket FIRST
      //these are apparently only unidirectional ports..
      LCDString("Open socket...", LCDRight(8)-55, 11); 
      
      if( rsi_socket_tcp_open (dltcp_sock) != RSI_NOERROR )
      {
          LCDString("Socketopen FAIL", LCDRight(8)-55, 21); 
          return;
      }
      do {
        status = rsi_read_cmd_rsp(&dltcp_sock->handle);
      } while (status == RSI_ERROR_NO_RX_PENDING);
      if (status != RSI_NOERROR){
          memset(tempbuff, '\0', sizeof(tempbuff));
          sprintf(tempbuff, "Socketopen %d", (signed int)status);
          LCDString(tempbuff, LCDRight(8)-55, 21); 
          return;
      }
      LCDString("Socketopen GOOD!", LCDRight(8)-55, 21); 
      dltcp_sock->buf = (uint8 *)"Hello world\n";
      dltcp_sock->buf_len = strlen(dltcp_sock->buf);
      rsi_send(dltcp_sock);
      return;
      /*while(1){
          retval = rsi_read_data ((uint8*)&lib_rspBuf, &app_event); 
          if (retval != RSI_ERROR_NO_RX_PENDING){
              if (app_event == RSI_EVENT_SOCKET_CLOSE){
                  LCDString("Socketevent CLOSE", LCDRight(8)-55, 31);
                  rsi_socket_close(dltcp_sock->handle);
                  //We should be able to retry here...
              }
              LCDString("Socketevent OTHER", LCDRight(8)-55, 31); 
          }
      }*/
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

