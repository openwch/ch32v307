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
   I2S master transmitter slave receiver interrupt routine:
     This routine demonstrates two boards - one board I2S3 is used as a slave to receive,
     and the other board I2S2 is used as a master to send.
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

//#define  data_len_mode   _16bit__data_mode
#define  data_len_mode   _32bit__data_mode

/* I2S Mode Definition */
#define HOST_MODE     0
#define SLAVE_MODE    1

/* I2S Communication Mode Selection */
#define I2S_MODE   HOST_MODE
//#define I2S_MODE   SLAVE_MODE

#define  Len    10

volatile u8 t=0, p=0, flag_end = 0;

#if (I2S_MODE == HOST_MODE)
u32 I2S2_Tx[Len];
void SPI2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#else

u32 I2S3_Rx[Len];
void SPI3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
#endif


#if (I2S_MODE == HOST_MODE)
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

    SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_TXE);

    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);

    I2S_Cmd(SPI2, ENABLE);

}

/*********************************************************************
 * @fn      SPI2_IRQHandler
 *
 * @brief   This function handles SPI2 exception.
 *
 * @return  none
 */
void SPI2_IRQHandler(void)
{
    static u8 flag = 0;

    if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) == 1){
#if (data_len_mode == _16bit__data_mode)
        SPI_I2S_SendData(SPI2, I2S2_Tx[t++]);

#elif (data_len_mode == _32bit__data_mode)
        if(flag==0){
            SPI_I2S_SendData(SPI2, (u16)(I2S2_Tx[t]>>16));
            flag =1;
        }
        else{
            SPI_I2S_SendData(SPI2, (u16)(I2S2_Tx[t++]));;
            flag =0;
        }

#endif

    }

}
#else
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

    SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, ENABLE);


    I2S_Cmd(SPI3, ENABLE);
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

        if(p==10){
            flag_end = 1;
            SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, DISABLE);
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

        if(p==20){
            flag_end = 1;
            SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_RXNE, DISABLE);
        }
#endif

    }

}

#endif

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

    Delay_Ms(1000);


#if (I2S_MODE == HOST_MODE)

    Delay_Ms(1000);
    for(i=0; i<Len; i++){
#if(data_len_mode == _16bit__data_mode)
    I2S2_Tx[i] = 0x5aa1 +i;

#elif (data_len_mode == _32bit__data_mode)
    I2S2_Tx[i] = 0x5AA15aa1 +i+ (i<<16);

#endif
    }
    SPI_I2S_DeInit(SPI2);
    I2S2_Init();

    Delay_Ms(10);
    printf("Tx data:\r\n");
    for(i=0; i<Len; i++){
        printf("%08x\r\n", I2S2_Tx[i]);
    }

#else


    SPI_I2S_DeInit(SPI3);
    I2S3_Init();

    while(flag_end==0);

    printf("Rx data:\r\n");
    for(i=0; i<Len; i++){
        printf("%08x\r\n", I2S3_Rx[i]);
    }

#endif

    while(1);
}



