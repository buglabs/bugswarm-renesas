/**
 * @file
 * Copyright(C) 2011 Redpine Signals Inc.
 * All rights reserved by Redpine Signals.
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief  AT command interface for -22 module.
 *
 * @author  
 *         Redpine
 */

#include "../gen/r_cg_macrodriver.h"
#include "rsi_data_types.h"
#include "rsi_uart_api.h"
#include "rsi_at_command_processor.h"
#include "rsi_hal.h"
#include <stdio.h>

#define IS_RX_PENDING() (rsi_read_buf->pending)


#define RSI_AT_READ_CMD_LEN 11
/*! @brief Data read AT command */
static const char *rsi_at_read_str = "AT+RSI_READ";

#define RSI_AT_CLOSE_CMD_LEN 12
/*! @brief Socket close AT command */
static const char *rsi_at_sk_cls_str = "AT+RSI_CLOSE";

/*! @brief AT command's common prefix */
static const char *rsi_at_cmd_hdr = "at+rsi_";

/*! @brief AT command common prefix length */
static const int16 rsi_at_cmd_hdr_len = 7;

/*! @brief Pointer to the rx write buffer  */
struct rsi_recv_s *rsi_write_buf;
/*! @brief Pointer to the rx read buffer  */
struct rsi_recv_s *rsi_read_buf;

/*! @brief AT commands array */
static const char *rsi_at_commands[] = {
  BAND_STR,
  INIT_STR,
  SCAN_PASSIVE_STR,
  SCAN_STR,
  SCAN_NEXT_STR,
  SCAN_NUM_STR,
  NETWORK_TYPE_STR,
  AUTH_MODE_STR,
  PRE_SHARED_KEY_STR,
  JOIN_STR,
  DISCONNECT_STR,
  POWER_MODE_STR,
  SLEEP_TIMER_STR,
  IP_CONFIG_STR,
  TCP_CONNECT_REQ_STR,
  OPEN_LUDP_STR,
  OPEN_UDP_STR,
  OPEN_LTCP_STR,
  TCP_CSTATUS_STR,
  CLOSE_SOCKET_STR,
  TX_DATA_STR,
  FW_VERSION_STR,
  MAC_STR,
  QRY_RSSI_STR,
  QRY_NET_PARAMS_STR,
  QRY_BSSID_STR,
  QRY_NWTYPE_STR,
  QRY_NUMSCAN_STR,
  SOFT_RESET_STR,
  BAUD_RATE_STR,
  OPEN_MULTICAST_STR,
  DNS_SERVER_STR,
  DNS_GET_STR,
  CFG_SAVE_STR,
  CFG_ENABLE_STR,
  CFG_GET_STR
};

/*! @brief structure used maintain state */
static struct rsi_host_app_cb_s rsi_host_cb;

/*! @brief Array used to store socket types(TCP/UDP) */
static uint8 rsi_lib_sock_array[MAX_SOCKET_ID];

/*! @brief converts integer number to ASCII
 *
 *  @param  temp_dig     integer to be converted to ASCII 
 *  @param  dst          character pointer to hold the ASCII equivalent value
 *  @param  dst_len      MAX length of the dst buffer
 *
 *  @retval uint16       Actual size of the dst buffer used to store the ASCII value
 */
uint16
rsi_os_itoa (uint32 temp_dig, uint8 * dst, uint16 dst_len)
{
  int16 ii, jj;
  uint8 t_buf[10];

  if (temp_dig == 0)
  {
      dst[0] = ITOA_8 (0);
      dst[1] = '\0';
      return 1;
  }
  for (ii = 0; (temp_dig != 0) && (ii < 10); ii++)
  {
      t_buf[ii] = ITOA_8 ((uint8) (temp_dig % 10));
      temp_dig = temp_dig / 10;
  }
  ii--;
  for (jj = 0; (ii >= 0) && (dst_len--); ii--, jj++)
  {
      dst[jj] = t_buf[ii];
  }
  dst[jj] = '\0';

  return jj;
}


