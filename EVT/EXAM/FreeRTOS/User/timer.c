/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/12/1
* Description        : Main program body.
*******************************************************************************/



#include "debug.h"
#include "Config.h"

#define TICK_PER_SECOND 500

/*********************************************************************
 * @fn      init_timer
 *
 * @brief   Initializes the systick
 *
 * @return  none
 */
void init_timer(void)
{
    NVIC_SetPriority(SysTicK_IRQn,0xff);
    NVIC_EnableIRQ(SysTicK_IRQn);
    SysTick->CTLR= 0;
    SysTick->SR  = 0;
    SysTick->CNT = 0;
    SysTick->CMP = SystemCoreClock/TICK_PER_SECOND;
    SysTick->CTLR= 0xf;
}




