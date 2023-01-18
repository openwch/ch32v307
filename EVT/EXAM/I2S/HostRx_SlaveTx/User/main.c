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
   I2S master receiver slave transmitter routine:
      This routine demonstrates that I2S3 acts as a slave to send,
      and I2S2 acts as a master to receive.
 SPI2-I2S2:
     WS -- PB12
     CK -- PB13
     SD -- PB15
     MCK-- PC6

 SPI3-I2S3:
     WS -- PA15
     CK -- PB3
     SD -- PB5
     MCK-- PC7
*/
#include "debug.h"
#include "string.h"

/* Global Variable */
#define   _16bit__data_mode   0    //<= 16bit
#define   _32bit__data_mode   1    //> 16bit

#define  data_len_mode   _16bit__data_mode
//#define  data_len_mode   _32bit__data_mode

#define  Len    10

u32 I2S3_Tx[Len];
u32 I2S2_Rx[Len];


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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterRx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;

#if(data_len_mode == _16bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;

#elif (data_len_mode == _32bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;

#endif

    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
    I2S_Init(SPI2, &I2S_InitStructure);
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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);



    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_3 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveTx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;

#if(data_len_mode == _16bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;

#elif (data_len_mode == _32bit__data_mode)
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;

#endif

    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
    I2S_Init(SPI3, &I2S_InitStructure);
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
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(921600);	
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    for(i=0; i<Len; i++){
#if(data_len_mode == _16bit__data_mode)
        I2S3_Tx[i] = 0x5aa1 +i;

#elif (data_len_mode == _32bit__data_mode)
        I2S3_Tx[i] = 0x1AA15aa1 +i+ (i<<28);

#endif

    }

    I2S3_Init();
    I2S2_Init();

#if(data_len_mode == _16bit__data_mode)
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1);
    SPI_I2S_SendData(SPI3, I2S3_Tx[0]);
#elif (data_len_mode == _32bit__data_mode)
        while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1);
        SPI_I2S_SendData(SPI3, (u16)(I2S3_Tx[0]>>16));

#endif

    I2S_Cmd(SPI3, ENABLE);
    I2S_Cmd(SPI2, ENABLE);

#if(data_len_mode == _16bit__data_mode)
    while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1);
    SPI_I2S_SendData(SPI3, I2S3_Tx[1]);

#elif (data_len_mode == _32bit__data_mode)
        while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1);
        SPI_I2S_SendData(SPI3, (u16)(I2S3_Tx[0]));


#endif

    for(i=0; i<Len; i++){
#if(data_len_mode == _16bit__data_mode)
        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != 1);
        I2S2_Rx[i] = SPI_I2S_ReceiveData(SPI2);

        while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1);
        SPI_I2S_SendData(SPI3, (u16)I2S3_Tx[i+2]);

#elif (data_len_mode == _32bit__data_mode)

        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != 1);
        I2S2_Rx[i] = SPI_I2S_ReceiveData(SPI2);
        while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1);
        SPI_I2S_SendData(SPI3, (u16)(I2S3_Tx[i+1]>>16));
        while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != 1);
        I2S2_Rx[i] = (u32)(SPI_I2S_ReceiveData(SPI2)) + (u32)(I2S2_Rx[i]<<16);
        while(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) != 1);
        SPI_I2S_SendData(SPI3, (u16)(I2S3_Tx[i+1]));

#endif

    }

    I2S_Cmd(SPI3, DISABLE);
    I2S_Cmd(SPI2, DISABLE);

    printf("Rx data:\r\n");
    for(i=0; i<Len; i++){

        printf("%08x %08x\r\n", I2S3_Tx[i], I2S2_Rx[i]);
    }

    while(1);
}