/*! @brief Sends command/data to UART interface
 *
 *  @param  tx_buf        Buffer to be sent
 *  @param  cmd_len       Length of the buffer
 *
 *  @retval status        Returns the return value of rputchar function
 */
int16
rsi_send_cmd_to_device (uint8 * tx_buf, uint16 cmd_len)
{
  uint16 ii;
  int16 status = 0;
  for (ii = 0; (ii < cmd_len) && (status == UART_NOERROR); ii++)
  {
      if ((tx_buf[ii] == '\r') && (tx_buf[ii + 1] == '\n')
          && (ii != (cmd_len - 2)))
      {
          status = rsi_rputchar (0xDB);
          status = rsi_rputchar (0xDC);
          ii++;
      }
      else if (tx_buf[ii] == 0xDB)
      {
          status = rsi_rputchar (0xDB);
          status = rsi_rputchar (0xDD);
      }
      else
      {
          status = rsi_rputchar (tx_buf[ii]);
      }
  }
  return status;
}

/*! @brief Reads the data/ any other asynchronous packet
 *
 *  @param  rsp           A generic pointer to store the data/async packet
 *  @param  event[OUT]    An Event (RSI_EVENT_RX_DATA, RSI_EVENT_SOCKET_CLOSE, 
 *                        RSI_EVENT_SLEEP, RSI_EVENT_CMD_RESPONSE) is stored.
 *
 *  @retval 0 for success
 *  @retval RSI_ERROR_NO_RX_PENDING if there is no packet. 
 *  @retval RSI_ERROR_JUNK_PKT_RECVD if junk packet is received
 *
 */
int16
rsi_read_data(void *rsp, uint16 *event)
{
  int16 status = RSI_ERROR;
  uint8 *tmp_rx_buf;
  uint8 lib_sock_id;

  if (!IS_RX_PENDING())
    return RSI_ERROR_NO_RX_PENDING;

  tmp_rx_buf = rsi_read_buf->rx_buf;

  *event = 0;

  /* Handle receive data */
  if (strncmp ((char*)tmp_rx_buf, rsi_at_read_str, RSI_AT_READ_CMD_LEN) == 0)
  {
	  //printf("rcv ");
    *event = RSI_EVENT_RX_DATA;
    rsi_process_recv_data(tmp_rx_buf + RSI_AT_READ_CMD_LEN, rsp);
    status = RSI_NOERROR;
  }
  /* process socket close at command */
  else if (strncmp ((char*)tmp_rx_buf, rsi_at_sk_cls_str, RSI_AT_CLOSE_CMD_LEN) == 0)
  {
	  //printf("sock ");
    *event =  RSI_EVENT_SOCKET_CLOSE;
    lib_sock_id = tmp_rx_buf[RSI_AT_CLOSE_CMD_LEN];
    if (lib_sock_id <= MAX_SOCKET_ID)
    {
      rsi_lib_sock_array[lib_sock_id] = LIB_PROTO_CLOSED;
    }
    *(uint8*)rsp = lib_sock_id;
    status = RSI_NOERROR;
  }
  else if (strncmp ((char*)tmp_rx_buf, "SLEEP", 5) == 0)
  {
    *event =  RSI_EVENT_SLEEP;
     status = RSI_NOERROR;
     //printf("sleep ");
  }
  else /* Command response */
  {
     status  = rsi_read_cmd_rsp(rsp);
     if (status != RSI_ERROR_NO_CMD_RSP_PENDING)
     {
	     //printf("cmd ");
       *event = RSI_EVENT_CMD_RESPONSE;
     }
     else
     {
	//printf("oth ");
       status = RSI_ERROR_JUNK_PKT_RECVD;
       rsi_process_raw_data(tmp_rx_buf, rsp);
       rsi_update_read_buf();
       //printf("(ot) ");
     }
     return status;
  }
  
  if (*event)
  {
    rsi_update_read_buf();
    //printf("(ev) ");
  }

  return status;
}

/*! @brief Updates the read buffer
 *
 */
void rsi_update_read_buf(void)
{
  //DISABLE_RX_UART_INTERRUPT();
  rsi_read_buf->pending = 0;
  rsi_read_buf->rx_buf_len = 0;
  rsi_read_buf = rsi_read_buf->next;
  //printf("CLR ");
  //READ_CHAR(); /*TODO Add code to flush UART RX FIFO */
  //ENABLE_RX_UART_INTERRUPT();
}

