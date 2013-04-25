
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <system/platform.h>
#include <NVSettings.h> 
#include <system/Switch.h>
#include <drv/Glyph/lcd.h>
#include <mstimer.h>
#include <system/console.h>
#include <jsmn/jsmn.h>
#include "r_cg_timer.h"
#include <sensors/Accelerometer.h>
#include <sensors/LightSensor.h> 
#include <sensors/Temperature.h>
#include <sensors/Potentiometer.h>
#include "Apps.h"
#include "led.h"
#include <drv/EInk/user_app.h>
#include "App_Swarm.h"

#define UPDATE_PERIOD 5000
#define NUM_SENSOR 5
#define CAPABILITIES_PERIOD 10000
#define MAX_PROD_ERRORS 5

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
uint16_t period = NUM_SENSOR;

/*  Default swarm connection parameters  */

const char SwarmHost[] = "107.20.250.52";  //api.bugswarm.
//const char SwarmHost[] = "192.168.11.230";  //api.bugswarm.
//These parameters must be hardcoded, are not retrieved.
const char participation_key[] = "bc60aa60d80f7c104ad1e028a5223e7660da5f8c";
const char configuration_key[] = "359aff0298658552ec987b9354ea754b684a4047";
const char prod_swarm_id[] = "69df1aea11433b3f85d2ca6e9c3575a9c86f8182";
const char cons_swarm_id[] = "5dbaf819af6eeec879a1a1d6c388664be4595bb3";
const char true_val[] = "true\0";
const char false_val[] = "false\0";
//In the future, all of the above parameters can be retrieved with only this:
const char password_hash[] = "cmVuZXNhczpyZW5lc2FzcHNr";
//This is the default resource id, "UnknownDevice".
//It will be overwritten by getResourceID()
char resource_id[] = "14f762c59815e24973165668aff677659b973d62";

/*  Swarm API Call message headers and payload formatting:  */

const char configure_resource_header[] = "POST /renesas/configure HTTP/1.1\r\n"
    "Host: api.bugswarm.net\r\nAccept: */*\r\nx-bugswarmapikey: %s\r\n"
    "content-type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n";
const char configure_resource_body[] = 
	"{\"name\":\"%s\",\"swarmid\":[{\"id\":\"%s\",\"type\":\"producer\"},"
	"{\"id\":\"%s\",\"type\":\"consumer\"}],"
	"\"description\":\"an RL78G14 board running bugswarm-renesasG14 1.0\"}";
const char list_resources_header[] = "GET /resources HTTP/1.1\r\n"
	"Host: api.bugswarm.net\r\nAccept: */*\r\nx-bugswarmapikey: %s\r\n\r\n";
const char apikey_header[] = "GET /keys/configuration HTTP/1.1\r\n"
"Authorization: Basic %s\r\nHost: api.bugswarm.net\r\nAccept: */*\r\n\r\n";
const char produce_header[] = "POST /stream?swarm_id=%s&swarm_id=%s&"
  "resource_id=%s HTTP/1.1\r\nHost: api.bugswarm.com\r\n"
  "x-bugswarmapikey: %s\r\ntransfer-encoding: chunked\r\nconnection: keep-alive"
  "\r\nContent-Type: application/json\r\n\r\n";
//  "\r\nContent-Type: application/json\r\n\r\n1\r\n\n\r\n";
const char feed_request[] = "{\"capabilities\": {\"feeds\": [\"Acceleration\","
  "\"Temperature\",\"Button\",\"Light\",\"Sound Level\",\"Potentiometer\"]}}";
const char message_header[] = "%x\r\n{\"message\": {\"payload\":%s}}\r\n\r\n";

char    MACaddr[20];
char    MACstr[13];
char    pkt[1500];
char 	msg[512];
bool    connected;
ATLIBGS_TCPMessage tcp_pkt;
uint16_t mic_level;
uint32_t tone_stop;
uint32_t rx;
uint32_t tx;
uint32_t rxold;
uint32_t txold;

/*----------------------------------------------------------------------------*
 *	Routine: App_SwarmConnector
 *----------------------------------------------------------------------------*
 *	Description:
 *		Run the swarm connector demo indefinitely
 *----------------------------------------------------------------------------*/
