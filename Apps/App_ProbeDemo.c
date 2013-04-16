/*-------------------------------------------------------------------------*
 * File:  App_ProbeDemo.c
 *-------------------------------------------------------------------------*
 * Description:
 *    This demo implements a UDP server using the GainSpan GS1011 
 *    Wi-Fi module. The UDP server listens on port 9930 for requests
 *    from Micrium's uC/Probe to read the values of the potentiometer,
 *    light sensor and accelerometer. uC/Probe is a Windows application
 *    that displays the readings in all kinds of virtual indicators such
 *    as gauges, numerics and charts among others.
 *    For more information on uC/Probe go online to: 
 *    http://micrium.com/tools/ucprobe/overview
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <HostApp.h>
#include <system/platform.h>
#include <ATCmdLib/AtCmdLib.h>
#include <drv/Glyph/lcd.h>
#include <mstimer.h>
#include <system/console.h>
#include <GainSpan_SPI.h>
#include <led.h>
#include "Apps.h"
#include <probe_wifi.h>
#include <includes.h>


/*-------------------------------------------------------------------------*
 * Externally declared functions:
 *-------------------------------------------------------------------------*/

extern void      App_StartupProbeDemo(uint8_t isLimiteAPmode);

extern int16_t  *Accelerometer_Get(void);
extern void      Accelerometer_Init(void);

extern void      Temperature_Init(void);
extern uint16_t  Temperature_Get(void);

extern void      Potentiometer_Init(void);
extern uint32_t  Potentiometer_Get(void);

extern void      LightSensor_Init(void);
extern int16_t   LightSensor_Get(void);

extern int16_t 	 gAccData[3];
extern uint16_t  gAmbientLight;
extern float     gTemp_F;


/*-------------------------------------------------------------------------*
 * Global variables:
 *-------------------------------------------------------------------------*/

uint8_t   AppProbeDemoPotPercent;
uint8_t   AppProbeDemoLEDs;
int8_t    AppProbeDemoAccX;
int8_t    AppProbeDemoAccY;
int8_t    AppProbeDemoAccZ;
uint8_t   AppProbeDemoTempF;
uint16_t  AppProbeDemoLight;
int8_t    AppProbeDemoPitchAngle;
int8_t    AppProbeDemoRollAngle;


/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

#define APP_PROBE_DEMO_MODE_LIMITED_AP_EN               1u

#define APP_PROBE_DEMO_UPD_STATE_POT                    1u
#define APP_PROBE_DEMO_UPD_STATE_LIGHT                  2u
#define APP_PROBE_DEMO_UPD_STATE_ACCEL                  3u
#define APP_PROBE_DEMO_UPD_STATE_LEDS                   4u
#define APP_PROBE_DEMO_UPD_STATE_TEMP                   5u


