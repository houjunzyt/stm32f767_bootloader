#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "sdram.h"
#include "sdmmc_sdcard.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usbd_msc_bot.h"
#include "w25qxx.h"

USB_OTG_CORE_HANDLE USB_OTG_dev;
extern uint8_t USB_STATUS_REG;		//USB状态
extern uint8_t bDeviceState;		//USB连接 情况
uint8_t usb_buff[MSC_MEDIA_PACKET];
int main(void)
{
    uint8_t offline_cnt = 0;
    uint8_t USB_STA;
	uint8_t Divece_STA;
    uint8_t tct = 0;
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
    else
    {    
        show_sdcard_info();
    }
    MSC_BOT_Data = usb_buff;
    USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
    delay_ms(1800);
	while(1)
	{
        delay_ms(1);				  
        if(USB_STA!=USB_STATUS_REG)//状态改变了 
		{	 						   		  	   
			if(USB_STATUS_REG&0x01)//正在写		  
			{
				LED1(0);
			}
			if(USB_STATUS_REG&0x02)//正在读
			{
				LED1(0);		 
			}	 										  
			if(USB_STATUS_REG&0x04)
                printf("USB Write Err\n");//提示写入错误	  
			if(USB_STATUS_REG&0x08)
                printf("USB Read  Err\n");//提示读出错误  
			USB_STA=USB_STATUS_REG;//记录最后的状态
		}
		if(Divece_STA!=bDeviceState) 
		{
			if(bDeviceState==1)
                printf("USB Connected\n");//提示USB连接已经建立
			else 
                printf("USB DisConnected\n");//提示USB被拔出了
			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;
			LED1(1);
			LED0_Toggle;//提示系统在运行
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=1;
			}else//没有得到轮询 
			{
				offline_cnt++;  
				if(offline_cnt>10)bDeviceState=0;//2s内没收到在线标记,代表USB被拔出了
			}
			USB_STATUS_REG=0;
		} 
	}
}
