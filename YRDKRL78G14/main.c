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
    PROGRAM_MODE,
    UART_PT_MODE,
    SPI_PT_MODE,
    RUN_MY_TEST,
    RUN_PROVISIONING,
    RUN_OVER_AIR_DOWNLOAD,
    GAINSPAN_DEMO,
    RUN_PROBE,
    SWARM_CONN_MODE     // RUN_BugLab
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
    AppMode_T AppMode; APP_STATE_E state=UPDATE_TEMPERATURE; 
    char LCDString[30], temp_char[2]; uint16_t temp; float ftemp;
    uint8_t isLimiteAPmode=1, rxData; uint32_t start;  
  
    HardwareSetup();  

    /* Default app mode */
    //AppMode = GAINSPAN_DEMO;
	AppMode = SWARM_CONN_MODE;
    
    /* Determine SW1, SW2 & SW3 is pressed at power up to dertmine which demo will run  */
    if(Switch1IsPressed() && Switch2IsPressed() && Switch3IsPressed()) 
    {
         AppMode = SWARM_CONN_MODE;
    }
    else if(Switch1IsPressed() && Switch2IsPressed()) 
    {
         AppMode = PROGRAM_MODE;
    }
    else if(Switch1IsPressed() && Switch3IsPressed())
    {
        isLimiteAPmode=0;                            // run the WiFi client demo
    }
    else if(Switch2IsPressed() && Switch3IsPressed())
    {
        AppMode = SPI_PT_MODE;                      // run as the Gainspan Eval board
    }
    else if(Switch1IsPressed())
    {
        AppMode = RUN_MY_TEST;
    }
    else if(Switch2IsPressed())
    {
        AppMode = RUN_PROVISIONING;
    }
    else if(Switch3IsPressed())
    {
        AppMode = RUN_PROBE;
    }
    
    /************************initializa LCD module********************************/
    SPI2_Init();
    InitialiseLCD();
    led_init();
    MSTimerInit();
    
    DisplayLCD(LCD_LINE1, "Starting..."); 
    /*****************************************************************************/  
    SPI_Init(GAINSPAN_SPI_RATE);  
   /* Setup LCD SPI channel for Chip Select P10, active low, active per byte  */
    SPI_ChannelSetup(GAINSPAN_SPI_CHANNEL, false, true);
    GainSpan_SPI_Start();
        
    // Set SAU0 enable and UART0/UART1 baud rate
    RL78G14RDK_UART_Start(GAINSPAN_CONSOLE_BAUD, GAINSPAN_CONSOLE_BAUD);
    PM15 &= ~(1 << 2);       //EInk hand
    P15 &= ~(1 << 2);

    if(AppMode == SPI_PT_MODE)
      App_PassThroughSPI();                             // run SPI pass through, so the board can be used as a GS eval board
   
    if(AppMode == PROGRAM_MODE) {
        App_ProgramMode();                              // Program GS1011 firmware and external flash
    }    
    if(AppMode == GAINSPAN_DEMO) {
        LCDDisplayLogo();
        LCDSelectFont(FONT_SMALL);
        DisplayLCD(LCD_LINE3, "RL78G14 RDK    V3.4");
        DisplayLCD(LCD_LINE4, "   Wi-Fi & Cloud   ");
        DisplayLCD(LCD_LINE5, "     demos by:     ");
        DisplayLCD(LCD_LINE6, "Gainspan           ");
        DisplayLCD(LCD_LINE7, "Test            ");
        DisplayLCD(LCD_LINE8, "Future Designs, Inc");
        MSTimerDelay(3500);
        ClearLCD();
        DisplayLCD(LCD_LINE1, "Demo Modes:        ");
        DisplayLCD(LCD_LINE2, "-RST no key:       ");
        DisplayLCD(LCD_LINE3, "   GS Web Server   ");
        DisplayLCD(LCD_LINE4, "-RST + SW1:        ");
        DisplayLCD(LCD_LINE5, "   Test AP   ");
        DisplayLCD(LCD_LINE6, "-RST + SW2:        ");
        DisplayLCD(LCD_LINE7, "Provisioning & OTA ");
        DisplayLCD(LCD_LINE8, "-RST + SW3: uCProbe");
        MSTimerDelay(3000);
        ClearLCD();
        
        LCDSelectFont(FONT_LARGE);
    }

    if (AppMode == RUN_MY_TEST)
    {          
        DisplayLCD(LCD_LINE1, " MY TEST ");
        LimitedAP_TCP_SereverBulkMode();               // Run my testing 
    }
    else if(AppMode == RUN_PROVISIONING)
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
       DisplayLCD(LCD_LINE1, "BugLab Demo");           // Run the BugLab cloud demo.                              
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