void App_SwarmConnector(void) {
	ATLIBGS_MSG_ID_E r;
	uint8_t cid = 0;
	rx=0;
	tx=0;

	initEink();
	setLogo(2);

	R_TAU0_Create();
	//R_TAU0_Channel0_Freq(0);
	//R_TAU0_Channel0_Start();
	
	App_InitModule();
	while(1) {
    	r = AtLibGs_GetMAC(MACaddr);
		if(r != ATLIBGS_MSG_ID_OK) {
			ConsolePrintf("Couldn't get MAC: %02X\n",r);
			DisplayLCD(LCD_LINE6, "Get MAC Failed!");
			DisplayLCD(LCD_LINE8, "Retry GET MAC");
			MSTimerDelay(1000);
			continue;
		} 
		break;
    } 
    AtLibGs_ParseGetMacResponse(MACstr);
    ConsolePrintf("Module MAC Address: %s\r\n", MACstr);
	ConsolePrintf("Connecting to AP...");
	DisplayLCD(LCD_LINE7, "Connecting to AP");
    App_aClientConnection();		//Will block until connected
	AtLibGs_SetNodeAssociationFlag();
	memset(msg, '\0', sizeof(msg));
	sprintf(msg, "IP: ");
	AtLibGs_GetIPAddress((uint8_t*) msg+4);
	DisplayLCD(LCD_LINE4,(const uint8_t *) msg);
	DisplayLCD(LCD_LINE7, "                 ");
	ConsolePrintf("Connected.\n");
	DisplayLCD(LCD_LINE1, (const uint8_t *) "Swarm Client R0.6.0");
	DisplayLCD(LCD_LINE8, "Getting ResID");
	led_all_off();

    //TODO - Check EEPROM for a cached resourceid,
    //       possibly load preset swarm credentials.
    r = getResourceID(MACstr, pkt
					  , sizeof(pkt), resource_id);
    if (r != ATLIBGS_MSG_ID_OK) {
    	ConsolePrintf("WARN: Couldn't get Resource ID: %02X\n",r);
		DisplayLCD(LCD_LINE6, "Get ResID Failed!");
		DisplayLCD(LCD_LINE8, "+default resID");
		//MSTimerDelay(2000);
		//return;
    }
	ConsolePrintf("Using Resource %s\n",resource_id);
	sprintf(msg, "MAC:   %s",MACstr);
	DisplayLCD(LCD_LINE3, (const uint8_t *)msg);
	
	Temperature_Init();
    Potentiometer_Init();
	Accelerometer_Init();

    MSTimerDelay(100);
    connected = false;

	while(1) {
		if (!AtLibGs_IsNodeAssociated()) {
			DisplayLCD(LCD_LINE7, "Reconnecting AP");
			App_aClientConnection();
			AtLibGs_SetNodeAssociationFlag();
			memset(msg, '\0', sizeof(msg));
			sprintf(msg, "IP: ");
			AtLibGs_GetIPAddress((uint8_t*) msg+4);
			DisplayLCD(LCD_LINE4,(const uint8_t *) msg);
			DisplayLCD(LCD_LINE7, "                 ");
			ConsolePrintf("Connected.\n");
		}
		ConsolePrintf("Creating a production session\r\n");
		DisplayLCD(LCD_LINE8, "Contacting Swarm");

		r = createProductionSession(&cid, 
									 (char *)SwarmHost,
									 (char *)prod_swarm_id,
									 (char *)cons_swarm_id,
									 (char *)resource_id,
									 (char *)participation_key);
		if (r != ATLIBGS_MSG_ID_OK) {
			ConsolePrintf("Unable to open production session (%02X), retrying\n",r);
			DisplayLCD(LCD_LINE8, "produce socket err");
			connected = false;
			MSTimerDelay(10000);		//TODO - exponential backoff
			continue;
		}
		ConsolePrintf("Connected to Swarm\n");
		DisplayLCD(LCD_LINE8, "Connected!");
		sprintf(msg, "RX: %lu",rx);
		DisplayLCD(LCD_LINE6, (const uint8_t *)msg);
		sprintf(msg, "TX: %lu",tx);
		DisplayLCD(LCD_LINE7, (const uint8_t *)msg);
		connected = true;
		while (connected) {
			r = App_SwarmProducer(cid);
			if (r != ATLIBGS_MSG_ID_OK){
				DisplayLCD(LCD_LINE8, "Disconnected");
				connected = false;
			}
			//readForAtLeast(cid, 1000);
		}
		AtLibGs_Close(cid);
		ConsolePrintf("Disconnected, attempting to reconnect...\n");
		DisplayLCD(LCD_LINE6, "Reconnecting");
		MSTimerDelay(5000);		//TODO - exponential backoff
	}	
}

