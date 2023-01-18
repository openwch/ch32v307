/********************************** (C) COPYRIGHT *******************************
 * File Name  :usbd_compatibility_hid.h
 * Author     :OWNER
 * Version    : v0.01
 * Date       : 2022��7��8��
 * Description:
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef USER_USBD_COMPATIBILITY_HID_H_
#define USER_USBD_COMPATIBILITY_HID_H_

#include "usb_desc.h"
#define DEF_UART2_BUF_SIZE            8192
#define DEF_UART2_TOUT_TIME           30             // NOTE: the timeout time should be set according to the actual baud rate.

#define SET_REPORT_DEAL_OVER          0x00
#define SET_REPORT_WAIT_DEAL          0x01


extern uint8_t  HID_Report_Buffer[DEF_USBD_HS_PACK_SIZE];               // HID Report Buffer
extern volatile uint8_t HID_Set_Report_Flag;

extern volatile uint16_t Data_Pack_Max_Len;
extern volatile uint16_t Head_Pack_Len;

extern void UART2_Tx_Service( void );
extern void UART2_Rx_Service( void );
extern void UART2_Init( void );
extern void UART2_DMA_Init( void );
extern void TIM2_Init( void );
extern void HID_Set_Report_Deal( void );

#endif /* USER_USBD_COMPATIBILITY_HID_H_ */
