/***********************************************************************************************************************
* File Name    : YRDKRL78G13.h
* Version      : 0.1 [16 APR 2012]
* Device(s)    : R5F100LE
* Tool-Chain   : CA78K0R
* Description  : Implements board-specific functions of the YRDKRL78G13 dev board
* Creation Date: 4/16/2012
***********************************************************************************************************************/

#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_it.h"
#include "r_cg_serial.h"
#include "YRDKRL78G13.h"
#include "../swarmlibs/swarm.h"
#include "../redpine/rsi_uart_api.h"
#include "../redpine/rsi_at_command_processor.h"
#include <stdio.h>
#include <string.h>

uint8_t i2cbuf[10];
uint8_t mac_addr[6];
char butbuff[100];
char compbuff[250];
const uint8_t * led_portmap[6] = {&P5, &P6, &P6, &P5, &P5, &P5};
const uint8_t led_pinmap[6] = {5, 2, 3, 2, 3, 4};
uint8_t button_status[3] = {0, 0, 0};
boolean button_changed;
uint16_t stat;
int remaining;
rsi_uUartRsp response;
uint16_t result;
uint8_t errcounter;

rsi_socketFrame_t      insock;
rsi_socketFrame_t      outsock;

//Handle error reporting here, to spare r_main the complexity.
void md_err(MD_STATUS ret, const char * where){
	set_gpio(&P6, 2, 0);	
	printf("\r\n**i2cerr** ret=%02x at %s\r\n",ret,where);
	delay_ms(100);
}

/***********************************************************************************************************************
* ADXL345 Accelerometer:
***********************************************************************************************************************/

MD_STATUS setup_accel(){
	MD_STATUS ret;
	memset(i2cbuf, 0x0, 10);
	
	//Configure the ADXL345 for +/- 16G range, full resolution
	i2cbuf[0] = ACCEL_REG_DATA_FORMAT;
	i2cbuf[1] = ACCEL_BIT_RANGE16G | ACCEL_BIT_FULL_RES;
	
	while (iica0_busy || IICBSY0){ ; } 	//Make sure bus is ready for xfer
	iica0_busy = 1;
	ret = R_IICA0_Master_Send(ACCEL_ADDR, i2cbuf, 2, 0);
	if (ret != MD_OK){
		md_err(ret,"accel set format");
		return ret;
	}
	while (iica0_busy || IICBSY0){ ; } 	//Wait until the xfer is complete
	//ADXL345 will use the default rate of 100Hz
	//Put the ADXL345 into measure mode - start capturing acceleration
	i2cbuf[0] = ACCEL_REG_POWER_CTL;
	i2cbuf[1] = ACCEL_BIT_MEASURE;
	iica0_busy = 1;
	ret = R_IICA0_Master_Send(ACCEL_ADDR, i2cbuf, 2, 0);
	if (ret != MD_OK){
		md_err(ret,"accel measure mode");
		return ret;
	}
	while (iica0_busy || IICBSY0){ ; } 	//Wait until the xfer is complete
	return MD_OK;
}

