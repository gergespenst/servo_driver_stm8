/*
 * uart.h
 *
 *  Created on: 3 нояб. 2017 г.
 *      Author: USER
 */

#ifndef USER_UART_H_
#define USER_UART_H_
#include "stm8s.h"
#define BUF_SIZE 16 // размер буфера обязательно степень двойки
#define BUF_MASK (BUF_SIZE - 1)

uint8_t uart_irq_rx(uint8_t byte);
uint8_t uart_get_char();
uint8_t uart_comand_complete();//проверка протокола,
//в первом байте количество байт в команде
void  uart_buf_clear();


#endif /* USER_UART_H_ */
