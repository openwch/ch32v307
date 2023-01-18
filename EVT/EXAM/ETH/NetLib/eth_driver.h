/********************************** (C) COPYRIGHT *******************************
* File Name          : eth_driver.h
* Author             : WCH
* Version            : V1.3.0
* Date               : 2022/06/02
* Description        : This file contains the headers of the ETH Driver.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __ETH_DRIVER__
#define __ETH_DRIVER__

#ifdef __cplusplus
 extern "C" {
#endif 

#include "debug.h"
#include "wchnet.h"

#define USE_10M_BASE                            1  /* Internal 10M PHY */
#define USE_MAC_MII                             2

#ifndef PHY_MODE
#define PHY_MODE                                USE_10M_BASE
#endif

#define PHY_ADDRESS                             1

#define ETH_DMARxDesc_FrameLengthShift          16

#define ROM_CFG_USERADR_ID                      0x1FFFF7E8

#define PHY_LINK_TASK_PERIOD                    50

#define PHY_ANLPAR_SELECTOR_FIELD               0x1F
#define PHY_ANLPAR_SELECTOR_VALUE               0x01       /* 5B'00001 */

#define PHY_LINK_INIT                           0x00
#define PHY_LINK_SUC_P                          (1<<0)
#define PHY_LINK_SUC_N                          (1<<1)
#define PHY_LINK_WAIT_SUC                       (1<<7)

#define PHY_PN_SWITCH_P                         (0<<2)
#define PHY_PN_SWITCH_N                         (1<<2)
#define PHY_PN_SWITCH_AUTO                      (2<<2)

#ifndef WCHNETTIMERPERIOD
#define WCHNETTIMERPERIOD                       10   /* Timer period, in Ms. */
#endif

#define ETH_DMA_RX_ERR   (  ETH_DMA_IT_FBE |\
                            ETH_DMA_IT_ER |\
                            ETH_DMA_IT_ET |\
                            ETH_DMA_IT_RO )

#define ETH_DMA_TX_ERR   (  ETH_DMA_IT_TJT |\
                            ETH_DMA_IT_TBU |\
                            ETH_DMA_IT_TPS )

#define ETH_DMA_INT      (  ETH_DMA_IT_PHYLINK  |\
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

#define QUERY_STAT_FLAG  ((LastQueryPhyTime == (LocalTime / 1000)) ? 0 : 1)

#define GET_CHIP_VER()   ((*((volatile uint32_t *)0x1FFFF704) & 0xF0) >> 4)
#define CHIP_C_VER_NUM   2

#define PHY_PN_SWITCH(PNMode)   do{\
    RegVal = ETH_ReadPHYRegister(gPHYAddress, PHY_BCR);\
    RegVal |= PHY_Restart_AutoNegotiation;\
    ETH_WritePHYRegister( gPHYAddress, PHY_BCR, RegVal);\
    phyPN = PNMode;\
    ETH_WritePHYRegister(gPHYAddress, PHY_MDIX, phyPN);\
}while(0)

#define PHY_NEGOTIATION_PARAM_INIT()      do{\
    phyRetryCnt = 0;\
    phyStatus = 0;\
    phySucCnt = 0;\
    phyLinkCnt = 0;\
    phyLinkStatus = PHY_LINK_INIT;\
    phyPN = PHY_PN_SWITCH_AUTO;\
    ETH_WritePHYRegister(gPHYAddress, PHY_MDIX, phyPN);\
}while(0)

#define PHY_RESTART_NEGOTIATION()       do{\
    if(phyRetryCnt++ == (5000 / PHY_LINK_TASK_PERIOD))\
    {\
        phyRetryCnt = 0;\
        ETH_WritePHYRegister(gPHYAddress, PHY_BCR, PHY_Reset);\
        ETH_WritePHYRegister(gPHYAddress, PHY_MDIX, PHY_PN_SWITCH_AUTO);\
    }\
}while(0)

extern ETH_DMADESCTypeDef *DMATxDescToSet;
extern ETH_DMADESCTypeDef *DMARxDescToGet;
extern SOCK_INF SocketInf[ ];

void ETH_PHYLink( void );
void WCHNET_ETHIsr( void );
void WCHNET_MainTask( void );
void ETH_LedConfiguration(void);
void ETH_Init( uint8_t *macAddr );
void ETH_LedLinkSet( uint8_t mode );
void ETH_LedDataSet( uint8_t mode );
void WCHNET_TimeIsr( uint16_t timperiod );
void ETH_Configuration( uint8_t *macAddr );
uint8_t ETH_LibInit( uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *macaddr);

#ifdef __cplusplus
}
#endif

#endif
