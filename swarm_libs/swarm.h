#ifndef _SWARM_H_
#define _SWARM_H_

void swarm_init();
void swarm_send_produce(const char *swarm_id, const char *resource_id, const char *part_key, struct rsi_socketFrame_s *sock);
void swarm_produce(char *message, struct rsi_socketFrame_s *sock);

#endif  //swarm.h