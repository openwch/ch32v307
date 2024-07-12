/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/03/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 HSI or HSI/2 as PLL input clock routine:
 This example demonstrates that HSI or HSI/2 is used as the PLL input clock,
 and the system clock uses the PLL as the clock source.
 Refer to system_ch32v30x.c, system_ch32v30x.h.
 HSI is used as the PLL input clock, and the demonstration system clocks are 24MHz, 48MHz, and 72MHz.
 HSI/2 is used as the PLL input clock, and the demonstration system clock is 24MHz, 48MHz

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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    USART_Printf_Init(115200);
    SystemCoreClockUpdate();
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    while(1);
}







