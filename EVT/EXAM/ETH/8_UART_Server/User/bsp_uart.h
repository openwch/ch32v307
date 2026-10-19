/********************************** (C) COPYRIGHT *******************************
* File Name          : bsp_uart.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/01/16
* Description        : uart init
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef _BSP_UART_H_
#define _BSP_UART_H_

#include "ch32v30x.h"

/* Global define */

typedef enum { IDLE = 0, BUSY = !IDLE} Uart_TX_DMA_State;

#define ETH_RECEIVE_SIZE    			800
#define UART_TX_BUF_NUM     			5
#define UART_RX_DMA_SIZE    			(2*ETH_RECEIVE_SIZE)
#define BAUD_RATE  921600      			//BAUDRATE
#define DEBUG_BAUD_RATE  921600       	//DEBUG BAUDRATE

struct uart_data
{
	uint8_t (*TX_buffer)[ETH_RECEIVE_SIZE];   /* receive from eth */
	uint8_t *RX_buffer;                       /* receive from uart */
	uint16_t *TX_data_length;
	uint16_t last_RX_DMA_length;
	/* The four members below are written by the UART TX DMA completion
	 * interrupts (ch32v30x_it.c) and read by the main loop, so they are
	 * declared volatile: the ISR hands a finished TX buffer back to the main
	 * loop through them. */
	volatile uint8_t  tx_read;
	volatile uint8_t  tx_write;
    volatile uint8_t  tx_remainBuffNum;
	volatile Uart_TX_DMA_State  uart_tx_dma_state;    /* 0 -> idle, 1 -> busy */
};

extern struct uart_data  uart_data_t[8];


extern void BSP_Uart_Init(void);


#endif /* end of bsp_uart.h */
