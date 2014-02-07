/*-------------------------------------------------------------------------*
 * File:  Potentiometer.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Potentiometer driver that uses the RX62N's S12AD A/D input.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <system/platform.h>
#include <drv/ADC.h>
#include "Potentiometer.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  Potentiometer_Init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the potentiometer driver.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void Potentiometer_Init(void)
{
    ADC_Start();
    ADC_EnableChannel(POTENTIOMETER_CHANNEL);
	ADC_EnableChannel(MICROPHONE_CHANNEL);	
}

/*---------------------------------------------------------------------------*
 * Routine:  Potentiometer_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the potentiometer and get the percent it is turned.
 * Inputs:
 *      void
 * Outputs:
 *      uint32_t -- value of 0 to 1000 for 0% to 100%
 *---------------------------------------------------------------------------*/
uint32_t Potentiometer_Get(void)
{
    int32_t adc;

    // ADC sensor reading
    adc = ADC_GetReading(POTENTIOMETER_CHANNEL);

    // Get the temperature and show it on the LCD
    adc *= 1000; // scale it from 0 to 1000
    adc /= (1 << 10); // 10-bit reading

    return adc;
}

uint32_t Microphone_Get(void)
{
    int32_t adc;

    // ADC sensor reading
    adc = ADC_GetReading(MICROPHONE_CHANNEL);

    // Get the temperature and show it on the LCD
    adc *= 1000; // scale it from 0 to 1000
    adc /= (1 << 10); // 10-bit reading

    return adc;
}
/*-------------------------------------------------------------------------*
 * End of File:  Potentiometer.c
 *-------------------------------------------------------------------------*/
