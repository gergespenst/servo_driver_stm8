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
#include "stm8s_flash.h"

#include "stm8s_tim2.h"
#include "stm8s_uart1.h"
#include "stm8s_spi.h"

#include "task_query.h"

#include <stdlib.h>

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


#pragma inline=forced
static void InitTIM2(){//config GCLK out
  TIM2_DeInit();
  TIM2_TimeBaseInit(TIM2_PRESCALER_1,1);
  TIM2_OC1Init(TIM2_OCMODE_TOGGLE,TIM2_OUTPUTSTATE_ENABLE,1, TIM2_OCPOLARITY_HIGH);
  TIM2_OC1PreloadConfig(ENABLE);
  TIM2_ARRPreloadConfig(ENABLE);
  TIM2_Cmd(ENABLE);
}



///////////////////////////////////////////////////////////
#pragma inline=forced
void InitUart(){
	UART1_DeInit();
	UART1_Init(9600,UART1_WORDLENGTH_8D,UART1_STOPBITS_1,UART1_PARITY_NO,
				UART1_SYNCMODE_CLOCK_DISABLE,UART1_MODE_TXRX_ENABLE);
	UART1_ITConfig( UART1_IT_RXNE, ENABLE);
	UART1_Cmd(ENABLE);
}


//first stepper control input
#define STEP_PORT_1 GPIOA
#define STEP_PIN_1 GPIO_PIN_1     
#define DIR_PORT_1 GPIOA
#define DIR_PIN_1 GPIO_PIN_2


void DelayDisable1();
void DelayDisable2();
void ST_1_Step();
void ST_2_Step();

void FirstStepperInterrupt(){
  AddTask(ST_1_Step,0,0);
     
 
}

//second stepper control input
#define STEP_PORT_2 GPIOB
#define STEP_PIN_2  GPIO_PIN_4
#define DIR_PORT_2  GPIOB
#define DIR_PIN_2   GPIO_PIN_5



void SecondStepperInterrupt(){
  AddTask(ST_2_Step,0,0);
 
 
}


//Stepper driver control port
#define ST_DRV_PORT GPIOC
#define ST_DRV_CS1  GPIO_PIN_4
#define ST_DRV_CS2  GPIO_PIN_7
#define ST_DRV_EN1  GPIO_PIN_3
#define ST_DRV_SPI_MOSI  GPIO_PIN_6
#define ST_DRV_SPI_SCK   GPIO_PIN_5

#define ST_DRV_EN2_PORT  GPIOD
#define ST_DRV_EN2       GPIO_PIN_3

#define NUM_OF_SPI 2

#pragma inline=forced
static void InitSPI(){
  SPI_DeInit();
  SPI_Init(SPI_FIRSTBIT_MSB,SPI_BAUDRATEPRESCALER_32,SPI_MODE_MASTER,
           SPI_CLOCKPOLARITY_HIGH,SPI_CLOCKPHASE_2EDGE,SPI_DATADIRECTION_1LINE_TX,
           SPI_NSS_SOFT,0x00);
  SPI_Cmd(ENABLE);
}

void SendSpiData(uint8_t st_num,uint16_t spi_data){
  st_num = st_num%NUM_OF_SPI;
  if(st_num == 0) GPIO_WriteLow(ST_DRV_PORT,ST_DRV_CS1);
  if(st_num == 1) GPIO_WriteLow(ST_DRV_PORT,ST_DRV_CS2);
  
  SPI_SendData((spi_data & 0xFF00) >> 8);
  while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
  SPI_SendData((spi_data & 0x00FF));
  while (SPI_GetFlagStatus(SPI_FLAG_BSY) == SET);
  
  if(st_num == 0) GPIO_WriteHigh(ST_DRV_PORT,ST_DRV_CS1);
  if(st_num == 1) GPIO_WriteHigh(ST_DRV_PORT,ST_DRV_CS2);
}


//Step driver control logic
 
typedef struct {
	uint16_t br_blank_time:2;
	uint16_t br_off_time:5;
	uint16_t br_fast_decay:4;
	uint16_t br_sync_control:1;
	uint16_t unused:2;
	uint16_t reg_num:2;
	} T_0_1_REG;

typedef struct {
	uint16_t br_2_int_pwm_mode:1;
	uint16_t br_2_ext_pwm_mode:1;
	uint16_t br_2_phase:1;
	uint16_t br_2_DAC:4;
	uint16_t br_1_int_pwm_mode:1;
	uint16_t br_1_ext_pwm_mode:1;
	uint16_t br_1_phase:1;
	uint16_t br_1_DAC:4;
	uint16_t reg_num:2;
} T_2_REG;

typedef struct {
	uint16_t therm_mon:1;
	uint16_t charge_pump:1;
	uint16_t unused:12;
	uint16_t reg_num:2;
} T_3_REG;


#define TO_INT(x) ( *((uint16_t*) &(x)) )

//global stepper variables
T_2_REG g_steps[4];
T_0_1_REG g_0_reg;
T_0_1_REG g_1_reg;
T_2_REG g_2_reg;
T_3_REG g_3_reg;


void TestSpi(){
GPIO_WriteHigh(ST_DRV_PORT,ST_DRV_EN1);//Enable DRV
T_2_REG temp;
temp.reg_num = 2;
temp.br_1_DAC = 0x0F;
temp.br_2_DAC = 0x00;
  SendSpiData(0,TO_INT(temp));
  
}

