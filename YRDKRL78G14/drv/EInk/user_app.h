#ifndef _USER_APP_H
#define _USER_APP_H

#define NUM_SWITCHES (3)
#define LED15       (P4_bit.no1)


uint8_t doDemo(uint8_t);
void initEink(void);
void setLogo(int logo);
void clearLogo(int logo);
#endif  //_USER_APP_H