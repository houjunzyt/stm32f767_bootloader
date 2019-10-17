#include "usbd_usr.h"
#include "usb_dcd_int.h"
#include <stdio.h> 
#include "usart.h" 

//表示USB连接状态
//0,没有连接;
//1,已经连接;
vu8 bDeviceState=0;		//默认没有连接  

extern USB_OTG_CORE_HANDLE  USB_OTG_dev;

/*****************************************************************
*函数功能：USB OTG 中断服务函数处理所有USB中断
*传入参数：无
*返回值  ：无
*****************************************************************/
void OTG_FS_IRQHandler(void)
{
  	USBD_OTG_ISR_Handler(&USB_OTG_dev);
    SCB_CleanInvalidateDCache();
}  

//指向DEVICE_PROP结构体
//USB Device 用户回调函数. 
USBD_Usr_cb_TypeDef USR_cb =
{
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,
  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,    
};

/*****************************************************************
*函数功能：USB Device 用户自定义初始化函数
*传入参数：无
*返回值  ：无
*****************************************************************/
void USBD_USR_Init(void)
{
	//printf("USBD_USR_Init\r\n");
} 

/*****************************************************************
*函数功能：USB Device 复位
*传入参数：speed:USB速度,0,高速;1,全速;其他,错误.
*返回值  ：无
*****************************************************************/
void USBD_USR_DeviceReset (uint8_t speed)
{
	switch (speed)
	{
		case USB_OTG_SPEED_HIGH:
			printf("USB Device Library v1.1.0  [HS]\r\n");
			break; 
		case USB_OTG_SPEED_FULL: 
			printf("USB Device Library v1.1.0  [FS]\r\n");
			break;
		default:
			printf("USB Device Library v1.1.0  [??]\r\n"); 
			break;
	}
}

/*****************************************************************
*函数功能：USB Device 配置成功
*传入参数：无
*返回值  ：无
*****************************************************************/
void USBD_USR_DeviceConfigured (void)
{
    bDeviceState=1;
	printf("MSC Interface started.\r\n"); 
} 

/*****************************************************************
*函数功能：USB Device挂起
*传入参数：无
*返回值  ：无
*****************************************************************/
void USBD_USR_DeviceSuspended(void)
{
    bDeviceState=0;
	printf("Device In suspend mode.\r\n");
} 

/*****************************************************************
*函数功能：USB Device恢复
*传入参数：无
*返回值  ：无
*****************************************************************/
void USBD_USR_DeviceResumed(void)
{ 
	printf("Device Resumed\r\n");
}

/*****************************************************************
*函数功能：USB Device连接成功
*传入参数：无
*返回值  ：无
*****************************************************************/
void USBD_USR_DeviceConnected (void)
{
	bDeviceState=1;
	printf("USB Device Connected.\r\n");
}

/*****************************************************************
*函数功能：USB Device未连接
*传入参数：无
*返回值  ：无
*****************************************************************/
void USBD_USR_DeviceDisconnected (void)
{
	bDeviceState=0;
	printf("USB Device Disconnected.\r\n");
} 








