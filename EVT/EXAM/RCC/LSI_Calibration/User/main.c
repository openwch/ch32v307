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

/**
 * @note
 * The code initializes the RTC (Real-Time Clock) module and performs a calibration to determine the
 * LSI (Low-Speed Internal) clock frequency.
 */
#include "debug.h"

/* Global define */
#define LSI_SamplingTime 20
/* Global Variable */
volatile uint32_t CentLSI_Circle[2] = {0};

/*********************************************************************
 * @fn      Input_Capture_Init
 *
 * @brief   Initializes TIM5 input capture.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void Input_Capture_Init(u16 arr, u16 psc)
{
    TIM_ICInitTypeDef TIM_ICInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_TIM5CH4_LSI, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;

    TIM_ICInit(TIM5, &TIM_ICInitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM5, TIM_IT_CC4 | TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM5, ENABLE);
}

void TIM5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      TIM5_IRQHandler
 *
 * @brief   This function handles TIM5  Capture Compare Interrupt exception.
 *
 * @return  none
 */
void TIM5_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM5, TIM_IT_CC4) != RESET)
    {

        if(CentLSI_Circle[0]){
        CentLSI_Circle[1] = TIM5->CH4CVR;

        TIM_Cmd(TIM5, DISABLE);
        TIM5->CNT = 0;}
        else {
            CentLSI_Circle[0] = TIM5->CH4CVR;
        }


        TIM5->INTFR = (uint16_t)~TIM_IT_CC4;
    }
    else if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        CentLSI_Circle[0] = 0;
        CentLSI_Circle[1] = 0;
        TIM5->INTFR = (uint16_t)~TIM_IT_Update;
    }
}

uint32_t GetActualLSIFreq()
{
    volatile uint32_t AveLSI_Circle = 0, AveLSI_Freq = 0;
    static RCC_ClocksTypeDef Clock;
    static uint32_t TIMCLK = 0;
    RCC_GetClocksFreq(&Clock);
    if ((RCC->CFGR0 >> 10 & 0x1))
    {
        TIMCLK = 2 * Clock.PCLK1_Frequency;
    }

    for (uint16_t i = 0; i < LSI_SamplingTime; i++)
    {
        CentLSI_Circle[0] = 0;
        CentLSI_Circle[1] = 0;
        TIM_Cmd(TIM5, ENABLE);
        while(!CentLSI_Circle[1]);
        AveLSI_Circle += CentLSI_Circle[1] - CentLSI_Circle[0];
    }
    AveLSI_Circle /= (LSI_SamplingTime);
    AveLSI_Freq = TIMCLK / AveLSI_Circle;
    return AveLSI_Freq;
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

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("RTC Test\r\n");

    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    Input_Capture_Init(65535, 0);
    uint32_t temp = GetActualLSIFreq();
    printf("LSI_Freq: %d\r\n", temp);
    RTC_SetPrescaler(temp);
    WWDG_SetPrescaler(temp);
    while (1)
    {
        Delay_Ms(1000);
    }
}
