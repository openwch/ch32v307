/********************************** (C) COPYRIGHT *******************************
* File Name          : bsp_uart.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/05/10
* Description        : uart cfg.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef _BSP_UART_H_
#define _BSP_UART_H_

#include "ch32v30x.h"
#include "net_config.h"

/* Global define */
#define BAUD_RATE  1000000

#define size(a)   (sizeof(a) / sizeof(*(a)))

#define MIN(X,Y)  ((X) < (Y) ? (X) : (Y))

#define UART_RX_DMA_SIZE    2048            //Must be a power of 2 ( a%2^n = a&(2^n - 1) )

#define ETH_RECEIVE_SIZE    RECE_BUF_LEN

#define UART_TX_BUF_NUM     3

typedef enum { IDLE = 0, BUSY = !IDLE} Uart_TX_DMA_State;

struct uart_data
{
    uint8_t (*TX_buffer)[ETH_RECEIVE_SIZE];   /* receive from eth */
    uint8_t *RX_buffer;                       /* receive from uart */
    uint16_t *TX_data_length;
    uint16_t RX_data_length;
    uint16_t last_RX_DMA_length;
    uint32_t rx_read;
    uint32_t rx_write;
    uint8_t  tx_read;
    uint8_t  tx_write;
    uint8_t  tx_remainBuffNum;
    Uart_TX_DMA_State  uart_tx_dma_state;    /* 0 -> idle, 1 -> busy */
};

extern struct uart_data  uart_data_t;

extern void BSP_Uart_Init(void);

#endif /* end of bsp_uart.h */

