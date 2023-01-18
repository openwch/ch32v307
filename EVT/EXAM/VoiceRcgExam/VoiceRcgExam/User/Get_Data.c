/********************************** (C) COPYRIGHT *******************************
* File Name          : Get_Data.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        :
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "Get_Data.h"
#include "VoiceRcg.h"


extern volatile uint8_t g_data_ready;

#if USE_ES8388
#include "es8388.h"
extern __attribute__((aligned(4))) uint16_t V_Data[SampleDataLen*2];
void I2S2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2S_InitTypeDef  I2S_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterRx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;

    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_8k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
    I2S_Init(SPI2, &I2S_InitStructure);

    SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Rx, ENABLE);
    I2S_Cmd(SPI2,DISABLE);
}

//I2S DMA config
void DMA_Rx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize )
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );
    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init( DMA_CHx, &DMA_InitStructure );
    DMA_ITConfig(DMA_CHx,DMA_IT_TC,ENABLE);
}


//用于拆分数据
void DMA_Data_Tran(DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );
    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init( DMA_CHx, &DMA_InitStructure );
    DMA_ITConfig(DMA_CHx,DMA_IT_TC,ENABLE);
    DMA_Cmd( DMA_CHx, ENABLE );
}


void voice_init(void)
{
    GPIO_WriteBit(GPIOA,GPIO_Pin_8,1);              //PA8 设置录音模式
    ES8388_Init();
    ES8388_Set_Volume(22);
    ES8388_I2S_Cfg(0,3);                            //飞利浦格式，16bit
    ES8388_ADDA_Cfg(1,0);                           //开启AD 关闭DA
    I2S2_Init();
    NVIC_SetPriority(DMA1_Channel4_IRQn,0xE0);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);

    NVIC_SetPriority(DMA1_Channel5_IRQn,0xE0);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
    printf("init es8388\r\n");
}

/*********************************************************************
 * @fn      DMA1_Channel4_IRQHandler
 *
 * @brief   This function DMA1 Channel4 exception.
 *
 * @return  none
 */
void DMA1_Channel4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel4_IRQHandler(void)
{
   if(DMA_GetITStatus(DMA1_IT_TC4))
   {
       DMA_ClearITPendingBit(DMA1_IT_TC4|DMA1_IT_GL4|DMA1_IT_HT4);
       I2S_Cmd(SPI2,DISABLE);
//       printf("tc4\r\n");
//       for(int i=0;i<SampleDataLen*2;i++)
//       {
//           printf("%04x  ",V_Data[i]);
//           if((i%16==0) && (i!=0))
//           {
//               printf("\r\n");
//           }
//       }
       DMA_Data_Tran(DMA1_Channel5, (u32)V_Data, (u32)V_Data,SampleDataLen);//利用DMA每个字只取半字
   }
}

/*********************************************************************
 * @fn      DMA1_Channel5_IRQHandler
 *
 * @brief   This function DMA1 Channel5 exception.
 *
 * @return  none
 */
void DMA1_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel5_IRQHandler(void)
{
   if(DMA_GetITStatus(DMA1_IT_TC5))
   {
//       printf("TC5\r\n");
       for(uint16_t i=0;i<SampleDataLen;i++)//原始数据为有符号16位，传入参数需要无符号16位
       {
           V_Data[i]=(uint16_t)((int16_t)V_Data[i]+32768);
       }
       g_data_ready=1;
       DMA_ClearITPendingBit(DMA1_IT_TC5|DMA1_IT_GL5|DMA1_IT_HT5);
   }
}

#else
extern __attribute__((aligned(4))) uint16_t V_Data[SampleDataLen];
void voice_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    ADC_InitTypeDef ADC_InitStructure={0};
    DMA_InitTypeDef DMA_InitStructure={0};

    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure={0};
    TIM_OCInitTypeDef         TIM_OCInitStructure={0};

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 90-1;
    TIM_TimeBaseStructure.TIM_Prescaler = 200-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    /* TIM1 channel1 configuration in PWM mode */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0x10;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    /* DMA1 channel1 configuration ----------------------------------------------*/
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&(ADC1->RDATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)V_Data;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = SampleDataLen;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
    DMA_Cmd(DMA1_Channel1, ENABLE);

    /* ADC1 configuration ------------------------------------------------------*/
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    /* ADC1 regular channel1 configuration */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_41Cycles5);
    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);
    /* ADC1 外部触发使能 */
    ADC_ExternalTrigConvCmd(ADC1,ENABLE);
    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);


    NVIC_SetPriority(DMA1_Channel1_IRQn,0xE0);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

/*********************************************************************
 * @fn      DMA1_Channel1_IRQHandler
 *
 * @brief   This function DMA1 Channel1 exception.
 *
 * @return  none
 */
void DMA1_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel1_IRQHandler(void)
{
   if(DMA_GetITStatus(DMA1_IT_TC1))
   {
       TIM_Cmd(TIM1, DISABLE);
//       printf("tc1\r\n");
       g_data_ready=1;
       DMA_ClearITPendingBit(DMA1_IT_TC1|DMA1_IT_GL1|DMA1_IT_HT1);
   }
}
#endif



