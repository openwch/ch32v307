/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/06/11
* Description        : Definition for PING.c.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __PINC_H__
#define __PINC_H__
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "wchnet.h"
#include "debug.h"

#define ICMP_SOKE_CON                0
#define ICMP_SEND_ERR                1
#define ICMP_SEND_SUC                2
#define ICMP_RECV_ERR                3
#define ICMP_RECV_SUC                4
#define ICMP_UNRECH                  5
#define ICMP_REPLY                   6
#define ICMP_REPLY_SUC               7
#define ICMP_KEEP_NO                 10


#define ICMP_HEAD_TYPE               8
#define ICMP_HEAD_REPLY              0
#define ICMP_HEAD_CODE               0
#define ICMP_HEAD_ID                 512
#define ICMP_HEAD_SEQ                100
#define ICMP_DATA_BYTES              32

#define PING_SEND_CNT                5

/*ICMP header field data structure*/
typedef struct _icmphdr 
{
    u8   i_type;                 //ICMP message type
    u8   i_code;                 //code number in the type
    u16  i_cksum;                //checksum
    u16  i_id;                   //identifier
    u16  i_seq;                  //sequence
    u8   i_data[32];             //data area
}IcmpHeader,*IcmpHead;

extern u8 DESIP[4];
extern u8 ICMPSuc;

extern void InitPING( void );

extern void InitParameter( void );

extern void WCHNET_PINGCmd( void );

extern void WCHNET_PINGInit( void );

extern void mStopIfError(u8 iError);

extern void Respond_PING( u8 *pDat );

extern void WCHNET_ICMPRecvData( u32 len, u8 *pDat );

extern void WCHNET_PINGSendData( u8 *PSend, u32 Len,u8 id );

extern void WCHNET_ProcessReceDat( char *recv_buff,u8 check_type,u8 socketid );

#endif
