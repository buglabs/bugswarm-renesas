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

rsi_socketFrame_t      pd_dltcp_socket;
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
         
     
      struct rsi_socketFrame_s *dltcp_sock = &pd_dltcp_socket;    

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
      
      /* Initialize TCP server socket port, it is listening socket in WiFi module */
      dltcp_sock->lport = 14046;
      
      /* Tcp Server socket handle */
      dltcp_sock->handle = 0; 
      
      /* Open  Server TCP Socket */
      if( rsi_socket_ltcp_open (dltcp_sock) != RSI_NOERROR )
      {
          return;
      }
      
      /* wait for response from WiFi module */
      do {
          rsi_delayMs(2);
          status = rsi_read_cmd_rsp(&dltcp_sock->handle);
      }while ((status != RSI_NOERROR));
        
       
      /* wait for Remote TCP connection */
      do
      {
          status = rsi_query_ltcp_status (dltcp_sock->handle);
          if (status == RSI_NOERROR)
          {
              rsi_delayMs(10);
              /* read message from WiFi module */
              status = rsi_read_cmd_rsp(&lib_rspBuf);
              if((status == RSI_NOERROR) && (lib_rspBuf.SocketLtcpQueryRsp.socketHandle != -1))
              {
                break;
              }
          }
      }while (1);
      
      socket0 = dltcp_sock->handle;
      /* Initialize TCP server socket buffer with Transmission buffers */    

      while(1)
      {       	
	      if(((loopCount%1000) == 0))
              {
                  /* Read Temperature and Light sensor values */
                  send_sensor_reading(dltcp_sock);		  
	      }		
              
              retryCount = 0;
              do
              {
		 /* Read if any receive data on sockets */
		  retval = rsi_read_data ((uint8*)&lib_rspBuf, &app_event); 
		  retryCount++;
              } while ( (retval == RSI_ERROR_NO_RX_PENDING) && (retryCount < 5));
              
              /* Process the received message from WiFi module */
              if (retval == RSI_NOERROR)
              {
                  switch (app_event)
		  {
		    case RSI_EVENT_RX_DATA:		         
		         /*If throughput or wifi_config information is received*/
                         processdata(&strApi,NULL, dltcp_sock,NULL);                         						
		         break;		  
		    case RSI_EVENT_SOCKET_CLOSE:
			 if(lib_rspBuf.socketHandle == sock_desc[2])
			 {
			     rsi_socket_close(sock_desc[2]);
                             sock_desc[2]= 0;
			     //socket_not_creatted =0;
                         }
                         else if(lib_rspBuf.socketHandle == socket0)
			 {
                            /* Put LEDs in off state */
                             LED1 = TRUE;
                             LED2 = TRUE;
                             LED3 = TRUE;
                             /* Re-open TCP server sonnection */
			     socket0 = 0;                            
                             /* Open  Server TCP Socket */
                             if( rsi_socket_ltcp_open (dltcp_sock) != RSI_NOERROR )
                             {
                                return;
                             }
                            /* wait for response from WiFi module */
                            do {
                                 rsi_delayMs(2);
                                 status = rsi_read_cmd_rsp(&dltcp_sock->handle);
                            }while ((status != RSI_NOERROR));
        
       
                            /* wait for Remote TCP connection */
                            do
                            {
                              status = rsi_query_ltcp_status (dltcp_sock->handle);
                               if (status == RSI_NOERROR)
                                {
                                    rsi_delayMs(10);
                                    status = rsi_read_cmd_rsp(&lib_rspBuf);
                                    if((status == RSI_NOERROR) && (lib_rspBuf.SocketLtcpQueryRsp.socketHandle != -1))
                                    {
                                      break;
                                    }
                                }
                            }while (1);
      
                           socket0 = dltcp_sock->handle;
                         }
                         break;
                    case RSI_EVENT_SLEEP:						
			 break;								
		    case RSI_EVENT_CMD_RESPONSE:
			 break;		 
		    default:
			 break;
                  }
              }
              /* reset WiFi module event flag */
              app_event = 0;
              loopCount++;
              if(loopCount == 500000)
              {
                loopCount = 0;
              }
      }
           
      /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

