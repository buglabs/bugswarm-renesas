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

uint8_t i2cbuf[10];
uint8_t errbuf[30];

//Handle error reporting here, to spare r_main the complexity.
void md_err(MD_STATUS ret, const char * where){
	set_gpio(&P6, 2, 0);	
	sprintf(errbuf, "\r\n**i2cerr** ret=%02x at %s\r\n",ret,where);
	R_UART0_Send(errbuf, strlen(errbuf));
	delay_ms(100);
}

/***********************************************************************************************************************
* ADXL345 Accelerometer:
***********************************************************************************************************************/

MD_STATUS setup_accel(){
	MD_STATUS ret;
	memset(i2cbuf, 0x0, 10);
	
	//Configure the ADXL345 for +/- 2G range
	i2cbuf[0] = ACCEL_REG_DATA_FORMAT;
	i2cbuf[1] = ACCEL_BIT_RANGE2G;
	
	ret = R_IICA0_Master_Send(ACCEL_ADDR, i2cbuf, 2, 0);
	if (ret != MD_OK){
		md_err(ret,"format send");
		return ret;
	}
	
	delay_ms(1);
	
	//ADXL345 will use the default rate of 100Hz
	//Put the ADXL345 into measure mode - start capturing acceleration
	i2cbuf[0] = ACCEL_REG_POWER_CTL;
	i2cbuf[1] = ACCEL_BIT_MEASURE;
	
	ret = R_IICA0_Master_Send(ACCEL_ADDR, i2cbuf, 2, 0);
	if (ret != MD_OK){
		md_err(ret,"measure send");
		return ret;
	}
	
	delay_ms(1);
	
	return MD_OK;
}

MD_STATUS read_accel(accelData * dat){
	MD_STATUS ret;
	
	dat->x = 0;
	dat->y = 0;
	dat->z = 0;
	
	i2cbuf[0] = ACCEL_REG_DATA;
	
	//iica0_multibyte = 1;
	ret = R_IICA0_Master_Send(ACCEL_ADDR, i2cbuf, 1, 0);
	if (ret != MD_OK){
		md_err(ret, "register set");
		return ret;
	} 
	delay_ms(1);
	ret = R_IICA0_Master_Receive(ACCEL_ADDR, i2cbuf, 7, 0);
	if (ret != MD_OK){
		md_err(ret, "read");
		return ret;
	} else {
		dat->time = millis;
		dat->x = (uint16_t)i2cbuf[1] | ((uint16_t)i2cbuf[2] << 8);
		dat->y = (uint16_t)i2cbuf[3] | ((uint16_t)i2cbuf[4] << 8);
		dat->z = (uint16_t)i2cbuf[5] | ((uint16_t)i2cbuf[6] << 8);
	}
	
	//TODO - This should be a far shorter delay, on the order of 10-50 us
	//TODO - Check if this delay is even necessary...
	delay_ms(1);
	
	return ret;
}

/***********************************************************************************************************************
* PH5551A2NA1 LIGHT SENSOR:
***********************************************************************************************************************/

MD_STATUS setup_light(){
	MD_STATUS ret = MD_OK;
	return ret;
}

MD_STATUS read_light(lightData * dat){
	MD_STATUS ret = MD_OK;
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