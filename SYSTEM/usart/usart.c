#include "usart.h"
            
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 
      
int fputc(int ch, FILE *f)
{ 	
	while((USART1->ISR&0X40) == 0);//循环发送,直到发送完毕   
	USART1->TDR = (uint8_t)ch;      
	return ch;
}

UART_HandleTypeDef UART1_Handler; //UART1句柄

/*****************************************************************
*函数功能：串口初始化
*传入参数：波特率
*返回值  ：无
*****************************************************************/
void uart_init(uint32_t bound)
{	
	//UART 初始化设置
	UART1_Handler.Instance = USART1;					    //USART1
	UART1_Handler.Init.BaudRate = bound;				    //波特率
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity = UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
}

/*****************************************************************
*函数功能：UART底层初始化，时钟使能，引脚配置，中断配置
*传入参数：huart:串口句柄
*返回值  ：无
*说明    ：此函数会被HAL_UART_Init()调用
*****************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance == USART1)//如果是串口1，进行串口1 MSP初始化
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
	
		GPIO_Initure.Pin = GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;		//复用推挽输出
		GPIO_Initure.Pull = GPIO_PULLUP;			//上拉
		GPIO_Initure.Speed = GPIO_SPEED_FAST;		//高速
		GPIO_Initure.Alternate = GPIO_AF7_USART1;	//复用为USART1
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);	   	//初始化PA9	
	}
}

