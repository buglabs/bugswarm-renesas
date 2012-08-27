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
* Creation Date: 8/26/2012
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
#include "r_cg_wdt.h"
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
uint16_t mic_level;
uint16_t period;

accelData last_accel;
lightData last_light;
tempData last_temp;

api strApi;
DECLARE_AND_INIT_GLOBAL_STRUCT(api, strApi);
#define LIB_RX_BUF_SIZE       200
#define LIB_NETWORK_HDR_LEN   50

/* Swarm IDs
 * Edit the following values based on desired BUGSwarm configuration */
const char swarm_server_ip[] = "107.20.250.52";  //api.bugswarm.net
const char swarm_id[] =          "2eaf3f05cd0dd4becc74d30857caf03adb85281e";
const char resource_id[] =       "ff5e5cb493f4e7fd2b3a17fbce183925e76094c8";
const char participation_key[] = "bc60aa60d80f7c104ad1e028a5223e7660da5f8c";

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
    period = UPDATE_PERIOD;
    //Initialize RL78 peripherals
    R_IT_Start();
    R_UART0_Start();
    R_UART2_Start();
    R_ADC_Start();
    R_ADC_Set_OperationOn();
    R_INTC0_Start();
    R_INTC1_Start();
    R_INTC2_Start();
    R_WDT_Create();
    
    printf(message);
    
    //Initialize i2c devices, give them time to start up
    setup_accel();
    setup_light();
    setup_temp();
    delay_ms(100);
    
    reconnect:
    //Initialize UART and enable Module power and reset pins
    printf("initializing redpine device...");
    MY_WDT_Restart();
    rsi_init();
    printf("done!\r\n");
    
    //We are enabling two buffers for use with the redpine device
    rsi_set_rx_buffer(lib_rx_buffer1, (LIB_RX_BUF_SIZE + LIB_NETWORK_HDR_LEN));
    rsi_set_rx_buffer(lib_rx_buffer2, (LIB_RX_BUF_SIZE + LIB_NETWORK_HDR_LEN));
    
    //Preform preform autobaud procedure and initialize redpine device
    printf("booting redpine device...");
    MY_WDT_Restart();
    ret = rsi_boot_device();
    if (ret != RSI_NOERROR){
	printf("ERROR: %02x\r\n",ret);
	return;
    }
    printf("done!\r\n");
       
    //Attempt to connect to wifi AP, watchdog will restart if this takes longer than 60s
    printf("connecting to wifi ap...");
    MY_WDT_Restart_For(60000);	//Allow this operation to take at most approximately 60 seconds
    ret = rsi_wifi_init (strApi.band, &strApi.ScanFrame, &strApi.JoinFrame, &strApi.IPparamFrame);     
    printf("%04x\r\n",ret);
    
    MY_WDT_Restart();
    
    read_mac_addr();
    
    //Initial sensor readings used to seed random number generator
    //This needs to be truly random so that we don't accidentally try to recycle the same port
    //Attempting to re-open an existing port will fail unless the TCP timeout has been exceeded
    read_accel(&last_accel);
    read_light(&last_light);
    read_temp(&last_temp);
    seed = (millis+last_accel.xraw+last_accel.yraw+last_accel.zraw+last_light.light+last_temp.raw)&0xFFFF;
    srand(seed);
    
    //Open a socket to swarm server (defined above), port 80.
    MY_WDT_Restart();
    if (!openHTTPConnection(swarm_server_ip))
    	goto reconnect;
    printf("Connected to socket\r\n");
    
    // open HTTP streaming socket to swarm by sending HTTP participation header 
    swarm_send_produce(swarm_id, resource_id, participation_key, &outsock);
      
    // TODO - read from socket until we see headers back from the server
    // This delay is dependent on swarm service latency.
    if (!doWork(8000)) {
	delay_ms(1000);
	goto reconnect;
    }
      
    // Send capabilities message, assuming a webUI is listening for it 
    capabilities_announce(&outsock);
    printf("Connected to Swarm!\r\n");
    while (1U)
    {
	//Retrieve new samples from all sensors, first.
	MY_WDT_Restart();
	toggle_led(5);
	read_accel(&last_accel);
	read_light(&last_light);
	read_temp(&last_temp);
	
	//Mic signal is balanced to VCC/2, subtracting this DC offset.
	mic_level = an5_max - 0x0200;
	an5_max = 0;
	printf("A(%f,%f,%f) L[%u] T<%f> {%04x} {%04x} (%lu)\r\n", last_accel.x, last_accel.y, last_accel.z, last_light.light, last_temp.tempF, an4_last, mic_level, last_temp.time);
	
	//Delay at least (period/NUM_SENSOR) ms, reading from socket and buttons in the meantime.
	if (!doWork(period/NUM_SENSOR)) {
		delay_ms(1000);
		goto reconnect;
	}
	
	//Send accel:
	memset(tempbuff, '\0', sizeof(tempbuff));
	sprintf(tempbuff, "{\"name\":\"Acceleration\",\"feed\":{\"x\":%f,\"y\":%f,\"z\":%f}}",
		last_accel.x, last_accel.y, last_accel.z);
	if (!swarm_produce(tempbuff,&outsock)) {
		delay_ms(1000);
		goto reconnect;
	}
	
	//Delay at least (period/NUM_SENSOR) ms, reading from socket and buttons in the meantime.
	if (!doWork(period/NUM_SENSOR)) {
		delay_ms(1000);
		goto reconnect;
	}
	
	//Send light:
	memset(tempbuff, '\0', sizeof(tempbuff));
	sprintf(tempbuff, "{\"name\":\"Light\",\"feed\":{\"Value\":%u}}",
		last_light.light);
        if (!swarm_produce(tempbuff,&outsock)) {
		delay_ms(1000);
		goto reconnect;
	}
	
	//Delay at least (period/NUM_SENSOR) ms, reading from socket and buttons in the meantime.
	if (!doWork(period/NUM_SENSOR)) {
		delay_ms(1000);
		goto reconnect;
	}
	
	//Send temp:
	memset(tempbuff, '\0', sizeof(tempbuff));
	sprintf(tempbuff, "{\"name\":\"Temperature\",\"feed\":{\"TempF\":%f}}",
		last_temp.tempF);
        if (!swarm_produce(tempbuff,&outsock)) {
		delay_ms(1000);
		goto reconnect;
	}
	
	//Delay at least (period/NUM_SENSOR) ms, reading from socket and buttons in the meantime.
	if (!doWork(period/NUM_SENSOR)) {
		delay_ms(1000);
		goto reconnect;
	}
	
	//Send pot:
	memset(tempbuff, '\0', sizeof(tempbuff));
	sprintf(tempbuff, "{\"name\":\"Potentiometer\",\"feed\":{\"Raw\":%u}}",
		an4_last);
        if (!swarm_produce(tempbuff,&outsock)) {
		delay_ms(1000);
		goto reconnect;
	}
	
	//Delay at least (period/NUM_SENSOR) ms, reading from socket and buttons in the meantime.
	if (!doWork(period/NUM_SENSOR)) {
		delay_ms(1000);
		goto reconnect;
	}
	
	//Send pot:
	memset(tempbuff, '\0', sizeof(tempbuff));
	sprintf(tempbuff, "{\"name\":\"Sound Level\",\"feed\":{\"Raw\":%u}}",
		mic_level);
        if (!swarm_produce(tempbuff,&outsock)) {
		delay_ms(1000);
		goto reconnect;
	}
		
	if (millis%CAPABILITIES_PERIOD < UPDATE_PERIOD){
           capabilities_announce(&outsock);
        }
    }
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
