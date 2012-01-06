/**
 * @file
 *
 * Copyright(C) 2011 Redpine Signals Inc.
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief
 * UART, INIT, Wifi Initialization
 *
 */

 /**
  * Includes
  */
#include "rsi_data_types.h"
#include "rsi_uart_api.h"
#include "rsi_at_command_processor.h"
#include "rsi_hal.h"
#include "rsi_global.h"
#include  <stdarg.h>
#include  <stdlib.h>
#include  <string.h>
#include "glyph_api.h"
#include "lcd.h"
#include "rsi_demo_app.h"


/**
 * Global Variables
 */
rsi_uUartRsp lib_rspBuf;

void read_qry_rsp(void)
{
  int16 retval;
  
    do {
        rsi_delayMs(5);
        retval = rsi_read_cmd_rsp(&lib_rspBuf);
    }while (retval == RSI_ERROR_NO_RX_PENDING);
}

/*! @brief This function is used to send BAND, INIT, Scan, 
 *         Network type, authmode, psk, join and IP config commands in the order.
 *
 *  @param  band          BAND value
 *  @param  sf            Pointer to scan frame
 *  @param  jf            Pointer to join frame
 *  @param  ip_f          Pinter to ip config frame
 *         
 *
 *  @retval status        Returns the return value of wait_for_rsp function.
 */
int16
rsi_wifi_init (uint8 band_val, rsi_scanFrameSnd_t * sf, rsi_joinFrameSnd_t * jf,
            rsi_ipparamFrameSnd_t * ip_f)
{
  int16     retval = RSI_NOERROR;  
  int16     count = 0;
  int16     total_scaned_ap_count = 0;  // Total number of access points scanned  
  volatile  uint16 is_ap_found = 0; 
  uint8     ipstring_buffer[16];     
  volatile  uint16 cmd_type = CMD_BAND_SEL; 
  
  
  while (1)
  {
    switch (cmd_type)
    {
      case CMD_BAND_SEL:           
           /* Band Command */
           retval = rsi_band (band_val);           
           break;
      case CMD_INIT:
           retval = rsi_init_baseband ();
           break;
      case CMD_QRY_FWVERSION:                      
           retval = rsi_query_fwversion ();
           break;
      case CMD_SCAN_NUM:
           retval = rsi_scan_num (NUM_AP_SCAN);
           break;
      case CMD_SCAN:           
           retval = rsi_scan (sf);           
           break;
      case CMD_QRY_NUMSCAN:
           rsi_query_scan_num();
           break;
      case CMD_SCAN_NEXT:
           rsi_scan_next();          
           break;      
      case CMD_NETWORK_TYPE_SEL:
           retval =
               rsi_setNetworkType (jf->nwType, jf->ibssMode, jf->ibssChannel);
           break;
      case CMD_AUTH_MODE_SEL:
           retval = rsi_set_auth_mode (jf->auth_mode);
           break;
      case CMD_PRE_SHARED_KEY:
           retval = rsi_set_psk (jf->psk);
           break;
      case CMD_JOIN:           
           retval = rsi_join (jf);
           break;
      case CMD_IP_CONFIG:
           rsi_delayMs(20);
           retval = rsi_ipparam_set (ip_f);
           break;
      default:
           return retval;
    }
    
    /* Wait for response  from module */
    do {
        rsi_delayMs(10);
        retval = rsi_read_cmd_rsp(&lib_rspBuf);
    }while (retval == RSI_ERROR_NO_RX_PENDING);
    
    if (retval == RSI_NOERROR)
    {
      if(cmd_type == CMD_SCAN)
      {        
         for(count = 0; count < 3 ; count++)
         {
           if(strcmp((char const *)lib_rspBuf.scanInfo[count].ssid, (char const *)&jf->ssid[0])== 0)
           {               
              // Skip CMD_QRY_NUMSCAN;
              cmd_type++;
              // skip CMD_SCAN_NEXT
              cmd_type++;                          
              break;
           }      
         }
         cmd_type++;
      }
      else if(cmd_type == CMD_QRY_NUMSCAN)
      {
        total_scaned_ap_count = lib_rspBuf.scanned_AP_count;
        total_scaned_ap_count = (total_scaned_ap_count - 3);
        cmd_type++;
      }
      else if(cmd_type == CMD_SCAN_NEXT)
      {
        if(total_scaned_ap_count > 0)
        {
            for(count = 0; count < 3 ; count++)
            {
                if(strcmp((char const *)lib_rspBuf.scanInfo[count].ssid, (char const *)&jf->ssid[0]) == 0)
                {
                    /* send CMD_NETWORK_TYPE_SEL */
                    cmd_type++;                  
                    is_ap_found = 1;
                    break;
                }           
            }
            total_scaned_ap_count = (total_scaned_ap_count - 3);
            if((total_scaned_ap_count <= 0) && (is_ap_found == 0))
            {
              LCDString("AP Not Found", LCDRight(8)-50, 20);  
              while(1);
            }
         }
        
      }     
      else if(cmd_type == CMD_IP_CONFIG )
      {
        /* Convert IP address to dotted string */
        rsi_convert_ip_to_string(lib_rspBuf.IpparamRsp.ipaddr, ipstring_buffer);
        /* Display AP name to which WiFi module is conneted */ 
        //LCDString((char const *)jf->ssid, LCDRight(8)-55, 15);
        /* Display IP address of the WiFi module */
        LCDString((char const *)ipstring_buffer, LCDRight(8)-55, 1); 
       // LCDString("Status:Connected", LCDRight(8)-55, 35);          /* Comment this if CMD_QRY_FWVERSION in #if 0 is enabled */
        cmd_type++;
      }
      else
      {      
         cmd_type++;
      }

    }
    else
    {
       if(cmd_type == CMD_JOIN)
       {
         LCDString("Connection Failed", LCDRight(8)-50, 25);         
       }
       if(cmd_type == CMD_IP_CONFIG)
       {
         //LCDString("IP Config Failed", LCDRight(8)-50, 50);
       }
      
        rsi_delayMs(40); 
    }

  }
}






