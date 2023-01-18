/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_usbhs_device.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/08/20
* Description        : USB2.0高速设备操作相关头文件
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32V30X_USBHS_DEVICE_H__
#define __CH32V30X_USBHS_DEVICE_H__

/*******************************************************************************/
/* Header File */
#include "debug.h"
#include "string.h"
#include "ch32v30x_usb.h"
#include "usb_desc.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/* Macro Definition */

/* General */
#define pUSBHS_SetupReqPak            ((PUSB_SETUP_REQ)USBHS_EP0_Buf)

#define DEF_UEP_IN                    0x80
#define DEF_UEP_OUT                   0x00
/* Endpoint Number */
#define DEF_UEP_BUSY                  0x01
#define DEF_UEP_FREE                  0x00
#define DEF_UEP_NUM                   16
#define DEF_UEP0                      0x00
#define DEF_UEP1                      0x01
#define DEF_UEP2                      0x02
#define DEF_UEP3                      0x03
#define DEF_UEP4                      0x04
#define DEF_UEP5                      0x05
#define DEF_UEP6                      0x06
#define DEF_UEP7                      0x07
#define DEF_UEP8                      0x08
#define DEF_UEP9                      0x09
#define DEF_UEP10                     0x0A
#define DEF_UEP11                     0x0B
#define DEF_UEP12                     0x0C
#define DEF_UEP13                     0x0D
#define DEF_UEP14                     0x0E
#define DEF_UEP15                     0x0F

#define USBHSD_UEP_CFG_BASE           0x40023410
#define USBHSD_UEP_BUF_MOD_BASE       0x40023418
#define USBHSD_UEP_RXDMA_BASE         0x40023420
#define USBHSD_UEP_TXDMA_BASE         0x4002345C
#define USBHSD_UEP_TXLEN_BASE         0x400234DC
#define USBHSD_UEP_TXCTL_BASE         0x400234DE
#define USBHSD_UEP_TX_EN( N )         ( (uint16_t)( 0x01 << N ) )
#define USBHSD_UEP_RX_EN( N )         ( (uint16_t)( 0x01 << ( N + 16 ) ) )
#define USBHSD_UEP_DOUBLE_BUF( N )    ( (uint16_t)( 0x01 << N ) )
#define DEF_UEP_DMA_LOAD              0 /* Direct the DMA address to the data to be processed */
#define DEF_UEP_CPY_LOAD              1 /* Use memcpy to move data to a buffer */
#define USBHSD_UEP_RXDMA( N )         ( *((volatile uint32_t *)( USBHSD_UEP_RXDMA_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_RXBUF( N )         ( (uint8_t *)(*((volatile uint32_t *)( USBHSD_UEP_RXDMA_BASE + ( N - 1 ) * 0x04 ) ) ) + 0x20000000 )
#define USBHSD_UEP_TXCTRL( N )        ( *((volatile uint8_t *)( USBHSD_UEP_TXCTL_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_TXDMA( N )         ( *((volatile uint32_t *)( USBHSD_UEP_TXDMA_BASE + ( N - 1 ) * 0x04 ) ) )
#define USBHSD_UEP_TXBUF( N )         ( (uint8_t *)(*((volatile uint32_t *)( USBHSD_UEP_TXDMA_BASE + ( N - 1 ) * 0x04 ) ) ) + 0x20000000 )
#define USBHSD_UEP_TLEN( N )          ( *((volatile uint16_t *)( USBHSD_UEP_TXLEN_BASE + ( N - 1 ) * 0x04 ) ) )

// USB SPEED TYPE
#define USBHS_SPEED_TYPE_MASK         ((uint8_t)(0x03))
#define USBHS_SPEED_LOW               ((uint8_t)(0x02))
#define USBHS_SPEED_FULL              ((uint8_t)(0x00))
#define USBHS_SPEED_HIGH              ((uint8_t)(0x01))

/******************************************************************************/
/* Variable Declaration */

/* Ringbuffer define  */
#define DEF_Ring_Buffer_Max_Blks      16
#define DEF_RING_BUFFER_SIZE          (DEF_Ring_Buffer_Max_Blks*DEF_USBD_HS_PACK_SIZE)
#define DEF_RING_BUFFER_REMINE        4
#define DEF_RING_BUFFER_RESTART       12

/* Ring Buffer typedef */
typedef struct __attribute__((packed)) _RING_BUFF_COMM
{
    volatile uint8_t  LoadPtr;
    volatile uint8_t  DealPtr;
    volatile uint8_t  RemainPack;
    volatile uint8_t  StopFlag;
    volatile uint16_t PackLen[DEF_Ring_Buffer_Max_Blks];
} RING_BUFF_COMM, pRING_BUFF_COMM;

/* Ringbuffer variables */
extern RING_BUFF_COMM  RingBuffer_Comm;
extern __attribute__ ((aligned(4))) uint8_t  Data_Buffer[ ];

/* SetUp Request Values */
extern const uint8_t *pUSBHS_Descr;

/* Setup Request */
extern volatile uint8_t  USBHS_SetupReqCode;
extern volatile uint8_t  USBHS_SetupReqType;
extern volatile uint16_t USBHS_SetupReqValue;
extern volatile uint16_t USBHS_SetupReqIndex;
extern volatile uint16_t USBHS_SetupReqLen;

/* USB Device Status */
extern volatile uint8_t  USBHS_DevConfig;
extern volatile uint8_t  USBHS_DevAddr;
extern volatile uint8_t  USBHS_DevSleepStatus;
extern volatile uint8_t  USBHS_DevEnumStatus;

/* Endpoint tx busy flag */
extern volatile uint8_t  USBHS_Endp_Busy[ ];

/* Endpoint Buffer */
extern  __attribute__ ((aligned(4))) uint8_t USBHS_EP0_Buf[ ];
extern __attribute__ ((aligned(4))) uint8_t USBHS_EP3_Rx_Buf[ ];
extern __attribute__ ((aligned(4))) uint8_t USBHS_EP5_Rx_Buf[ ];
extern __attribute__ ((aligned(4))) uint8_t USBHS_EP4_Tx_Buf[ ];
extern __attribute__ ((aligned(4))) uint8_t USBHS_EP6_Tx_Buf[ ];

/********************************************************************************/
/* Function Declaration */
extern void USBHS_RCC_Init( void );
extern void USBHS_Device_Endp_Init ( void );
extern void USBHS_Device_Init ( FunctionalState sta );
extern void USBHS_Device_SetAddress( uint32_t address );
extern void USBHS_IRQHandler( void );
extern void USBHS_Sleep_WakeUp_Cfg( void );
extern void USBHD_Sleep_Wakeup_Operate( void );
extern uint8_t USBHS_Endp_DataUp( uint8_t endp, uint8_t *pbuf, uint16_t len, uint8_t mod );

#ifdef __cplusplus
}
#endif

#endif