/*! @brief Reads the response of an AT command issued
 *
 *  This function is used to read the response for an AT command
 *
 *  @param  rsp   A generic pointer. A union,  which has all the possible 
 *                response types is passed to this function.
 *  @retval 0 for success Error code for failure
 *  Error Codes
 *   -1  Waiting for the connection from peer
 *   -2  Unable to allocate socket 
 *   -3  Deauthentication from the Access Point
 *   -4  Illegal IP/Port Parameter
 *   -5  TCP/IP Configuration Failure
 *   -6  Invalid Socket
 *   -7  Association not done
 *   -8  Error in command
 *   -9  Error with byte stuffing for escape characters
 *  -10  IP Lease expired
 *  -11  TCP Connection Closed
 *  -16  DHCP Failure
 * -124  Connection establishment not supported
 * -127	 Socket already exists
 * -12   Pre-Shared Key not sent for connection to a secure access point
 * -13   No Access Points Scanned
 * -14   INIT command already issued. 
 * -15   JOIN command already issued. 
 * -17   Baud Rate Not Supported
 * -18   Encryption mode not supported
 */
int16
rsi_read_cmd_rsp (void *rsp)
{
  uint8 *tmp_rx_buf;
  int16 status = RSI_ERROR_NO_CMD_RSP_PENDING;

  if (!IS_RX_PENDING())
    return RSI_ERROR_NO_RX_PENDING;

  tmp_rx_buf = rsi_read_buf->rx_buf;

  /* Process the command response */
  if ( ((tmp_rx_buf[0] == 'O') && (tmp_rx_buf[1] == 'K')) ||
       ((tmp_rx_buf[0] == 'E') && (tmp_rx_buf[1] == 'R')) )
  {
    status = rsi_process_cmd_resp (tmp_rx_buf, rsi_read_buf->rx_buf_len, rsp);
  //printf("(rcr) ");
  }
  //TODO - ARG, this needs to be above, but it breaks normal AT command responses!
  //More specfically, this needs to be in the if above
  //But then it causes mysterious race conditions when initalizing the WIFI card.
  rsi_update_read_buf(); 
  
  return status;
}

/*! @brief Sends an AT command.
 *
 *  This function is used to send an AT command to -22 module.
 *
 *  @param  cmd_type      The type of an AT command.
 *  @param  args          A generic pointer to pass the arguments for an AT command.
 */
