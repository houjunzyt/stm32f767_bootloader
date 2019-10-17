#ifndef _SYS_H_
#define _SYS_H_

#include "stm32f7xx.h"
#include "core_cm7.h"
#include "stm32f7xx_hal.h"

#define Write_Through() (*(__IO uint32_t*)0XE000EF9C=1UL<<2) //Cache透写模式

void Cache_Enable(void);                                    //使能STM32F7的L1-Cahce
void Stm32_Clock_Init(uint32_t plln,uint32_t pllm,uint32_t pllp,uint32_t pllq); //配置系统时钟
uint8_t Get_ICahceSta(void);//判断I_Cache是否打开
uint8_t Get_DCahceSta(void);//判断I_Dache是否打开
//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(uint32_t addr);	//设置堆栈地址 

#endif
