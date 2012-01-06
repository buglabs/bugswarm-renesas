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



#include "CG_serial.h"
#include "rsi_data_types.h"
#include "rsi_uart_api.h"
#include "rsi_hal.h"
#include "rsi_global.h"
#include "rsi_hal_api.h"
#include "PinoutDef.h"
#include "rsi_demo_app.h"

extern uint8 			        socket0,socket1,socket2;
extern rsi_uUartRsp		        lib_rspBuf;
uint8          	                        calc_thruput	= 0;
int                                     temperature_data = 0;   
static  volatile uint8                  led1 = 0, led2 = 0; //, led3 = 0;




/*! @brief This function is used to read IIC peripheral
 *
 *  @param  aAddr          Address of the peripheral
 *  @param  aRXData        Pointer to receive data
 *  @param  aNumBytes      Number of bytes to read        
 *
 *  @retval status        Returns None. The received data from peripheral 
 *                        will be captured into aRXData.
 */

void ReadIIC(unsigned char aAddr, unsigned char *aRXData, int aNumBytes)
{ 
    rsi_delayMs(10);
    gIica0RxComplete = 1;
    while(IICA0_MasterReceiveStart(aAddr | (0x01), aRXData, aNumBytes, 100) != MD_OK);
    while(gIica0RxComplete == 1);
}

/*! @brief This function is used to write to IIC peripheral
 *
 *  @param  aAddr          Address of the peripheral
 *  @param  aTXData        Pointer to data to be transmitted
 *  @param  aNumBytes      Number of bytes to be transmitted
 *
 *  @retval status        Returns None. The received data from peripheral 
 *                        will be captured into aRXData.
 */

void WriteIIC(uint8_t aAddr, uint8_t *aTXData, int aNumBytes)
{
   
    rsi_delayMs(10);
    gIica0TxComplete = 1;
    while(IICA0_MasterSendStart(aAddr & ~(0x01), aTXData, aNumBytes, 100) != MD_OK);
    while(gIica0TxComplete == 1);
}

/*! @brief This function is used to read and write to IIC peripheral
 *
 *  @param  aAddr          Address of the peripheral
 *  @param  aReg           Data to be written to peripheral
 *  @param  aRXData        Pointer to receive data
 *  @param  aNumBytes      Number of bytes to be exchanged
 *
 *  @retval status        Returns None. The received data from peripheral 
 *                        will be captured into aRXData.
 */
void ReadIICReg(unsigned char aAddr, unsigned char aReg, unsigned char *aRXData, int aNumBytes)
{
    WriteIIC(aAddr, &aReg, 1);
    ReadIIC(aAddr, aRXData, aNumBytes);
}

/*! @brief This function is used to process the messages received from Demo GUI
 *
 *  @param  ptrStr         Pointer to initialization parameters memory
 *  @param  usock          Pointer to UDP socket descriptor
 *  @param  ltsock         Pointer to TCP server socket descriptor
 *  @param  dltsock        Pointer to data server socket descriptor
 *
 *  @retval status        Returns None. The received data from peripheral 
 *                        will be captured into aRXData.
 */

void processdata(api *ptrStr, rsi_socketFrame_t *usock, rsi_socketFrame_t *ltsock,rsi_socketFrame_t *dltsock)
{
    
  uint8  		type = lib_rspBuf.rcv_data.buffer[0];
  

  if(lib_rspBuf.rcv_data.handle == socket0)
  {
  	switch(type)
  	{   	 
	  case LED_CNTRL: 
	       process_led_data(ltsock);
	       break;	  			
          default   :  
	      break;						  
  	} 
   }

}

/*! @brief This function is used to process the LED messages received from Demo GUI 
 * and transmits response
 *
 
 *  @param  ltcp_sock     Pointer to TCP server socket descriptor, on which data to be 
 *                        transmitted to Demo GUI.
 *  @retval status        Returns None. The received data from peripheral 
 *                        will be captured into aRXData.
 */
