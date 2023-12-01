/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *Systick interrupt:
 *USART1_Tx(PA9).HCLK is systick clock source
 */

#include "debug.h"

/* Global typedef */
/* Global define */
/* Global Variable */

uint32_t counter = 0;
void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      SYSTICK_Init_Config
 *
 * @brief   SYSTICK_Init_Config.
 *
 * @return  none
 */
void SYSTICK_Init_Config(u_int64_t ticks)
{
    SysTick->SR &= ~(1 << 0);//clear State flag
    SysTick->CMP = ticks;
    SysTick->CNT = 0;
    SysTick->CTLR = 0xF;

    NVIC_SetPriority(SysTicK_IRQn, 15);
    NVIC_EnableIRQ(SysTicK_IRQn);
}
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    SystemCoreClockUpdate();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n",SystemCoreClock);
    SYSTICK_Init_Config(SystemCoreClock-1);

    while(1)
    {
    }
}

/*********************************************************************
 * @fn      SysTick_Handler
 *
 * @brief   SysTick_Handler.
 *
 * @return  none
 */
void SysTick_Handler(void)
{
    if(SysTick->SR == 1)
    {
        SysTick->SR = 0;//clear State flag
        printf("welcome to WCH\r\n");
        counter++;
        printf("Counter:%d\r\n",counter);
    }
}

