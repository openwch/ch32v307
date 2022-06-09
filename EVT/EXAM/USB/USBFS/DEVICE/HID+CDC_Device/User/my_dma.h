#ifndef __MY_DMA_H_
#define __MY_DMA_H_

#include "ch32v30x_rcc.h"

extern u16 rx_len;
extern u8 SendBuffer[64];
extern u8 ReceiveBuffer[64];
extern u16 BufferLen;
extern u16 Sendlen_backup;
extern u8 rx_flag;

void Usart_Rx_Config();
void myDMA_Enable( DMA_Channel_TypeDef*DMA_CHx );
void uart2_Send( u8 *buf, u16 len );
void copy_data( u8 *buf, u16 len );
#endif
