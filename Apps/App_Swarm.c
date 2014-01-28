
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
#include <drv/EInk/eink_driver.h>
#include <drv/EInk/user_app.h>
#include "App_Swarm.h"

#define UPDATE_PERIOD 5000
#define NUM_SENSOR 5
#define CAPABILITIES_PERIOD 10000
#define MAX_PROD_ERRORS 5

#define CLIENT_VER	"Swarm Client R0.6.1"
/*-------------------------------------------------------------------------*
* Constants:
*-------------------------------------------------------------------------*/
uint16_t period = NUM_SENSOR;

/*  Default swarm connection parameters  */
#define DWEET_HOST "dweet.io"
#define DWEET_HOST_FALLBACK_IP "216.178.216.62"
static char dweetIP[24];
const char dweetRequestFormat[] = "POST /dweet%s HTTP/1.1\r\nHost: %s\r\nAccept: application/json\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n";
const char true_val[] = "true";
const char false_val[] = "false";

char	animation[] = " -\0 \\\0 |\0 /\0";
char    thingPath[32];
char 	msg[512];
bool    connected;
ATLIBGS_TCPMessage tcp_pkt;

/*----------------------------------------------------------------------------*
*	Routine: App_SwarmConnector
*----------------------------------------------------------------------------*
*	Description:
*		Run the swarm connector demo indefinitely
*----------------------------------------------------------------------------*/

void App_SwarmConnector(void) {
	
	thingPath[0] = NULL;
	
	ATLIBGS_MSG_ID_E r;
	uint8_t cid = 0;
	
	initEink();
	setLogo(11);
	
	R_TAU0_Create();
	
	App_InitModule();
	
	DisplayLCD(LCD_LINE1, "");
	DisplayLCD(LCD_LINE2, "dweet.io");
	DisplayLCD(LCD_LINE3, "");
	DisplayLCD(LCD_LINE4, "");
	DisplayLCD(LCD_LINE5, "");
	DisplayLCD(LCD_LINE6, "");
	DisplayLCD(LCD_LINE7, "");
	DisplayLCD(LCD_LINE8, "connecting.......");
	
	App_aClientConnection();		//Will block until connected
	AtLibGs_SetNodeAssociationFlag();
	memset(msg, '\0', sizeof(msg));
	
	// Get DNS for our API server
	if(AtLibGs_DNSLookup(DWEET_HOST, 2, 5) == ATLIBGS_MSG_ID_OK)
	{
		AtLibGs_ParseDNSLookupResponse(dweetIP);
		*(strstr(dweetIP,"\r\nOK")) = '\0';   // Parse doesn't remove "OK" at end of string??
	}
	else
	{
		strcpy(dweetIP, DWEET_HOST_FALLBACK_IP);
	}
	
	led_all_off();
	
	
	Temperature_Init();
	Potentiometer_Init();
	Accelerometer_Init();
	
	MSTimerDelay(100);
	connected = false;
	
	char* animationIndex = animation;
	
	while(1) {
		if (!AtLibGs_IsNodeAssociated()) {
			DisplayLCD(LCD_LINE8, "connecting.......");
			App_aClientConnection();
			AtLibGs_SetNodeAssociationFlag();
		}
		
		connected = true;
		while (connected)
		{
			DisplayLCD(LCD_LINE1, "visit:");
			DisplayLCD(LCD_LINE2, "dweet.io/follow");
			DisplayLCD(LCD_LINE4, "thing:");
			
			DisplayLCD(LCD_LINE8, animationIndex);
			
			animationIndex += strlen(animationIndex) + 1;
			if(animationIndex >= animation + sizeof(animation) - 1)
			{
				animationIndex = animation;
			}
			
			r = dweetData(cid);
			if (r != ATLIBGS_MSG_ID_OK)
			{
				connected = false;
			}
			
			MSTimerDelay(1000);
		}
		
		AtLibGs_Close(cid);
		DisplayLCD(LCD_LINE8, "connecting.......");
		MSTimerDelay(5000);		//TODO - exponential backoff
	}	
}

void addJSONKeyNumberValue(char* jsonString, const char* keyName, float value)
{
	int pos = strlen(jsonString);
	
	if(jsonString[pos - 1] != '{')
	{
		strcat(jsonString, ",");
		pos++;
	}
	
	sprintf(jsonString + pos, "\"%s\":%.2g", keyName, value);
}

