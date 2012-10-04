/*-------------------------------------------------------------------------*
 * File:  App_Common.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Common routines used by all the Apps.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <HostApp.h>
#include <HostApp.h>
#include <system/console.h>
#include <system/platform.h>
#include <CmdLib/AtCmdLib.h>
#include <CmdLib/GainSpan_SPI.h>
#include <sensors/Temperature.h>
#include <sensors/Potentiometer.h>
#include <sensors/LightSensor.h>
#include <sensors/Accelerometer.h>
#include <system/mstimer.h>
#include <drv/Glyph/lcd.h>
#include "Apps.h"
#include "HostApp.h"
#ifndef APP_MAX_RECEIVED_DATA
    #error "APP_MAX_RECEIVED_DATA must be defined in platform.h"
#endif

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
uint8_t G_received[APP_MAX_RECEIVED_DATA + 1];
unsigned int G_receivedCount = 0;

/*---------------------------------------------------------------------------*
 * Routine:  App_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      ATCmdLib callback to write a string of characters to the module.
 * Inputs:
 *      const uint8_t *txData -- string of bytes
 *      uint32_t dataLength -- Number of bytes to transfer
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_Write(const void *txData, uint16_t dataLength)
{
    const uint8_t *tx = (uint8_t *)txData;
#ifdef ATLIBGS_INTERFACE_SPI
    while (dataLength--) {
        /* Keep trying to send this data until it goes */
        while (!GainSpan_SPI_SendByte(*tx)) {
            /* Process any incoming data as well */
            GainSpan_SPI_Update(GAINSPAN_SPI_CHANNEL);
        }

        tx++;
    }
#else
    while (dataLength--) {
        /* Keep trying to send this data until it goes */
        while (!GainSpan_UART_SendByte(*tx)) {
        }
        tx++;
    }
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  App_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      ATCmdLib callback to read a string of characters from the module.
 *      This routine can block if needed until the data arrives.
 * Inputs:
 *      uint8_t *rxData -- Pointer to a place to store a string of bytes
 *      uint16_t dataLength -- Number of bytes to transfer
 *      uint8_t blockFlag -- true/non-zero to wait for bytes, else false/zero.
 * Outputs:
 *      bool -- true if character received, else false
 *---------------------------------------------------------------------------*/