int16
rsi_send_at_command (uint16 cmd_type, void *args)
{
  uint8 *cmd = rsi_host_cb.at_cmd;
  uint16 cmd_len = 0;
  int16 tlen = 0;
  uint16_t temp_sz;
  rsi_socketFrame_t *sd = NULL;
  int16 status = UART_TIMEOUT_ERROR;

  if (cmd_type > MAX_AT_COMMANDS)
  {
      return RSI_ERROR;
  }

  /* Put the prefix hdr */
  memcpy (cmd, rsi_at_cmd_hdr, rsi_at_cmd_hdr_len);
  cmd += rsi_at_cmd_hdr_len;

  /* Put message type hdr */
  temp_sz = strlen(rsi_at_commands[cmd_type]);
  memcpy (cmd, rsi_at_commands[cmd_type], temp_sz);
  cmd += temp_sz;

  switch (cmd_type)
  {
    case INIT:
    case DISCONNECT:
    case SCAN_NEXT:
    case FWVERSION:
    case MAC_REQ:
    case QRY_RSSI:
    case QRY_NET_PARAMS:
    case QRY_BSSID:
    case QRY_NWTYPE:
    case QRY_NUMSCAN:
    case SOFT_RESET:
    case CFG_SAVE:
    case CFG_GET:
      break;
    case BAND_SEL:
    case AUTH_MODE_SEL:
    case POWER_MODE_SEL:
    case TCP_CSTATUS:
    case CLOSE_SOCKET:
    case CFG_ENABLE:
      PUT_PARAM_8 (cmd, ITOA_8 (*(uint8 *) args));
      break;
    case SCAN_PASSIVE:
      tlen = rsi_os_itoa (*(uint8 *) args, cmd, 4);
      cmd += tlen;
      break;
    case SCAN:
      {
        struct rsi_scanFrameSnd_s *s = (struct rsi_scanFrameSnd_s *) args;
        tlen = rsi_os_itoa (s->channel, cmd, 3);
        cmd += tlen;
        temp_sz = strlen ((char *) s->ssid);
        if (temp_sz > 0)
        {
            *cmd++ = ',';
            strcpy ((char *) cmd, (char *) s->ssid);
            cmd += temp_sz;
        }
      }
      break;
    case SCAN_NUM:
      tlen = rsi_os_itoa (*(uint8 *) args, cmd, 3);
      cmd += tlen;
      break;
    case NETWORK_TYPE_SEL:
      {
        struct rsi_nwType_s *nw = (struct rsi_nwType_s *) args;

        if (nw->nwType == NWTYPE_INFRASTRUCTURE)
        {
            PUT_STR (cmd, "INFRASTRUCTURE");
        }
        else
        {
            if (nw->nwType == NWTYPE_IBSS)
            {
                PUT_STR (cmd, "IBSS");
            }
            else
            {
                PUT_STR (cmd, "IBSS_SEC");
            }
            PUT_PARAM_8 (cmd, ',');
            /* IBSS type */
            PUT_PARAM_8 (cmd, ITOA_8 (nw->ibss_type));
            PUT_PARAM_8 (cmd, ',');
            /* IBSS channel num */
            tlen = rsi_os_itoa (nw->channel_num, cmd, 3);
            cmd += tlen;
        }
      }
      break;
    case PRE_SHARED_KEY:
      {
        temp_sz = strlen ((char *) args);
        memcpy (cmd, (uint8 *) args, temp_sz);
        cmd += temp_sz;
      }
      break;
    case JOIN:
      {
        struct rsi_joinFrameSnd_s *jn = (struct rsi_joinFrameSnd_s *) args;
        temp_sz = strlen ((char *) jn->ssid);
        strcpy ((char *) cmd, (char *) jn->ssid);
        cmd += temp_sz;
        PUT_PARAM_8 (cmd, ',');
        tlen = rsi_os_itoa (jn->dataRate, cmd, 3);
        cmd += tlen;
        PUT_PARAM_8 (cmd, ',');
        tlen = rsi_os_itoa (jn->powerLevel, cmd, 3);
        cmd += tlen;
      }
      break;
    case SLEEP_TIMER:
      tlen = rsi_os_itoa ((*(uint16 *) args), cmd, 5);      /* MAX value is 10000 */
      cmd += tlen;
      break;
    case IP_CONFIG:
      {
        rsi_ipparamFrameSnd_t *ip_cfg = (rsi_ipparamFrameSnd_t *) args;

        PUT_PARAM_8 (cmd, ITOA_8 (ip_cfg->dhcpMode));

        /* If manual configuration enabled ? */
        if (ip_cfg->dhcpMode == 0)
        {
            PUT_PARAM_8 (cmd, ',');
            PUT_STR (cmd, ip_cfg->ipaddr);

            PUT_PARAM_8 (cmd, ',');
            PUT_STR (cmd, ip_cfg->netmask);     /* Subnet mask */

            PUT_PARAM_8 (cmd, ',');
            PUT_STR (cmd, ip_cfg->gateway);
        }
      }
      break;
    case TCP_CONNECT_REQ:
    case OPEN_UDP:
    case OPEN_MULTICAST:
      {
        rsi_socketFrame_t *tc = (rsi_socketFrame_t *) args;
        if (tc == NULL)
          return RSI_ERROR;
        PUT_STR (cmd, tc->remote_ip);
        PUT_PARAM_8 (cmd, ',');
        tlen = rsi_os_itoa (tc->rport, cmd, 6);
        cmd += tlen;
        PUT_PARAM_8 (cmd, ',');
        tlen = rsi_os_itoa (tc->lport, cmd, 6);
        cmd += tlen;
      }
      break;
    case OPEN_LTCP:
    case OPEN_LUDP:
      {
        rsi_socketFrame_t *tc = (rsi_socketFrame_t *) args;
        if (tc == NULL)
          return RSI_ERROR;
        tlen = rsi_os_itoa (tc->lport, cmd, 6);
        cmd += tlen;
      }
      break;
    case TX_DATA:
      {
        uint16 tlen;
        sd = (rsi_socketFrame_t *) args;
        if (sd == NULL)
          return RSI_ERROR;

        PUT_PARAM_8 (cmd, ITOA_8 (sd->handle));
        PUT_PARAM_8 (cmd, ',');

        tlen = rsi_os_itoa (sd->buf_len, cmd, 8);
        cmd += tlen;

        PUT_PARAM_8 (cmd, ',');
        PUT_STR (cmd, sd->remote_ip);
        PUT_PARAM_8 (cmd, ',');

        tlen = rsi_os_itoa (sd->rport, cmd, 6);
        cmd += tlen;
        PUT_PARAM_8 (cmd, ',');
      }
      break;
    case BAUD_RATE:
      tlen = rsi_os_itoa ((*(uint32 *) args), cmd, 8);  /* MAX value 3600000 */ 
      cmd += tlen;
      break;
    case DNS_SERVER:
      {
         PUT_STR (cmd, (uint8*)args);
      }
      break;
    case DNS_GET:
      PUT_STR (cmd, (uint8*)args);
      break; 
    default:
#if (DBG_LVL & PL1)
      //DPRINT("Wrong Comamnd !!!");
#endif
      return RSI_ERROR;
  } 

  /* Change the state */
  rsi_host_cb.state = cmd_type;
  cmd_len = cmd - rsi_host_cb.at_cmd;
  
  /* Send command */
  status = rsi_send_cmd_to_device (rsi_host_cb.at_cmd, cmd_len);

  /* Send data */
  if (cmd_type == TX_DATA)
  {
      if (sd && sd->buf)
      {
          status = rsi_send_cmd_to_device (sd->buf, sd->buf_len);
      }
      else
      {
          return RSI_ERROR;
      }
  }

  /* Send tail */
  rsi_rputchar ('\r');
  rsi_rputchar ('\n');

  return status;
}

