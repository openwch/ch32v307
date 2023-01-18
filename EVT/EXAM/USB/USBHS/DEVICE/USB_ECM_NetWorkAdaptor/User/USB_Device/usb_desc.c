/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/20
 * Description        : usb device descriptor,configuration descriptor,
 *                      string descriptors and other descriptors.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "usb_desc.h"

/* Device Descriptor */
const uint8_t  MyDevDescr[ ] =
{
    0x12,                           /* Size of this descriptor, in bytes */
    0x01,                           /* DEVICE descriptor */
    0x00,
    0x02,                           /* bcdUSB USB2.0 */
    0x00,                           /* Miscellaneous Device Class */
    0x00,                           /* Common Class */
    0x00,                           /* Interface Association Descriptor */
    DEF_USBD_UEP0_SIZE,             /* Max packet 64 */
    ( DEF_USB_VID & 0xFF ),
    ( DEF_USB_VID >> 8 ),           /* VID */
    ( DEF_USB_PID & 0xFF ),
    ( DEF_USB_PID >> 8 ),           /* PID */
    DEF_IC_PRG_VER,
    0x00,                           /* bcdDevice */
    0x01,                           /* iManufacturer */
    0x02,                           /* iProduct */
    0x03,                           /* iSerialNumber */
    0x01,                           /* bNumConfigurations */
};

/* USB配置描述符(高速) */
const uint8_t  MyCfgDescr_HS[ ] =
{
    /* 配置描述符 */
    0x09,                           /* bLength: Configuation Descriptor size */
    0x02,                           /* bDescriptorType: Configuration */
    0x50,                           /* wTotalLength: Bytes returned */
    0x00,
    0x02,                           /* bNumInterfaces: 2 interface */
    0x00,                           /* bConfigurationValue: Configuration value */
    0x00,                           /* iConfiguration: Index of string descriptor describing the configuration */
    DEF_USB_ATTRIBUTES,             /* bmAttributes: Bus powered */
    DEF_USB_MAXPOWER,               /* MaxPower 100 mA: this current is used for detecting Vbus */

    /* 控制接口(接口0)描述符 */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x00,                           /* bInterfaceNumber */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x01,                           /* bNumEndpoints */
    0x02,                           /* bInterfaceClass: 接口实现通信设备/接口类 */
    0x06,                           /* bInterfaceSubClass: 接口实现以太网组网控制模型子类 */
    0x00,                           /* bInterfaceProtocol: 接口使用不需要特定于类的协议 */
    0x05,                           /* iInterface */

    /* CDC ECM Header功能描述符 */
    0x05,                           /* bLength */
    0x24,                           /* bDescriptorType */
    0x00,                           /* bDescriptorSubType */
    0x10,                           /* bcdCDC: USB通信类定义通信规范1.10版 */
    0x01,

    /* CDC ECM Union功能描述符 */
    0x05,                           /* bLength */
    0x24,                           /* bDescriptorType */
    0x06,                           /* bDescriptorSubType */
    0x00,                           /* bControlInterface: 通信或数据类接口的接口号 */
    0x01,                           /* bSubordinateInterface0: 下级接口的接口号 */

    /* CDC ECM Function描述符 */
    0x0D,                           /* bLength */
    0x24,                           /* bDescriptorType: CS_INTERFACE */
    0x0F,                           /* bDescriptorSubType */
    0x04,                           /* iMACAddress: 字符串描述符索引,ECM/NCM通过序列号描述符上报MAC地址 */
    0x00,                           /* bmEthernetStatistics: 设备不支持GetEthernetStatistic请求 */
    0x00,
    0x00,
    0x00,
    0xEA,                           /* wMaxSegmentSize: 太网设备能够支持的最大段大小: 1514 */
    0x05,
    0x00,                           /* wNumberMCFilters: 多播筛选器位图 */
    0x00,
    0x00,                           /* bNumberPowerFilters: 包含可用于唤醒主机的模式筛选器数 */

    /* 端点描述符 */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x81,                           /* bEndpointAddress */
    0x03,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP1_HS_SIZE, (uint8_t)( DEF_USB_EP1_HS_SIZE >> 8 ), /* wMaxPacketSize 64 */
    0x08,                           /* bInterval */

    /* 数据接口(接口1)描述符alt0 */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x01,                           /* bInterfaceNumber: 0x01 */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x00,                           /* bNumEndpoints: 0 */
    0x0A,                           /* bInterfaceClass: 接口实现数据接口类 */
    0x00,                           /* bInterfaceProtocol: 接口使用不需要特定于类的协议 */
    0x00,                           /* bInterfaceSubClass */
    0x00,                           /* iInterface */

    /* 数据接口(接口1)描述符alt1 */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x01,                           /* bInterfaceNumber: 0x01 */
    0x01,                           /* bAlternateSetting: 0x01 */
    0x02,                           /* bNumEndpoints: 2 */
    0x0A,                           /* bInterfaceClass */
    0x00,                           /* bInterfaceProtocol */
    0x00,                           /* bInterfaceSubClass */
    0x00,                           /* iInterface */

    /* 端点描述符 */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x82,                           /* bEndpointAddress */
    0x02,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP2_HS_SIZE, (uint8_t)( DEF_USB_EP2_HS_SIZE >> 8 ), /* wMaxPacketSize 512 */
    0x00,                           /* bInterval */

    /* 端点描述符 */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x03,                           /* bEndpointAddress */
    0x02,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP3_HS_SIZE, (uint8_t)( DEF_USB_EP3_HS_SIZE >> 8 ), /* wMaxPacketSize 512 */
    0x00,
};