MD_STATUS read_accel(accelData * dat){
	MD_STATUS ret;
	int i;
	
	dat->xraw = 0;
	dat->yraw = 0;
	dat->zraw = 0;
	dat->x = 0.0;
	dat->y = 0.0;
	dat->z = 0.0;
	
	while (iica0_busy || IICBSY0){ ; } 	//Make sure bus is ready for xfer
	i2cbuf[0] = ACCEL_REG_DATA;
	iica0_busy = 1;
	ret = R_IICA0_Master_Send(ACCEL_ADDR, i2cbuf, 1, 0);
	if (ret != MD_OK){
		md_err(ret, "accel set reg");
		return ret;
	} 
	while (iica0_busy || IICBSY0){ ; } 	//Wait until the xfer is complete
	i2cbuf[0] = 0;
	iica0_busy = 1;
	ret = R_IICA0_Master_Receive(ACCEL_ADDR, i2cbuf, 6, 0);
	while (iica0_busy || IICBSY0){ ; } 	//Wait until the xfer is complete
	if (ret != MD_OK){
		md_err(ret, "accel read");
		return ret;
	} else {
		dat->time = millis;
		dat->xraw = (signed int)((uint16_t)i2cbuf[0] | ((uint16_t)i2cbuf[1] << 8));
		dat->yraw = (signed int)((uint16_t)i2cbuf[2] | ((uint16_t)i2cbuf[3] << 8));
		dat->zraw = (signed int)((uint16_t)i2cbuf[4] | ((uint16_t)i2cbuf[5] << 8));
		dat->x = (float)dat->xraw/ACCEL_SCALE;
		dat->y = (float)dat->yraw/ACCEL_SCALE;
		dat->z = (float)dat->zraw/ACCEL_SCALE;
	}
	
	return ret;
}

/***********************************************************************************************************************
* PH5551A2NA1 LIGHT SENSOR:
***********************************************************************************************************************/

MD_STATUS setup_light(){
	MD_STATUS ret = MD_OK;
	while (iica0_busy || IICBSY0){ ; } 	//Make sure bus is ready for xfer
	i2cbuf[0] = LIGHT_BIT_SEL_MEASURE | LIGHT_BIT_PTR_INC | LIGHT_BIT_POWER;
	iica0_busy = 1;
	ret = R_IICA0_Master_Send(LIGHT_ADDR, i2cbuf, 1, 0);
	if (ret != MD_OK){
		md_err(ret, "light set reg");
		return ret;
	}
	while (iica0_busy || IICBSY0){ ; } 	//Wait until the xfer is complete
	return ret;
}

MD_STATUS read_light(lightData * dat){
	MD_STATUS ret = MD_OK;
	int len = 0; 
	int i;
	
	while (iica0_busy || IICBSY0){ ; } 	//Wait until the xfer is complete
	dat->light = 0;
	iica0_busy = 1;
	ret = R_IICA0_Master_Receive(LIGHT_ADDR, i2cbuf, 2, 0);
	while (iica0_busy || IICBSY0){ ; } 	//Wait until the xfer is complete
	if (ret != MD_OK){
		md_err(ret, "light read");
		return ret;
	} else {
		dat->time = millis;
		dat->light = (uint16_t)i2cbuf[0] | ((uint16_t)i2cbuf[1] << 8);
	}
	return ret;
}

/***********************************************************************************************************************
* ADT7420 TEMP SENSOR:
***********************************************************************************************************************/

MD_STATUS setup_temp(){
	MD_STATUS ret = MD_OK;
	while (iica0_busy || IICBSY0){ ; } 	//Make sure bus is ready for xfer
	i2cbuf[0] = TEMP_BIT_HIRES;
	iica0_busy = 1;
	ret = R_IICA0_Master_Send(TEMP_ADDR, i2cbuf, 1, 0);
	if (ret != MD_OK){
		md_err(ret, "temp config res");
		return ret;
	}
	while (iica0_busy || IICBSY0){ ; } 	//Wait until the xfer is complete
	return ret;
}

