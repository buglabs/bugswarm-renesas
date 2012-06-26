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

#include "../gen/r_cg_macrodriver.h"
#include "../redpine/rsi_data_types.h"
#include "../redpine/rsi_uart_api.h"

/***********************************************************************************************************************
* Sensor specific defines and structures
************************************************************************************************************************
* ADXL345 Accelerometer:
***********************************************************************************************************************/

#define ACCEL_ADDR		0x3A

//Rate register - write any of the RATExHZ values into the ACCEL_REG_RATE register directly.
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

//Power control - set ACCEL_BIT_MEASURE high to enter measurement mode (actually take samples)
#define ACCEL_REG_POWER_CTL	0x2D
#define ACCEL_BIT_MEASURE	(1<<3)

//Data format - write the RANGExG values into ACCEL_REG_DATA_FORMAT to set the measurement range
//set ACCEL_BIT_FULL_RES high to enable full resolution (up to 13-bit)
//set ACCEL_BIT_JUSTIFY to set the data format - see ADXL345 datasheet.
#define ACCEL_REG_DATA_FORMAT	0x31
#define ACCEL_BIT_FULL_RES	(1<<3)
#define ACCEL_BIT_JUSTIFY	(1<<2)
#define ACCEL_BIT_RANGE2G	0x0
#define ACCEL_BIT_RANGE4G	0x1
#define ACCEL_BIT_RANGE8G	0x2
#define ACCEL_BIT_RANGE16G	0x3

#define ACCEL_REG_DATA		0x32

//Accelerometer scale
#define ACCEL_SCALE 		256.0	//LSB per G force

typedef struct
{
	signed int xraw;		//X axis, proportional to G force
	signed int yraw;		//Y axis, proportional to G force
	signed int zraw;		//Z axis, proportional to G force
	float x;
	float y;
	float z;
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

//These bits are used for the instruction code - used with every write
#define LIGHT_BIT_SEL_MEASURE	(1<<7)
#define LIGHT_BIT_SEL_CONTROL_N	(1<<6)
#define LIGHT_BIT_PTR_INC	(1<<4)
#define LIGHT_BIT_PTR_REG	(1<<5)
#define LIGHT_BIT_POWER		(1<<0)

//LIGHT_REG_CONTROL is only valid in control mode.  Most intterupt-related registers have been omitted
//set LIGHT_BIT_RESOLUTION high to enable the 16 lux multiplier
//set LIGHT_BIT_POWER high to enable the sensor
#define LIGHT_REG_CONTROL	0x00
#define LIGHT_BIT_RESOLUTION	(1<<2)
//#define LIGHT_BIT_POWER		(1<<0)




typedef struct
{
	uint16_t light;
	unsigned long time;	//time offset from CPU start, in ms
} lightData;

/***********************************************************************************************************************
* ADT7420 TEMP SENSOR:
***********************************************************************************************************************/

#define TEMP_ADDR	0x90

#define TEMP_REG_DATA		0x00

#define TEMP_REG_CONFIG		0x03
#define TEMP_BIT_CONTINUOUS	(0x00 << 5)
#define TEMP_BIT_ONESHOT	(0x01 << 5)
#define TEMP_BIT_1SPS		(0x02 << 5)
#define TEMP_BIT_OFF		(0x03 << 5)
#define TEMP_BIT_HIRES		(1 << 7)

typedef struct
{
	uint16_t raw;
	float tempC;
	float tempF;
	unsigned long time;	//time offset from CPU start, in ms
} tempData;

/***********************************************************************************************************************
* Functions:
***********************************************************************************************************************/

//Sensor access
MD_STATUS setup_accel();
MD_STATUS read_accel(accelData * dat);
MD_STATUS setup_light();
MD_STATUS read_light(lightData * dat);
MD_STATUS setup_temp();
MD_STATUS read_temp(tempData * dat);

extern rsi_socketFrame_t      outsock;

//LED manipulation
void set_led(uint8_t num, uint8_t val);
void toggle_led(uint8_t num);

void sendButtonInfo();
boolean doWork(unsigned long duration);
boolean readData();
void button_callback(uint8_t num, uint8_t value);
void getButtons(uint8_t * buttons);
#endif