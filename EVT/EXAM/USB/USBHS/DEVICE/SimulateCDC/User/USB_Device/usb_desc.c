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
    0x12,       // bLength
    0x01,       // bDescriptorType (Device)
    0x00, 0x02, // bcdUSB 2.00
    0x02,       // bDeviceClass
    0x00,       // bDeviceSubClass
    0x00,       // bDeviceProtocol
    DEF_USBD_UEP0_SIZE,   // bMaxPacketSize0 64
    (uint8_t)DEF_USB_VID, (uint8_t)(DEF_USB_VID >> 8),  // idVendor 0x1A86
    (uint8_t)DEF_USB_PID, (uint8_t)(DEF_USB_PID >> 8),  // idProduct 0x5537
    DEF_IC_PRG_VER, 0x00, // bcdDevice 0.01
    0x01,       // iManufacturer (String Index)
    0x02,       // iProduct (String Index)
    0x03,       // iSerialNumber (String Index)
    0x01,       // bNumConfigurations 1
};

/* Configuration Descriptor (FS) */
const uint8_t  MyCfgDescr_FS[ ] =
{
    /* Configure descriptor */
    0x09, 0x02, 0x43, 0x00, 0x02, 0x01, 0x00, 0x80, 0x32,

    /* Interface 0 (CDC) descriptor */
    0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x01,  0x00,

    /* Functional Descriptors */
    0x05, 0x24, 0x00, 0x10, 0x01,

    /* Length/management descriptor (data class interface 1) */
    0x05, 0x24, 0x01, 0x00, 0x01,
    0x04, 0x24, 0x02, 0x02,
    0x05, 0x24, 0x06, 0x00, 0x01,

    /* Interrupt upload endpoint descriptor */
    0x07, 0x05, 0x83, 0x03, (uint8_t)DEF_USB_EP3_FS_SIZE, (uint8_t)( DEF_USB_EP3_FS_SIZE >> 8 ), 0x01,

    /* Interface 1 (data interface) descriptor */
    0x09, 0x04, 0x01, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x00,

    /* Endpoint descriptor */
    0x07, 0x05, 0x02, 0x02, (uint8_t)DEF_USB_EP2_FS_SIZE, (uint8_t)( DEF_USB_EP2_FS_SIZE >> 8 ), 0x00,

    /* Endpoint descriptor */
    0x07, 0x05, 0x82, 0x02, (uint8_t)DEF_USB_EP2_FS_SIZE, (uint8_t)( DEF_USB_EP2_FS_SIZE >> 8 ), 0x00,
};

/* Configuration Descriptor (HS) */
const uint8_t  MyCfgDescr_HS[ ] =
{
    /* Configure descriptor */
    0x09, 0x02, 0x43, 0x00, 0x02, 0x01, 0x00, 0x80, 0x32,

    /* Interface 0 (CDC) descriptor */
    0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x01, 0x00,

    /* CDC Functional Descriptors */
    0x05, 0x24, 0x00, 0x10, 0x01,

    /* Length/management descriptor (data class interface 1) */
    0x05, 0x24, 0x01, 0x00, 0x01,
    0x04, 0x24, 0x02, 0x02,
    0x05, 0x24, 0x06, 0x00, 0x01,

    /* Interrupt upload endpoint descriptor */
    0x07, 0x05, 0x83, 0x03, (uint8_t)DEF_USB_EP3_FS_SIZE, (uint8_t)( DEF_USB_EP3_FS_SIZE >> 8 ), 0x01,

    /* Interface 1 (data interface) descriptor */
    0x09, 0x04, 0x01, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x00,

    /* Endpoint descriptor */
    0x07, 0x05, 0x02, 0x02, (uint8_t)DEF_USB_EP2_HS_SIZE, (uint8_t)( DEF_USB_EP2_HS_SIZE >> 8 ), 0x00,

    /* Endpoint descriptor */
    0x07, 0x05, 0x82, 0x02, (uint8_t)DEF_USB_EP2_HS_SIZE, (uint8_t)( DEF_USB_EP2_HS_SIZE >> 8 ), 0x00,
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
    0x16, 0x03, 'U', 0x00, 'S', 0x00, 'B', 0x00, ' ', 0x00, 'S', 0x00, 'e', 0x00,
                'r', 0x00, 'i', 0x00, 'a', 0x00, 'l', 0x00
};

/* Serial Number Information */
const uint8_t  MySerNumInfo[ ] =
{
    0x16, 0x03, '0', 0x00, '1', 0x00, '2', 0x00, '3', 0x00, '4', 0x00, '5', 0x00
              , '6', 0x00, '7', 0x00, '8', 0x00, '9', 0x00
};

/* Device Qualified Descriptor */
const uint8_t MyQuaDesc[ ] =
{
    0x0A, 0x06, 0x00, 0x02, 0xFF, 0xFF, 0xFF, 0x40, 0x01, 0x00,
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
