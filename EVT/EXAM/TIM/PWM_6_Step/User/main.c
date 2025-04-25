/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/11/26
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 * 6_six_PWM output routine:
 * TIM1_CH1(PA8) TIM1_CH1N(PB13)
 * TIM1_CH2(PA9) TIM1_CH2N(PB14)
 * TIM1_CH2(PA10) TIM1_CH3N(PB15)
 * PA9 is USRAT1_TX, if you need printf function, you should reselect USART in debug.h.
 * Only channel 1,2,3 have this function.
*/

#include "debug.h"

/*********************************************************************
 * @fn      TIM1_PWMOut_Init
 *
 * @brief   Initializes TIM1 output compare.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWMOut_Init( u16 arr, u16 psc, u16 ccp )
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	TIM_OCInitTypeDef TIM_OCInitStructure={0};
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB | RCC_APB2Periph_TIM1, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

	TIM_TimeBaseInitStructure.TIM_Period = arr;
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = ccp;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OC1Init( TIM1, &TIM_OCInitStructure );
    TIM_OC2Init( TIM1, &TIM_OCInitStructure );
    TIM_OC3Init( TIM1, &TIM_OCInitStructure );

	TIM_CtrlPWMOutputs(TIM1, ENABLE );
	TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable );
    TIM_OC2PreloadConfig( TIM1, TIM_OCPreload_Disable );
    TIM_OC3PreloadConfig( TIM1, TIM_OCPreload_Disable );
	TIM_ARRPreloadConfig( TIM1, ENABLE );

    TIM_CCPreloadControl(TIM1, ENABLE);//CCPC
    TIM_SelectCOM(TIM1, ENABLE);//CCUS

	TIM_Cmd( TIM1, ENABLE );
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
	Delay_Init();
    USART_Printf_Init(115200);
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	TIM1_PWMOut_Init( 100-1, 48000-1, 75 );

    TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);
    TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
    TIM_ForcedOC1Config(TIM1, TIM_ForcedAction_Active);

    TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
    TIM_ForcedOC2Config(TIM1, TIM_ForcedAction_InActive);

    TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);
    TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
    TIM_ForcedOC3Config(TIM1, TIM_ForcedAction_InActive);

    Delay_Ms(500);

    TIM_GenerateEvent(TIM1, TIM_EventSource_COM);

	while(1);
}