bool App_Read(uint8_t *rxData, uint16_t dataLength, uint8_t blockFlag)
{
#ifdef ATLIBGS_INTERFACE_SPI
    bool got_data = false;

    /* Keep getting data if we have a number of bytes to fetch */
    while (dataLength) {
        /* Try to get a byte */
        if (GainSpan_SPI_ReceiveByte(GAINSPAN_SPI_CHANNEL, rxData)) {
            /* Got a byte, move up to the next position */
            rxData++;
            dataLength--;
            got_data = true;
        } else {
            /* Did not get a byte, are we block?  If not, stop here */
            if (!blockFlag)
                break;
        }
    }

    return got_data;
#else
    bool got_data = false;

    /* Keep getting data if we have a number of bytes to fetch */
    while (dataLength) {
        /* Try to get a byte */
        if (GainSpan_UART_ReceiveByte(rxData)) {
            /* Got a byte, move up to the next position */
            rxData++;
            dataLength--;
            got_data = true;
        } else {
            /* Did not get a byte, are we block?  If not, stop here */
            if (!blockFlag)
            break;
        }
    }

    return got_data;
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  App_RSSIReading
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading of the RSSI level with the WiFi and show it on
 *      the LCD display.
 * Inputs:
 *      bool updateLCD (if true, LCD is updated)
 * Outputs:
 *      int16_t rssi (return value)
 *---------------------------------------------------------------------------*/
int16_t App_RSSIReading(int16_t * rssi, bool updateLCD)
{
    char line[20];
    int rssiFound = 0;

    if (AtLibGs_IsNodeAssociated()) {
        if (AtLibGs_GetRssi() == ATLIBGS_MSG_ID_OK) {
            if (AtLibGs_ParseRssiResponse(rssi)) {
                sprintf(line, "RSSI: %d", (*rssi));
                DisplayLCD(LCD_LINE6, (const uint8_t *)line);
                rssiFound = 1;
            }
        }
    }
    if (!rssiFound) {
        DisplayLCD(LCD_LINE5, "RSSI: ----");
        rssi = 0;
    }
    return *rssi;
}

/*---------------------------------------------------------------------------*
 * Routine:  App_TemperatureReadingUpdate
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading of a temperature and show it on the LCD display.
 * Inputs:
 *      bool updateLCD (if true, LCD is updated), pointer to 2 member uint16_t
 *      array to update values in
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_TemperatureReadingUpdate(uint16_t * G_temp_int, bool updateLCD)
{
    char lcd_buffer[20];

    // Temperature sensor reading
    int16_t temp;
    temp = Temperature_Get();
    // Get the temperature and show it on the LCD
    G_temp_int[0] = (int16_t)temp / 16;
    G_temp_int[1] = (int16_t)((temp & 0x000F) * 10) / 16;

    if(updateLCD)
    {
    // Display the contents of lcd_buffer onto the debug LCD 
    sprintf((char *)lcd_buffer, "TEMP: %d.%d C", G_temp_int[0], G_temp_int[1]);
    DisplayLCD(LCD_LINE3, (const uint8_t *)lcd_buffer);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  App_PotentiometerUpdate
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading of the potentiometer and show it on the LCD display.
 * Inputs:
 *      bool updateLCD (if true, LCD is updated), pointer to 2 member uint16_t
 *      array to update values in
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_PotentiometerUpdate(int16_t * G_adc_int, bool updateLCD)
{
    char lcd_buffer[20];

    // Potentiometer sensor reading
    int32_t percent;
    percent = Potentiometer_Get();
    G_adc_int[0] = (int16_t)(percent / 10);
    G_adc_int[1] = (int16_t)(percent % 10);

    if(updateLCD)
    {
    sprintf((char *)lcd_buffer, " POT: %d.%d %%", G_adc_int[0], G_adc_int[1]);
    /* Display the contents of lcd_buffer onto the debug LCD */
    DisplayLCD(LCD_LINE4, (const uint8_t *)lcd_buffer);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  App_LightSensorReadingUpdate
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading of a lightSensor and show it on the LCD display.
 * Inputs:
 *      bool updateLCD (if true, LCD is updated), pointer to 2 member uint16_t
 *      array to update values in
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_LightSensorReadingUpdate(uint16_t * G_light_int, bool updateLCD)
{
    char lcd_buffer[20];

    // Temperature sensor reading
    *G_light_int = LightSensor_Get();

    if(updateLCD)
    {
      // Display the contents of lcd_buffer onto the debug LCD 
      sprintf((char *)lcd_buffer, "Light: %d ", *G_light_int);
      DisplayLCD(LCD_LINE5, (const uint8_t *)lcd_buffer);
    }
}


/*---------------------------------------------------------------------------*
 * Routine:  App_PrepareIncomingData
 *---------------------------------------------------------------------------*
 * Description:
 *      ATCmdLib callback to reset the incoming data state.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_PrepareIncomingData(void)
{
    G_receivedCount = 0;
    G_received[0] = '\0';
}

/*---------------------------------------------------------------------------*
 * Routine:  App_ProcessIncomingData
 *---------------------------------------------------------------------------*
 * Description:
 *      ATCmdLib callback that is called when a byte has come in for a
 *      specific connection.
 * Inputs:
 *      uint8_t rxData -- Byte received
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_ProcessIncomingData(uint8_t rxData)
{
    if (G_receivedCount < APP_MAX_RECEIVED_DATA) {
        G_received[G_receivedCount++] = rxData;
        G_received[G_receivedCount] = '\0';
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  App_Update
 *---------------------------------------------------------------------------*
 * Description:
 *      Allow the AtLibGs to update its state when nothing is happening
 *      in the application.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void App_Update(void)
{
#ifdef ATLIBGS_INTERFACE_SPI
    GainSpan_SPI_Update(GAINSPAN_SPI_CHANNEL);
#else
#endif
}

void readSensor(APP_STATE_E state){
	char temp_char[2]; uint16_t temp; float ftemp; float gTemp_F; 
	char LCDString[30]; uint16_t gAmbientLight; extern int16_t	gAccData[3];
	extern int16_t *Accelerometer_Get(void);
	ConsolePrintf("Reading sensor %d: ",state);
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
	  ConsolePrintf("%s\r\n",LCDString);
	  DisplayLCD(LCD_LINE6, (const uint8_t *)LCDString);  
	  state = UPDATE_LIGHT;
	  break;
	  
	case UPDATE_LIGHT:
	  // Light sensor reading
	  gAmbientLight = LightSensor_Get();
	  // Display the contents of lcd_buffer onto the debug LCD 
	  sprintf((char *)LCDString, "Light: %d ", gAmbientLight);
	  ConsolePrintf("%s\r\n",LCDString);	  
	  DisplayLCD(LCD_LINE7, (const uint8_t *)LCDString);
	  state = UPDATE_ACCELEROMETER;
	  break;
	  
	case UPDATE_ACCELEROMETER: 
	  // 3-axis accelerometer reading
	  Accelerometer_Get();
	  sprintf((char *)LCDString, "x%2d y%2d z%2d", gAccData[0], gAccData[1], gAccData[2]);
	  ConsolePrintf("%s\r\n",LCDString);	  
	  DisplayLCD(LCD_LINE8, (const uint8_t *)LCDString); 
	  state = UPDATE_TEMPERATURE;
	  break;
	}  
}


/*-------------------------------------------------------------------------*
 * End of File:  App_Common.c
 *-------------------------------------------------------------------------*/
