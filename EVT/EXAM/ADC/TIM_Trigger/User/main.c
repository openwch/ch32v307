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
 *TIM trigger ADC conversion routine:
 *ADC channel 1 (PA1) - injection group channel, TIM2 CH1 pin (PA0)  TIM trigger,
 *In this mode, an ADC conversion is triggered by an event on TIM2 CH1 TIM trigger, and a JEOC interrupt is generated after
 *the conversion is completed.
 *
 */

#include "debug.h"

/* Global Variable */
s16 Calibrattion_Val = 0;

/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void ADC_Function_Init(void)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_T2_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);
    ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE); //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    Calibrattion_Val = Get_CalibrationValue(ADC1);

}

/*********************************************************************
 * @fn      TIM1_PWM_In
 *
 * @brief   Initializes PWM.
 *
 * @return  none
 */

void TIM1_PWM_In(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
    TIM_Cmd(TIM2, ENABLE);
}
/*********************************************************************
 * @fn      Get_ConversionVal1
 *
 * @brief   Get Conversion Value.
 *
 * @param   val - Sampling value
 *
 * @return  val+Calibrattion_Val - Conversion Value.
 */
u16 Get_ConversionVal(s16 val)
{
    if((val + Calibrattion_Val) < 0||val==0)
        return 0;
    if((Calibrattion_Val + val) > 4095 || val==4095)
        return 4095;
    return (val + Calibrattion_Val);
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
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    ADC_Function_Init();
    printf("CalibrattionValue:%d\n", Calibrattion_Val);
    TIM1_PWM_In(7200,1000-1,3600);

    while(1);
}

void ADC1_2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      ADC1_2_IRQHandler
 *
 * @brief   ADC1_2 Interrupt Service Function.
 *
 * @return  none
 */
void ADC1_2_IRQHandler()
{
    u16 ADC_val;

    if(ADC_GetITStatus(ADC1, ADC_IT_JEOC))
    {
        printf("ADC Extline trigger conversion...\r\n");
        ADC_val = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);

        printf("JADC %04d\r\n", Get_ConversionVal(ADC_val));
    }

    ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
}
