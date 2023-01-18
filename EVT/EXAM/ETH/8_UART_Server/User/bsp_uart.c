/********************************** (C) COPYRIGHT *******************************
* File Name          : bsp_uart.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/16
* Description        : uart init.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "bsp_uart.h"

/* Global Variable */

struct uart_data  uart_data_t[8];

#ifndef CH32V307_DEBUG
/* UART1 */
uint8_t  UART1_RX_DMA_DataBuf[UART_RX_DMA_SIZE]= {0};
uint8_t  UART1_TX_DMA_DataBuf[UART_TX_BUF_NUM][ETH_RECEIVE_SIZE] = {0};
uint16_t UART1_TX_Buf_Length[UART_TX_BUF_NUM] = {0};
#endif

/* UART2 */
uint8_t  UART2_RX_DMA_DataBuf[UART_RX_DMA_SIZE]= {0};
uint8_t  UART2_TX_DMA_DataBuf[UART_TX_BUF_NUM][ETH_RECEIVE_SIZE] = {0};
uint16_t UART2_TX_Buf_Length[UART_TX_BUF_NUM] = {0};


/* UART3 */
uint8_t  UART3_RX_DMA_DataBuf[UART_RX_DMA_SIZE]= {0};
uint8_t  UART3_TX_DMA_DataBuf[UART_TX_BUF_NUM][ETH_RECEIVE_SIZE] = {0};
uint16_t UART3_TX_Buf_Length[UART_TX_BUF_NUM] = {0};

/* UART4 */
uint8_t  UART4_RX_DMA_DataBuf[UART_RX_DMA_SIZE]= {0};
uint8_t  UART4_TX_DMA_DataBuf[UART_TX_BUF_NUM][ETH_RECEIVE_SIZE] = {0};
uint16_t UART4_TX_Buf_Length[UART_TX_BUF_NUM] = {0};

/* UART5 */
uint8_t  UART5_RX_DMA_DataBuf[UART_RX_DMA_SIZE]= {0};
uint8_t  UART5_TX_DMA_DataBuf[UART_TX_BUF_NUM][ETH_RECEIVE_SIZE] = {0};
uint16_t UART5_TX_Buf_Length[UART_TX_BUF_NUM] = {0};

/* UART6 */
uint8_t  UART6_RX_DMA_DataBuf[UART_RX_DMA_SIZE]= {0};
uint8_t  UART6_TX_DMA_DataBuf[UART_TX_BUF_NUM][ETH_RECEIVE_SIZE] = {0};
uint16_t UART6_TX_Buf_Length[UART_TX_BUF_NUM] = {0};

/* UART7 */
uint8_t  UART7_RX_DMA_DataBuf[UART_RX_DMA_SIZE]= {0};
uint8_t  UART7_TX_DMA_DataBuf[UART_TX_BUF_NUM][ETH_RECEIVE_SIZE] = {0};
uint16_t UART7_TX_Buf_Length[UART_TX_BUF_NUM] = {0};

/* UART8 */
uint8_t  UART8_RX_DMA_DataBuf[UART_RX_DMA_SIZE]= {0};
uint8_t  UART8_TX_DMA_DataBuf[UART_TX_BUF_NUM][ETH_RECEIVE_SIZE] = {0};
uint16_t UART8_TX_Buf_Length[UART_TX_BUF_NUM] = {0};

/* Global typedef */
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/**********************************************************************
 *  @fn      UART_GPIO_Init
 *
 *  @brief   initialize uart1~uart8 io
 *
 *           UART1  TX-->PA9   RX-->PA10
 *           UART2  TX-->PA2   RX-->PA3
 *           UART3  TX-->PB10  RX-->PB11
 *           UART4  TX-->PC10  RX-->PC11
 *           UART5  TX-->PC12  RX-->PD2
 *           UART6  TX-->PC0   RX-->PC1   |  TX_2-->PE10   RX_2-->PE11
 *           UART7  TX-->PC2   RX-->PC3
 *           UART8  TX-->PC4   RX-->PC5
 *
 *  @param   none
 *
 *  @return  none
 * */
void UART_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB| \
    		               RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);

