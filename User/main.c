/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    30-September-2014
  * @brief   Main program body
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Step motor driver programm
H-bridge convert to driver interface (step+direction lines)*/
/* Includes ------------------------------------------------------------------*/
//#define 
#undef USE_FULL_ASSERT
#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_clk.h"
#include "stm8s_tim4.h"
#include "stm8s_it.h"

#include "stm8s_tim2.h"
#include "stm8s_uart1.h"
#include "stm8s_spi.h"

#include "task_query.h"
#include "led_ws2812.h"
#include "led_effects.h"

void Blink(){
	GPIOB->ODR ^= GPIO_PIN_5;

}

#pragma inline=forced
static void InitTIM4(){
	//Конфигурация системного таймера, должен вызывать прерывания раз в 1 мс
	TIM4_DeInit();
	//Делитель на 128 - 8 мкс/тик, 124 - начальное значение
	//в сумме дают прерывание раз в 1 мс
	TIM4_TimeBaseInit(TIM4_PRESCALER_128,124);
	//Включаем прерывания от таймера 4
	TIM4_ITConfig(TIM4_IT_UPDATE,ENABLE);
	//Включаем таймер
	TIM4_Cmd(ENABLE);
}


#include <stdlib.h>




///////////////////////////////////////////////////////////
#pragma inline=forced
void InitUart(){
	UART1_DeInit();
	UART1_Init(9600,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,
				UART1_SYNCMODE_CLOCK_DISABLE,UART1_MODE_TXRX_ENABLE);
	UART1_ITConfig( UART1_IT_RXNE, ENABLE);
	UART1_Cmd(ENABLE);
}

//stepper
#define STEPPER_PIN_0 GPIO_PIN_3
#define STEPPER_PIN_1 GPIO_PIN_4
#define STEPPER_PIN_2 GPIO_PIN_5
#define STEPPER_PIN_3 GPIO_PIN_6
#define STEPPER_PORT    GPIOC
#define STEPPER_MASK (STEPPER_PIN_0 | STEPPER_PIN_1 | STEPPER_PIN_2 | STEPPER_PIN_3)

#define STEP_PORT GPIOA
#define STEP_PIN  GPIO_PIN_1     
#define DIR_PORT  GPIOA
#define DIR_PIN   GPIO_PIN_2

//second stepper
#define STEP_PORT_2 GPIOB
#define STEP_PIN_2  GPIO_PIN_4
#define DIR_PORT_2  GPIOB
#define DIR_PIN_2   GPIO_PIN_5

#define STEPPER_2_PIN_0 GPIO_PIN_3
#define STEPPER_2_PIN_1 GPIO_PIN_4
#define STEPPER_2_PIN_2 GPIO_PIN_5
#define STEPPER_2_PIN_3 GPIO_PIN_6
#define STEPPER_2_PORT    GPIOD

#define STEPPER_2_MASK (STEPPER_2_PIN_0 | STEPPER_2_PIN_1 | STEPPER_2_PIN_2 | STEPPER_2_PIN_3)

uint8_t stepTable[4] = {STEPPER_PIN_0,STEPPER_PIN_2,STEPPER_PIN_1,STEPPER_PIN_3};
        


void full_step(){
// GPIOB->ODR ^= GPIO_PIN_5;
                  enableInterrupts();
  static uint8_t  state = 0;
  if(state > 3) state = 0;
  if( (DIR_PORT->IDR & DIR_PIN) == 0)
      STEPPER_PORT->ODR = (STEPPER_PORT->ODR  & ~STEPPER_MASK) | stepTable[3-state];
  else
      STEPPER_PORT->ODR = (STEPPER_PORT->ODR  & ~STEPPER_MASK) | stepTable[state];
  state++;
}
uint8_t halfStepTable[8] = {STEPPER_PIN_0,STEPPER_PIN_0|STEPPER_PIN_2,
                            STEPPER_PIN_2,STEPPER_PIN_2|STEPPER_PIN_1,
                            STEPPER_PIN_1,STEPPER_PIN_1|STEPPER_PIN_3,
                            STEPPER_PIN_3,STEPPER_PIN_3|STEPPER_PIN_0};
            
void half_step(){
 
  enableInterrupts();
  static uint8_t  state = 0;
  if(state > 7) state = 0;
  
  if( (DIR_PORT->IDR & DIR_PIN) == 0)
      STEPPER_PORT->ODR = (STEPPER_PORT->ODR  & ~STEPPER_MASK) | halfStepTable[7-state];
  else
      STEPPER_PORT->ODR = (STEPPER_PORT->ODR  & ~STEPPER_MASK) | halfStepTable[state];
  
  state++;
}

uint8_t halfStep2Table[8] = {STEPPER_2_PIN_0,STEPPER_2_PIN_0|STEPPER_2_PIN_2,
                             STEPPER_2_PIN_2,STEPPER_2_PIN_2|STEPPER_2_PIN_1,
                             STEPPER_2_PIN_1,STEPPER_2_PIN_1|STEPPER_2_PIN_3,
                             STEPPER_2_PIN_3,STEPPER_2_PIN_3|STEPPER_2_PIN_0};
void half_step_2(){
 
  enableInterrupts();
  static uint8_t  state = 0;
  if(state > 7) state = 0;
  
  if( (DIR_PORT_2->IDR & DIR_PIN_2) == 0)
      STEPPER_2_PORT->ODR = (STEPPER_2_PORT->ODR  & ~STEPPER_2_MASK) | halfStep2Table[7-state];
  else
      STEPPER_2_PORT->ODR = (STEPPER_2_PORT->ODR  & ~STEPPER_2_MASK) | halfStep2Table[state];
  
  state++;
}


void main(void)
{
 	//Ставим частоту 16 МГц
        CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	//
	GPIO_Init(GPIOB,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_FAST);
        //Step Motor port init
	GPIO_Init(STEPPER_PORT,STEPPER_MASK,GPIO_MODE_OUT_PP_HIGH_FAST);
        //second step motor port init
        GPIO_Init(STEPPER_2_PORT,STEPPER_2_MASK,GPIO_MODE_OUT_PP_HIGH_FAST);
        //Input lines
        GPIO_Init(STEP_PORT,STEP_PIN,GPIO_MODE_IN_FL_IT);
        GPIO_Init(DIR_PORT,DIR_PIN,GPIO_MODE_IN_PU_NO_IT);
        //Second input lines
        GPIO_Init(STEP_PORT_2,STEP_PIN_2,GPIO_MODE_IN_FL_IT);
        GPIO_Init(DIR_PORT_2,DIR_PIN_2,GPIO_MODE_IN_PU_NO_IT);
        //Configure interrupt
        EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA,EXTI_SENSITIVITY_RISE_ONLY);
        EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB,EXTI_SENSITIVITY_RISE_ONLY);
	InitTIM4();//Инициализация таймера для диспетчера задач
        //	InitUart();
	//Заполняем очередь задач
	AddTask(Blink,0,500);
       

	/////////////////////////
	enableInterrupts();
 /* Infinite loop */
  while (1)
  { 
    
	  Dispatcher();

  }
  
}