void DelayDisable1(){
  GPIO_WriteLow(ST_DRV_PORT,ST_DRV_EN1);
  TIM2_Cmd(DISABLE);
}

void ST_1_Step(){
  static uint8_t state = 0;
  TIM2_Cmd(ENABLE);
    GPIO_WriteHigh(ST_DRV_PORT,ST_DRV_EN1);//Enable DRV
   if(GPIO_ReadInputPin(DIR_PORT_1,DIR_PIN_1) == RESET)
                    SendSpiData(0,TO_INT(g_steps[state]));
   else
                    SendSpiData(0,TO_INT(g_steps[3 - state]));
  state++;
  (state > 3)?(state = 0):(state = state);
  AddTask(DelayDisable1,300,0);

}

void DelayDisable2(){
  GPIO_WriteLow(ST_DRV_EN2_PORT,ST_DRV_EN2);
  TIM2_Cmd(DISABLE);
}

void ST_2_Step(){
  static uint8_t state = 0;
    TIM2_Cmd(ENABLE);
    GPIO_WriteHigh(ST_DRV_EN2_PORT,ST_DRV_EN2);//Enable DRV
   if(GPIO_ReadInputPin(DIR_PORT_2,DIR_PIN_2) == RESET)
                    SendSpiData(1,TO_INT(g_steps[state]));
   else
                    SendSpiData(1,TO_INT(g_steps[3 - state]));
  state++;
  (state > 3)?(state = 0):(state = state); 
   AddTask(DelayDisable2,300,0);
}



#define AMPL 0x07
void ReloadRegs(){
  //Init global stepper regs
 
  for(uint8_t i = 0 ; i < 4; i++){
    g_steps[i].reg_num = 2;
    g_steps[i].br_1_DAC  = g_2_reg.br_1_DAC *(1 - i%2);
    g_steps[i].br_1_phase = i/2*(1 - i%2);
    g_steps[i].br_2_DAC  = g_2_reg.br_2_DAC*(i%2);
    g_steps[i].br_2_phase = i/2*(i%2);
    g_steps[i].br_1_int_pwm_mode = 1;
    g_steps[i].br_2_int_pwm_mode = 1;
      
  }
}
void LoadFromEEPROM(){
 // uint16_t* pReg = (uint16_t*) &g_0_reg;
//  *pReg = FLASH_ReadByte(0x4000) << 8 + FLASH_ReadByte(0x4001);
  
  g_2_reg.br_1_DAC = AMPL;
  g_2_reg.br_2_DAC = AMPL;
}

void main(void)
{
  

 	//Ставим частоту 16 МГц
        CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
              
        //Step control input lines 1
        GPIO_Init(STEP_PORT_1,STEP_PIN_1,GPIO_MODE_IN_FL_IT);
        GPIO_Init(DIR_PORT_1,DIR_PIN_1,GPIO_MODE_IN_PU_NO_IT);
        //Step control input lines 2
        GPIO_Init(STEP_PORT_2,STEP_PIN_2,GPIO_MODE_IN_FL_IT);
        GPIO_Init(DIR_PORT_2,DIR_PIN_2,GPIO_MODE_IN_PU_NO_IT);
        //Configure interrupt
        EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOA,EXTI_SENSITIVITY_RISE_ONLY);
        EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB,EXTI_SENSITIVITY_RISE_ONLY);
        //
	InitTIM4();//Инициализация таймера для диспетчера задач
        //	InitUart();
        InitTIM2();//GCLK Timer init
	//Drv control PORT init        
        GPIO_Init(ST_DRV_PORT,ST_DRV_CS1  ,GPIO_MODE_OUT_PP_HIGH_FAST);
        GPIO_Init(ST_DRV_PORT,ST_DRV_CS2  ,GPIO_MODE_OUT_PP_HIGH_FAST);
        GPIO_Init(ST_DRV_PORT,ST_DRV_EN1  ,GPIO_MODE_OUT_PP_HIGH_FAST);
        GPIO_Init(ST_DRV_PORT,ST_DRV_SPI_MOSI,GPIO_MODE_OUT_PP_HIGH_FAST);
        GPIO_Init(ST_DRV_PORT,ST_DRV_SPI_SCK ,GPIO_MODE_OUT_PP_HIGH_FAST);

        GPIO_Init(ST_DRV_EN2_PORT,ST_DRV_EN2 ,GPIO_MODE_OUT_PP_HIGH_FAST);
          //Disable DRV output
        GPIO_WriteLow(ST_DRV_PORT,ST_DRV_EN1);
        GPIO_WriteLow(ST_DRV_EN2_PORT,ST_DRV_EN2);
          
        InitSPI();
        //Fill task query
        //AddTask(TestSpi,0,50);
      // AddTask(ST_1_Step,0,50);
        InitUart();
	/////////////////////////
        
        LoadFromEEPROM();
        ReloadRegs();
        ///////////////////////
        SendSpiData(0,0x0040);
        SendSpiData(0,0x4040);
        /////////////////////////
	enableInterrupts();
 /* Infinite loop */
  while (1)
  { 
    
	  Dispatcher();

  }
  
}

