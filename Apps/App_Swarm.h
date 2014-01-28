#ifndef SWARM_H_
#define SWARM_H_

#include <jsmn/jsmn.h>

void App_SwarmConnector(void);
ATLIBGS_MSG_ID_E dweetData(uint8_t cid);
const char * ledValue(int idx);
int findKey(char * jsonpos, jsmntok_t * tokens, int toklen, const char * key);

#endif // SWARM_H_