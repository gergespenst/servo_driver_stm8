#include "led_effects.h"

static uint8_t 	g_colorEffect = 0, g_colorSpeed = 0,
				g_brEffect = 0, g_brSpeed = 0;
T_COLOR_PALETTE g_currentPalette = COLOR_RING;

T_PIXEL rainbow[7] = {
		{.red=0xFF,.green=0x00,.blue=0x00},//red
		{.red=0xFF,.green=0x7F,.blue=0x00},//orange
		{.red=0xFF,.green=0xFF,.blue=0x00},//yellow
		{.red=0x00,.green=0xFF,.blue=0x00},//green
		{.red=0x42,.green=0xaa,.blue=0xff},//light blue
		{.red=0x00,.green=0x00,.blue=0xFF},//blue
		{.red=0x8b,.green=0x00,.blue=0xFF} //violet

};


#define BRIGHT_STEPS 16
uint8_t linBr[BRIGHT_STEPS] = {0 , 1 , 2 , 3 , 5 ,
					 8 , 13, 21, 34, 55,
					 89, 144, 233, 238, 243,
					 255};

#define IND_MAX_BRIGHT (BRIGHT_STEPS - 1)
#define MAX_BRIGHT	linBr[IND_MAX_BRIGHT]
#define LIN_BRIGH(x) linBr[(x%BRIGHT_STEPS)]
#define INV_LINBRIGH(x) linBr[IND_MAX_BRIGHT - ((x)%BRIGHT_STEPS)]

T_PIXEL Wheel(uint8_t WheelPos) {//ѕалитра типа цветового круга r - g - b -r
	T_PIXEL temp_pix;
  if(WheelPos < 85) {
	  temp_pix.red = 255 - WheelPos * 3;
	  temp_pix.green = WheelPos * 3;
	  temp_pix.blue = 0;
	  temp_pix.br = 0xff;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
	  temp_pix.red = 0;
	  temp_pix.green = 255 - WheelPos * 3;
	  temp_pix.blue = WheelPos * 3;
	  temp_pix.br = 0xff;
  } else {
   WheelPos -= 170;
	  temp_pix.red = WheelPos * 3;
	  temp_pix.green = 0;
	  temp_pix.blue = 255 - WheelPos * 3;
	  temp_pix.br = 0xff;
  }
  return temp_pix;
}



//‘ункци€ выбирает цвета из палитры. ÷вета от 0 до 255
T_PIXEL ColorFromPalette(T_COLOR_PALETTE palette,uint8_t colorInd){
	T_PIXEL tempPixel;
	switch (palette) {
		case RAINBOW:{
			tempPixel = rainbow[colorInd%7];
					}break;
		case COLOR_RING:{
			tempPixel = Wheel(colorInd);
					}break;
		case FLAME:{
			colorInd = ((uint16_t)colorInd*(uint16_t)84)/255;
			if(colorInd%84 < 42)
                            tempPixel = Wheel( colorInd%42);
			else
                            tempPixel = Wheel(42 - colorInd%42);
					}break;
		case COLD:{
			if(colorInd >= 128)
				colorInd = 127 - colorInd%128;

			if(colorInd <= 128){
			tempPixel.red = 0;
			tempPixel.green = 0xFF - colorInd*2;
			tempPixel.blue = colorInd*2;
			}
					}break;
		case WARM:{
			if(colorInd >= 128)
				colorInd = 127 - colorInd%128;

			if(colorInd <= 128){
			tempPixel.red =  0xFF - colorInd*2;
			tempPixel.green = 0;
			tempPixel.blue = colorInd*2;
			}

		}break;
		case COLD_WHITE:{
			tempPixel.red = 0xf0;
			tempPixel.green = 0xf8;
			tempPixel.blue = 0xff;
					}break;
		case WARM_WHITE:{
			tempPixel.red = 0xff;
			tempPixel.green = 0xcf;
			tempPixel.blue = 0x48;
					}break;
		default:
			break;
	}
	tempPixel.br = 0xFF;
	return tempPixel;
}


void LinUpDown(){
	static uint16_t duty = 1,sign;
	for(uint8_t i = 0; i < NUM_OF_LEDS; i++){
			SetPixBrightness(i,LIN_BRIGH(duty));
	}
	(sign)?(duty = duty - 1):( duty = duty + 1);
		if(duty == 15) sign = TRUE;
		if(duty == 0) sign = FALSE;
}

void LinUpDownChet(){
	static uint16_t duty = 1,sign;
	for(uint8_t i = 0; i < NUM_OF_LEDS; i++){
		if(i%2 == 0)
			SetPixBrightness(i,LIN_BRIGH(duty));
		else
			SetPixBrightness(i,INV_LINBRIGH(duty));
	}
	(sign)?(duty = duty - 1):( duty = duty + 1);
		if(duty == 15) sign = TRUE;
		if(duty == 0) sign = FALSE;
}

