#include "key.h"
#include "delay.h"

/*****************************************************************
*函数功能：按键初始化函数
*传入参数：无
*返回值  ：无
    k1------>PH2
    k2------>PH3
    K3------>PC13
*****************************************************************/
void key_init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();           //开启GPIOC时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();           //开启GPIOH时钟
    
    GPIO_Initure.Pin = GPIO_PIN_13;           //PC13
    GPIO_Initure.Mode = GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull = GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
    
    GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3; //PH2,3
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
}

/*****************************************************************
*函数功能：按键处理函数
*传入参数：mode:0,不支持连续按;1,支持连续按;
*返回值  ：返回按键值 1-3
注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
*****************************************************************/
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1;     //按键松开标志
    if(mode == 1)
        key_up = 1;    //支持连按
    if(key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0))
    {
        delay_ms(10);
        key_up = 0;
        if(KEY0 == 0)       return KEY0_PRES;
        else if(KEY1 == 0)  return KEY1_PRES;
        else if(KEY2 == 0)  return KEY2_PRES;         
    }
    else
    if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1)
        key_up = 1;
    return 0;   //无按键按下
}
