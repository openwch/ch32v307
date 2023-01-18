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

#include "net_config.h"
#include "string.h"
#include "debug.h"
#include "ch32v30x_usbhs_device.h"
#include "ch32v30x.h"

#define USE_10M_BASE                            1  // Internal 10M PHY
#define USE_MAC_MII                             2

#ifndef PHY_MODE
#define PHY_MODE                                USE_MAC_MII
#endif

#define Valid_link_established                  1
#define No_valid_link_established               0

#define PHY_ADDRESS                             1

#define ETH_DMARxDesc_FrameLengthShift          16

#define ROM_CFG_USERADR_ID                      0x1FFFF7E8

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

#define define_IPD(a,b) \
  GPIO_InitStructure.GPIO_Pin = b;\
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;\
  GPIO_Init(a, &GPIO_InitStructure)

#define DEF_NETSTAT_LINK_RDY       0x40 /* Valid link established */
#define DEF_NETSTAT_LINK_DIS       0x00 /* No valid link established */
#define DEF_NETSTAT_ANC_RDY        0x20 /* Auto negotiation process completed */
#define DEF_NETSTAT_ANC_DIS        0x00 /* Auto negotiation process not completed */
#define DEF_NETSTAT_100MBITS       0x80 /* Link Speed 100Mbits */
#define DEF_NETSTAT_10MBITS        0x00 /* Link Speed 10MBits */
#define DEF_NETSTAT_FULLDUPLEX     0x01 /* Link Duplex full-duplex */
#define DEF_NETSTAT_HALFDUPLEX     0x00 /* Link Duplex half-duplex */

#define QUERY_STAT_FLAG  ((LastQueryPhyTime == (LocalTime / 1000)) ? 0 : 1)


/* Ringbuffer define  */
#define DEF_U2E_REMINE                (2)/* usb to eth 停止传输的剩余队列个数 */
#define DEF_U2E_RESTART               (2)/* usb to eth 重新开始传输的队列个数 */
#define DEF_U2E_MAXBLOCKS             (ETH_TXBUFNB)/* usb to eth 最大队列深度 */
#define DEF_U2E_PACKHEADOFFSET        (4)/* usb to eth 头部包偏移，单位uint8_t */
#define DEF_U2E_PACKTAILOFFSET        (1532)/* usb to eth 尾部偏移，单位uint8_t */
#define DEF_E2U_REMINE                (1)/* eth to usb 停止传输的剩余队列个数 */
#define DEF_E2U_RESTART               (1)/* eth to usb 重新开始传输的队列个数 */
#define DEF_E2U_MAXBLOCKS             (ETH_RXBUFNB)/* eth to usb 最大队列深度 */
#define DEF_E2U_PACKLOADOFFSET        (4)/* usb to eth 头部包偏移，单位uint8_t */
#define DEF_U2E_PACKTAILOFFSET        (1532)/* usb to eth 尾部偏移，单位uint8_t */

/* Ring Buffer typedef */
typedef struct __attribute__((packed)) _RING_BUFF_COMM
{
    volatile uint32_t  LoadPtr;
    volatile uint32_t  DealPtr;
    volatile uint32_t  RemainPack;
    volatile uint32_t  StopFlag;
} RING_BUFF_COMM, pRING_BUFF_COMM;

extern __attribute__((__aligned__(4))) RING_BUFF_COMM U2E_Trance_Manage;
extern __attribute__((__aligned__(4))) uint32_t U2E_PackAdr[ ];
extern __attribute__((__aligned__(4))) uint32_t U2E_PackLen[ ];

extern __attribute__((__aligned__(4))) RING_BUFF_COMM E2U_Trance_Manage;
extern __attribute__((__aligned__(4))) uint32_t E2U_PackAdr[ ];
extern __attribute__((__aligned__(4))) uint32_t E2U_PackLen[ ];

extern __attribute__((__aligned__(4))) ETH_DMADESCTypeDef *DMARxDealTabs[ ];

extern uint32_t volatile U2E_PackCnounter;
extern uint32_t volatile E2U_PackCnounter;

extern volatile uint8_t loadlock;

extern ETH_DMADESCTypeDef *DMATxDescToSet;
extern ETH_DMADESCTypeDef *DMARxDescToGet;

extern uint8_t  ETH_NETWork_Status;
extern __attribute__((__aligned__(4))) uint8_t MACRxBuf[ ];     /* MAC receive buffer, 4-byte aligned */
extern __attribute__((__aligned__(4))) uint8_t MACTxBuf[ ];     /* MAC send buffer, 4-byte aligned */

void ETH_Query_AtuoNego( void );
void PHY_QueryPhySta( void );
void ETH_PHYLink( void );
void ETH_Isr( void );
void ETH_LedConfiguration(void);
void ETH_Init( uint8_t *macAddr );
void PHY_GetMacAddr( uint8_t *p );
void ETH_LedLinkSet( uint8_t mode );
void ETH_LedDataSet( uint8_t mode );
void ETH_Configuration( uint8_t *macAddr );
uint32_t ETH_TxPktChainMode(uint16_t len, uint32_t pBuff );

#endif