#ifndef CH32V307_DEBUG
    /* UART1 TX-->PA9   RX-->PA10 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

    /* UART2 TX-->PA2   RX-->PA3 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* UART3 TX-->PB10  RX-->PB11 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* UART4 TX-->PC10  RX-->PC11 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* UART5 TX-->PC12  RX-->PD2 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* UART6 TX-->PC0  RX-->PC1  |  TX_2-->PE10   RX_2-->PE11 */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_PinRemapConfig(GPIO_FullRemap_USART6,ENABLE);

	/* UART7 TX-->PC2  RX-->PC3 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* UART8 TX-->PC4  RX-->PC5 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/**********************************************************************
 *  @fn      UART_DMA_Init
 *
 *  @brief   initialize uart1~uart8 DMA
 *
 *           UART1  TX-->DMA1_Channel4   RX-->DMA1_Channel5
 *           UART2  TX-->DMA1_Channel7   RX-->DMA1_Channel6
 *           UART3  TX-->DMA1_Channel2   RX-->DMA1_Channel3
 *           UART4  TX-->DMA2_Channel5   RX-->DMA2_Channel3
 *           UART5  TX-->DMA2_Channel4   RX-->DMA2_Channel2
 *           UART6  TX-->DMA2_Channel6   RX-->DMA2_Channel7
 *           UART7  TX-->DMA2_Channel8   RX-->DMA2_Channel9
 *           UART8  TX-->DMA2_Channel10  RX-->DMA2_Channel11
 *
 *  @param   none
 *
 *  @return  none
 * */
void UART_DMA_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure={0};
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1|RCC_AHBPeriph_DMA2, ENABLE);

#ifndef CH32V307_DEBUG
	/* UART1  TX-->DMA1_Channel4   RX-->DMA1_Channel5 */
	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[0].TX_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	//DMA_InitStructure.DMA_BufferSize = size(TX_DMA_dataBuf);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);

	DMA_DeInit(DMA1_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[0].RX_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);

	DMA_ClearFlag(DMA1_IT_TC4);

	DMA_Cmd(DMA1_Channel5, ENABLE);
	USART_DMACmd(USART1,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);
#endif

	/* UART2  TX-->DMA1_Channel7   RX-->DMA1_Channel6 */
	DMA_DeInit(DMA1_Channel7);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[1].TX_buffer;
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
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[1].RX_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);

	DMA_ClearFlag(DMA1_IT_TC7);

	DMA_Cmd(DMA1_Channel6, ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);

	/*UART3  TX-->DMA1_Channel2   RX-->DMA1_Channel3*/
	DMA_DeInit(DMA1_Channel2);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[2].TX_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//DMA_InitStructure.DMA_BufferSize = (uint32_t)uart_data_t[2].RX_buffer_size;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);

	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[2].RX_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);

	DMA_ClearFlag(DMA1_IT_TC2);

	DMA_Cmd(DMA1_Channel3, ENABLE);
	USART_DMACmd(USART3,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);

	/* UART4  TX-->DMA2_Channel5   RX-->DMA2_Channel3 */
	DMA_DeInit(DMA2_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART4->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[3].TX_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //DMA_InitStructure.DMA_BufferSize = (uint32_t)uart_data_t[3].RX_buffer_size;
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);

    DMA_DeInit(DMA2_Channel3);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART4->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[3].RX_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init(DMA2_Channel3, &DMA_InitStructure);

    DMA_ClearFlag(DMA2_IT_TC3);

    DMA_Cmd(DMA2_Channel3, ENABLE);
    USART_DMACmd(UART4,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);


    /* UART5  TX-->DMA2_Channel4   RX-->DMA2_Channel2 */
	DMA_DeInit(DMA2_Channel4);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART5->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[4].TX_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //DMA_InitStructure.DMA_BufferSize = (uint32_t)uart_data_t[4].RX_buffer_size;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);

    DMA_DeInit(DMA2_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART5->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[4].RX_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init(DMA2_Channel2, &DMA_InitStructure);

    DMA_ClearFlag(DMA2_IT_TC2);

    DMA_Cmd(DMA2_Channel2, ENABLE);
    USART_DMACmd(UART5,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);

	/* UART6  TX-->DMA2_Channel6   RX-->DMA2_Channel7 */
	DMA_DeInit(DMA2_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART6->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[5].TX_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //DMA_InitStructure.DMA_BufferSize = (uint32_t)uart_data_t[5].RX_buffer_size;
    DMA_Init(DMA2_Channel6, &DMA_InitStructure);

    DMA_DeInit(DMA2_Channel7);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART6->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[5].RX_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init(DMA2_Channel7, &DMA_InitStructure);

    DMA_ClearFlag(DMA2_IT_TC7);

    DMA_Cmd(DMA2_Channel7, ENABLE);
    USART_DMACmd(UART6,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);

    /* UART7  TX-->DMA2_Channel8   RX-->DMA2_Channel9 */
    DMA_DeInit(DMA2_Channel8);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART7->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[6].TX_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//DMA_InitStructure.DMA_BufferSize = (uint32_t)uart_data_t[6].RX_buffer_size;
	DMA_Init(DMA2_Channel8, &DMA_InitStructure);

	DMA_DeInit(DMA2_Channel9);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART7->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[6].RX_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_Init(DMA2_Channel9, &DMA_InitStructure);

	DMA_ClearFlag(DMA2_IT_TC8);

	DMA_Cmd(DMA2_Channel9, ENABLE);
	USART_DMACmd(UART7,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);

    /* UART8  TX-->DMA2_Channel10  RX-->DMA2_Channel11 */
	DMA_DeInit(DMA2_Channel10);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART8->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[7].TX_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	//DMA_InitStructure.DMA_BufferSize = (uint32_t)uart_data_t[7].RX_buffer_size;
	DMA_Init(DMA2_Channel10, &DMA_InitStructure);

	DMA_DeInit(DMA2_Channel11);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&UART8->DATAR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_data_t[7].RX_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_DMA_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_Init(DMA2_Channel11, &DMA_InitStructure);

	DMA_ClearFlag(DMA2_IT_TC10);


	DMA_Cmd(DMA2_Channel11, ENABLE);
	USART_DMACmd(UART8,USART_DMAReq_Rx|USART_DMAReq_Tx,ENABLE);
}


/**********************************************************************
 *  @fn      UART_Interrupt_Init
 *
 *  @brief   initialize uart1~uart8 interrupt
 *
 *           TX -- enable  DMA_IT_TC
 *           RX -- enable  DMA_IT_HT
 *           enable  USARTx_IRQn-->USART_IT_IDLE
 *
 *  @param   none
 *
 *  @return  none
 * */
void UART_Interrupt_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure={0};

#ifndef CH32V307_DEBUG
	/* UART1  TX-->DMA1_Channel4   RX-->DMA1_Channel5 */
	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
	//DMA_ITConfig(DMA1_Channel5,DMA_IT_HT,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
//	NVIC_Init(&NVIC_InitStructure);
//
//	/* enable uart1 idle interrupt */
//	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_Init(&NVIC_InitStructure);

#endif

	/* UART2  TX-->DMA1_Channel7   RX-->DMA1_Channel6 */
	DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);
	//DMA_ITConfig(DMA1_Channel6,DMA_IT_HT,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