/*! @brief Processes the response of AT command.
 *
 *  This function is used to process the response of AT command.
 *  The decoded message is stored in drsp. 
 *
 *  @param  rsp           A character buffer, which contains the response.
 *  @param  len           Length of the "rsp" buffer.
 *  @param  drsp          decoded response buffer. It is an output parameter.
 *
 *  @retval -1            Returns error code for failure
 *  @retval  0            Returns 0 for the success.
 */
int16
rsi_process_cmd_resp (uint8 *rsp, uint16 len, uint8 * drsp)
{
  uint8 lib_sock_id;
  
//#if (DBG_LVL & PL3)
//  DPRINT ("rsi_process_cmd_resp\r\n");
//#endif

  if ((rsp[0] != 'O') || (rsp[1] != 'K'))
  {
     return ((int16) rsp[5]);
  }

  rsp += 2;                     /* OK len */
  len -= 2;

  len -= AT_MSG_TAIL_SZ;

  switch (rsi_host_cb.state)
  {
    case SCAN_SENT:
    case SCAN_NEXT_SENT:
      if (len < rsi_read_buf->rx_buf_max_size) /* MAX 10 scan responses */
      memcpy (drsp, rsp, len);
      break;
    case SCAN_NUM_SENT:
    case SCAN_PASSIVE_SENT:
    case BAND_SEL_SENT:
    case INIT_SENT:
    case NETWORK_TYPE_SENT:
    case PRE_SHARED_KEY_SENT:
    case JOIN_SENT:
    case AUTH_MODE_SEL_SENT:
    case DISCONNECT_SENT:
    case SOFT_RESET_SENT:
    case BAUD_RATE_SENT:
    case DNS_SERVER_SENT:
    case CFG_SAVE_SENT:
    case CFG_ENABLE_SENT:
      break;
    case POWER_MODE_SENT:
    case SLEEP_TIMER_SENT:
      break;
    case OPEN_LUDP_SENT:
    case OPEN_UDP_SENT:
      lib_sock_id = *rsp;
      if (lib_sock_id <= MAX_SOCKET_ID)
      {
        rsi_lib_sock_array[lib_sock_id] = LIB_PROTO_UDP;
      }
      if (drsp != NULL)
      {
          memcpy (drsp, rsp, len);
      }
      break;
    case OPEN_LTCP_SENT:
    case TCP_CONNECT_REQ_SENT:
    case OPEN_MULTICAST:
      lib_sock_id = *rsp;
      if (lib_sock_id <= MAX_SOCKET_ID)
      {
        rsi_lib_sock_array[lib_sock_id] = LIB_PROTO_TCP;
      }
      if (drsp != NULL)
      {
          memcpy (drsp, rsp, len);
      }
      break;
    case TCP_CSTATUS_SENT:
    case IP_CONFIG_SENT:
      if (drsp != NULL)
      {
          memcpy (drsp, rsp, len);
      }
      break;
    case TX_DATA_SENT:
    case FWVERSION_SENT:
    case MAC_REQ_SENT:
    case QRY_RSSI_SENT:
    case QRY_NET_PARAMS_SENT:
    case QRY_BSSID_SENT:
    case QRY_NWTYPE_SENT:
    case QRY_NUMSCAN_SENT:
    case DNS_GET_SENT:
    case CFG_GET_SENT:
      if (drsp != NULL)
      {
          memcpy (drsp, rsp, len);
      }
      break;
    default:;
      //return RSI_ERROR;
    }

  return UART_NOERROR;
}


