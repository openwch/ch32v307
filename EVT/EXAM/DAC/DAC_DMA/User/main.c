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
 DAC DMA conversion routines:
 DAC channel 0 (PA4) output
 DMA is configured in cyclic mode, a DAC conversion is triggered by a TIM8_TRGO event, and PA4 outputs
 the corresponding voltage.

*/

#include "debug.h"

/* Global define */


/* Global Variable */
u16 dacbuff16bit[8]={0x123,0x234,0x345,0x456,0x567,0x678,0x789,0x89a};


/*********************************************************************
 * @fn      Dac_Init
 *
 * @brief   Initializes DAC collection.
 *
 * @return  none
 */
void Dac_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	DAC_InitTypeDef DAC_InitType={0};

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);

	DAC_InitType.DAC_Trigger=DAC_Trigger_T8_TRGO;
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;
    DAC_Init(DAC_Channel_1,&DAC_InitType);

	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_DMACmd(DAC_Channel_1,ENABLE);
}

/*********************************************************************
 * @fn      DAC1_DMA_INIT
 *
 * @brief   Initializes DMA of DAC1 collection.
 *
 * @return  none
 */
void DAC1_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure={0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    DMA_StructInit( &DMA_InitStructure);
    /* Note:DAC1--->DMA1.CH3   DAC2--->DMA1.CH4 */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(DAC->R12BDHR1);
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&dacbuff16bit;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA2_Channel3, &DMA_InitStructure);
    DMA_Cmd(DMA2_Channel3, ENABLE);
}

/*********************************************************************
 * @fn      TIM8_Init
 *
 * @brief   Initializes TIM3 collection.
 *
 * @param   arr - TIM_Period
 *          psc - TIM_Prescaler
 *
 * @return  none
 */
void TIM8_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM8, ENABLE );

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter =  0x00;
    TIM_TimeBaseInit( TIM8, &TIM_TimeBaseInitStructure);

	TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);
	TIM_Cmd(TIM8, ENABLE);
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
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	printf("DAC DMA TEST\r\n");
	Dac_Init();
	DAC1_DMA_Init();

	TIM8_Init(0x7,48000-1);

	while(1)
    {
        Delay_Ms(1000);

		printf("run\r\n");
		printf("DAC->R12BDHR1:0x%04x\r\n",DAC->R12BDHR1);
		printf("DAC->DOR1:0x%04x\r\n",DAC->DOR1);
		printf("TIM8->CNT:%d\r\n",TIM8->CNT);
	}
}