ATLIBGS_MSG_ID_E getResourceID (char * mac_addr_str, char * buff, int bufflen, char * result){
	ATLIBGS_MSG_ID_E r;
	uint8_t cid = 0;
	int len;

	//See const char[] configure_resource_body above for the structure
	len = sprintf(msg, configure_resource_body, mac_addr_str, prod_swarm_id, cons_swarm_id);
	r = makeAPICall(&cid, buff, configure_resource_header, 
					configuration_key, len, msg);
	if (r != ATLIBGS_MSG_ID_OK) {
        ConsolePrintf("Unable to make API call:\r\n%s",buff);
		DisplayLCD(LCD_LINE8, "API call err");
		return r;
	}
 
	//Read the response into the provided buffer, waiting 5 seconds
	r = readOnePacket(buff, bufflen, &len, 5000);
	if (r != ATLIBGS_MSG_ID_OK){
		ConsolePrintf("Error retrieving response %d\r\n", r);
		DisplayLCD(LCD_LINE8, "swarm readcfg err");
		return r;
	}
	AtLibGs_Close(cid);
	ConsolePrintf("Got %d bytes\r\n",len);
	//respos - the end of HTTP headers and beginning of response
	//marked by a double CRLF (one blank line always follows headers)
	char * respos = strstr(buff, "\r\n\r\n")+4;
	//valid resource IDs are always 40 characters in width.
	if (strlen(respos) == 40) {
		memcpy(resource_id, respos, 40);
	} else {
		ConsolePrintf("Invalid resource id from server, using default "
					  "\"UnknownDevice\"\r\n");
		DisplayLCD(LCD_LINE8, "invalid cfg resp");
	}
	ConsolePrintf("Using resource id: %s\r\n",resource_id);

	return r;
}

/*  Retrieve the configuration API key associated with password_hash  */
ATLIBGS_MSG_ID_E getAPIKey(char * buff, int bufflen, char * result){
	ATLIBGS_MSG_ID_E r;
	uint8_t cid = 0;
	int len;
	jsmn_parser parser;
	jsmntok_t tokens[20];
	jsmnerr_t jr;

	jsmn_init(&parser);

	r = makeAPICall(&cid, buff, apikey_header, password_hash);
	if (r != ATLIBGS_MSG_ID_OK) {
		ConsolePrintf("Unable to make API call\r\n");
		return r;
	}
 
	r = readOnePacket(buff, bufflen, &len, 5000);
	if (r != ATLIBGS_MSG_ID_OK){ 
		ConsolePrintf("Error retrieving response %d\r\n", r);
		return r;
	}
	ConsolePrintf("Got %d bytes\r\n",len);
	AtLibGs_Close(cid);
	char * jsonpos = strstr(buff, "\r\n\r\n")+4;
	if (jsonpos >= buff+strlen(buff)){
		ConsolePrintf("Could not find a body to the response: %s\r\n", buff);
		return ATLIBGS_MSG_ID_INVALID_INPUT;
	}
	ConsolePrintf("APIkey JSON: %s\r\n",jsonpos);
	jr = jsmn_parse(&parser, jsonpos, tokens, 20);
	if (jr != JSMN_SUCCESS){
		ConsolePrintf("Error parsing json: %d\r\n", jr);
		return ATLIBGS_MSG_ID_INVALID_INPUT;
	}

	r = ATLIBGS_MSG_ID_INVALID_INPUT;

	for (int i=0;i<20;i++){
		if ((tokens[i].type == 3) && (tokens[i].size == 0) && 
					(tokens[i].end-tokens[i].start == 3)){
			if (strncmp(jsonpos + tokens[i].start, "key", 3) == 0){
				ConsolePrintf("Found APIKEY \"%.40s\"\r\n",jsonpos+tokens[i+1].start);
				strncpy(result, jsonpos+tokens[i+1].start, 40);
				r = ATLIBGS_MSG_ID_OK;
				break;
			}
		} 
	}
	if (r == ATLIBGS_MSG_ID_INVALID_INPUT){
		ConsolePrintf("Unable to find 'key' in response\r\n");
	}

	return r;
}

