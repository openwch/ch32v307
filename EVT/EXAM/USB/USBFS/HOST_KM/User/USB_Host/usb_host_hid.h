/********************************** (C) COPYRIGHT  *******************************
 * File Name          : usb_hid.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/29
 * Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


#ifndef __USB_HOST_HID_H
#define __USB_HOST_HID_H

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************/
/* Header File */
#include "stdint.h"

/********************************************************************************/
/* Constant Definition */
#ifndef DEF_HID_DED_CMD
#define DEF_HID_DED_CMD
/* Set Protocol Command Packet */
__attribute__((aligned(4))) static const uint8_t SetupSetprotocol[ ] = 
{ 
    0x21, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

/* Set Idle Command Packet */
__attribute__((aligned(4))) static const uint8_t SetupSetidle[ ] = 
{ 
    0x21, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};

/* Set Report Command Packet */
__attribute__((aligned(4))) static const uint8_t SetupSetReport[ ] = 
{ 
    0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00 
};

/* Get Report Descriptor Command Packet */
__attribute__((aligned(4))) static const uint8_t SetupGetHidDes[ ] = 
{ 
    0x81, 0x06, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00 
};
#endif

/*******************************************************************************/
/* Function Declaration */
extern uint8_t HID_GetHidDesr( uint8_t usb_port, uint8_t ep0_size, uint8_t intf_num, uint8_t *pbuf, uint16_t *plen );
extern uint8_t HID_SetReport( uint8_t usb_port, uint8_t ep0_size, uint8_t intf_num, uint8_t *pbuf, uint16_t *plen );
extern uint8_t HID_SetIdle( uint8_t usb_port, uint8_t ep0_size, uint8_t intf_num, uint8_t duration, uint8_t reportid );

#ifdef __cplusplus
}
#endif

#endif
