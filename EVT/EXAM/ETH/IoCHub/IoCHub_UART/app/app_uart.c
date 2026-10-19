/*
 * app_uart.c
 *
 *  Created on: Sep 9, 2022
 *      Author: TECH66
 */
#include "app_uart.h"
#include "app_iochub.h"

extern uart_device_t s_uart_dev[COMNUM];

#if UART1_CFG_EN
/*********************************************************************
 * @fn      uart1_init
 *
 * @brief   Initializes the USART1 peripheral.
 *
 * @return  none
 */
void uart1_init (uint32_t baudrate) {

    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1, ENABLE);
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init (USART1, &USART_InitStructure);
    USART_ITConfig (USART1, USART_IT_IDLE, ENABLE);                   /* Enable serial port idle interrupt  */
    USART_Cmd (USART1, ENABLE);
    USART_DMACmd (USART1, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); /* Enable serial port DMA transceiver */

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn; /* UART1 DMA1Tx*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn; /* UART1 DMA1Rx*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
}

/**
 * @brief  uart1 dma send channel configuration
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart1_dmatx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA1_Channel4);
    DMA_Cmd (DMA1_Channel4, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART1->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; /* Transfer Direction:Memory->Peripherals */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA1_Channel4, &DMA_InitStructure);
    DMA_ITConfig (DMA1_Channel4, DMA_IT_TC | DMA_IT_TE, ENABLE);
    DMA_ClearFlag (DMA1_IT_TC4); /* Clearing the delivery completion flag */
    /*Left disabled on purpose: uart1_dmatx_send() enables the channel when a
     *transfer is actually started.*/
    DMA_Cmd (DMA1_Channel4, DISABLE);
}

/**
 * @brief  uart1 dma send enable
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart1_dmatx_send (uint8_t *mem_addr, uint32_t mem_size) {
    DMA1_Channel4->CFGR &= ~0x00000001;
    DMA1_Channel4->CNTR = mem_size;
    DMA1_Channel4->MADDR = (uint32_t)mem_addr;
    DMA_ClearFlag (DMA1_FLAG_TC4);
    DMA1_Channel4->CFGR |= 0x00000001;
}
/**
 * @brief  uart1 dma receive channel configuration
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart1_dmarx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA1_Channel5);
    DMA_Cmd (DMA1_Channel5, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART1->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; /* Transfer direction:Peripheral->Memory */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA1_Channel5, &DMA_InitStructure);
    DMA_ITConfig (DMA1_Channel5, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE); /* Enable DMA half-full, overflow, and error interrupts */
    DMA_ClearFlag (DMA1_IT_TC5);
    DMA_ClearFlag (DMA1_IT_HT5);
    DMA_Cmd (DMA1_Channel5, ENABLE);
}

uint16_t uart1_get_dmarx_buf_remain_size (uint8_t uart_id) {

    return DMA_GetCurrDataCounter (DMA1_Channel5);
}
#endif

#if UART2_CFG_EN
/*********************************************************************
 * @fn      uart2_init
 *
 * @brief   Initializes the USART2 peripheral.
 *
 * @return  none
 */
void uart2_init (uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1, ENABLE);


    /* USART2 TX-->A.2   RX-->A.3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init (USART2, &USART_InitStructure);
    USART_ITConfig (USART2, USART_IT_IDLE, ENABLE);                   /* Enable serial port idle interrupt  */
    USART_Cmd (USART2, ENABLE);
    USART_DMACmd (USART2, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); /* Enable serial port DMA transceiver */

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn; /* UART2 DMA1Tx*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn; /* UART2 DMA1Rx*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
}

/**
 * @brief  uart2 dma send channel configuration
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart2_dmatx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA1_Channel7);
    DMA_Cmd (DMA1_Channel7, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART2->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; /* Transfer Direction:Memory->Peripherals */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA1_Channel7, &DMA_InitStructure);
    DMA_ITConfig (DMA1_Channel7, DMA_IT_TC | DMA_IT_TE, ENABLE);
    DMA_ClearFlag (DMA1_IT_TC7); /* Clearing the delivery completion flag */
    /*Left disabled on purpose: uart2_dmatx_send() enables the channel when a
     *transfer is actually started.*/
    DMA_Cmd (DMA1_Channel7, DISABLE);
}

