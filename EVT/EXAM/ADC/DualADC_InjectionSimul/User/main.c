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
  Dual ADC injection simultaneous sampling routine:
 ADC1 channel 1 (PA1), ADC2 channel 2 (PA3).
*/

#include "debug.h"

/* Global Variable */
u16 ADC_val1,ADC_val2;
s16 Calibrattion_Val1 = 0;
s16 Calibrattion_Val2 = 0;

/*********************************************************************
 * @fn      ADC_Function_Init
 *
 * @brief   Initializes ADC collection.
 *
 * @return  none
 */
void  ADC_Function_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure={0};
    GPIO_InitTypeDef GPIO_InitStructure={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE );
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1  , ENABLE );
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2  , ENABLE );
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 |GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_DeInit(ADC2);

    ADC_InitStructure.ADC_Mode = ADC_Mode_InjecSimult;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigInjecConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_InitStructure.ADC_Pga = ADC_Pga_1;

    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_InjectedSequencerLengthConfig(ADC1, 1);
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    ADC_BufferCmd(ADC1, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
	Calibrattion_Val1 = Get_CalibrationValue(ADC1);

    ADC_Init(ADC2, &ADC_InitStructure);
    ADC_InjectedSequencerLengthConfig(ADC2, 1);
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5 );

    ADC_SoftwareStartInjectedConvCmd(ADC2, ENABLE);
    ADC_Cmd(ADC2, ENABLE);

    ADC_BufferCmd(ADC2, DISABLE);   //disable buffer
    ADC_ResetCalibration(ADC2);
    while(ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2));
	Calibrattion_Val2 = Get_CalibrationValue(ADC2);
	
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
u16 Get_ConversionVal1(s16 val)
{
	if((val+Calibrattion_Val1)<0) return 0;
	if((Calibrattion_Val2+val)>4095||val==4095) return 4095;
	return (val+Calibrattion_Val1);
}

/*********************************************************************
 * @fn      Get_ConversionVal2
 *
 * @brief   Get Conversion Value.
 *
 * @param   val - Sampling value
 *
 * @return  val+Calibrattion_Val - Conversion Value.
 */
u16 Get_ConversionVal2(s16 val)
{
    if((val+Calibrattion_Val2)<0) return 0;
    if((Calibrattion_Val2+val)>4095||val==4095) return 4095;
    return (val+Calibrattion_Val2);
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
	SystemCoreClockUpdate();
	Delay_Init();	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
	ADC_Function_Init();
	printf("CalibrattionValue1:%d\n", Calibrattion_Val1);
	printf("CalibrattionValue2:%d\n", Calibrattion_Val2);
	
	while(1)
	{
	    ADC_SoftwareStartInjectedConvCmd(ADC1, ENABLE);
        Delay_Ms(100);

        ADC_SoftwareStartInjectedConvCmd(ADC1, DISABLE);
        Delay_Ms(100);

        while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_JEOC ));
        ADC_val1 = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
        ADC_val2 = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_1);

        printf( "JADC1 ch2=%04d\r\n", Get_ConversionVal1(ADC_val1));
        printf( "JADC2 ch3=%04d\r\n", Get_ConversionVal2(ADC_val2));
	}
}




