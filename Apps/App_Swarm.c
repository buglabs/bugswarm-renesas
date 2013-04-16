
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <system/platform.h>
#include <NVSettings.h> 
#include <system/Switch.h>
#include <drv/Glyph/lcd.h>
#include <mstimer.h>
#include <system/console.h>
#include "Apps.h"
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
//These parameters must be hardcoded, are not retrieved.
const char participation_key[] = "bc60aa60d80f7c104ad1e028a5223e7660da5f8c";
const char configuration_key[] = "359aff0298658552ec987b9354ea754b684a4047";
const char swarm_id[] = "69df1aea11433b3f85d2ca6e9c3575a9c86f8182";
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
    "{\"name\":\"%s\",\"swarmid\":\"%s\",\"description\":"
    "\"an RL78G14 board running bugswarm-renesasG14 1.0\"}";
const char list_resources_header[] = "GET /resources HTTP/1.1\r\n"
	"Host: api.bugswarm.net\r\nAccept: */*\r\nx-bugswarmapikey: %s\r\n\r\n";
const char apikey_header[] = "GET /keys/configuration HTTP/1.1\r\n"
"Authorization: Basic %s\r\nHost: api.bugswarm.net\r\nAccept: */*\r\n\r\n";
const char produce_header[] = "POST /stream?swarm_id=%s&"
  "resource_id=%s HTTP/1.1\r\nHost:api.bugswarm.com\r\n"
  "x-bugswarmapikey:%s\r\ntransfer-encoding:chunked\r\nConnection:keep-alive"
  "\r\nContent-Type: application/json\r\n\r\n1\r\n\n\r\n";
const char feed_request[] = "{\"capabilities\": {\"feeds\": [\"Acceleration\","
  "\"Temperature\",\"Button\",\"Light\",\"Sound Level\",\"Potentiometer\"]}}";
const char message_header[] = "%x\r\n{\"message\": {\"payload\":%s}}\r\n\r\n";

bool connected;



/*----------------------------------------------------------------------------*
 *	Routine: App_SwarmConnector
 *----------------------------------------------------------------------------*
 *	Description:
 *		Run the swarm connector demo indefinitely
 *----------------------------------------------------------------------------*/
void App_SwarmConnector(void) {
	char MACaddr[20];
	char MACstr[13];
	char resource_id[40];
	ATLIBGS_MSG_ID_E r;

	App_InitModule();
	while(1) {
    	r = AtLibGs_GetMAC(MACaddr);
		if(r != ATLIBGS_MSG_ID_OK) {
			ConsolePrintf("Couldn't get MAC: %02X\n",r);
			DisplayLCD(LCD_LINE6, "Get MAC Failed!");
			MSTimerDelay(1000);
			continue;
		} 
		break;
    } 
    AtLibGs_ParseGetMacResponse(MACstr);
    ConsolePrintf("Module MAC Address: %s\r\n", MACstr);
	ConsolePrintf("Connecting to AP...");
    App_aClientConnection();		//Will block until connected
	ConsolePrintf("Connected.\n");

    //TODO - Check EEPROM for a cached resourceid,
    //       possibly load preset swarm credentials.
    r = getResourceID(MACstr, resource_id);
    if (r != ATLIBGS_MSG_ID_OK) {
    	//Dosomething aboutit
    }

    MSTimerDelay(100);
    connected = false;

	while(1) {
		if (!AtLibGs_IsNodeAssociated())
			App_aClientConnection();
		ConsolePrintf("Creating a production session\r\n");

		r = createProductionSession();//...
		if (r != ATLIBGS_MSG_ID_OK) {
			ConsolePrintf("Unable to open production session (%02X), retrying\n",r);
			connected = false;
			MSTimerDelay(1000);
			continue;
		}
		ConsolePrintf("Connected to Swarm\n");
		connected = true;
		while (connected) {
			//r = produceData(cid);
		}
		ConsolePrintf("Disconnected!\n");
	}	
}

ATLIBGS_MSG_ID_E getResourceID(char * macstr, char * resource_id) {
	return ATLIBGS_MSG_ID_OK;
}

ATLIBGS_MSG_ID_E createProductionSession() {//...
	return ATLIBGS_MSG_ID_OK;
}

ATLIBGS_MSG_ID_E produceData(uint8_t cid) {
	return ATLIBGS_MSG_ID_OK;
}