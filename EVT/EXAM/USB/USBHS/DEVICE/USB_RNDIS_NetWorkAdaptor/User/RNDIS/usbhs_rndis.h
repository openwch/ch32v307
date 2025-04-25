/********************************** (C) COPYRIGHT *******************************
* File Name          : usbhs_rndis.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/07/31
* Description        : usbhs_rndis program body.
*********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


#ifndef __USBHS_RNDIS_H_
#define __USBHS_RNDIS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "eth_driver.h"
#include "rndis_protocol.h"
#include "ch32v30x_eth.h"
#include "ch32v30x_usbhs_device.h"

#define RNDIS_MTU                   1500                           /* MTU value */
#define RNDIS_LINK_SPEED            100000000                      /* Link baudrate (100Mbit/s for CH32V307-PHY) */
#define RNDID_LINKSPD_1000M         1000000000
#define RNDID_LINKSPD_100M          100000000
#define RNDID_LINKSPD_10M           10000000
#define RNDIS_VENDOR                "WCH"                          /* NIC vendor name */

#define RNDIS_NOTIFICATION_IN_EP    0x81
#define RNDIS_DATA_IN_EP            0x82
#define RNDIS_DATA_OUT_EP           0x03

#define RNDIS_DATA_IN_SZ            DEF_USBD_HS_PACK_SIZE
#define RNDIS_DATA_OUT_SZ           DEF_USBD_HS_PACK_SIZE


/******************************************************************************/
/* NET Status Definition */
#define DEF_NETSTAT_LINK_RDY            0x40 /* Valid link established */
#define DEF_NETSTAT_LINK_DIS            0x00 /* No valid link established */
#define DEF_NETSTAT_ANC_RDY             0x20 /* Auto negotiation process completed */
#define DEF_NETSTAT_ANC_DIS             0x00 /* Auto negotiation process not completed */
#define DEF_NETSTAT_1000MBITS           0x10 /* Link Speed 1000Mbits, when this bit is 1, ignore DEF_NETSTAT_100MBITS/DEF_NETSTAT_10MBITS bit definition */
#define DEF_NETSTAT_100MBITS            0x80 /* Link Speed 100Mbits */
#define DEF_NETSTAT_10MBITS             0x00 /* Link Speed 10MBits */
#define DEF_NETSTAT_FULLDUPLEX          0x01 /* Link Duplex full-duplex */
#define DEF_NETSTAT_HALFDUPLEX          0x00 /* Link Duplex half-duplex */

/******************************************************************************/
/* Phy Operation Definition */
#define DEF_PHY_QUERY_TIMEOUT           20   /* Query Phy Status every 200ms, in 10ms */

/*********************************************************************
 * MAC queue configuration
 */
#define DEF_ETH_RX_USBPACK            3160
#define DEF_ETH_TX_USBPACK            1580
#define DEF_ETH_USBPACK               1536

/* Ringbuffer define  */
#define DEF_U2E_REMINE                 (2)/* usb to eth 停止传输的剩余队列个数 */
#define DEF_U2E_RESTART                (2)/* usb to eth 重新开始传输的队列个数 */
#define DEF_U2E_MAXBLOCKS              (ETH_TXBUFNB)/* usb to eth 最大队列深度 */
#define DEF_U2E_PACKHEADOFFSET         (4)/* usb to eth 头部包偏移，单位uint8_t */
#define DEF_U2E_PACKTAILOFFSET         (1532)/* usb to eth 尾部偏移，单位uint8_t */
#define DEF_E2U_REMINE                 (8)/* eth to usb 停止传输的剩余队列个数 */
#define DEF_E2U_RESTART                (2)/* eth to usb 重新开始传输的队列个数 */
#define DEF_E2U_MAXBLOCKS              (ETH_RXBUFNB)/* eth to usb 最大队列深度 */
#define DEF_E2U_PACKLOADOFFSET         (4)/* usb to eth 头部包偏移，单位uint8_t */
#define DEF_U2E_PACKTAILOFFSET         (1532)/* usb to eth 尾部偏移，单位uint8_t */

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
extern __attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMARxDscrTab[ ];       /* MAC receive descriptor, 4-byte aligned*/
extern __attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMATxDscrTab[ ];       /* MAC send descriptor, 4-byte aligned */
extern __attribute__((__aligned__(4))) uint8_t  MACRxBuf[ ];                     /* MAC receive buffer, 4-byte aligned */
extern __attribute__((__aligned__(4))) uint8_t  MACTxBuf[ ];                     /* MAC send buffer, 4-byte aligned */


extern rndis_state_t rndis_state;
extern uint8_t encapsulated_buffer[ ];
extern uint8_t RNDIS_RX_Buffer[ ];
extern uint8_t RNDIS_TX_Buffer[ ];
extern uint8_t RNDIS_EP1_Buffer[ ];
extern uint8_t MACAddr[ 6 ];

extern volatile uint8_t  RNDIS_Spd_ChangeReset;
extern volatile uint8_t  USBHS_UsbRxFlag;
extern volatile uint8_t  ECM_Pack_Filter;
extern volatile uint8_t  ETH_NETWork_Status;
extern volatile uint8_t  ETH_LastStatus;
extern volatile uint16_t USBHS_UsbRxLen;
extern volatile uint32_t U2E_PackCnounter;
extern volatile uint32_t E2U_PackCnounter;
extern uint8_t  PhyInit_Flag;

extern void USB2ETH_Trance( void );
extern void ETH2USB_Trance( void );
extern void RNDIS_Load_Status( void );
extern uint8_t ETH2USB_DataSend(uint16_t len, uint32_t *pBuff );
extern void RNDIS_MSG_Recv( void );
extern void ETH_GetMacAddr( uint8_t *p );
extern void ETH_DriverInit( uint8_t *addr );
extern void ETH_PhyAbility_Set( void );
extern void USBETH_Main( void );

/* extern from eth_driver.c */
extern volatile uint8_t LinkSta;
extern uint32_t LastQueryPhyTime;
extern ETH_DMADESCTypeDef *pDMARxSet;
extern ETH_DMADESCTypeDef *pDMATxSet;
extern uint32_t ETH_TxPktChainMode( uint16_t len, uint32_t *pBuff );
extern void USBETH_MainTask(void);

#endif
