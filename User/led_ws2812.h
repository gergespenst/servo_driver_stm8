/*
* led_ws2812.h
*
* Created: 28.10.2015 21:04:35
* Author: Nik
*/


#ifndef __WS2812_H__
#define __WS2812_H__

#include "stm8s.h"

#define NUM_OF_LEDS 5
#define LED_GPIO_PORT GPIOC
#define LED_GPIO_NUM_PIN 6
#define LED_GPIO_PIN  (1 << LED_GPIO_NUM_PIN)

#define INIT_LED_GPIO() (LED_GPIO_PORT)->CR2 &= (uint8_t)(~(LED_GPIO_PIN));\
						(LED_GPIO_PORT)->DDR |= (uint8_t)LED_GPIO_PIN;\
						(LED_GPIO_PORT)->CR1 |= (uint8_t)LED_GPIO_PIN;\
						(LED_GPIO_PORT)->ODR &= (uint8_t)(~(LED_GPIO_PIN));\
						(LED_GPIO_PORT)->DDR |= (uint8_t)LED_GPIO_PIN;

#define LED_PIN_HIGH() GPIOC->ODR |= LED_GPIO_PIN
#define LED_PIN_LOW() GPIOC->ODR &= ~LED_GPIO_PIN

typedef struct {
	uint8_t green;
	uint8_t red;
	uint8_t blue;
	uint8_t br;
} T_PIXEL;

void SendOnePixel(T_PIXEL* pixel);
void SendAllPixels();
uint8_t SetPixelColor(uint8_t pixnum,uint8_t r,uint8_t g,uint8_t b,uint16_t brightness);
uint8_t SetPixColor(uint8_t pixnum,T_PIXEL color,uint8_t br);
uint8_t SetPixBrightness(uint8_t pixnum,uint8_t br);
T_PIXEL GetPix(uint8_t pixnum);

#endif
