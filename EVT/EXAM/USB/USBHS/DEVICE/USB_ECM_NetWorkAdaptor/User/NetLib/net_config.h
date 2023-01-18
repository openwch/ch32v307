/********************************** (C) COPYRIGHT *******************************
* File Name          : net_config.h
* Author             : WCH
* Version            : V1.30
* Date               : 2022/06/02
* Description        : This file contains the configurations of 
*                      Ethernet protocol stack library
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __NET_CONFIG_H__
#define __NET_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * MAC queue configuration
 */
#define ETH_TXBUFNB                   8     /* Number of descriptors sent by the MAC  */

#define ETH_RXBUFNB                   16    /* Number of MAC received descriptors  */

#ifndef ETH_MAX_PACKET_SIZE
#define ETH_RX_BUF_SZE                1520  /* MAC receive buffer length, an integer multiple of 4 */
#define ETH_TX_BUF_SZE                1520  /* MAC send buffer length, an integer multiple of 4 */
#else
#define ETH_RX_BUF_SZE                ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SZE                ETH_MAX_PACKET_SIZE
#endif
#define DEF_ETH_RX_USBPACK            1540
#define DEF_ETH_TX_USBPACK            1540
#define DEF_ETH_USBPACK               1536
#define DEF_USB_TRANCE                1

#define DEF_PHY_QUERY_TIMEOUT         2   /* Query Phy Status every 200ms */

#ifdef __cplusplus
}
#endif
#endif
