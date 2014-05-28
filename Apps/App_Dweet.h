#ifndef DWEET_H_
#define DWEET_H_

#include <jsmn/jsmn.h>

void App_DweetConnector(void);
ATLIBGS_MSG_ID_E dweetData(uint8_t* cid);
ATLIBGS_MSG_ID_E checkData(uint8_t* cid);
int findKey(char * jsonpos, jsmntok_t * tokens, int toklen, const char * key);
int getValue(char * jsonpos, jsmntok_t * tokens, int toklen, const char * key, char* outValue);
ATLIBGS_MSG_ID_E httpRequest(ATLIBGS_HTTPSEND_E type, uint16_t timeout, char* page, char* inOutBuffer, uint8_t* cid);

#endif // DWEET_H_