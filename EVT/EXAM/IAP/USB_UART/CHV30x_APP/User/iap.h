/********************************** (C) COPYRIGHT  *******************************
 * File Name          : iap.h
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/01/09
 * Description        : IAP
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __IAP_H
#define __IAP_H

#include "ch32v30x.h"
#include "stdio.h"
#include "ch32v30x_usbfs_device.h"
#include "ch32v30x_usbhs_device.h"
#define PRINT     0

#define USBD_DATA_SIZE    64
#define FLASH_Base        0x08005000

#define Uart_Sync_Head1   0xaa
#define Uart_Sync_Head2   0x55

#define CMD_IAP_PROM      0x80
#define CMD_IAP_ERASE     0x81
#define CMD_IAP_VERIFY    0x82
#define CMD_IAP_END       0x83
#define CMD_JUMP_IAP      0x84

#define ERR_SUCCESS       0x00
#define ERR_ERROR         0x01
#define ERR_End           0x02

#define CalAddr           (0x08078000-4)
#define CheckNum          (0x5aa55aa5)

typedef union __attribute__ ((aligned(4)))_ISP_CMD {

struct{

    u8 Cmd;
    u8 Len;
    u8 data[64];
}UART;

struct{
    u8 Cmd;
    u8 Len;
    u8 data[62];
}program;

struct{
    u8 Cmd;
    u8 Len;
    u8 addr[4];
    u8 data[56];
}verify;

struct{
    u8 buf[64+2];
}other;

} isp_cmd;

typedef void (*iapfun)(void);

extern u8 EP2_Rx_Buffer[USBD_DATA_SIZE+4];

u8 RecData_Deal(void);
void USART3_CFG(u32 baudrate);

void UART_Rx_Deal(void);

#endif
