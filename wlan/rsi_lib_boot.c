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
 * @brief UART, AUTOBAUD, Autobaud function
 *
 * @section Description
 * Perform Autobaud
 */


/**
 * Includes
 */
#include "rsi_data_types.h"
#include "rsi_hal.h"
#include "rsi_uart_api.h"

/*===========================================================================
 * @fn          int16 rsi_autobaud(uint16 timeout)
 * @brief       Performs Autobaud
 * @param[in]	uint16 timeout, timeout value in MS
 * @return	errCode
 *               0xff = UART timeout error 
 *		 0 = OK
*/
int16
rsi_autobaud (void)
{
  int16 retval = UART_TIMEOUT_ERROR;
  uint8 retchar;
  uint8 retryCount = 0;
#if (DBG_LVL & PL3)
  DPRINT ("\r\nAutobaud Start");
#endif

  while (retryCount < ABRD_RETRY_COUNT)
  {
      /*
       * Clear the UART Rx FIFO 
       */
      do
      {
          retval = rsi_rgetchar (&retchar);
      } while (retval == UART_NOERROR);

      /*
       * send 0x1C, the autobaud character 
       */
      if (rsi_rputchar (0x1c) == UART_TIMEOUT_ERROR)
      {
          retryCount++;
          continue;
      }

      retval = rsi_rgetchar (&retchar);

      /*
       * Check for errors on read 
       */
      if (retval == UART_NOERROR)
      {
          if (retchar == 0x55)
          {
              /*
               * finish the autobaud handshake 
               */
              if (rsi_rputchar (0x55) == UART_TIMEOUT_ERROR)
              {
                  retryCount++;
                  continue;
              }
              break;
          }
      }
      retryCount++;
      rsi_delayMs (200);
  }

  if (retryCount == ABRD_RETRY_COUNT)
  {
    retval = UART_TIMEOUT_ERROR;
  }

  return retval;
}

/*===========================================================================
 *
 * @fn          int16 rsi_uart_welcome(void)
 * @brief       waits for and accepts the welcome message
 * @return      errcode
 *               0xff = uart timeout error 
 *		 0 = ok
 * read
 * welcome to redpine signals\r\n \r\n<space>
 * firmware upgrade (y/n)
*/
int16
rsi_uart_welcome (void)
{
  int16 retval;
  uint8 tBuf[40]; 
  uint16 maxLen = sizeof (tBuf);

  retval = RSI_ERROR;
#if (DBG_LVL & PL3)
  DPRINT ("\r\n\nWelcome Wait Start");
#endif

  /*
   * Get \r\n 
   */
  retval = rsi_getLine (tBuf, &maxLen);

  maxLen = sizeof (tBuf);
  /*
   * Receive "WELCOME TO REDPINE SIGNALS\r\n" message 
   */
  retval = rsi_getLine (tBuf, &maxLen);

  if (retval == UART_NOERROR)
  {
      maxLen = sizeof (tBuf);
     
      /*
       * Get \r\n 
       */
	  maxLen = 2;
      retval = rsi_getLine (tBuf, &maxLen);
      if (retval == UART_NOERROR)
      {
          /*
           * Receive "Firmware upgrade (y/n)" message 
           */
          maxLen = sizeof ("Firmware upgrade (y/n)");

          retval = rsi_getLine (tBuf, &maxLen);
          /*
           * If we received a (y/n), then respond with 'n' 
           */
          if (retval == UART_NOERROR)
          {
              rsi_rputchar ('n');
              rsi_rputchar ('\r');
              rsi_rputchar ('\n');

              maxLen = 3;
              retval = rsi_getLine (tBuf, &maxLen);

              maxLen = sizeof (tBuf);
              /*
               * Get "Loading... \r\n" message 
               */
              retval = rsi_getLine (tBuf, &maxLen);
              if (retval == UART_NOERROR)
              {
                  rsi_delayMs(50);
                  
                  maxLen = sizeof (tBuf);
                  /*
                   * Get "Loading Done\r\n" message 
                   */
                  retval = rsi_getLine (tBuf, &maxLen);

                  if ((retval == UART_NOERROR)
                      && (strlen ((char *) tBuf) > 10))
                  {
                      /*
                       * If "Done" not found 
                       */
                      if (strstr ((char *) tBuf, "Done") == NULL)
                      {
                          retval = RSI_ERROR;
#if (DBG_LVL & PL3)
                          DPRINT ("\r\nDone NOT Found");
#endif
                      }
                  }
              }
          }

      }
  }
#if (DBG_LVL & PL2)
  DPRINT ("\r\nWelcome Wait End, Error=%02x", (unsigned int) (retval));
#endif  
    
  return retval;
}

/*===========================================================================
 * @fn          int16 rsi_boot_device (void)
 * @brief       Performs ABRD
 * @retval      errcode
 *              0xff = uart timeout error 
 *		0 = ok
 */
int16
rsi_boot_device (void)
{
  int16 retval;

  /*
   * Perform Autobaud 
   */
  retval = rsi_autobaud ();
  if (retval == RSI_NOERROR)
  {
      /*
       * Check for Welcome 
       */
      retval = rsi_uart_welcome ();
  }
  
  return retval;
}
