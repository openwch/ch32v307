/********************************** (C) COPYRIGHT *******************************
* File Name          : Desc_Analysis.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/04/21
* Description        : USB Descriptor Analysis functions  header file
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef USER_DESC_ANALYSIS_H_
#define USER_DESC_ANALYSIS_H_

#include "debug.h"
#include "string.h"

#ifndef NULL
#define NULL                    0
#endif

#ifndef VOID
#define VOID                    void
#endif
#ifndef CONST
#define CONST                   const
#endif
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char           BOOLEAN;
#endif
#ifndef CHAR
typedef char                    CHAR;
#endif
#ifndef INT8
typedef char                    INT8;
#endif
#ifndef INT16
typedef short                   INT16;
#endif
#ifndef INT32
typedef long                    INT32;
#endif
#ifndef UINT8
typedef unsigned char           UINT8;
#endif
#ifndef UINT16
typedef unsigned short          UINT16;
#endif
#ifndef UINT32
typedef unsigned long           UINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile  UINT8V;
#endif
#ifndef UINT16V
typedef unsigned short volatile UINT16V;
#endif
#ifndef UINT32V
typedef unsigned long volatile  UINT32V;
#endif

#ifndef PVOID
typedef void                    *PVOID;
#endif
#ifndef PCHAR
typedef char                    *PCHAR;
#endif
#ifndef PCHAR
typedef const char              *PCCHAR;
#endif
#ifndef PINT8
typedef char                    *PINT8;
#endif
#ifndef PINT16
typedef short                   *PINT16;
#endif
#ifndef PINT32
typedef long                    *PINT32;
#endif
#ifndef PUINT8
typedef unsigned char           *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short          *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long           *PUINT32;
#endif
#ifndef PUINT8V
typedef volatile unsigned char  *PUINT8V;
#endif
#ifndef PUINT16V
typedef volatile unsigned short *PUINT16V;
#endif
#ifndef PUINT32V
typedef volatile unsigned long  *PUINT32V;
#endif

/************************************************/
/* USB主机端口下的单个设备接口最大个数 */
#define DEF_INTERFACE_NUM_MAX       4
#define DEF_ENDP_NUM_MAX            4
#define DEF_HOSTCTR_STRUCT_MAX      1

/* USB主机控制相关结构体 */
typedef struct _DEV_INTERFACE
{
    UINT8  Type;                                                            /* 该接口类型(键盘、鼠标、多媒体) */
    UINT16 HidDescLen;                                                      /* 该接口HID报表描述符长度 */
    UINT8  HidReportID;                                                     /* HID Report ID号 */
    UINT8  Full_KB_Flag;                                                    /* 全键盘模式标志 */

    UINT8  InEndpNum;                                                       /* 该接口输入端点个数 */
    UINT8  InEndpAddr[ DEF_ENDP_NUM_MAX ];                                  /* 该接口输入端点号 */
    UINT8  InEndpType[ DEF_ENDP_NUM_MAX ];                                  /* 该接口输入端点类型 */
    UINT16 InEndpSize[ DEF_ENDP_NUM_MAX ];                                  /* 该接口输入端点大小 */
    UINT8  InEndpTog[ DEF_ENDP_NUM_MAX ];                                   /* 该接口输入端点同步标志位 */
    UINT8  InEndpInterval[ DEF_ENDP_NUM_MAX ];                              /* 该接口输入端点轮询时间 */
    UINT8  InEndpTimeCount[ DEF_ENDP_NUM_MAX ];                             /* 该接口输入端点时间计数 */

    UINT8  OutEndpNum;                                                      /* 该接口输出端点个数 */
    UINT8  OutEndpAddr[ DEF_ENDP_NUM_MAX ];                                 /* 该接口输出端点号 */
    UINT8  OutEndpType[ DEF_ENDP_NUM_MAX ];                                 /* 该接口输入端点类型 */
    UINT16 OutEndpSize[ DEF_ENDP_NUM_MAX ];                                 /* 该接口输入端点大小 */
    UINT8  OutEndpTog[DEF_ENDP_NUM_MAX];                                    /* 该接口输出端点同步标志位 */

    UINT8  IDFlag;                                                          /* 该接口是否包含REPORT ID标志 */
    UINT8  Media_Sp_Flag;                                                   /* 多媒体特殊标记 */

    UINT8  Disbility;                                                       /* distinguishability */
    UINT8  Button;                                                          /* button */
    UINT8  Desktop_X;                                                       /* x/y */
    UINT8  Desktop_Y;                                                       /* x/y */
    UINT8  Wheel;                                                           /* wheel */
    UINT8  Asolute;                                                         /* resv */
    UINT8  Mouse_ReportID;                                                  /* Mouse Report ID号 */

    UINT8  NoSetReport_Flag;
}TPF__DEV_INTERFACE;

typedef struct __HOST_CTL
{
    UINT8  DeviceState;                                                         /* USB设备状态: 未连接、连接、枚举失败、操作成功 */
    UINT8  DeviceAddr;                                                          /* USB设备地址 */
    UINT8  DeviceSpeed;                                                         /* USB设备速度 */
    UINT8  DeviceType;                                                          /* USB设备类型: 位0: 1-包含键盘的设备;
                                                                                                                                                                                              位1：1-有输出端口;
                                                                                                                                                                                            位7：1- hub */
    UINT8  InterfaceNum;                                                        /* USB设备接口数 */
    UINT8  HubPortNum;                                                          /* HUB下游端口数量 */
    UINT8  HubInAddr;                                                           /* HUB输入端点地址 */
    UINT8  ErrorCount;                                                          /* 设备异常计数 */

    /* 接口相关变量定义 */
    TPF__DEV_INTERFACE Interface[ DEF_INTERFACE_NUM_MAX ];  /* 不发送SetReport标志 */

}TPF_HOSTCTL;

extern TPF_HOSTCTL HostCtl[ ];

#define DEC_SUCCESS               0

#define DEF_DECR_CONFIG           0x02
#define DEF_DECR_INTERFACE        0x04
#define DEF_DECR_ENDPOINT         0x05
#define DEF_DECR_HID              0x03

#define DEC_KEY                   0x01
#define DEC_MOUSE                 0x02
#define DEC_MEDIA                 0x00
#define DEC_UNKNOW                0xFF

#define USB_DEV_CLASS_AUDIO       0x01
#define USB_DEV_CLASS_CDC         0x02
#define USB_DEV_CLASS_HID         0x03
#define USB_DEV_CLASS_PHY         0x05
#define USB_DEV_CLASS_IMG         0x06
#define USB_DEV_CLASS_PRINTER     0x07
#define USB_DEV_CLASS_MASSSTORE   0x08
#define USB_DEV_CLASS_HUB         0x09
#define USB_DEV_CLASS_CDC_DATA    0x0A
#define USB_DEV_CLASS_SMARTCARD   0x0B
#define USB_DEV_CLASS_SECURITY    0x0D
#define USB_DEV_CLASS_VIDEO       0x0E
#define USB_DEV_CLASS_PHD         0x0F/* Personal HealthCare Device */
#define USB_DEV_CLASS_AV          0x10/* Audio & Video */
#define USB_DEV_CLASS_WLC         0xE0/* Wireless Controller */
#define USB_DEV_CLASS_OTH         0xEF


extern UINT8 KM_Analyse_ConfigDesc( UINT8 index );

#endif /* USER_DESC_ANALYSIS_H_ */