/**
 * @brief  uart2 dma send enable
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart2_dmatx_send (uint8_t *mem_addr, uint32_t mem_size) {
    DMA1_Channel7->CFGR &= ~0x00000001;
    DMA1_Channel7->CNTR = mem_size;
    DMA1_Channel7->MADDR = (uint32_t)mem_addr;
    DMA_ClearFlag (DMA1_FLAG_TC7);
    DMA1_Channel7->CFGR |= 0x00000001;
}

/**
 * @brief  uart2 dma receive channel configuration
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart2_dmarx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA1_Channel6);
    DMA_Cmd (DMA1_Channel6, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART2->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; /* Transfer direction:Peripheral->Memory */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA1_Channel6, &DMA_InitStructure);
    DMA_ITConfig (DMA1_Channel6, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE); /* Enable DMA half-full, overflow, and error interrupts */
    DMA_ClearFlag (DMA1_IT_TC6);
    DMA_ClearFlag (DMA1_IT_HT6);
    /*Circular RX channel, it has to run continuously from the start.*/
    DMA_Cmd (DMA1_Channel6, ENABLE);
}

uint16_t uart2_get_dmarx_buf_remain_size (uint8_t uart_id) {

    return DMA_GetCurrDataCounter (DMA1_Channel6);
}
#endif

#if UART3_CFG_EN
/*********************************************************************
 * @fn      uart3_init
 *
 * @brief   Initializes the USART3 peripheral.
 *
 * @return  none
 */
void uart3_init (uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1, ENABLE);

    /* USART3 TX-->B.10  RX-->B.11 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOB, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init (USART3, &USART_InitStructure);
    USART_ITConfig (USART3, USART_IT_IDLE, ENABLE);                   /* Enable serial port idle interrupt  */
    USART_Cmd (USART3, ENABLE);
    USART_DMACmd (USART3, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); /* Enable serial port DMA transceiver */

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn; /* UART3 DMA1Tx*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn; /* UART3 DMA1Rx*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
}

/**
 * @brief  uart3 dma send channel configuration
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart3_dmatx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA1_Channel2);
    DMA_Cmd (DMA1_Channel2, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART3->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; /* Transfer Direction:Memory->Peripherals */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA1_Channel2, &DMA_InitStructure);
    DMA_ITConfig (DMA1_Channel2, DMA_IT_TC | DMA_IT_TE, ENABLE);
    DMA_ClearFlag (DMA1_IT_TC2); /* Clearing the delivery completion flag */
    DMA_Cmd (DMA1_Channel2, DISABLE);
}
/**
 * @brief  uart3 dma send enable
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart3_dmatx_send (uint8_t *mem_addr, uint32_t mem_size) {
    DMA1_Channel2->CFGR &= ~0x00000001;
    DMA1_Channel2->CNTR = mem_size;
    DMA1_Channel2->MADDR = (uint32_t)mem_addr;
    DMA_ClearFlag (DMA1_FLAG_TC2);
    DMA1_Channel2->CFGR |= 0x00000001;
}

/**
 * @brief  uart3 dma receive channel configuration
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart3_dmarx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA1_Channel3);
    DMA_Cmd (DMA1_Channel3, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (USART3->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; /* Transfer direction:Peripheral->Memory */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA1_Channel3, &DMA_InitStructure);
    DMA_ITConfig (DMA1_Channel3, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE); /* Enable DMA half-full, overflow, and error interrupts */
    DMA_ClearFlag (DMA1_IT_TC3);
    DMA_ClearFlag (DMA1_IT_HT3);
    DMA_Cmd (DMA1_Channel3, ENABLE);
}


#endif


#if UART4_CFG_EN
/*********************************************************************
 * @fn      uart4_init
 *
 * @brief   Initializes the USART4 peripheral.
 *
 * @return  none
 */
