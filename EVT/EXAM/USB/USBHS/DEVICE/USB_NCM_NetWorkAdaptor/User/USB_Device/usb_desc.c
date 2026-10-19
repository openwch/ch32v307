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
    0x02,                           /* Miscellaneous Device Class */
    0x0D,                           /* Common Class */
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

/* USBConfiguration descriptor (high speed) */
const uint8_t  MyCfgDescr_HS[ ] =
{
    /* Configuration descriptor */
    0x09,                           /* bLength: Configuation Descriptor size */
    0x02,                           /* bDescriptorType: Configuration */
    0x56,                           /* wTotalLength: Bytes returned */
    0x00,
    0x02,                           /* bNumInterfaces: 2 interface */
    0x01,                           /* bConfigurationValue: Configuration value */
    0x00,                           /* iConfiguration: Index of string descriptor describing the configuration */
    DEF_USB_ATTRIBUTES,             /* bmAttributes: Bus powered */
    DEF_USB_MAXPOWER,               /* MaxPower 100 mA: this current is used for detecting Vbus */

    /* Control interface (interface 0) descriptor */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x00,                           /* bInterfaceNumber */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x01,                           /* bNumEndpoints */
    0x02,                           /* bInterfaceClass: The interface implements the communication device/interface class */
    0x0D,                           /* bInterfaceSubClass: The interface implements the Ethernet networking control model subclass */
    0x00,                           /* bInterfaceProtocol: The interface is used without a class-specific protocol */
    0x05,                           /* iInterface */

    /* Header Functional Descriptor */
    0x05,                           /* bLength */
    0x24,                           /* bDescriptorType */
    0x00,                           /* bDescriptorSubType */
    0x10,                           /* bcdCDC: USBThe communication class defines version 1.10 of the communication specification */
    0x01,

    /* Union Functional Descriptor */
    0x05,                           /* bLength */
    0x24,                           /* bDescriptorType */
    0x06,                           /* bDescriptorSubType */
    0x00,                           /* bControlInterface: Interface number of the communication or data class interface */
    0x01,                           /* bSubordinateInterface0: Interface number of the subordinate interface */
    
    /* Ethernet Networking Functional Descriptor */
    0x0D,                           /* bLength */
    0x24,                           /* bDescriptorType: CS_INTERFACE */
    0x0F,                           /* bDescriptorSubType */
    0x04,                           /* iMACAddress: String descriptor index; ECM/NCM report the MAC address through the serial number descriptor */
    0x00,                           /* bmEthernetStatistics: The device does not support the GetEthernetStatistic request */
    0x00,
    0x00,
    0x00,
    0xEA,                           /* wMaxSegmentSize: Maximum segment size supported by the Ethernet device: 1514 */
    0x05,
    0x00,                           /* wNumberMCFilters: Multicast filter bitmap */
    0x00,
    0x00,                           /* bNumberPowerFilters: Number of pattern filters available for waking the host */

    /* NCM Functional Descriptor */
    0x06,                           /* bLength */
    0x24,                           /* bDescriptorType */
    0x1A,                           /* bDescriptorSubType */
    0x00,
    0x01,                           /* bcdNcmVersion */
    0x00,                           /* bmNetworkCapabilities */

    /* Endpoint descriptor */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x81,                           /* bEndpointAddress */
    0x03,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP1_HS_SIZE, (uint8_t)( DEF_USB_EP1_HS_SIZE >> 8 ), /* wMaxPacketSize 64 */
    0x08,                           /* bInterval */

    /* Data interface (interface 1) descriptor alt0 */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x01,                           /* bInterfaceNumber: 0x01 */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x00,                           /* bNumEndpoints: 0 */
    0x0A,                           /* bInterfaceClass: The interface implements the data interface class */
    0x00,                           /* bInterfaceProtocol: The interface is used without a class-specific protocol */
    0x01,                           /* bInterfaceSubClass */
    0x00,                           /* iInterface */

    /* Data interface (interface 1) descriptor alt1 */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x01,                           /* bInterfaceNumber: 0x01 */
    0x01,                           /* bAlternateSetting: 0x01 */
    0x02,                           /* bNumEndpoints: 2 */
    0x0A,                           /* bInterfaceClass */
    0x00,                           /* bInterfaceProtocol */
    0x01,                           /* bInterfaceSubClass */
    0x00,                           /* iInterface */

    /* Endpoint descriptor */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x82,                           /* bEndpointAddress */
    0x02,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP2_HS_SIZE, (uint8_t)( DEF_USB_EP2_HS_SIZE >> 8 ), /* wMaxPacketSize 512 */
    0x00,                           /* bInterval */

    /* Endpoint descriptor */
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
    /* Configuration descriptor */
    0x09,                           /* bLength: Configuation Descriptor size */
    0x02,                           /* bDescriptorType: Configuration */
    0x56,                           /* wTotalLength: Bytes returned */
    0x00,
    0x02,                           /* bNumInterfaces: 2 interface */
    0x01,                           /* bConfigurationValue: Configuration value */
    0x00,                           /* iConfiguration: Index of string descriptor describing the configuration */
    DEF_USB_ATTRIBUTES,             /* bmAttributes: Bus powered */
    DEF_USB_MAXPOWER,               /* MaxPower 100 mA: this current is used for detecting Vbus */

    /* Control interface (interface 0) descriptor */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x00,                           /* bInterfaceNumber */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x01,                           /* bNumEndpoints */
    0x02,                           /* bInterfaceClass: The interface implements the communication device/interface class */
    0x0D,                           /* bInterfaceSubClass: The interface implements the Ethernet networking control model subclass */
    0x00,                           /* bInterfaceProtocol: The interface is used without a class-specific protocol */
    0x05,                           /* iInterface */

    /* Header Functional Descriptor */
    0x05,                           /* bLength */
    0x24,                           /* bDescriptorType */
    0x00,                           /* bDescriptorSubType */
    0x10,                           /* bcdCDC: USBThe communication class defines version 1.10 of the communication specification */
    0x01,

    /* Union Functional Descriptor */
    0x05,                           /* bLength */
    0x24,                           /* bDescriptorType */
    0x06,                           /* bDescriptorSubType */
    0x00,                           /* bControlInterface: Interface number of the communication or data class interface */
    0x01,                           /* bSubordinateInterface0: Interface number of the subordinate interface */

    /* Ethernet Networking Functional Descriptor */
    0x0D,                           /* bLength */
    0x24,                           /* bDescriptorType: CS_INTERFACE */
    0x0F,                           /* bDescriptorSubType */
    0x04,                           /* iMACAddress: String descriptor index; ECM/NCM report the MAC address through the serial number descriptor */
    0x00,                           /* bmEthernetStatistics: The device does not support the GetEthernetStatistic request */
    0x00,
    0x00,
    0x00,
    0xEA,                           /* wMaxSegmentSize: Maximum segment size supported by the Ethernet device: 1514 */
    0x05,
    0x00,                           /* wNumberMCFilters: Multicast filter bitmap */
    0x00,
    0x00,                           /* bNumberPowerFilters: Number of pattern filters available for waking the host */

    /* NCM Functional Descriptor */
    0x06,                           /* bLength */
    0x24,                           /* bDescriptorType */
    0x1A,                           /* bDescriptorSubType */
    0x00,
    0x01,                           /* bcdNcmVersion */
    0x00,                           /* bmNetworkCapabilities */

    /* Endpoint descriptor */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x81,                           /* bEndpointAddress */
    0x03,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP1_FS_SIZE, (uint8_t)( DEF_USB_EP1_FS_SIZE >> 8 ), /* wMaxPacketSize 64 */
    0x08,                           /* bInterval */

    /* Data interface (interface 1) descriptor alt0 */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x01,                           /* bInterfaceNumber: 0x01 */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x00,                           /* bNumEndpoints: 0 */
    0x0A,                           /* bInterfaceClass: The interface implements the data interface class */
    0x00,                           /* bInterfaceProtocol: The interface is used without a class-specific protocol */
    0x01,                           /* bInterfaceSubClass */
    0x00,                           /* iInterface */

    /* Data interface (interface 1) descriptor alt1 */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x01,                           /* bInterfaceNumber: 0x01 */
    0x01,                           /* bAlternateSetting: 0x01 */
    0x02,                           /* bNumEndpoints: 2 */
    0x0A,                           /* bInterfaceClass */
    0x00,                           /* bInterfaceProtocol */
    0x01,                           /* bInterfaceSubClass */
    0x00,                           /* iInterface */

    /* Endpoint descriptor */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x82,                           /* bEndpointAddress */
    0x02,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP2_FS_SIZE, (uint8_t)( DEF_USB_EP2_FS_SIZE >> 8 ), /* wMaxPacketSize 512 */
    0x00,                           /* bInterval */

    /* Endpoint descriptor */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    0x03,                           /* bEndpointAddress */
    0x02,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP3_FS_SIZE, (uint8_t)( DEF_USB_EP3_FS_SIZE >> 8 ), /* wMaxPacketSize 512 */
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

/* USBSerial number string descriptor */
uint8_t ECM_StrDesc_Sn[ ] =
{
    /* SnDefault serial number string descriptor 0x84 0xC2 0xE4 0x00 0x00 0x01 */
    /* For how it is generated, see the MACAddr_Change_To_SNDesc() function in USB.c */
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
