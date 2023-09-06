/********************************** (C) COPYRIGHT *******************************
* File Name          : bsp_uart.c
* Author             : WCH
* Version            : V1.0
* Date               : 2022/05/10
* Description        : uart cfg.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "bsp_uart.h"

/* Global Variable */

struct uart_data  uart_data_t;
/* UART2 */
u8  UART2_RX_DMA_DataBuf[UART_RX_DMA_SIZE]= {0};
u8  UART2_TX_DMA_DataBuf[UART_TX_BUF_NUM][ETH_RECEIVE_SIZE] = {0};
u16 UART2_TX_Buf_Length[UART_TX_BUF_NUM] = {0};

/**********************************************************************
 *  @fn      UART_GPIO_Init
 *
 *  @brief   initialize uart2 GPIO
 *
 *  @return  none
 * */
void UART_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* UART2 TX-->PA2   RX-->PA3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**********************************************************************
 *  @fn      UART_DMA_Init
 *
 *  @brief   initialize uart2 DMA
 *
 *  @return  none
 * */
void UART_DMA_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* UART2  TX-->DMA1_Channel7   RX-->DMA1_Channel6 */
    DMA_DeInit(DMA1_Channel7);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t.TX_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    //DMA_InitStructure.DMA_BufferSize = size(TX_DMA_dataBuf);
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);

    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t.RX_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);

    DMA_ClearFlag(DMA1_IT_TC7);

    DMA_Cmd(DMA1_Channel6, ENABLE);
    USART_DMACmd(USART2,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);
}

/**********************************************************************
 *  @fn      UART_Interrupt_Init
 *
 *  @brief   initialize uart2 interrupt
 *
 *  @return  none
 * */
void UART_Interrupt_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    /* UART2  TX-->DMA1_Channel7   RX-->DMA1_Channel6 */
    DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**********************************************************************
 *  @fn      BSP_Uart_Init
 *
 *  @brief   initialize uart2
 *
 *  @return  none
 * */
void BSP_Uart_Init(uint32_t baudrate )
{
    USART_InitTypeDef USART_InitStructure = {0};

    UART_GPIO_Init();

    /* USART2 TX-->PA2   RX-->PA3 */
    uart_data_t.RX_buffer = UART2_RX_DMA_DataBuf;
    uart_data_t.TX_buffer = &UART2_TX_DMA_DataBuf[0];
    uart_data_t.TX_data_length = UART2_TX_Buf_Length;
    uart_data_t.rx_read = 0;
    uart_data_t.rx_write = 0;
    uart_data_t.last_RX_DMA_length = UART_RX_DMA_SIZE;
    uart_data_t.RX_data_length = 0;
    uart_data_t.tx_read = 0;
    uart_data_t.tx_write = 0;
    uart_data_t.tx_remainBuffNum = UART_TX_BUF_NUM;
    uart_data_t.uart_tx_dma_state = IDLE;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);

    UART_DMA_Init();
    UART_Interrupt_Init();
}