/* Configuration Descriptor */
const uint8_t  MyCfgDescr_FS[ ] =
{
    /* 配置描述符 */
    0x09,                           /* bLength: Configuation Descriptor size */
    0x02,                           /* bDescriptorType: Configuration */
    0x50,                           /* wTotalLength: Bytes returned */
    0x00,
    0x02,                           /* bNumInterfaces: 2 interface */
    0x00,                           /* bConfigurationValue: Configuration value */
    0x00,                           /* iConfiguration: Index of string descriptor describing the configuration */
    DEF_USB_ATTRIBUTES,             /* bmAttributes: Bus powered */
    DEF_USB_MAXPOWER,               /* MaxPower 100 mA: this current is used for detecting Vbus */

    /* 控制接口(接口0)描述符 */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x00,                           /* bInterfaceNumber */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x01,                           /* bNumEndpoints */
    0x02,                           /* bInterfaceClass: 接口实现通信设备/接口类 */
    0x06,                           /* bInterfaceSubClass: 接口实现以太网组网控制模型子类 */
    0x00,                           /* bInterfaceProtocol: 接口使用不需要特定于类的协议 */
    0x05,                           /* iInterface */

    /* CDC ECM Header功能描述符 */
    0x05,                           /* bLength */
    0x24,                           /* bDescriptorType */
    0x00,                           /* bDescriptorSubType */
    0x10,                           /* bcdCDC: USB通信类定义通信规范1.10版 */
    0x01,

    /* CDC ECM Union功能描述符 */
    0x05,                           /* bLength */
    0x24,                           /* bDescriptorType */
    0x06,                           /* bDescriptorSubType */
    0x00,                           /* bControlInterface: 通信或数据类接口的接口号 */
    0x01,                           /* bSubordinateInterface0: 下级接口的接口号 */

    /* CDC ECM Function描述符 */
    0x0D,                           /* bLength */
    0x24,                           /* bDescriptorType: CS_INTERFACE */
    0x0F,                           /* bDescriptorSubType */
    0x03,                           /* iMACAddress: 字符串描述符索引,ECM/NCM通过序列号描述符上报MAC地址 */
    0x00,                           /* bmEthernetStatistics: 设备不支持GetEthernetStatistic请求 */
    0x00,
    0x00,
    0x00,
    0xEA,                           /* wMaxSegmentSize: 太网设备能够支持的最大段大小: 1514 */
    0x05,
    0x00,                           /* wNumberMCFilters: 多播筛选器位图 */
    0x00,
    0x00,                           /* bNumberPowerFilters: 包含可用于唤醒主机的模式筛选器数 */

    /* 端点描述符 */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x81,                           /* bEndpointAddress */
    0x03,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP1_FS_SIZE, (uint8_t)( DEF_USB_EP1_FS_SIZE >> 8 ), /* wMaxPacketSize 64 */
    0x08,                           /* bInterval */

    /* 数据接口(接口1)描述符alt0 */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x01,                           /* bInterfaceNumber: 0x01 */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x00,                           /* bNumEndpoints: 0 */
    0x0A,                           /* bInterfaceClass: 接口实现数据接口类 */
    0x00,                           /* bInterfaceProtocol: 接口使用不需要特定于类的协议 */
    0x00,                           /* bInterfaceSubClass */
    0x00,                           /* iInterface */

    /* 数据接口(接口1)描述符alt1 */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x01,                           /* bInterfaceNumber: 0x01 */
    0x01,                           /* bAlternateSetting: 0x01 */
    0x02,                           /* bNumEndpoints: 2 */
    0x0A,                           /* bInterfaceClass */
    0x00,                           /* bInterfaceProtocol */
    0x00,                           /* bInterfaceSubClass */
    0x00,                           /* iInterface */

    /* 端点描述符 */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x82,                           /* bEndpointAddress */
    0x02,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP2_FS_SIZE, (uint8_t)( DEF_USB_EP2_FS_SIZE >> 8 ), /* wMaxPacketSize 512 */
    0x00,                           /* bInterval */

    /* 端点描述符 */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x03,                           /* bEndpointAddress */
    0x02,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP2_FS_SIZE, (uint8_t)( DEF_USB_EP2_FS_SIZE >> 8 ), /* wMaxPacketSize 512 */
    0x00,
};

