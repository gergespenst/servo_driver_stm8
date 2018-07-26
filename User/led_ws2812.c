#include "led_ws2812.h"



T_PIXEL out_data_set[NUM_OF_LEDS] = {
			{.red=0xFF,.green=0x00,.blue=0x00},
			{.red=0xFF,.green=0x7F,.blue=0x00},
			{.red=0xFF,.green=0xFF,.blue=0x00},
			{.red=0x00,.green=0xFF,.blue=0x00},
			{.red=0x00,.green=0x00,.blue=0xFF}
};



#pragma optimize=z none
void SendOnePixel(T_PIXEL* pixel){
	volatile uint16_t x = 0;
	volatile uint8_t out_data;
	for(uint8_t j = 0; j < 3; j++){
		out_data = (((uint8_t*)pixel)[j]);
		if(out_data & 0x80){
			LED_PIN_HIGH();
			x++;
			x++;
			LED_PIN_LOW();
		}else{
			LED_PIN_HIGH();
			x++;
			LED_PIN_LOW();
			x++;
		}
		if(out_data & 0x40){
			LED_PIN_HIGH();
			x++;
			x++;
			LED_PIN_LOW();
		}else{

			LED_PIN_HIGH();
			x++;
			LED_PIN_LOW();
			x++;
		}
		if(out_data & 0x20){
			LED_PIN_HIGH();
			x++;
			x++;
			LED_PIN_LOW();
		}else{

			LED_PIN_HIGH();
			x++;
			LED_PIN_LOW();
			x++;
		}
		if(out_data & 0x10){
			LED_PIN_HIGH();
			x++;
			x++;
			LED_PIN_LOW();
		}else{

			LED_PIN_HIGH();
			x++;
			LED_PIN_LOW();
			x++;
		}
		if(out_data & 0x08){
			LED_PIN_HIGH();
			x++;
			x++;
			LED_PIN_LOW();
		}else{
			LED_PIN_HIGH();
			x++;
			LED_PIN_LOW();
			x++;
		}
		if(out_data & 0x04){
			LED_PIN_HIGH();
			x++;
			x++;
			LED_PIN_LOW();
		}else{
			LED_PIN_HIGH();
			x++;
			LED_PIN_LOW();
			x++;
		}
		if(out_data & 0x02){
			LED_PIN_HIGH();
			x++;
			x++;
			LED_PIN_LOW();
		}else{
			LED_PIN_HIGH();
			x++;
			LED_PIN_LOW();
			x++;
		}
		if(out_data & 0x01){
			LED_PIN_HIGH();
			x++;
			x++;
			LED_PIN_LOW();
		}else{
			LED_PIN_HIGH();
			x++;
			LED_PIN_LOW();
			x++;
		}
	}
}


uint8_t SetPixelColor(uint8_t pixnum,uint8_t r,uint8_t g,uint8_t b,uint16_t brightness){
	if(pixnum > NUM_OF_LEDS - 1) return 1;

	out_data_set[pixnum].blue = 	b;
	out_data_set[pixnum].red = 		r;
	out_data_set[pixnum].green = 	g;
	out_data_set[pixnum].br = brightness;

	return 0;
}

uint8_t SetPixColor(uint8_t pixnum,T_PIXEL color,uint8_t br){

	if(pixnum > NUM_OF_LEDS - 1) return 1;

	out_data_set[pixnum].blue = 	(color.blue);
	out_data_set[pixnum].red = 		(color.red );
	out_data_set[pixnum].green = 	(color.green );
	out_data_set[pixnum].br = br;


	return 0;
}

//#pragma optimize=z none
void SendAllPixels(){
	T_PIXEL temp_pix ;
	 for(uint8_t i = 0; i < NUM_OF_LEDS; i++){
		 temp_pix.blue = (out_data_set[i].blue * 	out_data_set[i].br) >> 8;
		 temp_pix.red  = (out_data_set[i].red  * 	out_data_set[i].br) >> 8;
		 temp_pix.green = (out_data_set[i].green * out_data_set[i].br) >> 8;
		 SendOnePixel(&(temp_pix));
	 }
}



uint8_t SetPixBrightness(uint8_t pixnum,uint8_t br){
	if(pixnum > NUM_OF_LEDS - 1) return 1;
	out_data_set[pixnum].br = br;
	return 0;
}

T_PIXEL GetPix(uint8_t pixnum){
	return out_data_set[pixnum];
}
