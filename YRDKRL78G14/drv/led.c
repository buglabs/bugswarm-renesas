/*
*******************************************************************************
* Copyright(C) 2011 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation. 
*
* 
* Filename	: led.c
* Version	: 1.0
* Device:	: RL78/G13 R5F100LE
* Description	: This file implements LED function.
*******************************************************************************
*******************************************************************************
* History	: DD.MM.YYYY Version Description
*		: 09.29.2011 1.00    First version.
******************************************************************************/

/*
******************************************************************************
* Include files
******************************************************************************
*/
//#include "r_cg_macrodriver.h"
#include "led.h"
#include <mstimer.h>
#include <string.h>

// LEDS: LED3-LED15
uint8_t G_LEDPort[] = { 6, 4, 6, 4, 6, 4, 6, 4, 6, 15, 6, 10, 4 };
uint8_t G_LEDPin[] = { 2, 2, 3, 3, 4, 4, 5, 5, 6, 2, 7, 1, 1 };
uint8_t G_LEDStatus[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Buttons
uint8_t G_BtnPort[] = { 7, 7, 7 };
uint8_t G_BtnPin[] = { 6, 4, 5 };	

const T_GPIOPort PORT0 = {
    (unsigned char *)&P0,
    (unsigned char *)&PM0,
};

const T_GPIOPort PORT1 = {
    (unsigned char *)&P1,
    (unsigned char *)&PM1,
};

const T_GPIOPort PORT2 = {
    (unsigned char *)&P2,
    (unsigned char *)&PM2,
};

const T_GPIOPort PORT3 = {
    (unsigned char *)&P3,
    (unsigned char *)&PM3,
};

const T_GPIOPort PORT4 = {
    (unsigned char *)&P4,
    (unsigned char *)&PM4,
};

const T_GPIOPort PORT5 = {
    (unsigned char *)&P5,
    (unsigned char *)&PM5,
};

const T_GPIOPort PORT6 = {
    (unsigned char *)&P6,
    (unsigned char *)&PM6,
};

const T_GPIOPort PORT7 = {
    (unsigned char *)&P7,
    (unsigned char *)&PM7,
};

const T_GPIOPort PORT10 = {
    (unsigned char *)&P10,
    (unsigned char *)&PM10,
};

const T_GPIOPort PORT12 = {
    (unsigned char *)&P12,
    (unsigned char *)&PM12,
};

const T_GPIOPort PORT13 = {
    (unsigned char *)&P13,
    0
};

const T_GPIOPort PORT14 = {
    (unsigned char *)&P14,
    (unsigned char *)&PM14,
};

const T_GPIOPort PORT15 = {
    (unsigned char *)&P15,
    (unsigned char *)&PM15,
};

const T_GPIOPort *GPIOTable[] = {
	&PORT0,
	&PORT1,
	&PORT2,
	&PORT3,
	&PORT4,
	&PORT5,
	&PORT6,
	&PORT7,
	0,
    0,
    &PORT10,
    0,
    &PORT12,
    &PORT13,
    &PORT14,
    &PORT15
};
void GPIO_OuputMode(unsigned char aPort, unsigned char aPin)
{
    *GPIOTable[aPort]->iPMn &= ~(1<<aPin);
}

void GPIO_SetHigh(unsigned char aPort, unsigned char aPin)
{
    *GPIOTable[aPort]->iPn |= (1<<aPin);
}

void GPIO_SetLow(unsigned char aPort, unsigned char aPin)
{
    *GPIOTable[aPort]->iPn &= ~(1<<aPin);
}

void GPIO_InputMode(unsigned char aPort, unsigned char aPin)
{
    *GPIOTable[aPort]->iPMn |= (1<<aPin);
}

uint8_t GPIO_Read(unsigned char aPort, unsigned char aPin)
{
    return (*GPIOTable[aPort]->iPn & (1<<aPin)) >> aPin;
}

uint8_t gSetLight_onoff;
void led_init(void)
{
   int n;
  for(n=0; n<NUM_LEDS; ++n)
  {
      GPIO_OuputMode(G_LEDPort[n], G_LEDPin[n]);
  }
}

void led_all_off(void)
{
  int n;
  for(n=0; n<NUM_LEDS; ++n)
  {
      GPIO_SetHigh(G_LEDPort[n], G_LEDPin[n]);
	  G_LEDStatus[n] = 0;
  }
}

void led_all_on(void)
{
  int n;
  for(n=0; n<NUM_LEDS; ++n)
  {
      GPIO_SetLow(G_LEDPort[n], G_LEDPin[n]);
	  G_LEDStatus[n] = 0;
  }
}

void led_on(int n)
{
    GPIO_SetLow(G_LEDPort[n], G_LEDPin[n]);
	G_LEDStatus[n] = 1;
}

void led_off(int n)
{
    GPIO_SetHigh(G_LEDPort[n], G_LEDPin[n]);
	G_LEDStatus[n] = 0;
}

uint8_t led_get(int n) {
	return G_LEDStatus[n];
}

void led_task(void)
{
	// LED1 = gSetLight_onoff;
  if(gSetLight_onoff)
    led_all_on();
  else
    led_all_off(); 
}

void LEDFlash(uint32_t timeout)
{
    static int flag =1;
    uint32_t start = MSTimerGet();

    while(1) 
    {
        /* Timeout? */
        if (MSTimerDelta(start) >= 500) 
        {  
            if(flag)
            {
              led_all_on();
            }
            else
            {
              led_all_off(); 
            }
            flag ^=1;
            start = MSTimerGet();
        }
    }
}


