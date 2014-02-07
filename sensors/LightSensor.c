/*-------------------------------------------------------------------------*
 * File:  Temperature_ADT7420.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Temperature sensor driver using the ADT7420 over I2C.
 *-------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "LightSensor.h"
#include <drv/I2C.h>
#include <mstimer.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
/* ADT7420 IIC Registers */
#define LIGHTSENSOR_ADDR            0x72
#define LIGHTSENSOR_CMD             0x51

/*---------------------------------------------------------------------------*
 * Routine:  LightSensor_Init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the LightSensor driver.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void LightSensor_Init(void)
{
    /* Declare error flag */
    uint8_t cmd[2] = { LIGHTSENSOR_CMD, 0x00 };
    I2C_Request r;
    uint32_t timeout = MSTimerGet();

    r.iAddr = LIGHTSENSOR_ADDR>>1;
    r.iSpeed = 100; /* kHz */
    r.iWriteData = cmd;
    r.iWriteLength = 2;
    r.iReadData = 0;
    r.iReadLength = 0;

    I2C_Start();
    I2C_Write(&r, 0);
    while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < 10))
        {}
}

/*---------------------------------------------------------------------------*
 * Routine:  LightSensor_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the value of the ADT7420 and return the LightSensor in Lux.
 * Inputs:
 *      void
 * Outputs:
 *      uint16_t -- LightSensor with 4 bits of fraction and 12 bits of
 *          integer.
 *---------------------------------------------------------------------------*/
int16_t LightSensor_Get(void)
{
    uint8_t target_reg=LIGHTSENSOR_CMD;
    uint8_t target_data[2] = {0x00, 0x00};
    uint16_t temp = 0;
    uint32_t timeout = MSTimerGet();
    I2C_Request r;

    r.iAddr = LIGHTSENSOR_ADDR>>1;
    r.iSpeed = 100;
    r.iWriteData = &target_reg;
    r.iWriteLength = 1;
    r.iReadData = target_data;
    r.iReadLength = 2;
    I2C_Write(&r, 0);
    while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < 10))
        {}
    I2C_Read(&r, 0);
    while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < 10))
        {}

    /* Convert the device measurement into a decimal number and insert
     into a temporary string to be displayed */
    temp = (target_data[1] << 8) + target_data[0];

    return temp;
}

/*-------------------------------------------------------------------------*
 * End of File:   LightSensor.c
 *-------------------------------------------------------------------------*/
