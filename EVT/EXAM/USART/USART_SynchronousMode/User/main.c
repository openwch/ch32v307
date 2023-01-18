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
synchronous mode, master/slave mode, transceiver routine:
 Master:USART2_CK(PA4)\USART2_Tx(PA2)\USART4_Rx(PA3).
 Slave:SPI1_SCK(PA5)\SPI1_MISO(PA6)\SPI1_MOSI(PA7).
 This example demonstrates using USART2 as the master and SPI1 as the slave,
 sending and receiving data in full duplex.
Hardware connection:
           PA4 -- PA5
           PA2  -- PA7
           PA3  -- PA6

*/

#include "debug.h"

/* Global typedef */
typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

/* Global define */
#define TxSize1    (size(TxBuffer1))
#define TxSize2    (size(TxBuffer2))
#define size(a)    (sizeof(a) / sizeof(*(a)))

/* Global Variable */
u8 TxBuffer1[] = "*Buffer1 Send from USART2 to SPI1 using SynchromousMode!"; /* Send by UART1 */
u8 TxBuffer2[] = "#Buffer2 Send from SPI1 to USART2 using SynchromousMode!"; /* Send by SPI1  */
u8 RxBuffer1[TxSize1] = {0};                                                 /* USART2 Using  */
u8 RxBuffer2[TxSize2] = {0};                                                 /* SPI1   Using  */

u8 TxCnt1 = 0, RxCnt1 = 0;
u8 TxCnt2 = 0, RxCnt2 = 0;
u8 Tempdata = 0x00;

TestStatus TransferStatus1 = FAILED;
TestStatus TransferStatus2 = FAILED;

/*********************************************************************
 * @fn      Buffercmp
 *
 * @brief   Compares two buffers
 *
 * @param   Buf1,Buf2 - buffers to be compared
 *          BufferLength - buffer's length
 *
 * @return  PASSED - Buf1 identical to Buf
 *          FAILED - Buf1 differs from Buf2
 */
TestStatus Buffercmp(uint8_t *Buf1, uint8_t *Buf2, uint16_t BufLength)
{
    while(BufLength--)
    {
        if(*Buf1 != *Buf2)
        {
            return FAILED;
        }
        Buf1++;
        Buf2++;
    }
    return PASSED;
}

/*********************************************************************
 * @fn      USART2_ReCFG
 *
 * @brief   ReInitializes the USART2 peripheral.
 *
 * @return  none
 */
void USART2_ReCFG(void)
{
    GPIO_InitTypeDef       GPIO_InitStructure = {0};
    USART_InitTypeDef      USART_InitStructure = {0};
    USART_ClockInitTypeDef USART_ClockInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* USART2  Ck-->A.4   TX-->A.2   RX-->A.3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_High;         /* Clock is active High */
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;        /* Data is captured on the second edge */
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable; /* The clock pulse of the last data bit is output to the SCLK pin */
    USART_ClockInit(USART2, &USART_ClockInitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);

    USART_Cmd(USART2, ENABLE);
}

/*********************************************************************
 * @fn      SPI1_INIT
 *
 * @brief   Initializes the SPI1 to be Slave Mode.
 *
 * @return  none
 */
void SPI1_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef  SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
    SPI_StructInit(&SPI_InitStructure);
    SPI_I2S_DeInit(SPI1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; /* SPI1 MISO-->PA.6 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7; /* SPI1 SCK-->PA.5 MOSI-->PA.7 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_LSB;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
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
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("USART SynchromousMode TEST\r\n");
    USART2_ReCFG(); /* USART2 ReInitializes */
    SPI1_INIT();

    while(TxCnt1 < TxSize1) /* USART2--->SPI1 */
    {
        USART_SendData(USART2, TxBuffer1[TxCnt1++]);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) /* waiting for sending finish */
        {
        }
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        {
        }
        RxBuffer2[RxCnt2++] = SPI_I2S_ReceiveData(SPI1);
    }
    USART_ReceiveData(USART2); /* Clear the USART2 Data Register */
    while(TxCnt2 < TxSize2)    /* SPI1--->USART2 */
    {
        while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) /* waiting for sending finish */
        {
        }
        SPI_I2S_SendData(SPI1, TxBuffer2[TxCnt2++]);
        USART_SendData(USART2, Tempdata); /* Send Tempdata for SCK */
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        {
        }
        while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
        {
        }
        RxBuffer1[RxCnt1++] = USART_ReceiveData(USART2);
    }

    TransferStatus1 = Buffercmp(TxBuffer1, RxBuffer2, TxSize1);
    TransferStatus2 = Buffercmp(TxBuffer2, RxBuffer1, TxSize2);
    if(TransferStatus1 && TransferStatus2)
    {
        printf("\r\nSend Success!\r\n");
    }
    else
    {
        printf("\r\nSend Fail!\r\n");
    }
    printf("TxBuffer1---->RxBuffer2     TxBuffer2---->RxBuffer1\r\n");
    printf("TxBuffer1:%s\r\n", TxBuffer1);
    printf("RxBuffer1:%s\r\n", RxBuffer1);
    printf("TxBuffer2:%s\r\n", TxBuffer2);
    printf("RxBuffer2:%s\r\n", RxBuffer2);

    while(1)
    {
    }
}