/*  Helper function for all HTTP requests agaisnt api.bugswarm.net
	Can be used like sprintf, using buff as scratchpad space for the packet  */
ATLIBGS_MSG_ID_E makeAPICall(uint8_t * cid, char * buff, const char *format, ...){
	ATLIBGS_MSG_ID_E r;
	int len;
	va_list args;
    va_start(args, format);

	//Open a new socket only, will not send any request
	r = AtLibGs_TCPClientStart((char *)SwarmHost, 80, cid);
	ConsolePrintf("Opened a socket:  %d,%d\r\n",r,*cid);
	if ((r != ATLIBGS_MSG_ID_OK) || (*cid == ATLIBGS_INVALID_CID)){
		ConsolePrintf("Unable to connect to TCP socket\r\n");
		DisplayLCD(LCD_LINE8, "API socket err");
		return r;
	}

	ConsolePrintf("Socket open, sending headers\r\n");
  
	len = vsprintf((char *)buff, format, args);
	//ConsolePrintf("Attempting to send: %s", buff);
	//Reset the RX buffer in anticipation of the response packet.
	App_PrepareIncomingData();      
	r = AtLibGs_SendTCPData(*cid, (uint8_t *)buff, len);
	//ConsolePrintf("Sent headers: %d\r\n", r);
	if (r != ATLIBGS_MSG_ID_OK) {
		ConsolePrintf("Error transmitting headers to server (%d)%s\r\n", r, buff);
		DisplayLCD(LCD_LINE8, "API send err");
		return r;
	}
	//AtLibGs_Close(*cid);
	return ATLIBGS_MSG_ID_OK;
}

/*  The production loop for the swarm connector
	reads sensor data and transmits it to bugswarm
	will exit on error, otherwise will continue forever
	needs the cid of a valid production session			*/
ATLIBGS_MSG_ID_E App_SwarmProducer(uint8_t cid) {
	ATLIBGS_MSG_ID_E r;
	uint16_t value;
	float temp, tempF;
	extern int16_t	gAccData[3];
	int idx = 0;

	/*  Read from each sensor and transmit data to swarm
		Make sure to readForAtLeast inbetween each transmission to the server
		all waiting data must be read before transmitting a packet.
		We stagger each sensor to reduce server load  */		
	ConsolePrintf("Temp: ");
	value = Temperature_Get();
	//value returned is in celcius, needs to be scaled to make floating pt
	//2.5 is an approximate measure of board self-heating, tested
	//with a temperature probe above the IC...
	temp = (((float)value)/128.0)-2.5;  
	tempF = (temp*1.8)+32.0;
	ConsolePrintf("%0.1fC %0.1fF\r\n", temp, tempF);
	r = produce(cid, "{\"name\":\"Temperature\",\"feed\":{\"TempF\":%0.2f}}",
				tempF);
	if (r > 1)
		return r;
	readForAtLeast(cid, UPDATE_PERIOD/NUM_SENSOR);

	ConsolePrintf("Light: ");
	value = LightSensor_Get();
	ConsolePrintf("%u\r\n", value);
	r = produce(cid, "{\"name\":\"Light\",\"feed\":{\"Value\":%u}}",
				value);
	if (r > 1)
		return r;
	sprintf(msg, "T: %0.2fF, L: %u",tempF, value);
	//DisplayLCD(LCD_LINE5, msg);
	readForAtLeast(cid, UPDATE_PERIOD/NUM_SENSOR);

	ConsolePrintf("Accel: ");
	Accelerometer_Get();
	ConsolePrintf("(%d, %d, %d)\r\n", gAccData[0], gAccData[1], gAccData[2]);	
	r = produce(cid, "{\"name\":\"Acceleration\","
				"\"feed\":{\"x\":%0.2f,\"y\":%0.2f,\"z\":%0.2f}}",
				(float)gAccData[0]/33.0, (float)gAccData[1]/33.0, (float)gAccData[2]/30.0);
	if (r > 1)
		return r;
	sprintf(msg, "A(%0.2f,%0.2f,%0.2f)",
			(float)gAccData[0]/33.0, (float)gAccData[1]/33.0, (float)gAccData[2]/30.0);
	//DisplayLCD(LCD_LINE6, msg);
	readForAtLeast(cid, UPDATE_PERIOD/NUM_SENSOR);

	ConsolePrintf("Mic: ");
	//value = Microphone_Get();	
	//value = Microphone_Get();
	//mic_level = abs((int)(value/4)-493);
	ConsolePrintf("%u, %04x\r\n", mic_level, value);		
	r = produce(cid, "{\"name\":\"Sound Level\",\"feed\":{\"Raw\":%u}}",
				mic_level);
	mic_level = 0;
	if (r > 1)
		return r;
	readForAtLeast(cid, UPDATE_PERIOD/NUM_SENSOR);

	ConsolePrintf("Pot: ");
	value = Potentiometer_Get();
	value = Potentiometer_Get();
	ConsolePrintf("%u, %04x\r\n", value/4, value);
	r = produce(cid, "{\"name\":\"Potentiometer\",\"feed\":{\"Raw\":%u}}",
				value/4);
	if (r > 1)
		return r;
	sprintf(msg, "P: %u M: %u", value/4, mic_level);
	//DisplayLCD(LCD_LINE7, msg);
	readForAtLeast(cid, UPDATE_PERIOD/NUM_SENSOR);

	//Every 5th iteration of the loop, send a capabilities message
	//Not necessary for network portal, but for demo.bugswarm.net
	if (idx++%5 == 0){
	   produce(cid, feed_request);
	}
	return ATLIBGS_MSG_ID_OK;
}

