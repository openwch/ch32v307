/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/

/*
 *@Note
 低功耗，待机模式例程：
 WKUP(PA0)
 本例程演示 WFI 进入待机模式，WKUP(PA0)引脚上升沿，退出待机模式，
 唤醒后程序复位。
 注：为尽可能的降低功耗，建议将不用的GPIO设置成上拉、下拉、模拟输入或设置成输出模式。

*/

#include "debug.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    /* 为降低功耗，需将不用的GPIO设置成上拉或下拉输入 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                               RCC_APB2Periph_GPIOC,
                           ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    Delay_Ms(1000);
    Delay_Ms(1000);

    printf("Standby Mode Test\r\n");

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
    {
        printf("\r\n Standby wake up reset \r\n");
    }
    else
    {
        printf("\r\n Power on reset \r\n");
        PWR_WakeUpPinCmd(ENABLE);
        PWR_EnterSTANDBYMode();
    }

    printf("\r\n ########## \r\n");
    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }
}
