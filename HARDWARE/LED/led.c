#include "led.h"
//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;	
	__HAL_RCC_GPIOB_CLK_ENABLE();	 	//使能PB 端口时钟

	GPIO_InitStructure.Pin = GPIO_PIN_1 | GPIO_PIN_2;				 	//LED0-->PB5端口配置
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 		//推挽输出
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB,&GPIO_InitStructure);					 	//根据设定参数初始化
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);						 		//PB5输出高
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);						 		//PE5输出高
}
 
