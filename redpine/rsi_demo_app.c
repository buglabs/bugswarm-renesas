/* *
 * @file
 * Copyright(C) Redpine Signals 2011
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief HEADER, INIT STRUCT, function prototypes & default init values
 *
 */  


//#include "../gen/r_cg_macrodriver.h"
//#include "../gen/r_cg_serial.h"
#include "rsi_data_types.h"
#include "rsi_uart_api.h"
#include "rsi_hal.h"
#include "rsi_global.h"
#include "rsi_hal_api.h"
//#include "PinoutDef.h"
#include "rsi_demo_app.h"

extern uint8 			        socket0,socket1,socket2;
extern rsi_uUartRsp		        lib_rspBuf;

/**
 * @fn          uint8_t rsi_strrev(void *src, void *dst, uint8_t  len)
 * @brief       API to reverse the string
 * @src[in]     Pointer to the string to be reversed
 * @dst[out]    Pointer to the reversed string
 * @len         Number of bytes to be reserved
 * @return      true or false
 *          
 * @section description 
 * This function is used to reverse the string
 *  
 */
uint8_t rsi_strrev(void *src, void *dst, uint8_t  len)
{
   uint8_t  *dst1 = dst;
   uint8_t  *src1 = src;
   src1 = src1 + len - 1;     
   while(len-- != 0)
    *dst1++ = *src1--;
   
   *dst1 = '\0';
   return 0;
}

/**
 * @fn          void rsi_strcat(void *src, void *dst, uint8_t  len)
 * @brief       API to concatenate two strings
 * @src[in]     Pointer to the string to which other string to be concatenated
 * @dst[out]    Pointer to the string which has to be concatenated 
 * @len         Number of bytes to be concatenated
 * @return      none
 *          
 * @section description 
 * This function is used to concatenate two strings
 *  
 */
void rsi_strcat(void *src, void *dst, uint8_t  len)
{
   uint8_t *dst1 = dst;
   uint8_t *src1 = src;
   while(len-- != 0)
   *dst1++ = *src1++;
}

/**
 * @fn              uint16 rsi_convert_ip_to_string(uint8 *num_buff, uint8 *ip_buff)
 * @brief           API to convert IP address to dotted IP address string
 * @num_buff[in]    Pointer to the string which has to be converted to dotted IP address format
 * @ip_buff[out]    Pointer to dotted IP address format string
 * @return          none
 *          
 * @section description 
 * This function is used to convert IP address to dotted IP address string
 *  
 */
uint16 rsi_convert_ip_to_string(uint8 *num_buff, uint8 *ip_buff)
{

 	uint8 temp_buf[8];
	uint8_t temp_buf2[8];
	uint16 temp;
	uint16 num_index = 0, ip_index = 0;
	uint16 temp_index = 0, temp_index2 = 0;
	memset(ip_buff, 16,0);
	while (num_index < 4)
	{
          temp = *(num_buff + num_index);
          num_index++;
	  do
	  {
             *(temp_buf + temp_index) = (temp%10) + '0';
             temp_index++;
	     temp = temp/10;
	     ip_index++;
	  }while(temp);

	  rsi_strrev(temp_buf, temp_buf2, temp_index);
	  rsi_strcat(temp_buf2, ip_buff + temp_index2, temp_index);
          temp_index = 0;
          if (num_index < 4)
          {
            *(ip_buff + ip_index) = '.';
            ip_index++;
          }     
	  temp_index2 = ip_index;	
	}
        ip_buff[ip_index] = 0;
	return ip_index;
}


