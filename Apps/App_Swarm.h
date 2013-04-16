
#ifndef SWARM_H_
#define SWARM_H_

void App_SwarmConnector(void);
ATLIBGS_MSG_ID_E getResourceID(char * macstr, char * resource_id);
ATLIBGS_MSG_ID_E createProductionSession();
ATLIBGS_MSG_ID_E produceData(uint8_t cid);

#endif // SWARM_H_