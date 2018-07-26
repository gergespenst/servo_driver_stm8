/*
 * fifo.h
 *
 *  Created on: 9 но€б. 2017 г.
 *      Author: USER
 */

#ifndef USER_FIFO_H_
#define USER_FIFO_H_

#define BUF_SIZE 16 // размер буфера об€зательно степень двойки
#define BUF_MASK (BUF_SIZE - 1)

typedef struct{
	unsigned char data[BUF_SIZE];
	unsigned char out_ind;
	unsigned char count;
} TFIFO;

unsigned char fifo_push(TFIFO* fifo,const unsigned char byte);
unsigned char fifo_pull(TFIFO* fifo,unsigned char* byte);
unsigned char fifo_get(const TFIFO* fifo,unsigned char index,unsigned char* byte);
unsigned char fifo_count(const TFIFO* fifo);
void fifo_reset(TFIFO* fifo);

#endif /* USER_FIFO_H_ */
