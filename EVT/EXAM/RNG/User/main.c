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
    随机数发生器例程
   产生32位随机数通过串口(PA9)打印。

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
    u32 random = 0;

    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n",SystemCoreClock);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_RNG, ENABLE);
    RNG_Cmd(ENABLE);

    while(1)
    {
        while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET)
        {
        }
        random=RNG_GetRandomNumber();
        printf("random:0x%08x\r\n",random);\
        Delay_Ms(500);
    }
}