MD_STATUS read_temp(tempData * dat){
	MD_STATUS ret = MD_OK;
	
	dat->raw = 0;
	dat->tempC = 0.0;
	dat->tempF = 0.0;
	
	while (iica0_busy || IICBSY0){ ; } 	//Make sure bus is ready for xfer
	i2cbuf[0] = TEMP_REG_DATA;
	iica0_busy = 1;
	ret = R_IICA0_Master_Send(TEMP_ADDR, i2cbuf, 1, 0);
	if (ret != MD_OK){
		md_err(ret, "temp set reg");
		return ret;
	} 
	while (iica0_busy || IICBSY0){ ; } 	//Wait until the xfer is complete
	i2cbuf[0] = 0;
	iica0_busy = 1;
	ret = R_IICA0_Master_Receive(TEMP_ADDR, i2cbuf, 2, 0);
	while (iica0_busy || IICBSY0){ ; } 	//Wait until the xfer is complete
	if (ret != MD_OK){
		md_err(ret, "temp read");
		return ret;
	} else {
		dat->time = millis;
		dat->raw = (uint16_t)i2cbuf[1] | ((uint16_t)i2cbuf[0] << 8);
		if (dat->raw & 0x8000){
			//Negative temperature
			dat->tempC = ((float)(dat->raw-65536))/128.0;
		} else {
			//Positive temperature
			dat->tempC = ((float)dat->raw)/128.0;
		}
		dat->tempF = ((9.0/5.0)*dat->tempC)+32;
	}
	return ret;
}

void set_led(uint8_t num, uint8_t val){
	if (num < 6){
		set_gpio(led_portmap[num], led_pinmap[num], !val);
	}
}

void toggle_led(uint8_t num){
	if (num < 6){
		toggle(led_portmap[num], led_pinmap[num]);
	}
}

void sendButtonInfo(){
	memset(butbuff, '\0', sizeof(butbuff));
	sprintf(butbuff, "{\"name\":\"Button\",\"feed\":{\"b1\":%u,\"b2\":%u,\"b3\":%u}}",
		button_status[0],button_status[1],button_status[2]);
	printf("Sending %s\r\n",butbuff);
        swarm_produce(butbuff,&outsock);
}

boolean doWork(unsigned long duration){
	unsigned long endTime = millis+duration;
	while(millis < endTime){
		if (!readData()){
			return 0;
		}
		if (button_changed) {
			sendButtonInfo();
			button_changed = 0;
		}
	}
	return 1;
}

boolean readData(){
	do {
		result = rsi_read_data((uint8*)&response,&stat);
		if (result == RSI_ERROR_NO_RX_PENDING){
			continue;
		}
		switch (stat) {
		case RSI_EVENT_CMD_RESPONSE:
			//printf("CMD result(%04x), stat(%04x)\r\n",result,stat);
			if (result > 0x0080U){
				errcounter++;
			} else {
				errcounter = 0;
			}
			if (errcounter > 10){
				printf("Multiple error responses returned, restarting\r\n");
				return 0;
			}
			break;
		case RSI_EVENT_RX_DATA:	
			//remaining = response.rcv_data.buf_len-len;
			//lookFor(response.rcv_data.buffer,"Renesas01");
			//memset(response.rcv_data.buffer, '\0', 200);
			delay_ms(20);
			break;		  
		case RSI_EVENT_SOCKET_CLOSE:
			printf("WARN: Socket closed!\r\n");
			return 0;
		case RSI_EVENT_SLEEP:
			printf("SLEEP\r\n");
			break;
		case RSI_EVENT_RAW_DATA:
			delay_ms(20);
			//remaining -= strlen(response.rcv_data.buffer);
			//lookFor(response.rcv_data.buffer,"Renesas01");
			//memset(response.rcv_data.buffer, '\0', 200);
			break;
		default:
			delay_ms(20);
			//printf("UNKNOWN DATA RECEIVED\r\n");
		}
	}while (result != RSI_ERROR_NO_RX_PENDING);
	return 1;
}

void lookFor(char * line, const char * key) {
	printf("RECV: %s\r\n",line);
}

void button_callback(uint8_t num, uint8_t value){
	button_status[num] = !value;
	button_changed = 1;
}

void getButtons(uint8_t * buttons){
	int i;
	for (i=0;i<3;i++){
		buttons[i] = button_status[i];
	}
}

