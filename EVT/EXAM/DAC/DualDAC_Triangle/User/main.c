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
  Dual DAC output triangular wave routine:
 DAC outputs from PA4 and PA5 respectively
   Output two triangular waves with different amplitudes and frequencies
*/

#include "debug.h"

/* Global define */


/* Global Variable */ 

/*********************************************************************
 * @fn      Dual_Dac_Init
 *
 * @brief   Initializes DAC collection.
 *
 * @return  none
 */
void Dual_Dac_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	DAC_InitTypeDef DAC_InitType={0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		     
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	
	DAC_InitType.DAC_Trigger=DAC_Trigger_T4_TRGO;
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_Triangle;
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_TriangleAmplitude_4095;
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;
    DAC_Init(DAC_Channel_1,&DAC_InitType);
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_TriangleAmplitude_2047;
    DAC_Init(DAC_Channel_2,&DAC_InitType);

    DAC_Cmd(DAC_Channel_1, ENABLE);
    DAC_Cmd(DAC_Channel_2, ENABLE);

    DAC_SetDualChannelData(DAC_Align_12b_R, 0,0);
}

/*********************************************************************
 * @fn      Timer4_Init
 *
 * @brief   Initializes TIM4.
 *
 * @return  none
 */
void Timer4_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 36-1;
    TIM_TimeBaseStructure.TIM_Prescaler =0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);
    TIM_Cmd(TIM4, ENABLE);
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
	printf("Dual DAC Triangle Generation Test\r\n");

	Dual_Dac_Init();
    Timer4_Init();
    while(1)
   {

        printf("CNT=%d\r\n",TIM4->CNT);
        printf("RD12BDHR=0x%04x\r\n",DAC->RD12BDHR);
        printf("DOR1=0x%04x\r\n",DAC->DOR1);
        printf("DOR2=0x%04x\r\n",DAC->DOR2);
        Delay_Ms(500);
   }
}

