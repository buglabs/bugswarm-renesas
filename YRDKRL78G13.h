/***********************************************************************************************************************
* File Name    : YRDKRL78G13.h
* Version      : 0.1 [16 APR 2012]
* Device(s)    : R5F100LE
* Tool-Chain   : CA78K0R
* Description  : Implements board-specific functions of the YRDKRL78G13 dev board
* Creation Date: 4/16/2012
***********************************************************************************************************************/

#ifndef YRDKRL78G13
#define YRDKRL78G13

#include "r_cg_macrodriver.h"

/***********************************************************************************************************************
* Sensor specific defines and structures
************************************************************************************************************************
* ADXL345 Accelerometer:
***********************************************************************************************************************/

#define ACCEL_ADDR		0x3A

#define ACCEL_REG_RATE		0x2C
#define ACCEL_BIT_RATE2HZ	0x4
#define ACCEL_BIT_RATE3HZ	0x5
#define ACCEL_BIT_RATE6HZ	0x6
#define ACCEL_BIT_RATE13HZ	0x7
#define ACCEL_BIT_RATE25HZ	0x8
#define ACCEL_BIT_RATE50HZ	0x9
#define ACCEL_BIT_RATE100HZ	0xA
#define ACCEL_BIT_RATE200HZ	0xB
#define ACCEL_BIT_RATE400HZ	0xC
#define ACCEL_BIT_RATE800HZ	0xD
#define ACCEL_BIT_RATE1600HZ	0xE
#define ACCEL_BIT_RATE3200HZ	0xF

#define ACCEL_REG_POWER_CTL	0x2D
#define ACCEL_BIT_MEASURE	(1<<3)

#define ACCEL_REG_DATA_FORMAT	0x31
#define ACCEL_BIT_FULL_RES	(1<<3)
#define ACCEL_BIT_JUSTIFY	(1<<2)
#define ACCEL_BIT_RANGE2G	0x0
#define ACCEL_BIT_RANGE4G	0x1
#define ACCEL_BIT_RANGE8G	0x2
#define ACCEL_BIT_RANGE16G	0x3

//TODO - multibyte reads apparently need to be the register before?
//Experimentally determined that reading right at 0x32 dropped the first byte
//(forcefully overwritten by the iic handler to be the address...)
#define ACCEL_REG_DATA		0x31

typedef struct
{
	uint16_t x;			//X axis, proportional to G force
	uint16_t y;			//Y axis, proportional to G force
	uint16_t z;			//Z axis, proportional to G force
	unsigned long time;	//time offset from CPU start, in ms
} accelData;

/***********************************************************************************************************************
* R1EX24512ASAA EEPROM:
***********************************************************************************************************************/

#define EEPROM_ADDR	0xA0

/***********************************************************************************************************************
* PH5551A2NA1 LIGHT SENSOR:
***********************************************************************************************************************/

#define LIGHT_ADDR	0x72

typedef struct
{
	uint16_t light
	unsigned long time;	//time offset from CPU start, in ms
} lightData;

/***********************************************************************************************************************
* ADT7420 TEMP SENSOR:
***********************************************************************************************************************/

#define TEMP_ADDR	0x90

typedef struct
{
	uint16_t temp
	unsigned long time;	//time offset from CPU start, in ms
} tempData;

/***********************************************************************************************************************
* Functions:
***********************************************************************************************************************/

MD_STATUS setup_accel();
MD_STATUS read_accel(accelData * dat);
MD_STATUS setup_light();
MD_STATUS read_light(lightData * dat);
MD_STATUS setup_temp();
MD_STATUS read_temp(tempData * dat);

#endif