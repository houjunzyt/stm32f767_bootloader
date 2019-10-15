#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED0(x) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,(GPIO_PinState)x)// PB5
#define LED1(x) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,(GPIO_PinState)x);// PE5

void LED_Init(void);//初始化

		 				    
#endif
