/********************************** (C) COPYRIGHT *******************************
* File Name          : net_filter.h
* Author             : WCH
* Version            : V1.30
* Date               : 2023/01/11
* Description        : This file contains the configurations of mac filter function
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __NET_FILTER_H__
#define __NET_FILTER_H__

/*FILTER FUNC:
 * 1¡¢DES_ADDR_FILTER£º
 *  MULTICAST_PERFECT_FILTER
 *  MULTICAST_HASH_FILTER
 *  MULTICAST_HASH_OR_PERFECT_FILTER
 *
 *  UNICAST_PERFECT_FILTER
 *  UNICAST_HASH_FILTER
 *  UNICAST_HASH_OR_PERFECT_FILTER
 *
 *  2¡¢SRC_ADDR_PERFECT_FILTER
 *
 *  3¡¢BROADCAST_FILTER
 * */

#define DES_ADDR_FILTER                             1

#define UNICAST_PERFECT_FILTER                      1
#define UNICAST_HASH_FILTER                         0
#define UNICAST_HASH_OR_PERFECT_FILTER              0

#define MULTICAST_PERFECT_FILTER                    0
#define MULTICAST_HASH_FILTER                       0
#define MULTICAST_HASH_OR_PERFECT_FILTER            0

#define SRC_ADDR_PERFECT_FILTER                     0

#define BROADCAST_FILTER                            0

#define UNICAST_HashOrPerfectFilter                 1
#define MULTICAST_HashOrPerfectFilter               2

#define CHECK_MAC_ISMULTICAST(addr)   ((*addr == 0x01) && \
                                        (*(addr + 1) == 0x00) && \
                                        (*(addr + 2) == 0x5e))

void ETH_ClearMACAddrReg(uint8_t index);
void ETH_HashOrPerfectFilter(uint8_t mode);
void ETH_HashFilter(const uint8_t *MACAddr);
uint8_t ETH_PerfectFilter(const uint8_t *MACAddr);
void ETH_BroadcastFilter(FunctionalState NewState);
uint8_t ETH_SrcAddrPerfectFilter(const uint8_t *MACAddr);

#endif

