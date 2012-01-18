/********************************************************************************
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
#include "swarm.h"

/*edit the following swarm details: */
const char swarm_server_ip[] = "107.20.250.52";  //api.bugswarm.net
//const char swarm_server_ip = "64.118.81.28";  //test.api.bugswarm.net
const char swarm_id[] = "32c570d1d07756d92eb1a0e1cb3afbc60bfd5a94";
const char resource_id[] = "ea8da467c44d4e3f2327b9346f0869787aa8342a";
const char participation_key[] = "dd370eabf1fde6beeab83ec9c288e0abb4639654";

api strApi;
DECLARE_AND_INIT_GLOBAL_STRUCT(api, strApi);
#define LIB_RX_BUF_SIZE       100
#define LIB_NETWORK_HDR_LEN   50

rsi_socketFrame_t      insock_obj;
rsi_socketFrame_t      outsock_obj;

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
/* Accelerometer constants */
#define ACCEL_WADDR         0x3A
#define ACCEL_RADDR         0x3B
#define ACCEL_REG_PWRCTL    0x2D
#define ACCEL_REG_DATASTART 0x32
#define ACCEL_CTL_MEASURE   0x08
unsigned char i2cbuf[4];/* Buffer for I2C transactions */
signed int datax = 0;   /* Store values to modulate frequency */
signed int datay = 0;   /* Store values to modulate duty cycle */
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
void mydelay(long ms){
  for (long i=0;i<(20*ms);i++)
        asm("NOP");
}

void getAccelSample(){
  i2cbuf[0] = ACCEL_REG_PWRCTL;
  i2cbuf[1] = ACCEL_CTL_MEASURE;
  IICA0_MasterSendStart(ACCEL_WADDR, i2cbuf, 2, 32);
  while (IICBSY0);        /* Busy-wait until previous I2C TX is done */

  /* Inform accelerometer of starting address at which to read next */
  i2cbuf[0] = ACCEL_REG_DATASTART; 
  IICA0_MasterSendStart(ACCEL_WADDR, i2cbuf, 1, 32);
  while (IICBSY0);    /* Busy-wait until previous I2C TX is done */

  /* Read x and y data (2 bytes each) from the accelerometer */
  IICA0_MasterReceiveStart(ACCEL_RADDR, i2cbuf, 4, 32);
  while (IICBSY0);    /* Busy-wait until previous I2C RX is done */

  /* Convert the returned x and y data bytes into signed data */
  datax = ((signed int)i2cbuf[1] << 8) | i2cbuf[0];
  datay = ((signed int)i2cbuf[3] << 8) | i2cbuf[2];
}

void  main(void)
{
   /* Start user code. Do not edit comment generated here */     
      int16     status = 0;
      char tempbuff[50];
         
     
      struct rsi_socketFrame_s *insock = &insock_obj;    
      struct rsi_socketFrame_s *outsock = &outsock_obj;          
      
      /* Put LEDs in off state */
      LED1 = TRUE;
      LED2 = TRUE;
      LED3 = TRUE; 

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
      
      /* Create listening socket */
      insock->lport = 80;
      
      /* Create outbound socket... */
      outsock->rport = 80;
      outsock->lport = 80;
            
      /* EDIT the following IP address is the machine that this board will
         attempt to connect to... */
      //strcpy(outsock->remote_ip, "192.168.1.29"); 
      strcpy(outsock->remote_ip, swarm_server_ip);
      
      LED1 = FALSE;
      
      //OPEN listening socket
      if( rsi_socket_ltcp_open (insock) != RSI_NOERROR )
      {
          LCDString("insock FAIL", LCDRight(8)-55, 21); 
          return;
      }
      do {
        mydelay(10);
        status = rsi_read_cmd_rsp(&insock->handle);
      } while (status == RSI_ERROR_NO_RX_PENDING);
      if (status != RSI_NOERROR){
          memset(tempbuff, '\0', sizeof(tempbuff));
          sprintf(tempbuff, "insock %X", (signed int)status);
          LCDString(tempbuff, LCDRight(8)-55, 21); 
          return;
      }
      
      do {
        //OPEN outgoing socket, which references listening socket
        if( rsi_socket_tcp_open (outsock) != RSI_NOERROR )
        {
            LCDString("outsock FAIL", LCDRight(8)-55, 21); 
        }
        do {
          mydelay(10);
          status = rsi_read_cmd_rsp(&outsock->handle);
        } while (status == RSI_ERROR_NO_RX_PENDING);
        if (status != RSI_NOERROR){
            memset(tempbuff, '\0', sizeof(tempbuff));
            sprintf(tempbuff, "ERR #%X, retrying", (signed int)status);
            LCDString(tempbuff, LCDRight(8)-55, 21); 
            LED1 = TRUE;
            mydelay(1000);
            LED1 = FALSE;
        }
      } while (status != RSI_NOERROR);
      LCDString("Connected!", LCDRight(8)-55, 21);
      
      //Send swarm participation header
      swarm_send_produce(swarm_id, resource_id, participation_key, outsock);
      mydelay(500);
      capabilities_announce(outsock);
      mydelay(500);
      
      //Send some test data to the socket.
      while(1){
        int dataxRounded = abs(datax/270);
        int datayRounded = abs(datay/270);
        memset(tempbuff, '\0', sizeof(tempbuff));
        sprintf(tempbuff, "{\"Acceleration\":{\"x\":%c%d.%04d,\"y\":%c%d.%04d}}",(datax < 0)?'-':' ',dataxRounded, abs((int)((((long)datax*10000L)/270)-((long)dataxRounded*10000L))),(datay < 0)?'-':' ',datayRounded, abs((int)((((long)datay*10000L)/270)-((long)datayRounded*10000L))));
        //sprintf(tempbuff, "{\"HelloWorld\":%d}",++idx);
        /*outsock->buf = (uint8 *)tempbuff;
        outsock->buf_len = strlen((const char *)outsock->buf);
        rsi_send(outsock);*/
        swarm_produce(tempbuff,outsock);
        LED2 = !LED2;
        getAccelSample();
        LCDClearLine(4);
        LCDClearLine(5);
        LCDClearLine(6);
        LCDClearLine(7);
        memset(tempbuff, '\0', sizeof(tempbuff));
        sprintf(tempbuff, "X: %c%d.%04d",(datax < 0)?'-':' ', dataxRounded, abs((int)((((long)datax*10000L)/270)-((long)dataxRounded*10000L))));
        LCDString(tempbuff, LCDRight(8)-55, 41);
        memset(tempbuff, '\0', sizeof(tempbuff));
        sprintf(tempbuff, "Y: %c%d.%04d",(datay < 0)?'-':' ', datayRounded, abs((int)((((long)datay*10000L)/270)-((long)datayRounded*10000L))));
        LCDString(tempbuff, LCDRight(8)-55, 51);        
        mydelay(1000);
      }
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
