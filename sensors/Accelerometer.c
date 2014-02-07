/*-------------------------------------------------------------------------*
 * File:  Accelerometer.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Accelerometer sensor driver using the ADT7420 over I2C.
 *-------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "Accelerometer.h"
#include <drv/I2C.h>
#include <mstimer.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define ACCEL_ADDR	    0x3Au

#define DATA_FORMAT_REG 0x31u   
#define POWER_CTL_REG  	0x2Du
#define FIFO_CTL_REG   	0x38u
#define DATAX_REG       0x32u
#define DATAY_REG       0x34u
#define DATAZ_REG       0x36u

#define SELF_TEST       0x80u
#define DATA_FORMAT     0x03u
#define PWR_CFG     	0x08u
#define FIFO_CFG     	0x00u

const uint8_t acc_reg_addr[3] = { DATAX_REG, DATAY_REG, DATAZ_REG };

int16_t	gAccData[3];

static uint8_t *pTxData;

const uint8_t acc_config[3][2] = {
	{DATA_FORMAT_REG, DATA_FORMAT},
	{POWER_CTL_REG, PWR_CFG},
	{FIFO_CTL_REG, FIFO_CFG}
};
/*---------------------------------------------------------------------------*
 * Routine:  Accelerometer_Init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the Accelerometer driver.
 * Inputs:
 *      void
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void Accelerometer_Init(void)
{
    I2C_Request r;
    uint32_t timeout;  uint8_t acc_config_cnt;
      
    for(acc_config_cnt=0; acc_config_cnt<3; acc_config_cnt++)
    {
      timeout = MSTimerGet();
      
      pTxData = (uint8_t *)acc_config[acc_config_cnt];
      r.iAddr = ACCEL_ADDR>>1;
      r.iSpeed = 100; /* kHz */
      r.iWriteData = pTxData;
      r.iWriteLength = 2;
      r.iReadData = 0;
      r.iReadLength = 0;
  
      I2C_Start();
      I2C_Write(&r, 0);
      while ((I2C_IsBusy()) && (MSTimerDelta(timeout) < 10))
          {}
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Accelerometer_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the value of the ADT7420 and return the LightSensor in Lux.
 * Inputs:
 *      void
 * Outputs:
 *      uint16_t -- LightSensor with 4 bits of fraction and 12 bits of
 *          integer.
 *---------------------------------------------------------------------------*/
int16_t *Accelerometer_Get(void)
{
    uint8_t target_reg, acc_axis;
    uint8_t target_data[2] = {0x00, 0x00};
    uint32_t timeout = MSTimerGet();
    I2C_Request r;
       
    //Accelerometer_Init();
    for(acc_axis=0; acc_axis<3; acc_axis++)
    {
      target_reg = acc_reg_addr[acc_axis];
  
      r.iAddr = ACCEL_ADDR>>1;
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
      gAccData[acc_axis] = (target_data[1] << 8) + target_data[0]; 
    }
    return gAccData;
}

/*-------------------------------------------------------------------------*
 * End of File:   LightSensor.c
 *-------------------------------------------------------------------------*/
