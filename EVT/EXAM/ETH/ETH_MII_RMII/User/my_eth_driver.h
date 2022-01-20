/********************************** (C) COPYRIGHT *******************************
* File Name          : my_eth_driver.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains the headers of the ETH Driver.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __MY_ETH_DRIVER__
#define __MY_ETH_DRIVER__

#include "debug.h"
#include "main.h"


#define ETH_RXBUFNB        6
#define ETH_TXBUFNB        2

#if 0
#define USE_LOOP_STRUCT  1
#else
#define USE_CHAIN_STRUCT  1
#endif

typedef struct
{
    u32 length;
    u32 buffer;
    ETH_DMADESCTypeDef *descriptor;
}FrameTypeDef;


FrameTypeDef ETH_RxPkt_ChainMode(void);
void mac_send(uint8_t * content_ptr, uint16_t content_len);
uint32_t ETH_Init(ETH_InitTypeDef* ETH_InitStruct, uint16_t PHYAddress);
void Ethernet_Configuration(void);
void PHY_control_pin_init(void);
void GETH_pin_init(void);
void FETH_pin_init(void);


#endif
