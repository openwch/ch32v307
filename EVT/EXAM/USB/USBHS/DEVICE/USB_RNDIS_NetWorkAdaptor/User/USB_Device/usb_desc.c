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
#include "usbhs_rndis.h"

/* Device Descriptor */
const uint8_t  MyDevDescr[ ] =
{
    0x12,                           /* Size of this descriptor, in bytes */
    0x01,                           /* DEVICE descriptor */
    0x00,
    0x02,                           /* bcdUSB USB2.0 */
    0x02,                           /* Miscellaneous Device Class */
    0x02,                           /* Common Class */
    0x0D,                           /* Interface Association Descriptor */
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

/* Configuration descriptor */
const uint8_t  MyCfgDescr_HS[ ] =
{
    /* Configuration descriptor */
    0x09,                           /* bLength: Configuation Descriptor size */
    0x02,                           /* bDescriptorType: Configuration */
    0x4B,                           /* wTotalLength: Bytes returned */
    0x00,
    0x02,                           /* bNumInterfaces: 2 interface */
    0x01,                           /* bConfigurationValue: Configuration value */
    0x00,                           /* iConfiguration: Index of string descriptor describing the configuration */
    DEF_USB_ATTRIBUTES,             /* bmAttributes: Bus powered */
    DEF_USB_MAXPOWER,               /* MaxPower 100 mA: this current is used for detecting Vbus */

    /* IAD descriptor */
    0x08,                           /* bLength */
    0x0B,                           /* bDescriptorType */
    0x00,                           /* bFirstInterface */
    0x02,                           /* bInterfaceCount */
    0xE0,                           /* bFunctionClass (Wireless Controller) */
    0x01,                           /* bFunctionSubClass */
    0x03,                           /* bFunctionProtocol */
    0x00,                           /* iFunction */

    /* Interface 0 descriptor */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x00,                           /* bInterfaceNumber */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x01,                           /* bNumEndpoints */
    0xE0,                           /* bInterfaceClass: Wireless Controller */
    0x01,                           /* bInterfaceSubClass: */
    0x03,                           /* bInterfaceProtocol: */
    0x00,                           /* iInterface */

    /* Header Functional Descriptor */
    0x05,                           /* bFunctionLength */
    0x24,                           /* bDescriptorType = CS Interface */
    0x00,                           /* bDescriptorSubtype */
    0x10,
    0x01,                           /* bcdCDC = 1.10 */

    /* Call Management Functional Descriptor */
    0x05,                           /* bFunctionLength */
    0x24,                           /* bDescriptorType = CS Interface */
    0x01,                           /* bDescriptorSubtype = Call Management */
    0x00,                           /* bmCapabilities */
    0x01,                           /* bDataInterface */

    /* Abstract Control Management Functional Descriptor */
    0x04,                           /* bFunctionLength */
    0x24,                           /* bDescriptorType = CS Interface */
    0x02,                           /* bDescriptorSubtype = Abstract Control Management */
    0x00,                           /* bmCapabilities = Device supports the notification Network_Connection */

    /* Union Functional Descriptor */
    0x05,                           /* bFunctionLength */
    0x24,                           /* bDescriptorType = CS Interface */
    0x06,                           /* bDescriptorSubtype = Union */
    0x00,                           /* bControlInterface = "RNDIS Communications Control" */
    0x01,                           /* bSubordinateInterface0 = "RNDIS Ethernet Data" */

    /* Endpoint descriptors for Communication Class Interface */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    RNDIS_NOTIFICATION_IN_EP,       /* bEndpointAddress */
    0x03,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP1_HS_SIZE, (uint8_t)( DEF_USB_EP1_HS_SIZE >> 8 ), /* wMaxPacketSize 64 */
    0x01,                           /* bInterval */

    /* Interface 1 descriptor */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x01,                           /* bInterfaceNumber: 0x01 */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x02,                           /* bNumEndpoints: 0 */
    0x0A,                           /* bInterfaceClass: CDC: */
    0x00,                           /* bInterfaceProtocol: */
    0x00,                           /* bInterfaceSubClass */
    0x00,                           /* iInterface */

    /* Endpoint descriptors for Data Class Interface */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    RNDIS_DATA_IN_EP,               /* bEndpointAddress */
    0x02,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP2_HS_SIZE, (uint8_t)( DEF_USB_EP2_HS_SIZE >> 8 ), /* wMaxPacketSize 512 */
    0x00,                           /* bInterval */

    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    RNDIS_DATA_OUT_EP,              /* bEndpointAddress */
    0x02,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP3_HS_SIZE, (uint8_t)( DEF_USB_EP3_HS_SIZE >> 8 ), /* wMaxPacketSize 512 */
    0x00,
};

/* Configuration descriptor */
const uint8_t  MyCfgDescr_FS[ ] =
{
    /* Configuration descriptor */
    0x09,                           /* bLength: Configuation Descriptor size */
    0x02,                           /* bDescriptorType: Configuration */
    0x4B,                           /* wTotalLength: Bytes returned */
    0x00,
    0x02,                           /* bNumInterfaces: 2 interface */
    0x01,                           /* bConfigurationValue: Configuration value */
    0x00,                           /* iConfiguration: Index of string descriptor describing the configuration */
    DEF_USB_ATTRIBUTES,             /* bmAttributes: Bus powered */
    DEF_USB_MAXPOWER,               /* MaxPower 100 mA: this current is used for detecting Vbus */

    /* IAD descriptor */
    0x08,                           /* bLength */
    0x0B,                           /* bDescriptorType */
    0x00,                           /* bFirstInterface */
    0x02,                           /* bInterfaceCount */
    0xE0,                           /* bFunctionClass (Wireless Controller) */
    0x01,                           /* bFunctionSubClass */
    0x03,                           /* bFunctionProtocol */
    0x00,                           /* iFunction */

    /* Interface 0 descriptor */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x00,                           /* bInterfaceNumber */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x01,                           /* bNumEndpoints */
    0xE0,                           /* bInterfaceClass: Wireless Controller */
    0x01,                           /* bInterfaceSubClass: */
    0x03,                           /* bInterfaceProtocol: */
    0x00,                           /* iInterface */

    /* Header Functional Descriptor */
    0x05,                           /* bFunctionLength */
    0x24,                           /* bDescriptorType = CS Interface */
    0x00,                           /* bDescriptorSubtype */
    0x10,
    0x01,                           /* bcdCDC = 1.10 */

    /* Call Management Functional Descriptor */
    0x05,                           /* bFunctionLength */
    0x24,                           /* bDescriptorType = CS Interface */
    0x01,                           /* bDescriptorSubtype = Call Management */
    0x00,                           /* bmCapabilities */
    0x01,                           /* bDataInterface */

    /* Abstract Control Management Functional Descriptor */
    0x04,                           /* bFunctionLength */
    0x24,                           /* bDescriptorType = CS Interface */
    0x02,                           /* bDescriptorSubtype = Abstract Control Management */
    0x00,                           /* bmCapabilities = Device supports the notification Network_Connection */

    /* Union Functional Descriptor */
    0x05,                           /* bFunctionLength */
    0x24,                           /* bDescriptorType = CS Interface */
    0x06,                           /* bDescriptorSubtype = Union */
    0x00,                           /* bControlInterface = "RNDIS Communications Control" */
    0x01,                           /* bSubordinateInterface0 = "RNDIS Ethernet Data" */

    /* Endpoint descriptors for Communication Class Interface */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    RNDIS_NOTIFICATION_IN_EP,       /* bEndpointAddress */
    0x03,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP1_FS_SIZE, (uint8_t)( DEF_USB_EP1_FS_SIZE >> 8 ), /* wMaxPacketSize 64 */
    0x01,                           /* bInterval */

    /* Interface 1 descriptor */
    0x09,                           /* bLength */
    0x04,                           /* bDescriptorType */
    0x01,                           /* bInterfaceNumber: 0x01 */
    0x00,                           /* bAlternateSetting: 0x00 */
    0x02,                           /* bNumEndpoints: 0 */
    0x0A,                           /* bInterfaceClass: CDC: */
    0x00,                           /* bInterfaceProtocol: */
    0x00,                           /* bInterfaceSubClass */
    0x00,                           /* iInterface */

    /* Endpoint descriptors for Data Class Interface */
    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    RNDIS_DATA_IN_EP,               /* bEndpointAddress */
    0x02,                           /* bmAttributes */
    (uint8_t)DEF_USB_EP2_FS_SIZE, (uint8_t)( DEF_USB_EP2_FS_SIZE >> 8 ), /* wMaxPacketSize 512 */
    0x00,                           /* bInterval */

    0x07,                           /* bLength */
    0x05,                           /* bDescriptorType */
    RNDIS_DATA_OUT_EP,              /* bEndpointAddress */
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
uint8_t TAB_USB_HS_OSC_DESC[ sizeof(MyCfgDescr_HS) ] =
{
    /* Other parts are copied through the program */
    0x09, 0x07,
};
