/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/04/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "string.h"
#include "debug.h"
#include "WCHNET.h"
#include "eth_driver.h"
#include "app_iochub.h"
#include "app_net.h"


extern volatile uint8_t speedflg;
extern volatile uint32_t speed;
extern volatile uint32_t recvcnt;
/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init (void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure =
    { 0 };

    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 100 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit (TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig (TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd (TIM2, ENABLE);
    TIM_ClearITPendingBit (TIM2, TIM_IT_Update);
    NVIC_SetPriority (TIM2_IRQn, 0x80);
    NVIC_EnableIRQ (TIM2_IRQn);
}

/*********************************************************************
 * @fn      AT_Init
 *
 * @brief   Initializes AT CMD port.
 *
 * @return  none
 */

void AT_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init (USART1, &USART_InitStructure);
    USART_ITConfig (USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    USART_Cmd (USART1, ENABLE);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main (void) {

    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init (115200);
    printf ("IoCHub Test\r\n");
    printf ("SystemClk:%d\r\n", SystemCoreClock);
    TIM2_Init();
    NET_Init();
    IoCHub_Init();
    AT_Init();
    while (1) {
        NET_Process();
        WCHIOCHUB_Process();
        AT_Process();
    }
}
