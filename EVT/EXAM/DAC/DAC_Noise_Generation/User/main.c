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
 Noise waveform generation routine:
DAC channel 0 (PA4) output
 A DAC conversion is triggered by software, and PA4 outputs a noise waveform.

*/

#include "debug.h"

/* Global define */
#define Num    7

/* Global Variable */

/*********************************************************************
 * @fn      Dac_Init
 *
 * @brief   Initializes DAC collection.
 *
 * @return  none
 */
void Dac_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    DAC_InitTypeDef  DAC_InitType = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    DAC_InitType.DAC_Trigger = DAC_Trigger_Software;
    DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits11_0;
    DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
    DAC_Init(DAC_Channel_1, &DAC_InitType);
    DAC_Cmd(DAC_Channel_1, ENABLE);

    DAC_SetChannel1Data(DAC_Align_12b_R, 0);
}

/*********************************************************************
 * @fn      DAC1_Nosie_Gen_Test
 *
 * @brief   Noise wava generation test.
 *
 * @return  none
 */
void DAC1_Nosie_Gen_Test(void)
{
    DAC->SWTR |= 0x01; /* Set by software, Reset by hardware */
    __asm volatile("nop");
    printf("DOR1=0x%04x\r\n", DAC->DOR1); /* Value will be 0x0AAA,0x0D55,0x0EAA ... */
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
    Dac_Init();	
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("Noise Wava Generation Test\r\n");
    while(1)
    {
        DAC1_Nosie_Gen_Test();
    }
}