boolean read_mac_addr(){
    int i=0;
    if( rsi_query_mac_addr() != RSI_NOERROR ) {
          printf("Can't get MAC address\r\n"); 
          return 0;       //If we cannot open a listening socket, do not continue    
    }
    do {
        delay_ms(10);
        stat = rsi_read_cmd_rsp(mac_addr);
    } while (stat == RSI_ERROR_NO_RX_PENDING);
    if (stat != RSI_NOERROR){
          printf("Error retrieving MAC address %d\r\n", (signed int)stat); 
          return 0;
    }
    printf("MAC address is ");
    for (i=0;i<6;i++) {
	printf("%02x",mac_addr[i]);    
    }
    printf("\r\n");	
    return 1;
}

boolean closeConnection(){
	rsi_socket_close(outsock.handle);
	return (rsi_socket_close(insock.handle) > 0);
}

boolean openHTTPConnection(const char * hostname){
	do {
		insock.lport = rand();
	} while (insock.lport < 1024);
	outsock.rport = 80;
	outsock.lport = insock.lport;
	strcpy(outsock.remote_ip, hostname);
	// OPEN listening socket 
	if( rsi_socket_ltcp_open (&insock) != RSI_NOERROR ) {
		printf("insock error\r\n"); 
		return 0;       //If we cannot open a listening socket, do not continue
	}
	printf("insock open sent, waiting for response...");
	do {
		delay_ms(10);
		stat = rsi_read_cmd_rsp(&insock.handle);
	} while (stat == RSI_ERROR_NO_RX_PENDING);
	if (stat != RSI_NOERROR) {
		printf("insock %X\r\n", (signed int)stat); 
		return 0;       //If we cannot open a listening socket, do not continue
	}
	printf("...insock open.\r\n");

	// Repeately try to connect to the swarm server until successful 
	do {
		if( rsi_socket_tcp_open (&outsock) != RSI_NOERROR ) {
			printf("Outsock error\r\n"); 
		}
		printf("outsock open sent, waiting for response...");
		do {
	  		delay_ms(10);
	  		stat = rsi_read_cmd_rsp(&outsock.handle);
		} while (stat == RSI_ERROR_NO_RX_PENDING);
		printf("...response recieved\r\n");
		if (stat != RSI_NOERROR) {
			printf("Can't connect to %s, retrying E%X\r\n", hostname, (signed int)stat);
			delay_ms(1000);
		}
	} while (stat != RSI_NOERROR);	

	return 1;
}

boolean openRAWConnection(const char * hostname, int port){
	do {
		insock.lport = rand();
	} while (insock.lport < 1024);
	outsock.rport = port;
	outsock.lport = insock.lport;
	strcpy(outsock.remote_ip, hostname);
	// OPEN listening socket 
	if( rsi_socket_ltcp_open (&insock) != RSI_NOERROR ) {
		printf("insock error\r\n"); 
		return 0;       //If we cannot open a listening socket, do not continue
	}
	printf("insock open sent, waiting for response...");
	do {
		delay_ms(10);
		stat = rsi_read_cmd_rsp(&insock.handle);
	} while (stat == RSI_ERROR_NO_RX_PENDING);
	if (stat != RSI_NOERROR) {
		printf("insock %X\r\n", (signed int)stat); 
		return 0;       //If we cannot open a listening socket, do not continue
	}
	printf("...insock open.\r\n");

	// Repeately try to connect to the swarm server until successful 
	do {
		if( rsi_socket_tcp_open (&outsock) != RSI_NOERROR ) {
			printf("Outsock error\r\n"); 
		}
		printf("outsock open sent, waiting for response...");
		do {
	  		delay_ms(10);
	  		stat = rsi_read_cmd_rsp(&outsock.handle);
		} while (stat == RSI_ERROR_NO_RX_PENDING);
		printf("...response recieved\r\n");
		if (stat != RSI_NOERROR) {
			printf("Can't connect to %s, retrying E%X\r\n", hostname, (signed int)stat);
			delay_ms(1000);
		}
	} while (stat != RSI_NOERROR);	

	return 1;
}