//	NVIC_Init(&NVIC_InitStructure);
//
//	/* enable uart2 idle interrupt */
//	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	NVIC_Init(&NVIC_InitStructure);


	/*UART3  TX-->DMA1_Channel2   RX-->DMA1_Channel3*/
	DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);
//	DMA_ITConfig(DMA1_Channel3,DMA_IT_HT,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
//	NVIC_Init(&NVIC_InitStructure);
//
//	/* enable uart3 idle interrupt */
//	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//	NVIC_Init(&NVIC_InitStructure);

	/* UART4  TX-->DMA2_Channel5   RX-->DMA2_Channel3 */
    DMA_ITConfig(DMA2_Channel5,DMA_IT_TC,ENABLE);
//    DMA_ITConfig(DMA2_Channel3,DMA_IT_HT,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel5_IRQn;
    NVIC_Init(&NVIC_InitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;
//    NVIC_Init(&NVIC_InitStructure);
//
//    /* enable uart4 idle interrupt */
//    USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);
//    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
//    NVIC_Init(&NVIC_InitStructure);

    /* UART5  TX-->DMA2_Channel4   RX-->DMA2_Channel2 */
    DMA_ITConfig(DMA2_Channel4,DMA_IT_TC,ENABLE);
//    DMA_ITConfig(DMA2_Channel2,DMA_IT_HT,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_IRQn;
    NVIC_Init(&NVIC_InitStructure);

//    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel2_IRQn;
//	NVIC_Init(&NVIC_InitStructure);
//
//    /* enable uart5 idle interrupt */
//    USART_ITConfig(UART5,USART_IT_IDLE,ENABLE);
//    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
//    NVIC_Init(&NVIC_InitStructure);

    /* UART6  TX-->DMA2_Channel6   RX-->DMA2_Channel7 */
    DMA_ITConfig(DMA2_Channel6,DMA_IT_TC,ENABLE);
//    DMA_ITConfig(DMA2_Channel7,DMA_IT_HT,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel6_IRQn;
    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel7_IRQn;
//    NVIC_Init(&NVIC_InitStructure);
//
//
//    /* enable uart6 idle interrupt */
//    USART_ITConfig(UART6,USART_IT_IDLE,ENABLE);
//    NVIC_InitStructure.NVIC_IRQChannel = UART6_IRQn;
//    NVIC_Init(&NVIC_InitStructure);


    /* UART7  TX-->DMA2_Channel8   RX-->DMA2_Channel9 */
    DMA_ITConfig(DMA2_Channel8,DMA_IT_TC,ENABLE);
//    DMA_ITConfig(DMA2_Channel9,DMA_IT_HT,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel8_IRQn;
    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel9_IRQn;
//    NVIC_Init(&NVIC_InitStructure);
//
//
//    /* enable uart7 idle interrupt */
//    USART_ITConfig(UART7,USART_IT_IDLE,ENABLE);
//    NVIC_InitStructure.NVIC_IRQChannel = UART7_IRQn;
//    NVIC_Init(&NVIC_InitStructure);


    /* UART8  TX-->DMA2_Channel10  RX-->DMA2_Channel11 */
    DMA_ITConfig(DMA2_Channel10,DMA_IT_TC,ENABLE);
//    DMA_ITConfig(DMA2_Channel11,DMA_IT_HT,ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel10_IRQn;
    NVIC_Init(&NVIC_InitStructure);
//    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel11_IRQn;
//    NVIC_Init(&NVIC_InitStructure);
//
//
//    /* enable uart8 idle interrupt */
//    USART_ITConfig(UART8,USART_IT_IDLE,ENABLE);
//    NVIC_InitStructure.NVIC_IRQChannel = UART8_IRQn;
//    NVIC_Init(&NVIC_InitStructure);

}

