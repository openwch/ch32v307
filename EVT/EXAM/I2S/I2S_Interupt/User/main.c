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
   I2S主发从收中断例程：
      本例程演示 I2S3作从机接收，I2S2作主机发送。
 SPI2-I2S2:
     WS —— PB12
     CK —— PB13
     SD —— PB15
     MCK—— PC6

 SPI3-I2S3:
     WS —— PA15
     CK —— PB3
     SD —— PB5
     MCK—— PC7
*/

#include "debug.h"
#include "string.h"

/* Global Variable */
#define   _16bit__data_mode   0    //<= 16bit
#define   _32bit__data_mode   1    //> 16bit

#define  data_len_mode   _16bit__data_mode
//#define  data_len_mode   _32bit__data_mode

#define  Len    10

u32 I2S2_Tx[Len];
u32 I2S3_Rx[Len];

void SPI3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SPI2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      I2S2_Init
 *
 * @brief   Init I2S2
 *
 * @return  none
 */
void I2S2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    I2S_InitTypeDef  I2S_InitStructure={0};
    NVIC_InitTypeDef NVIC_InitStructure={0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;

#if(data_len_mode == _16bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;

#elif (data_len_mode == _32bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;

#endif

    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(SPI2, &I2S_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    SPI_I2S_ClearITPendingBit(SPI3, SPI_I2S_IT_TXE);
    SPI_I2S_ClearITPendingBit(SPI3, SPI_I2S_IT_ERR);

    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_ERR, ENABLE);

    I2S_Cmd(SPI2, ENABLE);

}

/*********************************************************************
 * @fn      I2S3_Init
 *
 * @brief   Init I2S2
 *
 * @return  none
 */
void I2S3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    I2S_InitTypeDef  I2S_InitStructure={0};
    NVIC_InitTypeDef NVIC_InitStructure={0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_PinRemapConfig( GPIO_Remap_SWJ_JTAGDisable, ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_3 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveRx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;

#if(data_len_mode == _16bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;

#elif (data_len_mode == _32bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;

#endif

    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(SPI3, &I2S_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    SPI_I2S_ClearITPendingBit(SPI3, SPI_I2S_IT_RXNE);
    SPI_I2S_ClearITPendingBit(SPI3, SPI_I2S_IT_ERR);

    SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE);
    SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_ERR, ENABLE);

    I2S_Cmd(SPI3, ENABLE);
}

u8 t=0, p=0;

/*********************************************************************
 * @fn      SPI2_IRQHandler
 *
 * @brief   This function handles SPI2 exception.
 *
 * @return  none
 */
void SPI2_IRQHandler(void)
{
    static u8 falg = 0;

    if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == 1){
#if (data_len_mode == _16bit__data_mode)
        SPI_I2S_SendData(SPI2, I2S2_Tx[t++]);

#elif (data_len_mode == _32bit__data_mode)
        if(falg==0){
            SPI_I2S_SendData(SPI2, (u16)(I2S2_Tx[t]>>16));
            falg =1;
        }
        else{
            SPI_I2S_SendData(SPI2, (u16)(I2S2_Tx[t++]));;
            falg =0;
        }

#endif

    }

}

/*********************************************************************
 * @fn      SPI3_IRQHandler
 *
 * @brief   This function handles SPI3 exception.
 *
 * @return  none
 */
void SPI3_IRQHandler(void)
{
    static u8 falg = 0;

    if(SPI_I2S_GetITStatus(SPI3, SPI_I2S_IT_RXNE) == 1){
#if (data_len_mode == _16bit__data_mode)
        I2S3_Rx[p++] = SPI_I2S_ReceiveData(SPI3);

        if(p==11){

            SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, DISABLE);
      SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
        }

#elif (data_len_mode == _32bit__data_mode)
        if(falg==0){
            I2S3_Rx[p] = SPI_I2S_ReceiveData(SPI3);
            falg =1;
        }
        else{
            I2S3_Rx[p++] = (u32)(SPI_I2S_ReceiveData(SPI3)) + (u32)(I2S3_Rx[p]<<16);
            falg =0;
        }

        if(p==21){
            SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, DISABLE);
            SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
        }
#endif

    }

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
    u32 i;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    Delay_Init();
    USART_Printf_Init(921600);
    printf("SystemClk:%d\r\n",SystemCoreClock);

    Delay_Ms(1000);
    Delay_Ms(1000);

    for(i=0; i<Len; i++){
#if(data_len_mode == _16bit__data_mode)
    I2S2_Tx[i] = 0x5aa1 +i;

#elif (data_len_mode == _32bit__data_mode)
    I2S2_Tx[i] = 0x5AA15aa1 +i+ (i<<16);

#endif
    }

    SPI_I2S_DeInit(SPI2);
    SPI_I2S_DeInit(SPI3);

    I2S3_Init();
    I2S2_Init();

    printf("Rx data:\r\n");
    for(i=0; i<Len; i++){
        printf("%08x %08x\r\n", I2S2_Tx[i], I2S3_Rx[i]);
    }

    while(1);
}



