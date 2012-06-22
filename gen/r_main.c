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
* Creation Date: 6/4/2012
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
#include "r_cg_intc.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_it.h"
/* Start user code for include. Do not edit comment generated here */
#include <stdio.h>
#include <string.h>
//#include <float.h>
#include "../board/YRDKRL78G13.h"
#include "../redpine/rsi_data_types.h"
#include "../redpine/rsi_uart_api.h"
#include "../redpine/rsi_hal.h"
#include "../redpine/rsi_global.h"
#include "../redpine/rsi_hal_api.h"
#include "../swarmlibs/swarm.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* The sample period, in milliseconds.  This will be limited by BUGswarm.
 * Adjust with care and an eye on a swarm console.
 * NOTE - this isn't a precise value, it does not take into effect the time
 * spent retrieving the sample and sending it out to swarm.*/
#define UPDATE_PERIOD 1000
#define NUM_SENSOR 5
/* The number of messages to wait until sending out a swarm capabilities message
 */
#define CAPABILITIES_PERIOD 10000

const uint8_t * message = (const uint8_t *)"Hello World\r\n";

char tempbuff[200];
int dataxRounded;
int datayRounded;
int num = 0;
int len;
int seed;
uint16_t status;
uint16_t ret;
uint16_t pot;
uint16_t period;

accelData last_accel;
lightData last_light;
tempData last_temp;

api strApi;
DECLARE_AND_INIT_GLOBAL_STRUCT(api, strApi);
#define LIB_RX_BUF_SIZE       200
#define LIB_NETWORK_HDR_LEN   50

rsi_socketFrame_t      insock;
rsi_socketFrame_t      outsock;


/* Swarm IDs
 * Edit the following values based on desired BUGSwarm configuration */
const char swarm_server_ip[] = "107.20.250.52";  //api.bugswarm.net
//const char swarm_server_ip = "64.118.81.28";  //test.api.bugswarm.net
//const char swarm_server_ip[] = "192.168.11.204";
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
    R_ADC_Start();
    R_ADC_Set_OperationOn();
    R_INTC0_Start();
    R_INTC1_Start();
    R_INTC2_Start();
    printf(message);
    //Initialize i2c devices, give them time to start up
    period = UPDATE_PERIOD;
    setup_accel();
    setup_light();
    setup_temp();
    delay_ms(100);

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
    
    read_accel(&last_accel);
    read_light(&last_light);
    read_temp(&last_temp);
    seed = (millis+last_accel.xraw+last_accel.yraw+last_accel.zraw+last_light.light+last_temp.raw)&0xFFFF;
    srand(seed);
    do {
        insock.lport = rand();
    } while (insock.lport < 1024);
    printf("seed %04X port %04X", seed, insock.lport);
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
        delay_ms(10);
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
          delay_ms(10);
          status = rsi_read_cmd_rsp(&outsock.handle);
        } while (status == RSI_ERROR_NO_RX_PENDING);
        if (status != RSI_NOERROR){
            printf("Can't connect to SWARM, retrying E%X\r\n", (signed int)status);
            delay_ms(1000);
        }
      } while (status != RSI_NOERROR);
    
      printf("Connected to socket\r\n");
      //required by subsequent calls to the at+rsi_snd function
      //for TCP sockets, apparently these values need to be 0, after connecting...
      outsock.rport=0;
      strcpy(outsock.remote_ip, "0");
    // open HTTP streaming socket to swarm by sending participation header 
      swarm_send_produce(swarm_id, resource_id, participation_key, &outsock);
      //Wait until we get headers and presence messages!
      /*NOTE - the following does not work, this succeeds in waiting for the first line of data
      	BUT it does not succeed in waiting until the *LAST* line of data
	This causes us to send our 
	The problem:
	The rsi_uUartRsp struct doesn't expose the total byte length of the recieved packet!
	Ideally, we would read this value, and then subtract the actual read amount each iteration
	until we have read everything.
	In theory we could just keep reading until the UART buffer is empty, but in practice the buffer clears
	before the module sends the next line
	We could:
	A) Add a delay to readData to give the module time to send the next line
	B) Try to reach into the raw uart buffer to snag the packet length
	C) Totally rewrite the rsi_read_data functionality for our uses.
      
      status = 0;
      while (status != 0x02){
		readData();
      }
      */
      doWork(2000);
      
      //delay_ms(2000);
      // Send capabilities message, assuming a webUI is listening for it 
      //return;
      capabilities_announce(&outsock);
    printf("Connected to Swarm!\r\n");
    /***delay_ms(500); ***/
    while (1U)
    {
	toggle_led(5);
	read_accel(&last_accel);
	read_light(&last_light);
	read_temp(&last_temp);
	R_ADC_Get_Result(&pot);
	
	printf("A(%f,%f,%f) L[%u] T<%f> {%04x} (%lu)\r\n", last_accel.x, last_accel.y, last_accel.z, last_light.light, last_temp.tempF, pot, last_temp.time);
	
	doWork(period/NUM_SENSOR);
	
	//Send accel:
	memset(tempbuff, '\0', sizeof(tempbuff));
	//sprintf(tempbuff, "{\"name\":\"Acceleration\",\"feed\":{\"x\":%f,\"y\":%f}}",
	sprintf(tempbuff, "{\"name\":\"Acceleration\",\"feed\":{\"x\":%f,\"y\":%f,\"z\":%f}}",
		last_accel.x, last_accel.y, last_accel.z);
	printf("Sending %s\r\n",tempbuff);
	readData();
        swarm_produce(tempbuff,&outsock);
	readData();
	
	doWork(period/NUM_SENSOR);
	
	//Send light:
	memset(tempbuff, '\0', sizeof(tempbuff));
	sprintf(tempbuff, "{\"name\":\"Light\",\"feed\":{\"Value\":%u}}",
		last_light.light);
	printf("Sending %s\r\n",tempbuff);
	readData();
        swarm_produce(tempbuff,&outsock);
	readData();
	
	doWork(period/NUM_SENSOR);
	
	//Send temp:
	memset(tempbuff, '\0', sizeof(tempbuff));
	sprintf(tempbuff, "{\"name\":\"Temperature\",\"feed\":{\"TempF\":%f}}",
		last_temp.tempF);
	printf("Sending %s\r\n",tempbuff);
	readData();
        swarm_produce(tempbuff,&outsock);
	readData();
	
	doWork(period/NUM_SENSOR);
	
	//Send pot:
	memset(tempbuff, '\0', sizeof(tempbuff));
	sprintf(tempbuff, "{\"name\":\"Potentiometer\",\"feed\":{\"Raw\":%u}}",
		pot);
	printf("Sending %s\r\n",tempbuff);
	readData();
        swarm_produce(tempbuff,&outsock);
	readData();
	
	//Send button:
	//TODO - do this way better, move button checking to a work handler so it can be done OFTEN
		
	//while((millis%UPDATE_PERIOD) != 0) { ; }
	if (millis%CAPABILITIES_PERIOD < UPDATE_PERIOD){
		readData();
           capabilities_announce(&outsock);
	   readData();
        }
    }
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
