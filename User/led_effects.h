/*
* led_effects.h
*
* Created: 28.10.2015 21:04:35
* Author: Nik
*/


#ifndef __LED_EFFECTS_H__
#define __LED_EFFECTS_H__
#include "led_ws2812.h"
#include "task_query.h"
#include <stdlib.h>

typedef enum {RAINBOW,COLOR_RING,FLAME,COLD,WARM,COLD_WHITE,WARM_WHITE} T_COLOR_PALETTE;


void SetBrightnessEffect(uint8_t effect,uint8_t speed);
void SetColorEffect(uint8_t effect,uint8_t speed);
void UpdateBrightness();
void UpdateColor();
T_PIXEL ColorFromPalette(T_COLOR_PALETTE palette,uint8_t colorInd);

#endif