/*  Produce a piece of data to swarm
	NOTE, the message produced MUST be valid JSON.  */
ATLIBGS_MSG_ID_E produce(uint8_t cid, const char *format, ...) {
	va_list args;
	int len;

	va_start(args, format);
	len = vsprintf((char *)msg, format, args);

	len = sprintf(pkt, message_header, len+sizeof(message_header)-9, msg);
	//ConsolePrintf("Sending: *%s*", pkt);
	tx++;
	return AtLibGs_SendTCPData(cid, (uint8_t *)pkt, len);
}

/*  Open a new socket to the swarm server and establish a streaming connection.
	requires a valid resource id, along with other swarm parameters  */
ATLIBGS_MSG_ID_E createProductionSession(uint8_t *cid, 
                                         char * hostIP,
                                         char * prod_swarm_id,
										 char * cons_swarm_id,
                                         char * resource_id,
                                         char * participation_key) {
	ATLIBGS_MSG_ID_E r;
	int len;

	/*//Connect to Wifi if we are not connected.
	if (!AtLibGs_IsNodeAssociated()) {
		ConsolePrintf("Connecting to %s...",G_nvsettings.webprov.ssid);
		r = App_Connect(&G_nvsettings.webprov);
		ConsolePrintf(" %d\r\n",r);
		//TODO - verify connection attempt against enum
	}*/

	//Open a new socket to the swarm server
	ConsolePrintf("Attempting to connect to %s:%d ...",hostIP, 80);
	App_PrepareIncomingData();    
	r = AtLibGs_TCPClientStart(hostIP, 80, cid);
	ConsolePrintf(" %d,%d\r\n",r,(*cid));
	if ((r != ATLIBGS_MSG_ID_OK) || ((*cid) == ATLIBGS_INVALID_CID)){
		ConsolePrintf("Unable to connect to TCP socket\r\n");
		DisplayLCD(LCD_LINE8, "produce socket err");
		return r;
	}
	//Clear the RX buffer in anticipation of the response  
	ConsolePrintf("Socket open, sending headers\r\n");
  
	//Transmit the swarm API request header to open a production session
	len = sprintf(pkt, produce_header, prod_swarm_id, cons_swarm_id, resource_id, participation_key);
	ConsolePrintf("Attempting to send: %s ...", pkt);
	r = AtLibGs_SendTCPData((*cid), (uint8_t *)pkt, len);
	ConsolePrintf("SENT: %d\r\n", r);
	if (r != ATLIBGS_MSG_ID_OK) {
		ConsolePrintf("Error transmitting headers to server\r\n");
		DisplayLCD(LCD_LINE8, "produce header err");
		AtLibGs_Close(*cid);
		return r;
	}

	//DisplayLCD(LCD_LINE8, "waiting for presence");
 
	//Wait up to 5 seconds for presence messages.
	//If we do not wait, the swarm server will have not opened a connection to
	//the message passing layer and an error will be thrown on production.
	r = readOnePacket(pkt, sizeof(pkt)-1, &len, 5000);
	if (r != ATLIBGS_MSG_ID_OK) {
		ConsolePrintf("Error, no server response\n");
		DisplayLCD(LCD_LINE8, "No Swarm resp");
		AtLibGs_Close(*cid);
		return ATLIBGS_MSG_ID_ERROR_SOCKET_FAIL;
	}
	readForAtLeast(*cid, 2000);

	ConsolePrintf("Done waiting for presence, connected to swarm!\r\n");
	//DisplayLCD(LCD_LINE4, "                  ");
	return ATLIBGS_MSG_ID_OK;
}

