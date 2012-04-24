/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING 
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT 
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR 
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE 
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software 
* and to discontinue the availability of this software.  By using this software, 
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2010, 2011 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G13 V1.03.01 [11 Oct 2011]
* Device(s)    : R5F100LE
* Tool-Chain   : CA78K0R
* Description  : This file implements main function.
* Creation Date: 4/24/2012
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_it.h"
/* Start user code for include. Do not edit comment generated here */
#include <stdio.h>
#include <string.h>
#include "../board/YRDKRL78G13.h"
#include "../redpine/rsi_data_types.h"
#include "../redpine/rsi_uart_api.h"
#include "../redpine/rsi_hal.h"
#include "../redpine/rsi_global.h"
#include "../redpine/rsi_hal_api.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
const uint8_t * message = (const uint8_t *)"Hello World\r\n";

char buff[120];
int num = 0;
int len;
int status;
uint16_t ret;
accelData last_accel;
lightData last_light;

api strApi;
DECLARE_AND_INIT_GLOBAL_STRUCT(api, strApi);
#define LIB_RX_BUF_SIZE       100
#define LIB_NETWORK_HDR_LEN   50

rsi_socketFrame_t      insock;
rsi_socketFrame_t      outsock;

/* Swarm IDs
 * Edit the following values based on desired BUGSwarm configuration */
//const char swarm_server_ip[] = "107.20.250.52";  //api.bugswarm.net
//const char swarm_server_ip = "64.118.81.28";  //test.api.bugswarm.net
const char swarm_server_ip[] = "192.168.11.204";
const char swarm_id[] =          "27e5a0e7e2e5445c51be56de44f45b19701f36d3";
const char resource_id[] =       "b75538642bcadbdf4ae6d242d4f492266c11cb44";
const char participation_key[] = "7a849e6548dbd6f8034bb7cc1a37caa0b1a2654b";

char rxbyte;
uint8 lib_rx_buffer1[LIB_RX_BUF_SIZE+LIB_NETWORK_HDR_LEN]; 
uint8 lib_rx_buffer2[LIB_RX_BUF_SIZE+LIB_NETWORK_HDR_LEN];
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    /* Start user code. Do not edit comment generated here */
    R_IT_Start();		//Start the interval timer
    R_UART0_Start();		//Start the UART
    R_UART2_Start();
    printf(message);
    //Initialize i2c devices, give them time to start up
    //setup_accel();
    //setup_light();
    delay_ms(100);
    
    
//    struct rsi_socketFrame_s *insock = &insock_obj;
//    struct rsi_socketFrame_s *outsock = &outsock_obj;

    /* Initialize UART and enable Module power and reset pins */  
    printf("initializing redpine device...");
    rsi_init();
    printf("done!\r\n");
    rsi_set_rx_buffer(lib_rx_buffer1, (LIB_RX_BUF_SIZE + LIB_NETWORK_HDR_LEN));
    rsi_set_rx_buffer(lib_rx_buffer2, (LIB_RX_BUF_SIZE + LIB_NETWORK_HDR_LEN));
    printf("booting redpine device...");
    ret = rsi_boot_device();
    if (ret != RSI_NOERROR){
	printf("ERROR: %02x\r\n",ret);
	return;
    }
    printf("done!\r\n");
    
    printf("connecting to wifi ap...");
    ret = rsi_wifi_init (strApi.band, &strApi.ScanFrame, &strApi.JoinFrame, &strApi.IPparamFrame);     
    printf("%04x\r\n",ret);
    
    //TODO, re-implement random port selection
    insock.lport = 2345;
    outsock.rport = 80;
    outsock.lport = insock.lport;
    strcpy(outsock.remote_ip, swarm_server_ip);
    
    // OPEN listening socket 
      if( rsi_socket_ltcp_open (&insock) != RSI_NOERROR )
      {
          printf("insock FAIL\r\n"); 
          return;       //If we cannot open a listening socket, do not continue
      }
      do {
        rsi_delayMs(10);
        status = rsi_read_cmd_rsp(&insock.handle);
      } while (status == RSI_ERROR_NO_RX_PENDING);
      if (status != RSI_NOERROR){
          printf("insock %X\r\n", (signed int)status); 
          return;       //If we cannot open a listening socket, do not continue
      }
      
      // Repeately try to connect to the swarm server until successful 
      do {
        if( rsi_socket_tcp_open (&outsock) != RSI_NOERROR )
        {
            printf("outsock FAIL\r\n"); 
        }
        do {
          rsi_delayMs(10);
          status = rsi_read_cmd_rsp(&outsock.handle);
        } while (status == RSI_ERROR_NO_RX_PENDING);
        if (status != RSI_NOERROR){
            printf("Can't connect to SWARM, retrying E%X\r\n", (signed int)status);
            rsi_delayMs(1000);
        }
      } while (status != RSI_NOERROR);
    printf("Connected to Swarm!\r\n");
    delay_ms(500);
    while (1U)
    {
	/*toggle(&P5,4);	
	read_accel(&last_accel);
	read_light(&last_light);
	
	printf("%d-%d-%d (%ld) %u (%ld)\r\n", last_accel.x, last_accel.y, last_accel.z, last_accel.time, last_light.light, last_light.time);
	//memset(buff, '\0', 120);
	//len = sprintf(buff, "%d-%d-%d (%ld) %u (%ld)\r\n", last_accel.x, last_accel.y, last_accel.z, last_accel.time, last_light.light, last_light.time);
	//ret = R_UART0_Send((uint8_t *)buff, len);
	//if (ret != MD_OK){
		//P5 &= ~(1 << 5);
	//}
	while((millis%1000) != 0) { ; } */
    }
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