void uart4_init (uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd (RCC_APB1Periph_UART4, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA2, ENABLE);


    /* USART4 TX-->C.10  RX-->C.11 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init (UART4, &USART_InitStructure);
    USART_ITConfig (UART4, USART_IT_IDLE, ENABLE);                   /* Enable serial port idle interrupt  */
    USART_Cmd (UART4, ENABLE);
    USART_DMACmd (UART4, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); /* Enable serial port DMA transceiver */

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel5_IRQn; /* UART4 DMA1Tx*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn; /* UART4 DMA1Rx*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
}

/**
 * @brief  uart4 dma send channel configuration
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart4_dmatx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA2_Channel5);
    DMA_Cmd (DMA2_Channel5, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (UART4->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; /* Transfer Direction:Memory->Peripherals */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA2_Channel5, &DMA_InitStructure);
    DMA_ITConfig (DMA2_Channel5, DMA_IT_TC | DMA_IT_TE, ENABLE);
    DMA_ClearFlag (DMA2_IT_TC5); /* Clearing the delivery completion flag */
    DMA_Cmd (DMA2_Channel5, DISABLE);
}

/**
 * @brief  uart4 dma send enable
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart4_dmatx_send (uint8_t *mem_addr, uint32_t mem_size) {
    DMA2_Channel5->CFGR &= ~0x00000001;
    DMA2_Channel5->CNTR = mem_size;
    DMA2_Channel5->MADDR = (uint32_t)mem_addr;
    DMA_ClearFlag (DMA2_FLAG_TC5);
    DMA2_Channel5->CFGR |= 0x00000001;
}
/**
 * @brief  uart4 dma receive channel configuration
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart4_dmarx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA2_Channel3);
    DMA_Cmd (DMA2_Channel3, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (UART4->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; /* Transfer direction:Peripheral->Memory */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA2_Channel3, &DMA_InitStructure);
    DMA_ITConfig (DMA2_Channel3, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE); /* Enable DMA half-full, overflow, and error interrupts */
    DMA_ClearFlag (DMA2_IT_TC3);
    DMA_ClearFlag (DMA2_IT_HT3);
    DMA_Cmd (DMA2_Channel3, ENABLE);
}

uint16_t uart4_get_dmarx_buf_remain_size (uint8_t uart_id) {

    return DMA_GetCurrDataCounter (DMA2_Channel3);
}
#endif

#if UART5_CFG_EN
/*********************************************************************
 * @fn      uart5_init
 *
 * @brief   Initializes the USART5 peripheral.
 *
 * @return  none
 */
void uart5_init (uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB1PeriphClockCmd (RCC_APB1Periph_UART5, ENABLE);
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA2, ENABLE);


    /* USART5 TX-->C.12  RX-->D.2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init (GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init (UART5, &USART_InitStructure);
    USART_ITConfig (UART5, USART_IT_IDLE, ENABLE);                   /* Enable serial port idle interrupt  */
    USART_Cmd (UART5, ENABLE);
    USART_DMACmd (UART5, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE); /* Enable serial port DMA transceiver */

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_IRQn; /* UART5 DMA1Tx*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel2_IRQn; /* UART5 DMA1Rx*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
}

/**
 * @brief  uart5 dma send channel configuration
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart5_dmatx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA2_Channel4);
    DMA_Cmd (DMA2_Channel4, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (UART5->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; /* Transfer Direction:Memory->Peripherals */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA2_Channel4, &DMA_InitStructure);
    DMA_ITConfig (DMA2_Channel4, DMA_IT_TC | DMA_IT_TE, ENABLE);
    DMA_ClearFlag (DMA2_IT_TC4); /* Clearing the delivery completion flag */
    DMA_Cmd (DMA2_Channel4, DISABLE);
}

/**
 * @brief  uart5 dma send enable
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart5_dmatx_send (uint8_t *mem_addr, uint32_t mem_size) {
    DMA2_Channel4->CFGR &= ~0x00000001;
    DMA2_Channel4->CNTR = mem_size;
    DMA2_Channel4->MADDR = (uint32_t)mem_addr;
    DMA_ClearFlag (DMA2_FLAG_TC4);
    DMA2_Channel4->CFGR |= 0x00000001;
}
/**
 * @brief  uart5 dma receive channel configuration
 * @param   mem_addr - pointer to the data buffer
 * @param   mem_size - number of bytes to transfer
 * @retval  none
 */