/*! @brief Processes the received data
 *
 *
 *  @param  at_cmd        Buffer which has AT+RSI_READ  command.
 *  @param  rsp[OUT]      decoded response buffer. It is an output parameter.
 *
 */
void
rsi_process_recv_data(uint8 *at_cmd, void *rsp)
{
  rsi_recvSocketFrame_t * rcv = (rsi_recvSocketFrame_t*)rsp;

  
  /* Handle */
  rcv->handle = at_cmd[0];
	  
  /* Data len */
  rcv->buf_len = at_cmd[2];
  rcv->buf_len <<= 8;
  rcv->buf_len |= at_cmd[1];

  /* Data pointer for TCP */
  rcv->buffer = at_cmd + 3;
  
  if (rcv->handle <= MAX_SOCKET_ID)
  {
    if (rsi_lib_sock_array[rcv->handle] == LIB_PROTO_UDP)
    {
      /* IP */
      memcpy(rcv->remote_ip, at_cmd + 3, 4);
      rcv->rport = at_cmd[8]; 
      rcv->rport <<= 8;
      rcv->rport |= at_cmd[7]; 
      
      /* For UDP add length for IP(4) and port(2) */
      rcv->buffer += 6;
    }
  }

  return;
}

void rsi_process_raw_data(uint8 *at_cmd, void *rsp)
{
  rsi_recvSocketFrame_t * rcv = (rsi_recvSocketFrame_t*)rsp;

  rcv->buffer = at_cmd;

  return;
}

/*! @brief Initializes the Library
 */
void rsi_lib_init(void)
{
  rsi_host_cb.state = 0;
}

/*! @brief Receives a byte and stores it in a buffer
 *         And sets the pending event if \r\n (delimiter) is received
 */
void rsi_receive (void)
{
  uint8 temp_ch;
  uint8 *tmp_rx_buf = &rsi_write_buf->rx_buf[rsi_write_buf->rx_buf_len];
  
  if (rsi_write_buf->pending)
  {
    return;
  }
  if(UART_GetChar(&temp_ch) == UART_NOERROR)  
  {
    *tmp_rx_buf = temp_ch;
    rsi_write_buf->rx_buf_len++;
    //printf("*%c*",temp_ch);
    /* Check for \r\n delimiter */
    if ( ( (*tmp_rx_buf == '\n') && (rsi_write_buf->rx_buf_len > 1) && 
	    (*(tmp_rx_buf - 1) == '\r') ) ||
            (rsi_write_buf->rx_buf_len == rsi_write_buf->rx_buf_max_size)  )
    {
      rsi_write_buf->pending = 1;
      rsi_write_buf = rsi_write_buf->next;     
      //printf("NEW ");
    }
    tmp_rx_buf++;
  }
  
}

