/*
 * fifo.c
 *
 *  Created on: 9 но€б. 2017 г.
 *      Author: USER
 */

#include "fifo.h"

unsigned char fifo_push(TFIFO* fifo,const unsigned char byte){
	if(fifo->count >= BUF_SIZE){
		fifo->count = BUF_SIZE ;
		return 1;
	}
	unsigned char in_ind = (fifo->out_ind + fifo->count) & BUF_MASK;
	fifo->data[in_ind] = byte;
	fifo->count++;
	return 0;

}

unsigned char fifo_pull(TFIFO* fifo,unsigned char* byte){
	*byte = fifo->data[fifo->out_ind++];
	fifo->out_ind &= BUF_MASK;
	fifo->count--;
	if(fifo->count == 0) return 1;
	return 0;

}

unsigned char fifo_get(const TFIFO* fifo,unsigned char index,unsigned char* byte){
	if(fifo->count == 0) return 1;
	*byte = fifo->data[(fifo->out_ind + index) & BUF_MASK];
	return 0;
}


unsigned char fifo_count(const TFIFO* fifo){
	return fifo->count;
}

void fifo_reset(TFIFO* fifo){
	fifo->count = 0;
	fifo->out_ind = 0;
}
