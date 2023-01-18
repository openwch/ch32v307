/********************************** (C) COPYRIGHT  *******************************
 * File Name          : usbhd_host_km.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2020/04/30
 * Description        : This file contains all the functions prototypes for the USB
 *                      firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


#ifndef __APP_KM_H
#define __APP_KM_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************/
/* Header File */
#include "stdint.h"


/*******************************************************************************/
/* Macro Definition */

/* General */
#define DEF_COM_BUF_LEN                 1024                                    

/* USB HID Device Interface Type */
#define DEC_KEY                         0x01
#define DEC_MOUSE                       0x02
#define DEC_UNKNOW                      0xFF

/* USB Keyboard Lighting Key */
#define DEF_KEY_NUM                     0x53
#define DEF_KEY_CAPS                    0x39
#define DEF_KEY_SCROLL                  0x47


/*******************************************************************************/
/* Variable Declaration */
extern uint8_t  DevDesc_Buf[ ];                                         
extern uint8_t  Com_Buf[ ];   

                                         
/*******************************************************************************/
/* Function Declaration */
extern void TIM3_Init( uint16_t arr, uint16_t psc );
extern uint8_t USBH_CheckRootHubPortStatus( uint8_t usb_port );
extern void USBH_ResetRootHubPort( uint8_t usb_port, uint8_t mode );
extern uint8_t USBH_EnableRootHubPort( uint8_t usb_port );
extern uint8_t USBH_GetDeviceDescr( uint8_t usb_port );
extern uint8_t USBH_SetUsbAddress( uint8_t usb_port );
extern uint8_t USBH_GetConfigDescr( uint8_t usb_port, uint16_t *pcfg_len );
extern void USBH_AnalyseType( uint8_t *pdev_buf, uint8_t *pcfg_buf, uint8_t *ptype );
extern uint8_t USBH_SetUsbConfig( uint8_t usb_port, uint8_t cfg_val );
extern uint8_t USBH_GetStrDescr( uint8_t usb_port, uint8_t ep0_size, uint8_t str_num );
extern uint8_t USBH_GetHidData( uint8_t usb_port, uint8_t index, uint8_t intf_num, uint8_t endp_num, uint8_t *pbuf, uint16_t *plen );
extern uint8_t USBH_SendHidData( uint8_t usb_port, uint8_t index, uint8_t intf_num, uint8_t endp_num, uint8_t *pbuf, uint16_t len );
extern uint8_t USBH_ClearEndpStall( uint8_t usb_port, uint8_t endp_num );
extern uint8_t USBH_EnumRootDevice( uint8_t usb_port );
extern uint8_t KM_AnalyzeConfigDesc( uint8_t usb_port, uint8_t index );
extern void KM_AnalyzeHidReportDesc( uint8_t index, uint8_t intf_num );
extern uint8_t KM_DealHidReportDesc( uint8_t usb_port, uint8_t index, uint8_t ep0_size );
extern uint8_t USBH_EnumHidDevice( uint8_t usb_port, uint8_t index, uint8_t ep0_size );
extern uint8_t HUB_AnalyzeConfigDesc( uint8_t index );
extern uint8_t USBH_EnumHubDevice( uint8_t usb_port, uint8_t ep0_size );
extern uint8_t HUB_Port_PreEnum1( uint8_t usb_port, uint8_t hub_port, uint8_t *pbuf );
extern uint8_t HUB_Port_PreEnum2( uint8_t usb_port, uint8_t hub_port, uint8_t *pbuf );
extern uint8_t HUB_CheckPortSpeed( uint8_t usb_port, uint8_t hub_port, uint8_t *pbuf );
extern uint8_t USBH_EnumHubPortDevice( uint8_t usb_port, uint8_t hub_port, uint8_t *paddr, uint8_t *ptype );
extern void KB_AnalyzeKeyValue( uint8_t index, uint8_t intf_num, uint8_t *pbuf, uint16_t len );
extern uint8_t KB_SetReport( uint8_t usb_port, uint8_t index, uint8_t ep0_size, uint8_t intf_num );
extern void USBH_MainDeal( void );


#ifdef __cplusplus
}
#endif

#endif
