/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Sets up one of the hardware and drivers and then calls one of the
 *     tests or demos.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <system/platform.h>
#include <HostApp.h>
#include <init/hwsetup.h>
#include <drv\Glyph\lcd.h>
#include <system\mstimer.h>
#include <system/Switch.h>
#include <sensors\Potentiometer.h>
#include <sensors\Temperature.h>
//#include <Tests\Tests.h>
#include <system\console.h>
#include <drv\UART0.h>
#include <drv\UART2.h>
#include <Sensors\LightSensor.h>
#include <drv\SPI.h>
#include <CmdLib\GainSpan_SPI.h>
#include <Apps/NVSettings.h>
#include <Apps/Apps.h>
#include "stdio.h"
#include "string.h"
#include "led.h"
extern void LEDFlash(uint32_t timeout);
extern void led_task(void);
extern void DisplayLCD(uint8_t, const uint8_t *);
extern int16_t *Accelerometer_Get(void);
extern void Accelerometer_Init(void);
extern int16_t	gAccData[3];
/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
/* Set option bytes */
#pragma location = "OPTBYTE"
__root const uint8_t opbyte0 = 0xEFU;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte1 = 0xFFU;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte2 = 0xF8U;
#pragma location = "OPTBYTE"
__root const uint8_t opbyte3 = 0x04U;