void uart5_dmarx_config (uint8_t *mem_addr, uint32_t mem_size) {
    DMA_InitTypeDef DMA_InitStructure = {0};

    DMA_DeInit (DMA2_Channel2);
    DMA_Cmd (DMA2_Channel2, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (UART5->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)mem_addr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; /* Transfer direction:Peripheral->Memory */
    DMA_InitStructure.DMA_BufferSize = mem_size;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init (DMA2_Channel2, &DMA_InitStructure);
    DMA_ITConfig (DMA2_Channel2, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE); /* Enable DMA half-full, overflow, and error interrupts */
    DMA_ClearFlag (DMA2_IT_TC2);
    DMA_ClearFlag (DMA2_IT_HT2);
    DMA_Cmd (DMA2_Channel2, ENABLE);
}

uint16_t uart5_get_dmarx_buf_remain_size (uint8_t uart_id) {

    return DMA_GetCurrDataCounter (DMA2_Channel2);
}
#endif

uint16_t uart_get_dmarx_buf_remain_size (uint8_t uart_id) {
    uint16_t counter = 0;
    switch (uart_id) {
    case DEV_UART1:
#if UART1_CFG_EN
        counter = DMA_GetCurrDataCounter (DMA1_Channel5);
#endif
        break;
    case DEV_UART2:
#if UART2_CFG_EN
        counter = DMA_GetCurrDataCounter (DMA1_Channel6);
#endif
        break;
    case DEV_UART3:
#if UART3_CFG_EN
        counter = DMA_GetCurrDataCounter (DMA1_Channel3);
#endif
        break;
    case DEV_UART4:
#if UART4_CFG_EN
        counter = DMA_GetCurrDataCounter (DMA2_Channel3);
#endif
        break;
    case DEV_UART5:
#if UART5_CFG_EN
        counter = DMA_GetCurrDataCounter (DMA2_Channel2);
#endif
        break;
    default:
        break;
    }
    return counter;
}

/**
 * @brief  UART DMA Rx completion ISR
 * @param   uart_id - UART device index (DEV_UARTx)
 * @retval  none
 */
void uart_dmarx_done_isr (uint8_t uart_id) {
    uint16_t recv_size;

    recv_size = s_uart_dev[uart_id].dmarx_buf_size - s_uart_dev[uart_id].last_dmarx_size;

    lwrb_write_ex (s_uart_dev[uart_id].rx_fifo,
                   &(s_uart_dev[uart_id].dmarx_buf[s_uart_dev[uart_id].last_dmarx_size]), recv_size, NULL, 0);

    s_uart_dev[uart_id].last_dmarx_size = 0;
}

/**
 * @brief  UART DMA HT (Half-Transfer) ISR
 * @param   uart_id - UART device index (DEV_UARTx)
 * @retval  none
 */
void uart_dmarx_half_done_isr (uint8_t uart_id) {
    uint16_t recv_total_size;
    uint16_t recv_size;
    lwrb_sz_t wsize = 0;

    recv_total_size = s_uart_dev[uart_id].dmarx_buf_size - uart_get_dmarx_buf_remain_size (uart_id);

    recv_size = recv_total_size - s_uart_dev[uart_id].last_dmarx_size;

    lwrb_write_ex (s_uart_dev[uart_id].rx_fifo,
                   &(s_uart_dev[uart_id].dmarx_buf[s_uart_dev[uart_id].last_dmarx_size]), recv_size, &wsize, 0);
    s_uart_dev[uart_id].last_dmarx_size = recv_total_size;
}

/**
 * @brief  UART IDLE ISR
 * @param   uart_id - UART device index (DEV_UARTx)
 * @retval  none
 */
void uart_dmarx_idle_isr (uint8_t uart_id) {
    uint16_t recv_total_size;
    uint16_t recv_size;
    lwrb_sz_t wsize = 0;

    recv_total_size = s_uart_dev[uart_id].dmarx_buf_size - uart_get_dmarx_buf_remain_size (uart_id);

    recv_size = recv_total_size - s_uart_dev[uart_id].last_dmarx_size;

    lwrb_write_ex (s_uart_dev[uart_id].rx_fifo,
                   &(s_uart_dev[uart_id].dmarx_buf[s_uart_dev[uart_id].last_dmarx_size]), recv_size, &wsize, 0);

    s_uart_dev[uart_id].last_dmarx_size = recv_total_size;
}

/**
 * @brief  UART DMA TC (Transfer Complete) ISR
 * @param   uart_id - UART device index (DEV_UARTx)
 * @retval  none
 */
void uart_dmatx_done_isr (uint8_t uart_id) {
    s_uart_dev[uart_id].status = 0;
}