/**********************************************************************
 *  @fn      BSP_Uart_Init
 *
 *  @brief   initialize uart1~uart8
 *
 *  @param   none
 *
 *  @return  none
 * */
void BSP_Uart_Init(void)
{
	UART_GPIO_Init();
	USART_InitTypeDef USART_InitStructure={0};

#ifndef CH32V307_DEBUG
	/* USART1 TX-->PA9   RX-->PA10 */
	uart_data_t[0].RX_buffer = &UART1_RX_DMA_DataBuf[0];
	uart_data_t[0].TX_buffer = &UART1_TX_DMA_DataBuf[0];
	uart_data_t[0].TX_data_length = &UART1_TX_Buf_Length[0];
	uart_data_t[0].rx_read = 0;
	uart_data_t[0].rx_write = 0;
	uart_data_t[0].last_RX_DMA_length = UART_RX_DMA_SIZE;
	uart_data_t[0].RX_data_length = 0;
	uart_data_t[0].tx_read = 0;
	uart_data_t[0].rx_write = 0;
	uart_data_t[0].uart_tx_dma_state = IDEL;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);



	USART_InitStructure.USART_BaudRate = BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
#endif

	/* USART2 TX-->PA2   RX-->PA3 */
	uart_data_t[1].RX_buffer = &UART2_RX_DMA_DataBuf[0];
	uart_data_t[1].TX_buffer = &UART2_TX_DMA_DataBuf[0];
	uart_data_t[1].TX_data_length = &UART2_TX_Buf_Length[0];
	uart_data_t[1].rx_read = 0;
	uart_data_t[1].rx_write = 0;
	uart_data_t[1].last_RX_DMA_length = UART_RX_DMA_SIZE;
	uart_data_t[1].RX_data_length = 0;
	uart_data_t[1].tx_read = 0;
	uart_data_t[1].rx_write = 0;
	uart_data_t[1].uart_tx_dma_state = IDEL;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_InitStructure.USART_BaudRate = BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);

	/* USART3 TX-->PB10  RX-->PB11 */
	uart_data_t[2].RX_buffer = &UART3_RX_DMA_DataBuf[0];
	uart_data_t[2].TX_buffer = &UART3_TX_DMA_DataBuf[0];
	uart_data_t[2].TX_data_length = &UART3_TX_Buf_Length[0];
	uart_data_t[2].rx_read = 0;
	uart_data_t[2].rx_write = 0;
	uart_data_t[2].last_RX_DMA_length = UART_RX_DMA_SIZE;
	uart_data_t[2].RX_data_length = 0;
	uart_data_t[2].tx_read = 0;
	uart_data_t[2].rx_write = 0;
	uart_data_t[2].uart_tx_dma_state = IDEL;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	USART_Init(USART3, &USART_InitStructure);
	USART_Cmd(USART3, ENABLE);


	/* USART4 TX-->PC10  RX-->PC11 */
	uart_data_t[3].RX_buffer = &UART4_RX_DMA_DataBuf[0];
	uart_data_t[3].TX_buffer = &UART4_TX_DMA_DataBuf[0];
	uart_data_t[3].TX_data_length = &UART4_TX_Buf_Length[0];
	uart_data_t[3].rx_read = 0;
	uart_data_t[3].rx_write = 0;
	uart_data_t[3].last_RX_DMA_length = UART_RX_DMA_SIZE;
	uart_data_t[3].RX_data_length = 0;
	uart_data_t[3].tx_read = 0;
	uart_data_t[3].rx_write = 0;
	uart_data_t[3].uart_tx_dma_state = IDEL;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    USART_Init(UART4, &USART_InitStructure);
    USART_Cmd(UART4, ENABLE);

	/* USART5 TX-->PC12  RX-->PD2 */
	uart_data_t[4].RX_buffer = &UART5_RX_DMA_DataBuf[0];
	uart_data_t[4].TX_buffer = &UART5_TX_DMA_DataBuf[0];
	uart_data_t[4].TX_data_length = &UART5_TX_Buf_Length[0];
	uart_data_t[4].rx_read = 0;
	uart_data_t[4].rx_write = 0;
	uart_data_t[4].last_RX_DMA_length = UART_RX_DMA_SIZE;
	uart_data_t[4].RX_data_length = 0;
	uart_data_t[4].tx_read = 0;
	uart_data_t[4].rx_write = 0;
	uart_data_t[4].uart_tx_dma_state = IDEL;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

    USART_Init(UART5, &USART_InitStructure);
    USART_Cmd(UART5, ENABLE);

	/* USART6 TX-->PC0  RX-->PC1 */
	uart_data_t[5].RX_buffer = &UART6_RX_DMA_DataBuf[0];
	uart_data_t[5].TX_buffer = &UART6_TX_DMA_DataBuf[0];
	uart_data_t[5].TX_data_length = &UART6_TX_Buf_Length[0];
	uart_data_t[5].rx_read = 0;
	uart_data_t[5].rx_write = 0;
	uart_data_t[5].last_RX_DMA_length = UART_RX_DMA_SIZE;
	uart_data_t[5].RX_data_length = 0;
	uart_data_t[5].tx_read = 0;
	uart_data_t[5].rx_write = 0;
	uart_data_t[5].uart_tx_dma_state = IDEL;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART6, ENABLE);

    USART_Init(UART6, &USART_InitStructure);
    USART_Cmd(UART6, ENABLE);

	/* UART7  TX-->PC2   RX-->PC3 */
	uart_data_t[6].RX_buffer = &UART7_RX_DMA_DataBuf[0];
	uart_data_t[6].TX_buffer = &UART7_TX_DMA_DataBuf[0];
	uart_data_t[6].TX_data_length = &UART7_TX_Buf_Length[0];
	uart_data_t[6].rx_read = 0;
	uart_data_t[6].rx_write = 0;
	uart_data_t[6].last_RX_DMA_length = UART_RX_DMA_SIZE;
	uart_data_t[6].RX_data_length = 0;
	uart_data_t[6].tx_read = 0;
	uart_data_t[6].rx_write = 0;
	uart_data_t[6].uart_tx_dma_state = IDEL;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);

    USART_Init(UART7, &USART_InitStructure);
    USART_Cmd(UART7, ENABLE);


	/* UART8  TX-->PC4   RX-->PC5 */
	uart_data_t[7].RX_buffer = &UART8_RX_DMA_DataBuf[0];
	uart_data_t[7].TX_buffer = &UART8_TX_DMA_DataBuf[0];
	uart_data_t[7].TX_data_length = &UART8_TX_Buf_Length[0];
	uart_data_t[7].rx_read = 0;
	uart_data_t[7].rx_write = 0;
	uart_data_t[7].last_RX_DMA_length = UART_RX_DMA_SIZE;
	uart_data_t[7].RX_data_length = 0;
	uart_data_t[7].tx_read = 0;
	uart_data_t[7].rx_write = 0;
	uart_data_t[7].uart_tx_dma_state = IDEL;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE);

    USART_Init(UART8, &USART_InitStructure);
    USART_Cmd(UART8, ENABLE);


	UART_DMA_Init();
	UART_Interrupt_Init();
}








