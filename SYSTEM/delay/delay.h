#ifndef _DELAY_H_
#define _DELAY_H_

#include "sys.h"  

void delay_init(uint8_t SYSCLK);
void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

#endif

