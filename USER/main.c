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
#include "ff.h"
#include "exfuns.h"
#include "malloc.h"
#include <string.h>

#define BOOT_WAIT_TIME 5

USB_OTG_CORE_HANDLE USB_OTG_dev;
extern uint8_t USB_STATUS_REG;		//USB状态
extern uint8_t bDeviceState;		//USB连接 情况
uint8_t usb_buff[MSC_MEDIA_PACKET];

void update_firmware(void) //升级固件函数
{
    uint8_t offline_cnt = 0;
    uint8_t USB_STA;
	uint8_t Divece_STA;
    uint8_t tct = 0;
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

void boot_app(void)
{
    uint32_t total,free;
    FRESULT res;
    FIL fsrc;
    exfuns_init();							//为fatfs相关变量申请内存
    f_mount(fs[0],"0:",1); 					//挂载SD卡 
    while(exf_getfree((uint8_t *)"0:",&total,&free))	//得到SD卡的总容量和剩余容量
	{
		printf("SD Card Fatfs Error!\n");
		LED0_Toggle;//DS0闪烁
	}
    printf("SD total:%d MB,free:%d MB\n",total,free);
    res = f_open(&fsrc, "0:readme.txt",FA_READ|FA_WRITE);
    if(res == FR_OK)
    {
        uint8_t buff[50];
        UINT count=0;
        int size;
        printf("file exsit\n");
        res = f_read(&fsrc,buff, sizeof(buff), &count);
        printf("read:%s,%d\n",buff,count);
        strcpy((char *)buff,"houjun");
        res = f_write(&fsrc, buff, 6, &count);
        size = f_size(&fsrc);
        printf("write:%d,%d,size:%d\n",count,res,size);
        f_close(&fsrc);
    }
    while(1)
    {
    
    }
}

int main(void)
{
    uint32_t time_count=0;
	Cache_Enable();                 //打开L1-Cache
	HAL_Init();                     //初始化HAL库
	Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
    uart_init(115200);		//初始化串口波特率为115200 
	delay_init(216);
    key_init();
    led_init();
    sdram_init();
    my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMEX);		    //初始化外部内存池
	my_mem_init(SRAMDTCM);		    //初始化CCM内存池 
    if(SD_Init())
    {
        printf("sd init failed\n");
    }
    else
    {    
        show_sdcard_info();
    }
    printf("\n\nPress key 1 to entry update mode\n\n");
	while(1)
	{
        delay_ms(100);
        time_count++;
        if(key_scan(0) == KEY2_PRES)
        {
            update_firmware();
        }
        if(time_count >= 50)
        {
            printf("\nbegin boot to app\n\n");
            break;
        }
        if(time_count%10 == 0)
        {
            printf("\nboot wait time : %d s\n",BOOT_WAIT_TIME -(time_count/10));
        }
	}
    boot_app();
}