/*---------------------------------------------------------------------------*
 * Routine:  App_ProbeDemo
 *---------------------------------------------------------------------------*
 * Description:
 *      Take
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_ProbeDemo(void)
{
    ATLIBGS_MSG_ID_E rxMsgId = ATLIBGS_MSG_ID_NONE;
    ATLIBGS_NetworkStatus network_status;
    static int16_t G_adc_int[2] = { 0, 0 };
    char LCDString[32];
    int led_ix;
    char temp_char[2]; 
    uint16_t temp; 
    float ftemp;
    int32_t pot;

                                                                /* Initialize sensors.                                  */
    Temperature_Init();
    Potentiometer_Init();
    Accelerometer_Init();
    
    ProbeCom_Init();                                            /* Initialize uC/Probe generic communication.           */
    ProbeWiFi_Init();    

    DisplayLCD(LCD_LINE1, "  Micrium  ");
    LCDSelectFont(FONT_SMALL);
    DisplayLCD(LCD_LINE2, "uC/Probe over Wi-Fi");

    while (1) {
                                                                /* Read sensors and update LCD with the new readings.   */
                                                                /* Read the potentiometer.                              */
        pot = (uint32_t)(Potentiometer_Get() * 0.25);
        AppProbeDemoPotPercent = pot / 10;
        G_adc_int[0] = (int16_t)(pot / 10);
        G_adc_int[1] = (int16_t)(pot % 10);
        sprintf((char *)LCDString, "Pot  : %d.%d%%", G_adc_int[0], G_adc_int[1]);
        DisplayLCD(LCD_LINE3, (const uint8_t *)LCDString);

                                                                /* Read the light sensor.                               */
        AppProbeDemoLight = LightSensor_Get();
        sprintf((char *)LCDString, "Light: %04d lux", AppProbeDemoLight);
        DisplayLCD(LCD_LINE4, (const uint8_t *)LCDString);

                                                                /* Read the 3-axis accelerometer.                       */
        Accelerometer_Get();
        AppProbeDemoAccX = gAccData[0];
        AppProbeDemoAccY = gAccData[1];
        AppProbeDemoAccZ = gAccData[2];
        sprintf((char *)LCDString, "Accel: x%2d y%2d z%2d", gAccData[0], gAccData[1], gAccData[2]);
        DisplayLCD(LCD_LINE5, (const uint8_t *)LCDString);

        AppProbeDemoPitchAngle = (int8_t)(AppProbeDemoAccY *  2.72);
        AppProbeDemoRollAngle  = (int8_t)(AppProbeDemoAccX * -2.72);

                                                                /* Turn on and off the LEDs according to the slider ctrl*/
        if (AppProbeDemoLEDs > NUM_LEDS)
        {
            AppProbeDemoLEDs = NUM_LEDS;
        }
        for (led_ix = 0; led_ix < AppProbeDemoLEDs; ++led_ix)
        {
            led_on(led_ix);
        }
        for (led_ix = AppProbeDemoLEDs; led_ix < NUM_LEDS; ++led_ix)
        {
            led_off(led_ix);
        }
                                                                /* Read the temperature sensor.                         */
        temp = Temperature_Get();
        temp_char[1] = (temp & 0xFF00)>>8;
        temp_char[0] = temp & 0xFF;                  
        ftemp = *(uint16_t *)temp_char;
        AppProbeDemoTempF = (uint8_t)(((ftemp/5)*9)/128 + 22);


        if (!AtLibGs_IsNodeAssociated()) {                      /* If the WiFI module is not associated to any network..*/
                                                                /* ...connect to one.                                   */
#if APP_PROBE_DEMO_MODE_LIMITED_AP_EN > 0u                      /* The board will create a Wi-Fi HotSpot.               */
              App_StartupProbeDemo(APP_PROBE_DEMO_MODE_LIMITED_AP_EN);
              AtLibGs_SetNodeAssociationFlag();            
#else
              App_aClientConnection();                          /* The board will connect to a Wi-Fi router.            */
#endif
              do {
                  DisplayLCD(LCD_LINE7, "IP: ???.???.???.???");
                  rxMsgId = AtLibGs_GetNetworkStatus(&network_status);
              } while (ATLIBGS_MSG_ID_OK != rxMsgId);

              sprintf(LCDString, "IP: " _F8_ "." _F8_ "." _F8_ "." _F8_, 
                      network_status.addr.ipv4[0], network_status.addr.ipv4[1], 
                      network_status.addr.ipv4[2], network_status.addr.ipv4[3]);
              DisplayLCD(LCD_LINE7, (uint8_t *)LCDString);      /* Display the UDP server's IP address.                 */ 
        } else {
            ProbeWiFi_Task();                                   /* Receive and process a UDP package if any.            */

            sprintf(&LCDString[0],  "Rx: %05u", (uint16_t)ProbeCom_RxPktCtr);
            sprintf(&LCDString[9], " Tx: %05u", (uint16_t)ProbeCom_TxPktCtr);
            DisplayLCD(LCD_LINE8, (uint8_t *)LCDString);        /* Display the uC/Probe Rx and Tx counters.             */                       
        }
    }
}