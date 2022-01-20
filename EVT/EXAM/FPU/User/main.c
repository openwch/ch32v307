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
 FPU硬件浮点运算例程：
   本例程演示硬件浮点运算。

    注：开启硬件浮点M-RS配置参考 本例程配置
    具体配置-Properties -> C/C++ Build -> Setting -> Target Processor
    -> Floating point 选项配置成 Single precision extension （RVF）
       Floating point ABI 选项配置成 Single precision(f)

*/

#include "debug.h"

float val1=33.14;

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    int t,t1;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);

	val1 = (val1/2+11.12)*2;
    t=(int)(val1*10)%10;
    t1=(int)(val1*100)%10;

	printf("%d.%d%d\n", (int)val1, t, t1);

	while(1);
}


