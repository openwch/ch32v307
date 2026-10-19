/*
 * app_config.c
 *
 *  Created on: Apr 28, 2025
 *      Author: TECH66
 */
#include "app_config.h"
#include "app_iochub.h"

lwrb_t pRbUartToIoCHub[COMNUM];
static uint8_t s_uart_tx_buf[UART_TX_BUF_SIZE];
static uint8_t s_uart_rx_buf[COMNUM][UART_RX_BUF_SIZE];
static uint8_t s_uart_dmarx_buf[COMNUM][UART_DMA_RX_BUF_SIZE];
static uint8_t s_uart_dmatx_buf[COMNUM][UART_DMA_TX_BUF_SIZE];

uart_device_t s_uart_dev[COMNUM] = {0};

extern void uart3_dmatx_send(uint8_t *mem_addr, uint32_t mem_size);
void cfg_ringbufInit (void);

void uart_Init(void)
{
    uart3_init(DEFAULT_BAUDRATE_3);
    cfg_ringbufInit ();
}

void cfg_ringbufInit (void) {
    lwrb_init (&pRbUartToIoCHub[0], s_uart_rx_buf[0], UART_RX_BUF_SIZE);
    pRbUartToIoCHub[0].lock_fn = NULL;
    pRbUartToIoCHub[0].unlock_fn = NULL;
    s_uart_dev[0].tx_fifo = NULL;
    s_uart_dev[0].rx_fifo = &pRbUartToIoCHub[0];
    s_uart_dev[0].dmarx_buf = s_uart_dmarx_buf[0];
    s_uart_dev[0].dmarx_buf_size = UART_DMA_RX_BUF_SIZE;
    s_uart_dev[0].dmatx_buf = s_uart_dmatx_buf[0];
    s_uart_dev[0].dmatx_buf_size = UART_DMA_TX_BUF_SIZE;
    uart3_dmarx_config(s_uart_dev[0].dmarx_buf,UART_DMA_RX_BUF_SIZE);
    uart3_dmatx_config(s_uart_dev[0].dmatx_buf,UART_DMA_TX_BUF_SIZE);
    s_uart_dev[0].status  = 0;
}


void IoCHubToComTransData (uint8_t pdev, uint8_t com) {
    uint16_t len = s_uart_dev[com].dmatx_buf_size;

    if (0x01 == s_uart_dev[com].status) {
        return;
    }
    if (!WCHIOCHUB_DataRecv (pdev, s_uart_dev[com].dmatx_buf, &len)) {
        if (len != 0) {
            s_uart_dev[com].status = 0x01; 
            uart3_dmatx_send (s_uart_dev[com].dmatx_buf, len);
        }
    }
}

void ComToIoCHubTransData (uint8_t pdev, uint8_t com) {
    lwrb_t *prbuf = s_uart_dev[com].rx_fifo;
    uint16_t peeklen = 0;
    uint8_t* buff=s_uart_tx_buf;

    peeklen = lwrb_peek (prbuf, 0, buff, UART_TX_BUF_SIZE);
    if (peeklen) {
        /*WCHIOCHUB_DataSend writes back the length it really accepted, so only
         *that part is skipped and nothing is dropped on a short send.*/
        WCHIOCHUB_DataSend (pdev, buff, &peeklen);
        lwrb_skip (prbuf, peeklen);
    }
}