/*! @brief Sets the Rx buffer 
 *  @param buffer    Address of the buffer
 *  @param buf_size  Size of the buffer
 */
void rsi_set_rx_buffer (uint8 *buffer, uint16 buf_size)
{
  struct rsi_recv_s *tmp;
  struct rsi_recv_s *new_buf;

  new_buf = (struct rsi_recv_s*)buffer;
  new_buf->rx_buf = (uint8*)(new_buf + 1);
  new_buf->rx_buf_max_size = buf_size - sizeof(struct rsi_recv_s);
  new_buf->rx_buf_len = 0;
  new_buf->pending = 0;

  /* If it is the first one */
  if (!rsi_write_buf)
  {
    new_buf->next = new_buf;
    rsi_write_buf = new_buf;
    rsi_read_buf = new_buf;
    return;
  }

  /* Go to the end of the list */
  for ( tmp = rsi_write_buf; tmp->next != rsi_write_buf; tmp = tmp->next);
  tmp->next = new_buf;
  new_buf->next = rsi_write_buf;

  return;
}
/**
 * @fn          uint16 rsi_rgetchar(uint8 *dataByte)
 * @brief	Gets a byte from the UART interface
 * @param[out]	dataByte: Holds the read byte
 * @return	0xff UART timeout Failure 0- Success
 */
uint16 rsi_rgetchar (uint8 * dataByte)
{
    uint16 retryCount = 0;
    while ( (retryCount < 10) && (rsi_write_buf->rx_buf_len == 0) )
    {
       rsi_delayMs(5);
       retryCount++;
    }
    if (rsi_write_buf->rx_buf_len > 0)
    {
      *dataByte = rsi_write_buf->rx_buf[0];
      rsi_write_buf->rx_buf_len = 0;
      //printf(" g%02x ",*dataByte);
      return RSI_NOERROR;
    }
    return UART_TIMEOUT_ERROR;
}

/**
 * @fn          int16 rsi_getLine(uint8 *buffer, uint16 *maxLen)
 * @brief	Gets a string, ending with \r\n from the UART 
 * @param[in]	uint8 *buffer, data is returned here 
 * @param[in/out] maxLen Maximum limit to read, 
                  It also returns the number of bytes read
 * @return	-1 failure
                 0 success
 */  

int16 rsi_getLine (uint8 * buffer, uint16 * maxLen)
{
  uint16 retval = UART_TIMEOUT_ERROR;
  uint16 retryCount = 0;

  while( (retryCount < 50) && (!(rsi_read_buf->pending || (rsi_read_buf->rx_buf_len >= *maxLen) )))
  {
    /* Wait for some time */
    rsi_delayMs(1);
    retryCount++;
  }

  if (retryCount != 10)
  {    
    memcpy(buffer, rsi_read_buf->rx_buf, rsi_read_buf->rx_buf_len);
    if (!rsi_read_buf->pending)
    {
      rsi_read_buf->rx_buf_len = 0;
    }
    else
    {
	    
      rsi_update_read_buf();
      //printf("(gl) ");
    }
    retval = RSI_NOERROR;
  }
          
  return retval;
}

/**
 * @fn	        int16 rsi_putchar(uint8 c)
 * @brief       Transmits a character over UART interface
 * @param[in]	char c, character to be sent over uart
 * @retval      0xff - UART Timeout error, 
 *              0 - Success
 */
int16 rsi_rputchar (uint8 c)
{
  int16 status = UART_TIMEOUT_ERROR;
  int16 retryCount = 0;

  do
  {
    status = UART_PutChar ((uint8) c);
    retryCount++;
    //rsi_delayUs(1);
    rsi_delayMs(1);
  }while ((status != UART_NOERROR) && (retryCount < 10));
  
  return status;
}

/* EOF */
/*
$Log: $ 
*/