/*  Read a single packet from the wifi chipset into buff, waiting at least ms  
	If response is greater than len, it will be concatenated and the rest lost!
	resulting length written to 'written' */
ATLIBGS_MSG_ID_E readOnePacket(char * buff, int len, int * written, uint32_t ms){
	ATLIBGS_MSG_ID_E r;

	r = AtLibGs_WaitForTCPMessage(ms);
	if (r != ATLIBGS_MSG_ID_DATA_RX) {
		ConsolePrintf("Issue retrieving response: %u\r\n", r);
		return r;
	}
	AtLibGs_ParseTCPData(G_received, G_receivedCount, &tcp_pkt);
	//ConsolePrintf("*(%u)%s*",tcp_pkt.numBytes,tcp_pkt.message);    
	if (tcp_pkt.numBytes >= len){
		tcp_pkt.numBytes = len-1;
		ConsolePrintf("WARN: message greater than %d bytes, concatenated\r\n",len);
	}
	memset(buff, '\0', tcp_pkt.numBytes+1);
	memcpy(buff, tcp_pkt.message, tcp_pkt.numBytes);
	*written = tcp_pkt.numBytes;
	//Clear the RX buffer for the next reception.
	App_PrepareIncomingData();      
	return ATLIBGS_MSG_ID_OK;
}

/*  read TCP packets from the wifi chipset for at least ms
	This is a necessary worker loop to keep the RX buffer clear while still
	allowing for other tasks to run on the MCU.
	Make sure to run readForAtLeast in place of a sleep or delay whenever the
	device has an active TCP connection.  */
void readForAtLeast(uint8_t cid, uint32_t ms){
	//uint32_t curr = MSTimerGet();
  	uint32_t end;
	uint16_t value;
	//ConsolePrintf("**** %lu + %lu = %lu ****\n", MSTimerGet(), ms, end);
	ATLIBGS_MSG_ID_E r = ATLIBGS_MSG_ID_RESPONSE_TIMEOUT;
//	char * switches;
	//WHILE: bytes are waiting in buffer, WaitForTCPMessage has a new message,
	//		or it hasn't been ms milliseocnds:
	//WARN - WaitForTCPMessage() only works for TCP data, doesn't find DISCON
	// //(AtLibGs_WaitForTCPMessage(10) == ATLIBGS_MSG_ID_DATA_RX) ||
	end = MSTimerGet()+ms;
	while ((G_receivedCount) ||
		   (r != ATLIBGS_MSG_ID_RESPONSE_TIMEOUT) ||
		   (MSTimerGet() < end)){
		//ConsolePrintf("%lu-%lu-%d \n", end, MSTimerGet(), (end > MSTimerGet()));
		//ConsolePrintf("%08lX\n",MSTimerGet());
		r = AtLibGs_ReceiveDataHandle(10);
		//SOCKET_FAIL seems to be thrown on disconnect...
		if ((r == ATLIBGS_MSG_ID_DISCONNECT)||(r == ATLIBGS_MSG_ID_ERROR_SOCKET_FAIL)) {
			//ConsolePrintf("Socket disconnected!\n");
			connected = false;
		} else if (r == ATLIBGS_MSG_ID_DATA_RX) {
			AtLibGs_ParseTCPData(G_received, G_receivedCount, &tcp_pkt);
			//ConsolePrintf("*(%u)*",tcp_pkt.numBytes);                
			if (tcp_pkt.numBytes >= sizeof(pkt)){
				tcp_pkt.numBytes = sizeof(pkt)-1;
				ConsolePrintf("WARN: message greater than %d bytes, concatenated\r\n",sizeof(pkt));
			}
			memset(pkt, '\0', sizeof(pkt));
			memcpy(pkt, tcp_pkt.message, tcp_pkt.numBytes);
			parseMessage(pkt, cid);
			rx++;
			App_PrepareIncomingData();
		} else if (r == ATLIBGS_MSG_ID_RESPONSE_TIMEOUT) {
			//NO messages to deal with, do some auxillary work.
			value = Microphone_Get();	
			value = Microphone_Get();
			value = abs((int)(value/4)-493);
			if (value > mic_level){
				mic_level = value;
			}
			if (checkSwitches((char *)&value)) {
				ConsolePrintf("Switch Update Sw1: %u * SW2: %u * SW3: %u\r\n",Switch1IsPressed(), Switch2IsPressed(), Switch3IsPressed());
				produce(cid, "{\"name\":\"Button\",\"feed\":{\"b1\":%u,\"b2\":%u,\"b3\":%u}}",
					Switch1IsPressed(),Switch2IsPressed(),Switch3IsPressed());
			}
			if (MSTimerGet() > tone_stop) {
				//R_TAU0_Channel0_Freq(0);
				R_TAU0_Channel0_Stop();
			}
			if (rx != rxold) {
				sprintf(msg, "RX: %lu",rx);
				DisplayLCD(LCD_LINE6, (const uint8_t *)msg);
				rxold = rx;
			}
			if (tx != txold) {
				sprintf(msg, "TX: %lu",tx);
				DisplayLCD(LCD_LINE7, (const uint8_t *)msg);
				txold = tx;
			}
		} else {
			ConsolePrintf("Unknown event thrown: %02X\n",r);
		}
		//curr = MSTimerGet();
	} 
}

