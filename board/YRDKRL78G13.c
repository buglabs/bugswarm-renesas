/***********************************************************************************************************************
* File Name    : YRDKRL78G13.h
* Version      : 0.1 [16 APR 2012]
* Device(s)    : R5F100LE
* Tool-Chain   : CA78K0R
* Description  : Implements board-specific functions of the YRDKRL78G13 dev board
* Creation Date: 4/16/2012
***********************************************************************************************************************/

#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
#include "r_cg_it.h"
#include "r_cg_serial.h"
#include "YRDKRL78G13.h"
#include <stdio.h>
#include <string.h>

uint8_t i2cbuf[10];

//Handle error reporting here, to spare r_main the complexity.
void md_err(MD_STATUS ret, const char * where){
	set_gpio(&P6, 2, 0);	
	printf("\r\n**i2cerr** ret=%02x at %s\r\n",ret,where);
	delay_ms(100);
}

/***********************************************************************************************************************
* ADXL345 Accelerometer:
***********************************************************************************************************************/

MD_STATUS setup_accel(){
	MD_STATUS ret;
	memset(i2cbuf, 0x0, 10);
	
	//Configure the ADXL345 for +/- 16G range, full resolution
	i2cbuf[0] = ACCEL_REG_DATA_FORMAT;
	i2cbuf[1] = ACCEL_BIT_RANGE16G | ACCEL_BIT_FULL_RES;
	
	while (IICBSY0){ ; } 	//Make sure bus is ready for xfer
	ret = R_IICA0_Master_Send(ACCEL_ADDR, i2cbuf, 2, 0);
	if (ret != MD_OK){
		md_err(ret,"accel set format");
		return ret;
	}
	while (IICBSY0){ ; } 	//Wait until the xfer is complete
	//ADXL345 will use the default rate of 100Hz
	//Put the ADXL345 into measure mode - start capturing acceleration
	i2cbuf[0] = ACCEL_REG_POWER_CTL;
	i2cbuf[1] = ACCEL_BIT_MEASURE;
	
	ret = R_IICA0_Master_Send(ACCEL_ADDR, i2cbuf, 2, 0);
	if (ret != MD_OK){
		md_err(ret,"accel measure mode");
		return ret;
	}
	while (IICBSY0){ ; } 	//Wait until the xfer is complete
	return MD_OK;
}

MD_STATUS read_accel(accelData * dat){
	MD_STATUS ret;
	
	dat->x = 0;
	dat->y = 0;
	dat->z = 0;
	
	while (IICBSY0){ ; } 	//Make sure bus is ready for xfer
	i2cbuf[0] = ACCEL_REG_DATA;
	ret = R_IICA0_Master_Send(ACCEL_ADDR, i2cbuf, 1, 0);
	if (ret != MD_OK){
		md_err(ret, "accel set reg");
		return ret;
	} 
	while (IICBSY0){ ; } 	//Wait until the xfer is complete
	ret = R_IICA0_Master_Receive(ACCEL_ADDR, i2cbuf, 6, 0);
	while (IICBSY0){ ; } 	//Wait until the xfer is complete
	if (ret != MD_OK){
		md_err(ret, "accel read");
		return ret;
	} else {
		dat->time = millis;
		dat->x = (uint16_t)i2cbuf[0] | ((uint16_t)i2cbuf[1] << 8);
		dat->y = (uint16_t)i2cbuf[2] | ((uint16_t)i2cbuf[3] << 8);
		dat->z = (uint16_t)i2cbuf[4] | ((uint16_t)i2cbuf[5] << 8);
	}
	
	return ret;
}

/***********************************************************************************************************************
* PH5551A2NA1 LIGHT SENSOR:
***********************************************************************************************************************/

MD_STATUS setup_light(){
	MD_STATUS ret = MD_OK;
	while (IICBSY0){ ; } 	//Make sure bus is ready for xfer
	i2cbuf[0] = LIGHT_BIT_SEL_MEASURE | LIGHT_BIT_PTR_INC | LIGHT_BIT_POWER;
	ret = R_IICA0_Master_Send(LIGHT_ADDR, i2cbuf, 1, 0);
	if (ret != MD_OK){
		md_err(ret, "light set reg");
		return ret;
	}
	while (IICBSY0){ ; } 	//Wait until the xfer is complete
	return ret;
}

MD_STATUS read_light(lightData * dat){
	MD_STATUS ret = MD_OK;
	int len = 0; 
	int i;
	
	dat->light = 0;
	
	//while (IICBSY0 || iica0_busy){ ; } 	//Make sure bus is ready for xfer
	//i2cbuf[0] = LIGHT_BIT_SEL_MEASURE | LIGHT_BIT_PTR_INC | LIGHT_BIT_POWER;
	//ret = R_IICA0_Master_Send(LIGHT_ADDR, i2cbuf, 1, 0);
	//if (ret != MD_OK){
	//	md_err(ret, "light set reg");
	//	return ret;
	//} 
	while (IICBSY0){ ; } 	//Wait until the xfer is complete
	ret = R_IICA0_Master_Receive(LIGHT_ADDR, i2cbuf, 2, 0);
	while (IICBSY0){ ; } 	//Wait until the xfer is complete
	if (ret != MD_OK){
		md_err(ret, "light read");
		return ret;
	} else {
		dat->time = millis;
		dat->light = (uint16_t)i2cbuf[0] | ((uint16_t)i2cbuf[1] << 8);
	}
	return ret;
}

/***********************************************************************************************************************
* ADT7420 TEMP SENSOR:
***********************************************************************************************************************/

MD_STATUS setup_temp(){
	MD_STATUS ret = MD_OK;
	return ret;
}

MD_STATUS read_temp(tempData * dat){
	MD_STATUS ret = MD_OK;
	return ret;
}