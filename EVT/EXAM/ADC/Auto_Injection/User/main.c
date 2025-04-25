/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/01/07
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *Automatic injection mode routine:
 *ADC channel 1 (PA1) - regular group channel, channel 3 (PA3) - injection group channel
 *
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

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

               ADC_InjectedSequencerLengthConfig(ADC1, 1);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );
	ADC_InjectedChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5 );
	ADC_AutoInjectedConvCmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
	Calibrattion_Val = Get_CalibrationValue(ADC1);
	
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
	if((val+Calibrattion_Val)<0|| val==0) return 0;
	if((Calibrattion_Val+val)>4095||val==4095) return 4095;
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
	u16 adc_val;
	u16 adc_jval;

	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(115200);		
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	ADC_Function_Init();
	printf("CalibrattionValue:%d\n", Calibrattion_Val);

	while(1)
	{
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while( !ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) );
		while( !ADC_GetFlagStatus( ADC1, ADC_FLAG_JEOC ) );
		adc_val = ADC_GetConversionValue(ADC1);
		adc_jval = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
		Delay_Ms(500);
		printf( "val:%04d\r\n", Get_ConversionVal(adc_val));
		printf( "jval:%04d\r\n", Get_ConversionVal(adc_jval));
		Delay_Ms(2);
	}
}



