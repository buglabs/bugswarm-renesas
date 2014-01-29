
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
#define THING_ID_EEPROM_LOC 128
#define JSON_TOKEN_MAX 20

char dweetIP[24];
const char postDweetFormat[] = "POST /dweet%s HTTP/1.1\r\nHost: %s\r\nAccept: application/json\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n";
const char getDweetFormat[] = "GET /get/latest/dweet/for/%s-send HTTP/1.1\r\nHost: %s\r\nAccept: application/json\r\n\r\n";

const char true_val[] = "true";
const char false_val[] = "false";

char	animation[] = " -\0 \\\0 |\0 /\0";
char    thingID[32];
char	lastUpdate[32];

jsmn_parser parser;
jsmntok_t tokens[JSON_TOKEN_MAX];
jsmnerr_t jr;

static char dataToSend[256];
static char 	msg[512];

bool    connected;
ATLIBGS_TCPMessage tcp_pkt;

/*----------------------------------------------------------------------------*
*	Routine: App_SwarmConnector
*----------------------------------------------------------------------------*
*	Description:
*		Run the swarm connector demo indefinitely
*----------------------------------------------------------------------------*/

void App_SwarmConnector(void)
{
	thingID[0] = NULL;
	lastUpdate[0] = NULL;
	msg[0] = NULL;
	
	// If switch 3 is pressed, reset the EEPROM
	if(Switch3IsPressed())
	{
		EEPROM_Write(THING_ID_EEPROM_LOC, "\0", 1);
	}
	else
	{
	  // Do we have a thingID saved to our EEPROM?
	  EEPROM_Read(THING_ID_EEPROM_LOC, msg, sizeof(thingID));
	  
	  if(msg[0] == '!')
	  {
		  memcpy(thingID, msg + 1, sizeof(thingID));
		  msg[0] = NULL;
	  }
	}
	
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
	
	if(thingID[0] == NULL)
	{
		DisplayLCD(LCD_LINE5, "");
	}
	else
	{
		DisplayLCD(LCD_LINE4, "thing:");
		DisplayLCD(LCD_LINE5, thingID);
	}
	
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
			
			r = checkData(cid);
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

ATLIBGS_MSG_ID_E checkData(uint8_t cid)
{
	ATLIBGS_MSG_ID_E r;
	
	App_PrepareIncomingData(); 
	
	sprintf(msg, getDweetFormat, thingID, DWEET_HOST);
	
	r = AtLibGs_TCPClientStart((char *)dweetIP, 80, &cid);
	if ((r != ATLIBGS_MSG_ID_OK) || (cid == ATLIBGS_INVALID_CID))
	{
		return r;
	}
	
	App_PrepareIncomingData(); 
	AtLibGs_SendTCPData(cid, (uint8_t *)msg, strlen(msg));
	if (r != ATLIBGS_MSG_ID_OK)
	{
		AtLibGs_Close(cid);
		return r;
    }
	
	r = AtLibGs_WaitForTCPMessage(5000);
	if (r != ATLIBGS_MSG_ID_DATA_RX)
	{
		AtLibGs_Close(cid);
		return r;
	}
	
	AtLibGs_ParseTCPData(G_received, G_receivedCount, &tcp_pkt);
	
	*((char*)tcp_pkt.message[tcp_pkt.numBytes - 1]) = '\0';
	char* body = strstr(tcp_pkt.message, "\r\n\r\n");
	
	jsmn_init(&parser);
	
	jr = jsmn_parse(&parser, body, tokens, JSON_TOKEN_MAX);
	if (jr != JSMN_SUCCESS)
	{
		AtLibGs_Close(cid);
		return ATLIBGS_MSG_ID_OK;
	}
	
	if(getValue(body, tokens, JSON_TOKEN_MAX, "this", msg) && strcmp(msg, "succeeded") == 0)
	{
		if(getValue(body, tokens, JSON_TOKEN_MAX, "created", msg) && strcmp(msg, lastUpdate) != 0)
		{	
			strcpy(lastUpdate, msg); // Hold on to the update time so we don't re-use this
			
			if(getValue(body, tokens, JSON_TOKEN_MAX, "lcd_text", msg))
			{
				DisplayLCD(LCD_LINE7, msg);
			}
			
			if(getValue(body, tokens, JSON_TOKEN_MAX, "beep", msg))
			{
				R_TAU0_Channel0_Freq(1000);
				R_TAU0_Channel0_Start();
				MSTimerDelay(2000);
				R_TAU0_Channel0_Stop();
			}
		}
	}
	
	AtLibGs_Close(cid);
	
	return ATLIBGS_MSG_ID_OK;
}

ATLIBGS_MSG_ID_E dweetData(uint8_t cid)
{
	ATLIBGS_MSG_ID_E r;
	
	uint16_t iValue;
	float fValue;
	extern int16_t gAccData[3];
	
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
	addJSONKeyNumberValue(dataToSend, "x_tilt", gAccData[0] / 33.0 * 90);
	addJSONKeyNumberValue(dataToSend, "y_tilt", gAccData[1] / 33.0 * 90);
	addJSONKeyNumberValue(dataToSend, "z_tilt", gAccData[2] / 30.0 * 90);
	
	// Get our Buttons
	addJSONKeyNumberValue(dataToSend, "btn_1", Switch1IsPressed());
	addJSONKeyNumberValue(dataToSend, "btn_2", Switch2IsPressed());
	addJSONKeyNumberValue(dataToSend, "btn_3", Switch3IsPressed());
	
	// Get our pot
	addJSONKeyNumberValue(dataToSend, "pot", Potentiometer_Get());
	
	strcat(dataToSend, "}");
	
	char thingPath[32];
	
	if(thingID[0] != NULL)
	{
		sprintf(thingPath, "/for/%s", thingID);
	}
	else
	{
		thingPath[0] = '\0';
	}
	
	sprintf(msg, postDweetFormat, thingPath, DWEET_HOST, strlen(dataToSend) + 2, dataToSend);
	
	r = AtLibGs_TCPClientStart((char *)dweetIP, 80, &cid);
	if ((r != ATLIBGS_MSG_ID_OK) || (cid == ATLIBGS_INVALID_CID))
	{
		return r;
	}
	
	App_PrepareIncomingData(); 
	r = AtLibGs_SendTCPData(cid, (uint8_t *)msg, strlen(msg));
	if (r != ATLIBGS_MSG_ID_OK)
	{
		AtLibGs_Close(cid);
		return r;
    }
	
	r = AtLibGs_WaitForTCPMessage(5000);
	if (r != ATLIBGS_MSG_ID_DATA_RX)
	{
		AtLibGs_Close(cid);
		return r;
	}
	
	AtLibGs_ParseTCPData(G_received, G_receivedCount, &tcp_pkt);
	App_PrepareIncomingData();
	
	// If we don't have a thing. Get it.
	if(thingID[0] == NULL)
	{ 
	  *((char*)tcp_pkt.message[tcp_pkt.numBytes - 1]) = '\0';
	  char* body = strstr(tcp_pkt.message, "\r\n\r\n");
	  
	  jsmn_init(&parser);
	  
	  jr = jsmn_parse(&parser, body, tokens, JSON_TOKEN_MAX);
	  if (jr != JSMN_SUCCESS)
	  {
		  AtLibGs_Close(cid);
		  return ATLIBGS_MSG_ID_OK;
	  }
	  
	  if(getValue(body, tokens, JSON_TOKEN_MAX, "thing", thingID))
	  {
		  DisplayLCD(LCD_LINE4, "thing:");
		  DisplayLCD(LCD_LINE5, thingID);
		  
		  // Save it to EEPROM
		  EEPROM_Write(THING_ID_EEPROM_LOC, "!", 1);
		  EEPROM_Write(THING_ID_EEPROM_LOC + 1, thingID, strlen(thingID) + 1);
	  }
	}
	 
	AtLibGs_Close(cid);
	
	return ATLIBGS_MSG_ID_OK;
}

int getValue(char * jsonpos, jsmntok_t * tokens, int toklen, const char * key, char* outValue)
{
	outValue[0] = NULL;
	
	int tokenIndex = findKey(jsonpos, tokens, toklen, key);
	
	if (tokenIndex < 0)
	{
		return 0;
	}
	
	int tokenLen = tokens[tokenIndex+1].end - tokens[tokenIndex+1].start;
	memcpy(outValue, jsonpos + tokens[tokenIndex+1].start, tokenLen);
	outValue[tokenLen] = NULL;  // Add our string term character
	
	return 1;
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