void parseMessage(char * pkt, uint8_t cid){
	int ret;
	char token[50];
	char * jsonpos;
	char * tokpos;
	int start;
	int end;
	jsmn_parser parser;
	jsmntok_t tokens[40];
	jsmnerr_t jr;
	int val;

	jsmn_init(&parser);

	//ConsolePrintf("\n*(%u)%s*",strlen(pkt),pkt);
	
	jsonpos = strchr(pkt, '{');
	if (jsonpos == NULL) {
		return;
	}
	//ConsolePrintf("\n%s\n",jsonpos);
	
	jr = jsmn_parse(&parser, jsonpos, tokens, 40);
	if (jr != JSMN_SUCCESS){
		ConsolePrintf("Error parsing json: %d\r\n", jr);
		return;
	}

	ret = findKey(jsonpos, tokens, 40, "name");
	if ((ret < 0) || (tokens[ret+1].type != JSMN_STRING)) {
		ConsolePrintf("Couldn't find Name of feed\n");
		return;
	}
	tokpos = jsonpos+tokens[ret+1].start;
	ret = findKey(jsonpos, tokens, 40, "feed");
	if ((ret < 0) || (tokens[ret+1].type != JSMN_OBJECT)) {
		ConsolePrintf("Couldn't find Feed in payload\n");
		return;
	}
	start = tokens[ret+1].start;
	end = tokens[ret+1].end;
	if (strncmp(tokpos, "LED", 3) == 0) {
		//ConsolePrintf("LED command: %s\n", jsonpos+tokens[ret+1].start);
		for (int i=0;i<40;i++){
			if (tokens[i].type == JSMN_STRING) {
				if ((tokens[i].start < start)||(tokens[i].end > end))
					continue;
				if (strncmp(jsonpos+tokens[i].start, "led", 3) == 0) {
					val = atoi(jsonpos+tokens[i].start+3);
					if (jsonpos[tokens[i+1].start] == 't') {
						ConsolePrintf("LED %d ON\n", val);
						led_on(val);
					} else {
						ConsolePrintf("LED %d OFF\n", val);
						led_off(val);
					}
				}
			}
		}
		produce(cid, "{\"name\":\"LED\",\"feed\":{"
			"\"led0\":%s,\"led1\":%s,\"led2\":%s,\"led3\":%s,\"led4\":%s,"
			"\"led5\":%s,\"led6\":%s,\"led7\":%s,\"led8\":%s,\"led9\":%s,"
			"\"led10\":%s,\"led11\":%s,\"led12\":%s}}",
			ledValue(0), ledValue(1), ledValue(2), ledValue(3), ledValue(4),
			ledValue(5), ledValue(6), ledValue(7), ledValue(8), ledValue(9),
			ledValue(10), ledValue(11), ledValue(12));
	} else if (strncmp(tokpos, "LCD", 3) == 0) {
		ret = findKey(jsonpos, tokens, 40, "text");
		if (ret < 0)
			return;
		tokpos = jsonpos+tokens[ret+1].start;
		memset(token, '\0', sizeof(token));
		strncpy(token, tokpos, (tokens[ret+1].end-tokens[ret+1].start));
		ConsolePrintf("LCD text: %s\n", token);
		DisplayLCD(LCD_LINE8, (const uint8_t *)token);
	} else if (strncmp(tokpos, "Eink", 4) == 0) {
		//ConsolePrintf("Eink command: %s\n", jsonpos+tokens[ret+1].start);
		
		//We should use the following code for EINK display, however set/clear
		//Logo() do not work properly.  eink_driver/user_app need to be
		//modified such that setLogo and clearLogo() actually work on a
		//consistent logo index.  Currently only 0-2 work, the rest produce
		//unpredictable results.
		/*
		for (int i=0;i<40;i++){
			if (tokens[i].type == JSMN_STRING) {
				if ((tokens[i].start < start)||(tokens[i].end > end))
					continue;
				if (strncmp(jsonpos+tokens[i].start, "eink", 4) == 0) {
					val = atoi(jsonpos+tokens[i].start+4);
					if (jsonpos[tokens[i+1].start] == 't') {
						ConsolePrintf("EINK %d ON\n", val);
						setLogo(val);
					} else {
						ConsolePrintf("EINK %d OFF\n", val);
						clearLogo(val);
					}
				}
			}
		}*/
		//Instead, use doDemo, which is tested but limited.
		ret = findKey(jsonpos, tokens, 40, "demo");
		if (ret < 0)
			return;
		tokpos = jsonpos+tokens[ret+1].start;
		val = atoi(tokpos);
		ConsolePrintf("displaying eink demo %d\n", val);
		doDemo(val);
	} else if (strncmp(tokpos, "Beep", 4) == 0) {
		//ConsolePrintf("Beep command: %s\n", jsonpos+tokens[ret+1].start);
		ret = findKey(jsonpos, tokens, 40, "freq");
		if (ret < 0)
			return;
		tokpos = jsonpos+tokens[ret+1].start;
		val = atoi(tokpos);
		ConsolePrintf("Beep at Freq %d ", val);
		R_TAU0_Channel0_Freq(val);
		ret = findKey(jsonpos, tokens, 40, "duration");
		if (ret < 0)
			return;
		tokpos = jsonpos+tokens[ret+1].start;
		val = atoi(tokpos);
		ConsolePrintf("for %d milliseconds\n", val);
		tone_stop = MSTimerGet()+val;
		R_TAU0_Channel0_Start();
	}
	ret = findKey(jsonpos, tokens, 40, "payload");
	if (ret < 0)
		return;
	tokpos = jsonpos+tokens[ret+1].start;
	memset(pkt, '\0', (tokens[ret+1].end-tokens[ret+1].start)+2);
	strncpy(pkt, tokpos, (tokens[ret+1].end-tokens[ret+1].start));
	ConsolePrintf("Payload: %s\n", pkt);
	produce(cid, "%s", pkt);
}

const char * ledValue(int idx) {
	if (led_get(idx) == 1) {
		return true_val;
	} else {
		return false_val;
	}
}

char * getValue(char * jsonpos, jsmntok_t * tokens, int toklen, const char * key) {
	char * ret = NULL;
	int pos = findKey(jsonpos, tokens, toklen, key);
	if (pos < 0)
		return ret;
	ret = jsonpos+tokens[pos+1].start;
	return ret;
}

int findKey(char * jsonpos, jsmntok_t * tokens, int toklen, const char * key) {
	int ret = -1;
	for (int i=0;i<toklen;i++){
		if (tokens[i].type == JSMN_STRING) {
			if (strncmp(jsonpos+tokens[i].start, key, strlen(key)) == 0) {
				return i;
			}
		} 
	}
	return ret;
}