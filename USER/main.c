#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "sdram.h"
#include "sdmmc_sdcard.h"
//通过串口打印SD卡相关信息
void show_sdcard_info(void)
{
	switch(SDCardInfo.CardType)
	{
		case STD_CAPACITY_SD_CARD_V1_1:printf("Card Type:SDSC V1.1\r\n");break;
		case STD_CAPACITY_SD_CARD_V2_0:printf("Card Type:SDSC V2.0\r\n");break;
		case HIGH_CAPACITY_SD_CARD:printf("Card Type:SDHC V2.0\r\n");break;
		case MULTIMEDIA_CARD:printf("Card Type:MMC Card\r\n");break;
	}	
  	printf("Card ManufacturerID:%d\r\n",SDCardInfo.SD_cid.ManufacturerID);	//制造商ID
 	printf("Card RCA:%d\r\n",SDCardInfo.RCA);								//卡相对地址
	printf("Card Capacity:%d MB\r\n",(uint32_t)(SDCardInfo.CardCapacity>>20));	//显示容量
 	printf("Card BlockSize:%d\r\n\r\n",SDCardInfo.CardBlockSize);			//显示块大小
}
int main(void)
{
    int count = 0,times = 0,value = 0;
    uint8_t buff[512];
	Cache_Enable();                 //打开L1-Cache
	HAL_Init();                     //初始化HAL库
	Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
    uart_init(115200);		//初始化串口波特率为115200 
	delay_init(216);
    key_init();
    led_init();
    sdram_init();
    if(SD_Init())
    {
        printf("sd init failed\n");
    }
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
            FMC_SDRAM_WriteBuffer((uint8_t *)"houjun",1,6);
            FMC_SDRAM_ReadBuffer(buff,1,6);
            printf("read sdram:%s\n",buff);
            show_sdcard_info();
            
            printf("read sd:%s\n",buff);
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
