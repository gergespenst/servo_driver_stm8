/*
 * uart.c
 *
 *  Created on: 3 но€б. 2017 г.
 *      Author: USER
 */
#include "uart.h"



static uint8_t rx_buf[BUF_SIZE] = "0123456789ABCDE";
static uint8_t rx_buf_in = 0,rx_buf_out = 0;

uint8_t uart_irq_rx(uint8_t byte){//заполнение приемного буфера
	rx_buf[rx_buf_in] = byte;
	rx_buf_in++;
	rx_buf_in &= BUF_MASK;
	if(rx_buf_in == rx_buf_out) return 1;


	return 0;
}

uint8_t uart_get_num(){
	return (rx_buf_in - rx_buf_out )& BUF_MASK;
}

uint8_t uart_is_empty(){
	return rx_buf_in == rx_buf_out;
}

uint8_t uart_get_char(){

	uint8_t a = rx_buf[rx_buf_out];
	rx_buf_out++;
	rx_buf_out &= BUF_MASK;
	return a;
}

void  uart_buf_clear(){
	//rx_buf_tail = 0;
	//rx_buf_head = rx_buf_tail;
}

uint8_t uart_comand_complete(){//
	 if( (rx_buf[rx_buf_out] & 0x0F ) == (uart_get_num() - 1 ) )
		 return 1;
	 else
		 return 0;
}

