/*-------------------------------------------------------------------------*
 * File:  led.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Pin out definition file
 *
 *-------------------------------------------------------------------------*/
#ifndef __LED_H
#define __LED_H

#include <ior5f104pj.h>
#include <stdint.h>

typedef struct {
    unsigned char *iPn;
    unsigned char *iPMn;
} T_GPIOPort;

#define LED_ON	0
#define LED_OFF 1

#define NUM_LEDS 13


void led_init(void);
void led_all_off(void);
void led_all_on(void);
void led_on(int n);
void led_off(int n);
uint8_t led_get(int n);
void led_task(void);
void LEDFlash(uint32_t timeout);

#endif // _PINOUTDEF_H