ATLIBGS_MSG_ID_E dweetData(uint8_t cid)
{
	ATLIBGS_MSG_ID_E r;
	
	uint16_t iValue;
	float fValue;
	extern int16_t gAccData[3];
	
	char dataToSend[256];
	dataToSend[0] = '{';
	dataToSend[1] = '\0';
	
	// Get our temp
	iValue = Temperature_Get();
	fValue = (((float)iValue) / 128.0) - 2.5;  
    fValue = (fValue * 1.8) + 32.0;
	
	addJSONKeyNumberValue(dataToSend, "temp", fValue);
	
	// Get our light
	iValue = LightSensor_Get();
	addJSONKeyNumberValue(dataToSend, "light", iValue);
	
	// Get our accelerometer
	Accelerometer_Get();
	addJSONKeyNumberValue(dataToSend, "x-tilt", gAccData[0] / 33.0 * 90);
	addJSONKeyNumberValue(dataToSend, "y-tilt", gAccData[1] / 33.0 * 90);
	addJSONKeyNumberValue(dataToSend, "z-tilt", gAccData[2] / 30.0 * 90);
	
	// Get our Buttons
	addJSONKeyNumberValue(dataToSend, "btn-1", Switch1IsPressed());
	addJSONKeyNumberValue(dataToSend, "btn-2", Switch2IsPressed());
	addJSONKeyNumberValue(dataToSend, "btn-3", Switch3IsPressed());
	
	// Get our pot
	addJSONKeyNumberValue(dataToSend, "pot", Potentiometer_Get());
	
	strcat(dataToSend, "}");
	
	sprintf(msg, dweetRequestFormat, thingPath, DWEET_HOST, strlen(dataToSend) + 2, dataToSend);
	
	r = AtLibGs_TCPClientStart((char *)dweetIP, 80, &cid);
	
	AtLibGs_SendTCPData(cid, (uint8_t *)msg, strlen(msg));
	
	
	r = AtLibGs_WaitForTCPMessage(5000);
	if (r != ATLIBGS_MSG_ID_DATA_RX) {
		return r;
	}
	AtLibGs_ParseTCPData(G_received, G_receivedCount, &tcp_pkt);
	
	*((char*)tcp_pkt.message[tcp_pkt.numBytes - 1]) = '\0';
	char* body = strstr(tcp_pkt.message, "\r\n\r\n");
	
	jsmn_parser parser;
	jsmntok_t tokens[40];
	jsmnerr_t jr;
	
	jsmn_init(&parser);
	
	jr = jsmn_parse(&parser, body, tokens, 40);
	if (jr != JSMN_SUCCESS){
		ConsolePrintf("Error parsing json: %d\r\n", jr);
		return ATLIBGS_MSG_ID_OK;
	}
	
	int tokenIndex = findKey(body, tokens, 40, "thing");
	if ((tokenIndex < 0) || (tokens[tokenIndex+1].type != JSMN_STRING)) {
		ConsolePrintf("Couldn't find Name of feed\n");
		return ATLIBGS_MSG_ID_OK;
	}
	
	
	int tokenLen = tokens[tokenIndex+1].end - tokens[tokenIndex+1].start;
	
	char thingID[32];
	memcpy(thingID, body + tokens[tokenIndex+1].start, tokenLen);
	thingID[tokenLen] = NULL;
	
	DisplayLCD(LCD_LINE5, thingID);
	
	sprintf(thingPath, "/for/%s", thingID);
	
	App_PrepareIncomingData();  
	AtLibGs_Close(cid);
	
	return ATLIBGS_MSG_ID_OK;
}

/*  Read a single packet from the wifi chipset into buff, waiting at least ms  
If response is greater than len, it will be concatenated and the rest lost!
resulting length written to 'written' */
ATLIBGS_MSG_ID_E readResponse(char * buff, int len, int * written, uint32_t ms)
{
	ATLIBGS_MSG_ID_E r;
	
	r = AtLibGs_WaitForTCPMessage(ms);
	if (r != ATLIBGS_MSG_ID_DATA_RX)
	{
		return r;
	}
	AtLibGs_ParseTCPData(G_received, G_receivedCount, &tcp_pkt); 
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

const char * ledValue(int idx)
{
	if (led_get(idx) == 1)
	{
		return true_val;
	} 
	else
	{
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