#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
void Delay(__IO uint32_t nCount);
void Delay(__IO uint32_t nCount)
{
while(nCount--){}
}

int main(void)
{
	Cache_Enable();                 //打开L1-Cache
	HAL_Init();                     //初始化HAL库
	Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
	delay_init(216);
	
	LED_Init();		  		//初始化与LED连接的硬件接口
	while(1)
	{
		LED0(0);  //LED0对应引脚GPIOB.5拉低，亮  等同LED0=0;
		LED1(1);   //LED1对应引脚GPIOE.5拉高， 灭 等同LED1=1;
		delay_ms(500);  		       
		LED0(1);	   //LED0对应引脚GPIOB.5拉高，灭  等同LED0=1;
		LED1(0); //LED1对应引脚GPIOE.5拉低，亮 等同LED1=0;
		delay_ms(500); 
	}
}
