#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

void uart_init(u32 bound);
void DMA1_4_Init(u8 *USART_TX_BUF);
#endif


