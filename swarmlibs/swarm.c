#include "../gen/r_cg_macrodriver.h"
#include "../redpine/rsi_data_types.h"
#include "../redpine/rsi_uart_api.h"
#include "../redpine/rsi_at_command_processor.h"
#include "../redpine/rsi_hal.h"
#include "../redpine/rsi_global.h"
#include "../redpine/rsi_hal_api.h"
//#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

const char produce_header[] = "POST /stream?swarm_id=%s&"
  "resource_id=%s HTTP/1.1\r\nHost:api.bugswarm.com\r\n"
  "x-bugswarmapikey:%s\r\ntransfer-encoding:chunked\r\nConnection:keep-alive"
  "\r\nContent-Type: application/json\r\n\r\n1\r\n\n\r\n";
const char list_resource_header[] = "GET /resources HTTP/1.1\r\n"
	"Host: api.bugswarm.net\r\n"
	"Accept-Encoding: identity\r\n"
	"x-bugswarmapikey: 53ebd64e50ed786ef13f3e64f0cdbdd6223013bc\r\n\r\n";
const char feed_request[] = "{\"capabilities\": {\"feeds\": [\"Acceleration\",\"Temperature\",\"Button\",\"Light\",\"Sound Level\",\"Potentiometer\"]}}";
const char message_header[] = "%x\r\n{\"message\": {\"payload\":%s}}\r\n\r\n";

char pktbuff[500];
int pktlen;
uint16_t okcode;

void swarm_init(){
  
}

void swarm_send_produce(const char *swarm_id, const char *resource_id, const char *part_key, struct rsi_socketFrame_s *sock){
  memset(pktbuff, '\0', sizeof(pktbuff));
  sprintf(pktbuff, produce_header, swarm_id, resource_id, part_key);
  sock->buf = (uint8 *)pktbuff;
  sock->buf_len = strlen((const char *)sock->buf);
  rsi_send(sock);
}

void swarm_get_resources(struct rsi_socketFrame_s *sock){
  memset(pktbuff, '\0', sizeof(pktbuff));
  sprintf(pktbuff, list_resource_header);
  sock->buf = (uint8 *)pktbuff;
  sock->buf_len = strlen((const char *)sock->buf);
  rsi_send(sock);
}

boolean swarm_produce(char *message, struct rsi_socketFrame_s *sock){
  char resp[2];
  memset(pktbuff, '\0', sizeof(pktbuff));
  pktlen = strlen(message)+sizeof(message_header)-9;
  sprintf(pktbuff,message_header,pktlen,message);
  sock->buf = (uint8 *)pktbuff;
  sock->buf_len = strlen((const char *)sock->buf);
  rsi_send(sock);
  /*okcode = rsi_read_cmd_rsp((uint8*)resp);
  printf("produce returned %04x: %02x %02x\r\n",okcode,resp[0],resp[1]);
  if (okcode < 0x0003UL){
	 rsi_update_read_buf();
  } else {
	printf("should restart now\r\n");
	return 0;
  }*/
  return 1;
}

void capabilities_announce(struct rsi_socketFrame_s *sock){
  swarm_produce((char *)feed_request, sock);
}