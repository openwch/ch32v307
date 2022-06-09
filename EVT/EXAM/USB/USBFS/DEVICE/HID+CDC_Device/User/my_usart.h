#ifndef __MY_USART_H_
#define __MY_USART_H_
#include "ch32v30x_rcc.h"
#include "ch32v30x_usbotg_device.h"
#include "my_dma.h"
#include "ch32v30x_usart.h"

extern int8_t t_flag;


void USART2_CFG(__uint32_t USART_BaudRate,int8_t USART_WordLength,int8_t USART_StopBits,int8_t USART_Parity);


#endif
