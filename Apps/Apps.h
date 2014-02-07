/*-------------------------------------------------------------------------*
 * File:  Apps.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Routines in the common file and each of the different demo
 *     Apps to run.
 *-------------------------------------------------------------------------*/
#ifndef APPS_H_
#define APPS_H_

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "NVSettings.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern uint8_t G_received[APP_MAX_RECEIVED_DATA + 1];
extern unsigned int G_receivedCount;
extern NVSettings_t G_nvsettings;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void App_Pachube(void);
void App_UDPDemo(void);
void App_TCPServerDemo(void);
void App_TCPClientDemo(void);
void App_HTTPDemo(void);
void App_PassThroughUART(void);
void App_PassThroughSPI(void);
void App_ProgramMode(void);
void App_Startup(void);
void App_Menu(void);

void App_Write(const void *txData, uint16_t dataLength);
bool App_Read(uint8_t *rxData, uint16_t dataLength, uint8_t blockFlag);
void App_PrepareIncomingData(void);
void App_ProcessIncomingData(uint8_t rxData);
void App_PotentiometerUpdate(int16_t * G_adc_int, bool updateLCD);
void App_TemperatureReadingUpdate(char * G_temp_int, bool updateLCD);
void App_LightSensorReadingUpdate(char * G_light_int, bool updateLCD);
int16_t App_RSSIReading(bool updateLCD);
void App_Update(void);
ATLIBGS_MSG_ID_E App_Connect(ATLIBGS_WEB_PROV_SETTINGS *webprov);

void App_InitModule(void);
void App_StartupLimitedAP(char *mySSID);
void App_aClientConnection(void);
void App_WebProvisioning_OverAirPush(void);
void App_StartWPS(void);
void App_StartupADKDemo(uint8_t isLimiteAPmode);
void App_StartupProbeDemo(uint8_t isLimiteAPmode);
void App_OverTheAirProgramming(void);
void App_OverTheAirProgrammingPushMetheod(void);
void App_StartupTCPClinet(void);
int LimitedAP_TCP_SereverBulkMode(void);

#endif // APPS_H_
/*-------------------------------------------------------------------------*
 * End of File:  Apps.h
 *-------------------------------------------------------------------------*/
