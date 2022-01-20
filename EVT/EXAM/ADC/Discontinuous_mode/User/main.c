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
 间断模式例程：
 ADC通道2(PA2)-注入组通道，通道3(PA3)-注入组通道，通道4(PA4)-注入组通道，该模式
 下，通过TIM1_CC4事件触发一次ADC转换，每次依次转换上述1个注入组通道。

*/

#include "debug.h"
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
	ADC_InitTypeDef ADC_InitStructure={0};
	GPIO_InitTypeDef GPIO_InitStructure={0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE );
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_T1_CC4;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 3;
	ADC_Init(ADC1, &ADC_InitStructure);

    ADC_InjectedSequencerLengthConfig(ADC1, 3);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_239Cycles5);
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_239Cycles5);

    ADC_DiscModeChannelCountConfig( ADC1, 1);
	ADC_InjectedDiscModeCmd(ADC1 , ENABLE);
    ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
	Calibrattion_Val = Get_CalibrationValue(ADC1);
	
    ADC_BufferCmd(ADC1, ENABLE);   //enable buffer
}

/*********************************************************************
 * @fn      TIM1_PWM_In
 *
 * @brief   Initializes TIM1 PWM output.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWM_In( u16 arr, u16 psc, u16 ccp )
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_OCInitTypeDef TIM_OCInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = ccp;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC4Init( TIM1, &TIM_OCInitStructure );

	TIM_CtrlPWMOutputs(TIM1, ENABLE );
	TIM_OC4PreloadConfig( TIM1, TIM_OCPreload_Disable );
	TIM_ARRPreloadConfig( TIM1, ENABLE );
	TIM_SelectOutputTrigger( TIM1, TIM_TRGOSource_Update );
	TIM_Cmd( TIM1, ENABLE );
}

/*********************************************************************
 * @fn      Get_ConversionVal
 *
 * @brief   Get Conversion Value.
 *
 * @param   val - Sampling value
 *
 * @return  val+Calibrattion_Val - Conversion Value.
 */
u16 Get_ConversionVal(s16 val)
{
	if((val+Calibrattion_Val)<0) return 0;
	if((Calibrattion_Val+val)>4095) return 4095;
	return (val+Calibrattion_Val);
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
	USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);

	ADC_Function_Init();
    printf("CalibrattionValue:%d\n", Calibrattion_Val);
	
	TIM1_PWM_In( 1000, 48000-1, 500 );

	while(1){
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));
		ADC_ClearFlag( ADC1, ADC_FLAG_EOC);
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC ));
		ADC_ClearFlag( ADC1, ADC_FLAG_JEOC);

		printf("ADC Discontinuous injected group conversion...\r\n");
		printf( "%04d\r\n", Get_ConversionVal(ADC1->IDATAR1) );
		printf( "%04d\r\n", Get_ConversionVal(ADC1->IDATAR2) );
		printf( "%04d\r\n", Get_ConversionVal(ADC1->IDATAR3) );
		printf( "%04d\r\n", Get_ConversionVal(ADC1->IDATAR4) );
	}
}
