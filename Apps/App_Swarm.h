#ifndef SWARM_H_
#define SWARM_H_

#include <jsmn/jsmn.h>

void App_SwarmConnector(void);
ATLIBGS_MSG_ID_E getResourceID (char * mac_addr_str, char * buff, int bufflen, char * result);
ATLIBGS_MSG_ID_E getAPIKey(char * buff, int bufflen, char * result);
ATLIBGS_MSG_ID_E makeAPICall(uint8_t * cid, char * buff, const char *format, ...);
ATLIBGS_MSG_ID_E makeAPIPOST(uint8_t * cid, char * buff, const char *format, ...);
ATLIBGS_MSG_ID_E App_SwarmProducer(uint8_t cid);
ATLIBGS_MSG_ID_E produce(uint8_t cid, const char *format, ...);
ATLIBGS_MSG_ID_E createProductionSession(uint8_t *cid, 
                                         char * hostIP,
                                         char * prod_swarm_id,
										 char * cons_swarm_id,
                                         char * resource_id,
                                         char * participation_key);
ATLIBGS_MSG_ID_E readOnePacket(char * buff, int len, int * written, uint32_t ms);
void readForAtLeast(uint8_t cid, uint32_t ms);
void parseMessage(char * pkt, uint8_t cid);
const char * ledValue(int idx);
int findKey(char * jsonpos, jsmntok_t * tokens, int toklen, const char * key);

#endif // SWARM_H_