#define FIRE_LENGT 5
void RunFireBr(){
	static uint8_t indOfFire = 0;

	for(uint8_t i = 0; i < FIRE_LENGT; i++){
		SetPixBrightness( (indOfFire - i)%(NUM_OF_LEDS + 1 ),
							INV_LINBRIGH( (15*i)/(FIRE_LENGT-1) )
						);
	}
	indOfFire++;
}

void OneRandomBlink(){
	//пробегаем по €ркости от 0 до 31
	//если €ркость от 16 до 31 - €ркость увеличиваетс€
	//если €ркость от 0 до 15 - €ркость уменьшаетс€
	//если €ркость равна 0 - значит элемент снова погашен, мен€ем индекс
	static uint8_t index = 0,
				   br = IND_MAX_BRIGHT*2 + 1;

	if(0 == br){//€ркость вернулась на максимум, мен€ем индекс и ставим минимум €ркости
		SetPixBrightness(index,LIN_BRIGH(br));
		index = (rand()%255)%NUM_OF_LEDS;
		br = IND_MAX_BRIGHT*2 + 1;
	}
	if(br > 15)
		SetPixBrightness(index,INV_LINBRIGH(br));
	else
		SetPixBrightness(index,LIN_BRIGH(br));
	br--;

}

void UpdateBrightness(){
	if(g_brEffect == 0)	RunFireBr();
	if(g_brEffect == 1)	LinUpDown();
	if(g_brEffect == 2)	LinUpDownChet();
	if(g_brEffect == 3) OneRandomBlink();
}

////////////////////////////////////////////
void Rotate(){
	static	uint8_t shift = 0;
		for(uint8_t i = 0; i < NUM_OF_LEDS; i++){
			SetPixColor(i,ColorFromPalette(g_currentPalette,i*10 + shift),GetPix(i).br);
		}
		shift++;
}

void StackColor(){
static T_PIXEL tempPixel;
static uint8_t curPixInd = 0;
static uint8_t curEndOfLine = NUM_OF_LEDS -1;

	if(curPixInd < curEndOfLine){
		tempPixel = GetPix(curPixInd);
		SetPixColor(curPixInd,GetPix(curPixInd + 1),MAX_BRIGHT);
		SetPixColor(curPixInd + 1,tempPixel,MAX_BRIGHT);

		curPixInd++;
	}else{
		curPixInd = 0;
		curEndOfLine--;
		if(curEndOfLine == 1) {
			curEndOfLine = NUM_OF_LEDS - 1;
		//	SetPixColor(0,ColorFromPalette(WARM_WHITE,rand()%255),MAX_BRIGHT);
		}

	}

}

void StackComet(){
	//Ќулевой огонек начинает движение к концу линии
	//стира€ за собой пиксели
	//как только дойдет до конца линии то в нулевой позиции по€вл€етс€
	//случайный пиксель и начинает бежать до второй с конца позиции и тд
	static T_PIXEL tempPixel;
	static uint8_t curPixInd = 0;
	static uint8_t curEndOfLine = NUM_OF_LEDS ;

	if(curPixInd == 0){

		SetPixColor(curPixInd ,tempPixel,MAX_BRIGHT);
		curPixInd++;

	}else
		if(curPixInd < curEndOfLine){

		SetPixBrightness(curPixInd - 1,LIN_BRIGH(0));
		SetPixColor(curPixInd ,tempPixel,MAX_BRIGHT);

		curPixInd++;
	}else{
		curPixInd = 0;
		curEndOfLine--;
		tempPixel = ColorFromPalette(g_currentPalette,rand()%255);
		if(curEndOfLine == 0) {
			curEndOfLine = NUM_OF_LEDS ;

		}

	}
}

void RandomColor(){
		SetPixColor(rand()%NUM_OF_LEDS,
				ColorFromPalette(COLOR_RING,rand()%255),
				MAX_BRIGHT);
}

void RandomEffect(){
	g_colorEffect = rand()%4;
}
void UpdateColor(){
	if(g_colorEffect == 0) Rotate();
	if(g_colorEffect == 1) StackColor();
	if(g_colorEffect == 2) RandomColor();
	if(g_colorEffect == 3) StackComet();




}


void SetColorEffect(uint8_t effect,uint8_t speed){
	g_colorEffect = effect;
	g_colorSpeed = speed;

	AddTask(UpdateColor,0,10*speed);
	if(speed == 0) DeleteTask(UpdateColor);
}

 void SetBrightnessEffect(uint8_t effect,uint8_t speed){
	 g_brSpeed = speed;
	 g_brEffect = effect;
	 AddTask(UpdateBrightness,0,10*speed);
	 if(speed == 0) DeleteTask(UpdateBrightness);
 }
