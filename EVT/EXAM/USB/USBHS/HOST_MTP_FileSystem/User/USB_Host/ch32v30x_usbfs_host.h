/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_usbotg_host.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : USBOTG full speed host header file
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


#ifndef __CH32V30x_USBFS_HOST_H__
#define __CH32V30x_USBFS_HOST_H__

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/* Header File */
#include "ch32v30x.h"
#include "ch32v30x_usb.h"

/*******************************************************************************/
/* Macro Definition */

/* USB Setup Request */
#define pUSBFS_SetupRequest        ( (PUSB_SETUP_REQ)USBFS_TX_Buf )

/* USB Buffer Size */
#ifndef USBFS_MAX_PACKET_SIZE
#define USBFS_MAX_PACKET_SIZE       64    
#endif
/*******************************************************************************/
/* Constant Definition */
#ifndef DEF_USB_GEN_ENUM_CMD
#define DEF_USB_GEN_ENUM_CMD
/* Get Device Descriptor Command Packet */
__attribute__((aligned(4))) static const uint8_t  SetupGetDevDesc[ ] =
{
    USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof( USB_DEV_DESCR ), 0x00
};

/* Get Configuration Descriptor Command Packet */
__attribute__((aligned(4))) static const uint8_t SetupGetCfgDesc[ ] =
{
    USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00
};

/* Get String Descriptor Command Packet */
__attribute__((aligned(4))) static const uint8_t SetupGetStrDesc[ ] =
{
    USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_STRING, 0x09, 0x04, 0x04, 0x00
};

/* Set USB Address Command Packet */
__attribute__((aligned(4))) static const uint8_t SetupSetAddr[ ] =
{
    USB_REQ_TYP_OUT, USB_SET_ADDRESS, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* Set USB Configuration Command Packet */
__attribute__((aligned(4))) static const uint8_t SetupSetConfig[ ] =
{
    USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* Clear Endpoint STALL Command Packet */
__attribute__((aligned(4))) static const uint8_t SetupClearEndpStall[ ] =
{
    USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* Set Device Interface Command Packet */
__attribute__((aligned(4))) static const uint8_t SetupSetInterface[ ] =
{
    USB_REQ_RECIP_INTERF, USB_SET_INTERFACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

/*******************************************************************************/
/* Variable Declaration */
extern __attribute__((aligned(4))) uint8_t  USBFS_RX_Buf[ ];
extern __attribute__((aligned(4))) uint8_t  USBFS_TX_Buf[ ];

/*******************************************************************************/
/* Function Declaration */
extern void USBFS_RCC_Init( void );
extern void USBFS_Host_Init( FunctionalState sta );
extern uint8_t USBFSH_CheckRootHubPortStatus( uint8_t dev_sta );
extern uint8_t USBFSH_CheckRootHubPortEnable( void );
extern uint8_t USBFSH_CheckRootHubPortSpeed( void );
extern void USBFSH_SetSelfAddr( uint8_t addr );
extern void USBFSH_SetSelfSpeed( uint8_t speed );
extern void USBFSH_ResetRootHubPort( uint8_t mode );
extern uint8_t USBFSH_EnableRootHubPort( uint8_t *pspeed );
extern uint8_t USBFSH_Transact( uint8_t endp_pid, uint8_t endp_tog, uint32_t timeout );
extern uint8_t USBFSH_CtrlTransfer( uint8_t ep0_size, uint8_t *pbuf, uint16_t *plen );
extern uint8_t USBFSH_GetDeviceDescr( uint8_t *pep0_size, uint8_t *pbuf );
extern uint8_t USBFSH_GetConfigDescr( uint8_t ep0_size, uint8_t *pbuf, uint16_t buf_len, uint16_t *pcfg_len );
extern uint8_t USBFSH_GetStrDescr( uint8_t ep0_size, uint8_t str_num, uint8_t *pbuf );
extern uint8_t USBFSH_SetUsbAddress( uint8_t ep0_size, uint8_t addr );
extern uint8_t USBFSH_SetUsbConfig( uint8_t ep0_size, uint8_t cfg_val );
extern uint8_t USBFSH_ClearEndpStall( uint8_t ep0_size, uint8_t endp_num );
extern uint8_t USBFSH_GetEndpData( uint8_t endp_num, uint8_t *pendp_tog, uint8_t *pbuf, uint16_t *plen );
extern uint8_t USBFSH_SendEndpData( uint8_t endp_num, uint8_t *pendp_tog, uint8_t *pbuf, uint16_t len );


#ifdef __cplusplus
}
#endif

#endif
