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
#include <mstimer.h>
#include <system/Switch.h>
#include <sensors\Potentiometer.h>
#include <sensors\Temperature.h>
//#include <Tests\Tests.h>
#include <system\console.h>
#include <drv\UART.h>
#include <Sensors\LightSensor.h>
#include <drv\SPI.h>
#include <GainSpan_SPI.h>
#include <NVSettings.h>
#include <Apps/Apps.h>
#include <Apps/App_Swarm.h>
#include "stdio.h"
#include "string.h"
#include "led.h"
#include "system/Switch.h"
#include <includes.h>                                           /* Micrium firmware.                                    */

extern void LEDFlash(uint32_t timeout);
extern void led_task(void);
extern void DisplayLCD(uint8_t, const uint8_t *);
extern int16_t *Accelerometer_Get(void);
extern void Accelerometer_Init(void);
extern int16_t	gAccData[3];
extern void App_ProbeDemo(void);
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
    GAINSPAN_DEMO=0,      //   0 0 0
    SWARM_CONN_MODE,      //   0 0 1
    RUN_PROVISIONING,     //   0 1 0
    PROGRAM_MODE,         //   0 1 1
    RUN_PROBE,            //   1 0 0
    GAINSPAN_CLIENT,      //   1 0 1
    SPI_PT_MODE,          //   1 1 0
}AppMode_T;

typedef enum {
    UPDATE_LIGHT,
    UPDATE_TEMPERATURE,
    UPDATE_POTENIOMETER,
    UPDATE_ACCELEROMETER
} APP_STATE_E;

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
extern void SPI2_Init(void);
extern void SPI_Init(uint32_t bitsPerSecond);
extern void App_WebProvisioning_OverAirPush(void);
extern void App_StartupADKDemo(uint8_t isLimiteAPmode);
extern int LimitedAP_TCP_SereverBulkMode(void);

int  main(void)
{
    AppMode_T AppMode;  APP_STATE_E state=UPDATE_TEMPERATURE; 
    char LCDString[30], temp_char[2]; uint16_t temp; float ftemp;
    uint8_t isLimiteAPmode=1, rxData; uint32_t start;  
  
    HardwareSetup();  

    /* Default app mode */
    AppMode = SWARM_CONN_MODE;
   
    /* Determine SW1, SW2 & SW3 is pressed at power up to dertmine which demo will run  */
    if(Switch1IsPressed()) 
    {
         AppMode = GAINSPAN_DEMO;
    }
    else if(Switch1IsPressed() && Switch2IsPressed()) 
    {
         AppMode = PROGRAM_MODE;
    }
    else if(Switch2IsPressed() && Switch3IsPressed())
    {
        AppMode = SPI_PT_MODE;                      // run as the Gainspan Eval board
    }
    else if(Switch2IsPressed())
    {
        AppMode = RUN_PROVISIONING;
    }
    else if(Switch3IsPressed())
    {
        AppMode = GAINSPAN_DEMO;
         isLimiteAPmode=0;                            // run the WiFi client demo this should be GS demo via shared network (provisioned)
    }
    
    /************************initializa LCD module********************************/
    SPI2_Init();
    InitialiseLCD();
    led_init();
    MSTimerInit();
    
    // DisplayLCD(LCD_LINE1, "Starting..."); 
    /*****************************************************************************/  
    SPI_Init(GAINSPAN_SPI_RATE);  
   /* Setup LCD SPI channel for Chip Select P10, active low, active per byte  */
    SPI_ChannelSetup(GAINSPAN_SPI_CHANNEL, false, true);
    GainSpan_SPI_Start();
        
    // Set SAU0 enable and UART0/UART1 baud rate
    RL78G14RDK_UART_Start(GAINSPAN_CONSOLE_BAUD, GAINSPAN_CONSOLE_BAUD);
    PM15 &= ~(1 << 2);       //EInk hand
    P15 &= ~(1 << 2);
    
	LCDDisplayLogo();
	LCDSelectFont(FONT_SMALL);
	DisplayLCD(LCD_LINE3, "   VERIZON DEMO    ");
	DisplayLCD(LCD_LINE4, "   Wi-Fi & Cloud   ");
	DisplayLCD(LCD_LINE5, "   connectivity    ");
	DisplayLCD(LCD_LINE6, "Gainspan           ");
	DisplayLCD(LCD_LINE7, "BugLabs            ");
	DisplayLCD(LCD_LINE8, "Arrow Electronics  ");
	MSTimerDelay(3500);
	ClearLCD();
	DisplayLCD(LCD_LINE1, "SW Config after RST");
	DisplayLCD(LCD_LINE2, "1 2 3              ");
	DisplayLCD(LCD_LINE3, "0 0 0 BugLabs Demo ");
	DisplayLCD(LCD_LINE4, "1 0 0 GS AP Demo   ");
	DisplayLCD(LCD_LINE5, "0 1 0 Provisioning ");
	DisplayLCD(LCD_LINE6, "0 0 1 GS Clnt Demo ");
	DisplayLCD(LCD_LINE7, " ");
	DisplayLCD(LCD_LINE8, "  ");
	MSTimerDelay(5000);
	ClearLCD();

    if(AppMode == SPI_PT_MODE)
      App_PassThroughSPI();                             // run SPI pass through, so the board can be used as a GS eval board
   
    if(AppMode == PROGRAM_MODE) {
        App_ProgramMode();                              // Program GS1011 firmware and external flash
    }    
    if(AppMode == GAINSPAN_DEMO) {
        
        LCDSelectFont(FONT_LARGE);
    }

    if(AppMode == RUN_PROVISIONING)
    {
      App_WebProvisioning_OverAirPush();               // run provisioning and over air upgrade
    }
    else if(AppMode == RUN_PROBE)
    {
        App_ProbeDemo();                                // Run the uC/Probe demo. 
    }
    else if (AppMode == SWARM_CONN_MODE )
    {
        LCDSelectFont(FONT_SMALL);
        DisplayLCD(LCD_LINE1, "BugLabs Demo");           // Run the BugLabs cloud demo.                              
	App_SwarmConnector();
    }
    else
    {
        Temperature_Init();                            // start default the webserver demo
        Potentiometer_Init();
  
       // sprintf(LCDString, "RDK Demo %s", VERSION_TEXT);
       // DisplayLCD(LCD_LINE1, (const uint8_t *)LCDString);
   
        /* Before doing any tests or apps, startup the module */
        /* and nonvolatile stettings */
        
        App_StartupADKDemo(isLimiteAPmode);            // we may run limited AP or client
        
         /******************Start Processing Sensor data******************/
        
         start = MSTimerGet();  
         Accelerometer_Init();
               
         while(1) 
         {
           if(App_Read(&rxData, 1, 0)) 
             AtLibGs_ReceiveDataProcess(rxData);
                   
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
                   // Displays Data in ft/s2
                  // Quick conversion from ft/m2 to degrees
                  sprintf((char *)LCDString, "x%2d y%2d z%2d", (gAccData[0]*90)/32, (gAccData[1]*90)/32, (gAccData[2]*90)/32);
                 // sprintf((char *)LCDString, "x%2d y%2d z%2d", gAccData[0], gAccData[1], gAccData[2]);
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

