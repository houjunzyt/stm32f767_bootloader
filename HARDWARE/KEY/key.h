#ifndef _KEY_H_
#define _KEY_H_

#include "sys.h"

#define KEY0        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_3)  //KEY0按键PH3
#define KEY1        HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_2)  //KEY1按键PH2
#define KEY2        HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) //KEY2按键PC13

#define KEY0_PRES 	1
#define KEY1_PRES	2
#define KEY2_PRES	3

void key_init(void);
uint8_t key_scan(uint8_t mode);

#endif
