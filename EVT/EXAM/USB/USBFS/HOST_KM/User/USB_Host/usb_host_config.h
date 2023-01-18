/********************************** (C) COPYRIGHT  *******************************
 * File Name          : usb_host_config.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/29
 * Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


#ifndef __USB_HOST_CONFIG_H
#define __USB_HOST_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Header File */
#include "string.h"
#include "debug.h"
#include "ch32v30x_usb.h"
#include "ch32v30x_usbfs_host.h"
#include "ch32v30x_usbhs_host.h"
#include "usb_host_hid.h"
#include "usb_host_hub.h"
#include "app_km.h"

/******************************************************************************/
/* Debug Macro Definition */
#define DEF_DEBUG_PRINTF            1
#if ( DEF_DEBUG_PRINTF == 1 )
#define DUG_PRINTF( format, arg... )    printf( format, ##arg )
#else
#define DUG_PRINTF( format, arg... )    do{ if( 0 )printf( format, ##arg ); }while( 0 );
#endif

/******************************************************************************/
/* USB Host Communication Related Macro Definition */

/* USB Host Port General Control */
#define DEF_TOTAL_ROOT_HUB          2
#define DEF_USBFS_PORT_EN           1
#define DEF_USBHS_PORT_EN           0
#define DEF_USBFS_PORT_INDEX        0x00
#define DEF_USBHS_PORT_INDEX        0x01
#define DEF_ONE_USB_SUP_DEV_TOTAL   5
#define DEF_NEXT_HUB_PORT_NUM_MAX   4
#define DEF_INTERFACE_NUM_MAX       4

/* USB Root Device Status */
#define ROOT_DEV_DISCONNECT         0
#define ROOT_DEV_CONNECTED          1
#define ROOT_DEV_FAILED             2
#define ROOT_DEV_SUCCESS            3

/* USB Device Address */
#define USB_DEVICE_ADDR             0x02

/* USB Speed */
#define USB_LOW_SPEED               0x00
#define USB_FULL_SPEED              0x01
#define USB_HIGH_SPEED              0x02
#define USB_SPEED_CHECK_ERR         0xFF

/* Configuration Descriptor Type */
#define DEF_DECR_CONFIG             0x02
#define DEF_DECR_INTERFACE          0x04
#define DEF_DECR_ENDPOINT           0x05
#define DEF_DECR_HID                0x21

/* USB Communication Status Code */
#define ERR_SUCCESS                 0x00
#define ERR_USB_CONNECT             0x15
#define ERR_USB_DISCON              0x16
#define ERR_USB_BUF_OVER            0x17
#define ERR_USB_DISK_ERR            0x1F
#define ERR_USB_TRANSFER            0x20
#define ERR_USB_UNSUPPORT           0xFB
#define ERR_USB_UNAVAILABLE         0xFC
#define ERR_USB_UNKNOWN             0xFE

/* USB Device Enumeration Status Code */
#define DEF_DEV_DESCR_GETFAIL       0x45
#define DEF_DEV_ADDR_SETFAIL        0x46
#define DEF_CFG_DESCR_GETFAIL       0x47
#define DEF_REP_DESCR_GETFAIL       0x48
#define DEF_CFG_VALUE_SETFAIL       0x49
#define DEF_DEV_TYPE_UNKNOWN        0xFF
                       
/* USB Communication Time */
#define DEF_BUS_RESET_TIME          11          // USB bus reset time
#define DEF_RE_ATTACH_TIMEOUT       100         // Wait for the USB device to reconnect after reset, 100mS timeout
#define DEF_WAIT_USB_TRANSFER_CNT   1000        // Wait for the USB transfer to complete
#define DEF_CTRL_TRANS_TIMEOVER_CNT 200000/20   // Control transmission delay timing


/*******************************************************************************/
/* Struct Definition */
/* Note: Please modify it according to your project. */

/* HUB Port Device  */
typedef struct _HUB_DEVICE
{
    uint8_t  bStatus;
    uint8_t  bType;
    uint8_t  bAddress;
    uint8_t  bSpeed;
    uint8_t  bEp0MaxPks;
    uint8_t  DeviceIndex;
}HUB_DEVICE, *PHUB_DEVICE;

/* Root HUB Device Structure */
typedef struct _ROOT_HUB_DEVICE
{
    uint8_t  bStatus;
    uint8_t  bType;
    uint8_t  bAddress;
    uint8_t  bSpeed;
    uint8_t  bEp0MaxPks;
    uint8_t  DeviceIndex;
    uint8_t  bPortNum;
    HUB_DEVICE Device[ DEF_NEXT_HUB_PORT_NUM_MAX ];
} ROOT_HUB_DEVICE, *PROOT_HUB_DEVICE;

/* USB Host Control Structure */
typedef struct __HOST_CTL
{                                   
    uint8_t  InterfaceNum;
    uint8_t  ErrorCount;	
    
    struct interface
    {
        uint8_t  Type;
        uint16_t HidDescLen;
        uint8_t  HidReportID;
        uint8_t  Full_KB_Flag;

        uint8_t  InEndpNum;
        uint8_t  InEndpAddr[ 4 ];
        uint8_t  InEndpType[ 4 ];
        uint16_t InEndpSize[ 4 ];
        uint8_t  InEndpTog[ 4 ];
        uint8_t  InEndpInterval[ 4 ];
        uint8_t  InEndpTimeCount[ 4 ];

        uint8_t  OutEndpNum;
        uint8_t  OutEndpAddr[ 4 ];
        uint8_t  OutEndpType[ 4 ];
        uint16_t OutEndpSize[ 4 ];
        uint8_t  OutEndpTog[ 4 ];

        uint8_t  IDFlag;
        uint8_t  ReportID;

        uint8_t  LED_Usage_Min;
        uint8_t  LED_Usage_Max;

        uint8_t  SetReport_Swi;
        uint8_t  SetReport_Value;
        uint8_t  SetReport_Flag;

    }Interface[ DEF_INTERFACE_NUM_MAX ];
} HOST_CTL, *PHOST_CTL;

/*******************************************************************************/
/* Struct Declaration */
extern struct   _ROOT_HUB_DEVICE RootHubDev[ ];
extern struct   __HOST_CTL HostCtl[ ];


#ifdef __cplusplus
}
#endif

#endif
