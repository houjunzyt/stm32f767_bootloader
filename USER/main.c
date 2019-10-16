#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"

int main(void)
{
    int count = 0,times = 0,value = 0;
	Cache_Enable();                 //打开L1-Cache
	HAL_Init();                     //初始化HAL库
	Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
    uart_init(115200);		//初始化串口波特率为115200 
	delay_init(216);
    key_init();
    led_init();
	while(1)
	{
        if(count > 10)
        {
            count = 0;
            printf("test\n");
            if(times % 2)
            {
                LED0(1);
                LED1(0);
            }
            else
            {
                LED0(0);
                LED1(1);           
            }
            times++ ;
        }
        delay_ms(100);
        count++;
        value = key_scan(0);
        if(value)
        {
            printf("key %d press\n",value);
        }
	}
}
