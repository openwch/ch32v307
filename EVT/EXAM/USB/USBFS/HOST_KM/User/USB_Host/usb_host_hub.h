/********************************** (C) COPYRIGHT  *******************************
 * File Name          : usb_host_hub.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/29
 * Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
 

#ifndef __USB_HOST_HUB_H
#define __USB_HOST_HUB_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/
/* Header File */
#include "stdint.h"

/********************************************************************************/
/* Constant Definition */
#ifndef DEF_HUB_DED_CMD
#define DEF_HUB_DED_CMD
/* Get Port Status Command Packet */
__attribute__((aligned(4))) static const uint8_t GetPortStatus[ ] = 
{ 
    0xA3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00 
};

/* Clear Port Feature Command Packet */
__attribute__((aligned(4))) static const uint8_t ClearPortFeature[ ] = 
{ 
    0x23, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

/* Set Port Feature Command Packet */
__attribute__((aligned(4))) static const uint8_t SetPortFeature[ ] = 
{ 
    0x23, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

/* Get Hub Descriptor Command Packet */
__attribute__((aligned(4))) static const uint8_t GetHubDescr[ ] = 
{ 
    0xA0, 0x06, 0x00, 0x29, 0x00, 0x00, 0x02, 0x00 
};

/*******************************************************************************/
/* Function Declaration */
extern uint8_t HUB_GetPortStatus( uint8_t usb_port, uint8_t hub_ep0_size, uint8_t hub_port, uint8_t *pbuf );
extern uint8_t HUB_ClearPortFeature( uint8_t usb_port, uint8_t hub_ep0_size, uint8_t hub_port, uint8_t selector );
extern uint8_t HUB_SetPortFeature( uint8_t usb_port, uint8_t hub_ep0_size, uint8_t hub_port, uint8_t selector );
extern uint8_t HUB_GetClassDevDescr( uint8_t usb_port, uint8_t hub_ep0_size, uint8_t *pbuf, uint16_t *plen );

#endif

#ifdef __cplusplus
}
#endif

#endif