void process_led_data(rsi_socketFrame_t *ltcp_sock)
{
 
    uint8 txbuf[5];
    uint8 size =0;

    /* Led set request */
    if(lib_rspBuf.rcv_data.buffer[1] == 1)
    {
        /* LED1 */
        if(lib_rspBuf.rcv_data.buffer[2] == 0)
        {
            if(lib_rspBuf.rcv_data.buffer[3] == 1)
            {  
              /* LED1 ON */
              LED1 = FALSE;
	      lib_rspBuf.rcv_data.buffer[3] = led1 = 1;
            }
            else if(lib_rspBuf.rcv_data.buffer[3] == 0)
            {
              /* LED1 OFF */
              LED1 = TRUE;
              lib_rspBuf.rcv_data.buffer[3]= led1 = 0;
            }
        }     
        else if(lib_rspBuf.rcv_data.buffer[2] == 1) /* LED2 */
        {
            if(lib_rspBuf.rcv_data.buffer[3] == 1)
            {
              /* LED2 ON */
              LED2 = FALSE;
	      lib_rspBuf.rcv_data.buffer[3] = led2 = 1;
            }
            else if(lib_rspBuf.rcv_data.buffer[3] == 0)
            {
              /* LED1 OFF */
              LED2 = TRUE;
              lib_rspBuf.rcv_data.buffer[3]= led2 = 0;
            }      
        }
        else
        {
            lib_rspBuf.rcv_data.buffer[3] = 0;
        }
        /* LED set response message */
        txbuf[0] = LED_CNTRL;
        size +=1;
        txbuf[1] = 3;
        size +=1;
        txbuf[2] = lib_rspBuf.rcv_data.buffer[2];
        size +=1;
        txbuf[3] = lib_rspBuf.rcv_data.buffer[3];
        size +=1;      
    }  
    else if(lib_rspBuf.rcv_data.buffer[1] == 0)           /* LED Get request */
    {
        txbuf[0] = LED_CNTRL;
        size +=1;
        txbuf[1] = 2;
        size +=1;
        txbuf[2] = lib_rspBuf.rcv_data.buffer[2];
        size +=1;
    
        if(lib_rspBuf.rcv_data.buffer[2] == 0)
        {
          txbuf[3]= 0;
        }
        else if(lib_rspBuf.rcv_data.buffer[2] == 1)
        {
          txbuf[3]= 0; 
        }
        else 
        {
          txbuf[3]= 0; 
        }
        size +=1;
        rsi_delayMs(40);
    
    }
    /* Send LED Response message */
    ltcp_sock->buf = (uint8 *)txbuf;
    ltcp_sock->buf_len = size;
    if (rsi_send(ltcp_sock) != RSI_NOERROR)
    {
        rsi_delayMs(100);
    } 
  
}

/**
 * @fn          void send_sensor_reading(rsi_socketFrame_t *ltcp_sock)
 * @brief       API to read command response.
 * @param[in]   rsi_socketFrame_t *ltcp_sock,pointer to socket frame structure
 * @param[out]  none
 * @return      none
 *          
 * @section description 
 * This function is used to send temperature and Light sensor reading to a remote peer over a TCP socket
 *  
 */
void send_sensor_reading(rsi_socketFrame_t *ltcp_sock)
{
    int16     sensor_value;
    char      txbuf[4];
    unsigned char instrCode;
    uint8_t   readData[] = {0, 0};
    unsigned char readData1[] = {
            0,  // LSB
            0   // MSB
      }; 
    
    if(temperature_data == 0)
    {
      /* Read the temperature value from the sensor */
      ReadIICReg(TEMP_ADDR, TEMP_REG, readData, 2);
      sensor_value = (int16)(((readData[0] << 8) + readData[1]) * 0.0078);    
      txbuf[0] = TEMPERATURE_CNTRL;
      temperature_data = 1;
    }
    else
    {
      /* Light sensor */
      instrCode = LS_INSTR;
      WriteIIC(LS_ADDR, &instrCode, 1);
      ReadIIC(LS_ADDR, readData1, 1);
      sensor_value = ((readData1[1] << 8) + readData1[0]);  
      txbuf[0] = LIGHTSENSOR_CNTRL;
      temperature_data = 0;
    }
    memcpy(txbuf+1,(char *)&sensor_value,2);
    ltcp_sock->buf = (uint8 *)txbuf;
    ltcp_sock->buf_len = 3;
    /*Send the sensor value to the GUI */
    if (rsi_send(ltcp_sock) != RSI_NOERROR)
    {
         rsi_delayMs(100);
    }

}

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


