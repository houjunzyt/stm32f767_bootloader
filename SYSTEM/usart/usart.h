#ifndef _USART_H_
#define _USART_H_

#include "sys.h"
#include "stdio.h"	

extern UART_HandleTypeDef UART1_Handler; //UART1句柄
void uart_init(uint32_t bound);

#endif
