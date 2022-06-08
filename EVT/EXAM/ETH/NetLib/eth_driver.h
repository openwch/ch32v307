/********************************** (C) COPYRIGHT *******************************
* File Name          : eth_driver.h
* Author             : WCH
* Version            : V1.3.0
* Date               : 2022/06/02
* Description        : This file contains the headers of the ETH Driver.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __ETH_DRIVER__
#define __ETH_DRIVER__

#include "debug.h"
#include "WCHNET.h"


#define USE_10M_BASE                         1      // Internal 10M PHY
#define USE_MAC_MII                          2

#ifndef PHY_MODE
#define PHY_MODE                             USE_10M_BASE
#endif

#define PHY_ADDRESS                          1

#define ETH_DMARxDesc_FrameLengthShift       16

#define ROM_CFG_USERADR_ID                   0x1FFFF7E8

#ifndef WCHNETTIMERPERIOD
#define WCHNETTIMERPERIOD             10              //Timer period, in Ms.
#endif

#define ETH_DMA_RX_ERR   (  ETH_DMA_IT_FBE |\
  ETH_DMA_IT_ER |\
  ETH_DMA_IT_ET |\
  ETH_DMA_IT_RO )

#define ETH_DMA_TX_ERR   (   ETH_DMA_IT_TJT |\
  ETH_DMA_IT_TBU |\
  ETH_DMA_IT_TPS )


#define  ETH_DMA_INT     ( ETH_DMA_IT_PHYLINK  |\
  ETH_DMA_IT_AIS |\
  ETH_DMA_IT_R |\
  ETH_DMA_IT_T |\
  0|\
  ETH_DMA_RX_ERR|\
  ETH_DMA_TX_ERR )

#define define_O(a,b) \
  GPIO_InitStructure.GPIO_Pin = b;\
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;\
  GPIO_Init(a, &GPIO_InitStructure)

#define define_I(a,b) \
  GPIO_InitStructure.GPIO_Pin = b;\
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;\
  GPIO_Init(a, &GPIO_InitStructure)

extern ETH_DMADESCTypeDef *DMATxDescToSet;
extern ETH_DMADESCTypeDef *DMARxDescToGet;

extern SOCK_INF SocketInf[ ];

void ETH_Init( uint8_t *macAddr );
void ETH_PHYLink( void );
void ETH_Configuration( uint8_t *macAddr );
void ETH_LedConfiguration(void);
void ETH_LedLinkSet( uint8_t mode );
void ETH_LedDataSet( uint8_t mode );
uint8_t ETH_LibInit( uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *macaddr);

void WCHNET_MainTask( void );
void WCHNET_ETHIsr( void );
void WCHNET_TimeIsr( uint16_t timperiod );

#endif