/* Language Descriptor */
const uint8_t  MyLangDescr[ ] =
{
    0x04, 0x03, 0x09, 0x04
};

/* Manufacturer Descriptor */
const uint8_t  MyManuInfo[ ] =
{
    0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0
};

/* Product Information */
const uint8_t  MyProdInfo[ ] =
{
    0x12, 0x03, 'C', 0, 'H', 0, '3', 0, '2', 0, 'V', 0, '3', 0
              , '0', 0, 'x', 0
};

/* Serial Number Information */
const uint8_t  MySerNumInfo[ ] =
{
    0x16, 0x03, '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0
              , '6', 0, '7', 0, '8', 0, '9', 0
};

/* USB序列号字符串描述符 */
uint8_t ECM_StrDesc_Sn[ ] =
{
    /* Sn默认序列号字符串描述符 0x84 0xC2 0xE4 0x00 0x00 0x01  */
    /* 生成方法见 USB.c 中的 MACAddr_Change_To_SNDesc( ) 函数 */
    0x1A, 0x03, 0x38, 0x00, 0x34, 0x00, /* 84 */
                0x43, 0x00, 0x32, 0x00, /* C2 */
                0x45, 0x00, 0x34, 0x00, /* E4 */
                0x30, 0x00, 0x30, 0x00, /* 01 */
                0x30, 0x00, 0x30, 0x00, /* 00 */
                0x30, 0x00, 0x31, 0x00, /* 00 */
};

const uint8_t MyStrDesc_CDC_CTRL[ ]=
{
    /* CDC-Communication-Ctrl */
    0x2E, 0x03, 'C', 0x00, 'D', 0x00, 'C', 0x00, '-', 0x00, 'C', 0x00, 'o', 0x00, 'm', 0x00, 'm', 0x00,
                'u', 0x00, 'n', 0x00, 'i', 0x00, 'c', 0x00, 'a', 0x00, 't', 0x00, 'i', 0x00, 'o', 0x00,
                'n', 0x00, '-', 0x00, 'C', 0x00, 't', 0x00, 'r', 0x00, 'l', 0x00
};

/* Device Qualified Descriptor */
const uint8_t MyQuaDesc[ ] =
{
    0x0A, 0x06, 0x10, 0x02, 0x00, 0x00, 0x00, 0x40, 0x03, 0x00
};

/* Device BOS Descriptor */
const uint8_t MyBOSDesc[ ] =
{
    0x05, 0x0F, 0x0C, 0x00, 0x01,
    0x07, 0x10, 0x02, 0x02, 0x00, 0x00, 0x00,
};

/* USB Full-Speed Mode, Other speed configuration Descriptor */
uint8_t TAB_USB_FS_OSC_DESC[ sizeof(MyCfgDescr_HS) ] =
{
    /* Other parts are copied through the program */
    0x09, 0x07,
};

/* USB High-Speed Mode, Other speed configuration Descriptor */
uint8_t TAB_USB_HS_OSC_DESC[ sizeof(MyCfgDescr_FS) ] =
{
    /* Other parts are copied through the program */
    0x09, 0x07,
};