/* Set security ID */
#pragma location = "SECUID"
__root const uint8_t secuid[10] = 
    {0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
/* Application Modes */
typedef enum {
    PROGRAM_MODE,
    UART_PT_MODE,
    SWARM_CONN_MODE,
    RUN_EXOSITE,
    RUN_PROVISIONING,
    RUN_OVER_AIR_DOWNLOAD,
    GAINSPAN_DEMO
}AppMode_T;

//typedef enum {
//    UPDATE_LIGHT,
//    UPDATE_TEMPERATURE,
//    UPDATE_POTENIOMETER,
//    UPDATE_ACCELEROMETER
//} APP_STATE_E;

/*---------------------------------------------------------------------------*
 * Routine:  main
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the hardware, setup the peripherals, show the startup banner,
 *      wait for the module to power up, run a selected test or app.
 * Inputs:
 *      void
 * Outputs:
 *      int -- always 0.
 *---------------------------------------------------------------------------*/
uint16_t gAmbientLight;
float gTemp_F;
typedef union {
	int16_t		temp;
	uint8_t		T_tempValue[2];
} temp_u;
extern void SPI2_Init();
extern void SPI_Init(uint32_t bitsPerSecond);
extern void App_Exosite(void);
extern void App_WebProvisioning(void);
extern void App_OverTheAirProgrammingPushMetheod(void);
int  main(void)
{
    AppMode_T AppMode; APP_STATE_E state=UPDATE_TEMPERATURE; 
    char LCDString[30], temp_char[2]; uint16_t temp; float ftemp;
    
    UART0_Start(GAINSPAN_CONSOLE_BAUD);
    
    HardwareSetup();
    
    ConsolePrintf("\r\n***PROGRAMSTART***\r\n");
    
        /* Default app mode */
    AppMode = SWARM_CONN_MODE;
    
    /* Determine if SW1 & SW3 is pressed at power up to enter programming mode */
    if (Switch1IsPressed() && Switch3IsPressed()) {
         AppMode = PROGRAM_MODE;
    }
    else if(Switch1IsPressed())
    {
        AppMode = GAINSPAN_DEMO;
    }
    else if(Switch2IsPressed())
    {
        AppMode = RUN_PROVISIONING;
    }
    else if(Switch3IsPressed())
    {
        AppMode = RUN_OVER_AIR_DOWNLOAD;
    }
    
    /************************initializa LCD module********************************/
    SPI2_Init();
    InitialiseLCD();
    led_init();
    MSTimerInit();
    
    if(AppMode == SWARM_CONN_MODE) {
        LCDDisplayLogo();
        LCDSelectFont(FONT_SMALL);
        DisplayLCD(LCD_LINE3, "RL78G14 RDK    V2.0");
        DisplayLCD(LCD_LINE4, "   Wi-Fi & Cloud   ");
        DisplayLCD(LCD_LINE5, "     demos by:     ");
        DisplayLCD(LCD_LINE6, "Gainspan           ");
        DisplayLCD(LCD_LINE8, "Buglabs swarm      ");
        //MSTimerDelay(1000);
		MSTimerDelay(100);
        ClearLCD();
        DisplayLCD(LCD_LINE1, "Booting:           ");
		DisplayLCD(LCD_LINE2, " bugswarm connector");
		DisplayLCD(LCD_LINE3, "                   ");
		DisplayLCD(LCD_LINE4, "For other modes:   ");
		DisplayLCD(LCD_LINE5, "Hold down other key");
		DisplayLCD(LCD_LINE6, "and press RESET:   ");
		DisplayLCD(LCD_LINE7, " SW1 Gainspan Demo ");
		DisplayLCD(LCD_LINE8, " SW2 Change Wifi AP");		
        //DisplayLCD(LCD_LINE2, "-RST no key:       ");
        //DisplayLCD(LCD_LINE3, "   bugswarm app    ");
        //DisplayLCD(LCD_LINE5, "-RST + SW2:        ");
        //DisplayLCD(LCD_LINE6, "   AP Provisioning ");
        //DisplayLCD(LCD_LINE7, "-RST + SW3: OTA    ");
        //MSTimerDelay(4000);
		MSTimerDelay(100);
        ClearLCD();
        
        LCDSelectFont(FONT_SMALL);
    }
    
    DisplayLCD(LCD_LINE1, "Starting..."); 
    /*****************************************************************************/  
    SPI_Init(GAINSPAN_SPI_RATE);  
   /* Setup LCD SPI channel for Chip Select P10, active low, active per byte  */
    SPI_ChannelSetup(GAINSPAN_SPI_CHANNEL, false, true);
    GainSpan_SPI_Start();

    PM15 &= ~(1 << 2);
    P15 &= ~(1 << 2);
    
    if(AppMode == PROGRAM_MODE) {
        App_ProgramMode();
    }
    else if (AppMode == SWARM_CONN_MODE)
    {          
        DisplayLCD(LCD_LINE1, "BUGSWARM Connector");
	Temperature_Init();
        Potentiometer_Init();
        App_InitModule();
	Accelerometer_Init();
        if(NVSettingsLoad(&G_nvsettings))
          NVSettingsSave(&G_nvsettings);

        ConsolePrintf("Existing webprov SSID: %s\r\n",G_nvsettings.webprov.ssid);
        //TODO - better check if we have webprov data in nvram
        if (strlen(G_nvsettings.webprov.ssid)<1){
          ConsolePrintf("No SSID in memory: Running App_WebProvisioning!\r\n");
          DisplayLCD(LCD_LINE2, "Provisioning");
          App_WebProvisioning();
        }
        DisplayLCD(LCD_LINE2, G_nvsettings.webprov.ssid);
        ConsolePrintf("\r\nBegin swarm test app\r\n");
        App_RunConnector();
    }
    else if(AppMode == RUN_PROVISIONING)
    {
      ConsolePrintf("Begin App_WebProvisioning();\r\n");
      App_WebProvisioning();
    }
     else if(AppMode == RUN_OVER_AIR_DOWNLOAD)
    {
       App_OverTheAirProgrammingPushMetheod();
    }
    else{
        ConsolePrintf("\r\nBegin default demo application\r\n");
       // UART2_Start(GAINSPAN_UART_BAUD);
 
        Temperature_Init();
        Potentiometer_Init();
    
       // sprintf(LCDString, "RDK Demo %s", VERSION_TEXT);
       // DisplayLCD(LCD_LINE1, (const uint8_t *)LCDString);
   
        /* Before doing any tests or apps, startup the module */
        /* and nonvolatile stettings */
        App_Startup();
        // Now connect to the system
        //App_Connect(&G_nvsettings.webprov);
     
       //  App_PassThroughSPI();
         
         /******************Start Processing Sensor data******************/
         
         uint32_t start = MSTimerGet();  uint8_t c;
         Accelerometer_Init();
         while(1) 
         { 
          // if (GainSpan_SPI_ReceiveByte(GAINSPAN_SPI_CHANNEL, &c)) 
           if(App_Read(&c, 1, 0)) 
             AtLibGs_ReceiveDataProcess(c);
                   
        /* Timeout? */
           if (MSTimerDelta(start) >= 100)     // every 100 ms, read sensor data
           {  
              led_task();
              switch(state)
              {              
                case UPDATE_TEMPERATURE:         
                // Temperature sensor reading
                  temp = Temperature_Get();
#if 0                 
                   // Get the temperature and show it on the LCD
                  temp_char[0] = (int16_t)temp / 16;
                  temp_char[1] = (int16_t)((temp & 0x000F) * 10) / 16;
#endif 
                  temp_char[1] = (temp & 0xFF00)>>8;
                  temp_char[0] = temp & 0xFF;
                  
                  ftemp = *(uint16_t *)temp_char;
                  
                  gTemp_F = ((ftemp/5)*9)/128 + 22;
              
                  // Display the contents of lcd_buffer onto the debug LCD 
                  //sprintf((char *)LCDString, "TEMP: %d.%d C", temp_char[0], temp_char[1]);
                  sprintf((char *)LCDString, "TEMP: %.1fF", gTemp_F);
                  DisplayLCD(LCD_LINE6, (const uint8_t *)LCDString);  
                  state = UPDATE_LIGHT;
                break;
                
                case UPDATE_LIGHT:
                 // Light sensor reading
                  gAmbientLight = LightSensor_Get();
                    // Display the contents of lcd_buffer onto the debug LCD 
                  sprintf((char *)LCDString, "Light: %d ", gAmbientLight);
                  DisplayLCD(LCD_LINE7, (const uint8_t *)LCDString);
                  state = UPDATE_ACCELEROMETER;
                break;
                
                case UPDATE_ACCELEROMETER: 
                 // 3-axis accelerometer reading
                  Accelerometer_Get();
                  sprintf((char *)LCDString, "x%2d y%2d z%2d", gAccData[0], gAccData[1], gAccData[2]);
                  DisplayLCD(LCD_LINE8, (const uint8_t *)LCDString); 
                  state = UPDATE_TEMPERATURE;
                break;
              }
              start = MSTimerGet();
           }
         }          
    }    
    return 0;
}
/*-------------------------------------------------------------------------*
 * End of File:  main.c
 *-------------------------------------------------------------------------*/

