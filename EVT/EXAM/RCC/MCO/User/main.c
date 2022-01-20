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
 MCO引脚时钟输出例程：
 MCO(PA8)
 本例程演示 MCO(PA8) 引脚输出系统时钟。

*/

#include "debug.h"

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

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* MCO Output GPIOA 8 */
    RCC_MCOConfig(RCC_MCO_SYSCLK);

    while(1)
        ;
}
