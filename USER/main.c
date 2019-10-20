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

typedef  void (*iapfun)(void);				//定义一个函数类型的参数.

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

iapfun Jump_To_Application;
#define SDRAM_ADDR_SWP    ((uint32_t)(0X60000000)) //SDRAM开始地址
void boot_app(void)
{
    uint32_t total,free;
    volatile uint32_t JumpAddress;
    FRESULT res;
    FIL fsrc;
    exfuns_init();							//为fatfs相关变量申请内存
    f_mount(fs[0],"0:",1); 					//挂载SD卡 
    while(exf_getfree((uint8_t *)"0:",&total,&free))	//得到SD卡的总容量和剩余容量
    {
        printf("SD Card Fatfs Error!\n");
        LED0_Toggle;//DS0闪烁
    }
    SYSCFG->MEMRMP |= (1<<10);            
    delay_ms(5000);
    LED0_Toggle;//DS0闪烁
    printf("SD total:%d MB,free:%d MB\n",total,free);
    
    uint8_t * SDRAM_ADDR = NULL;
    SDRAM_ADDR = (uint8_t *)SDRAM_ADDR_SWP;//Bank5_SDRAM_ADDR;
    res = f_open(&fsrc, "0:app.bin",FA_READ);
    if(res == FR_OK)
    {      

        uint8_t buff[50];
        UINT count=0;
        int size;
        size = f_size(&fsrc);
        printf("file exsit,size:%d Byte\n",size);
        res = f_read(&fsrc,SDRAM_ADDR, size, &count);
        printf("read %d byte\n",count);
        f_close(&fsrc);
        res = f_open(&fsrc, "0:read.bin",FA_WRITE | FA_OPEN_ALWAYS);
        res = f_write(&fsrc,(uint8_t *)(SDRAM_ADDR), size, &count);
        f_close(&fsrc);
        if(count == size)
        {
            printf("in\n");
         //   __disable_irq();
         //   __asm("SVC 0x0");   //这条指令很重要，引发一个svc中断，在里面切换cpu工作模式，从用户模式切换为特权模式，否则app跳转必死无疑
            __asm("CPSID   I"); 
            printf("remap sdram\n");
            JumpAddress = *(uint32_t *)(SDRAM_ADDR_SWP+4);
            Jump_To_Application = (iapfun) JumpAddress;
            MSR_MSP(*(uint32_t*) SDRAM_ADDR_SWP);
            Jump_To_Application();  
        }  
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
//	my_mem_init(SRAMEX);		    //初始化外部内存池
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
