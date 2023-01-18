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
    0x10, 0x01, // bcdUSB 1.10
    0x00,       // bDeviceClass
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
const uint8_t  MyCfgDescr[ ] =
{
    /* Configure descriptor */
    0x09, 0x02, 0x6B, 0x00, 0x03, 0x01, 0x00, 0x80, 0x32,

    /* IAD Descriptor(interface 0/1)*/
    0x08, 0x0B, 0x00, 0x02, 0x02, 0x02, 0x01, 0x00,

    /* Interface 0 (CDC) descriptor */
    0x09, 0x04, 0x00, 0x00, 0x01, 0x02, 0x02, 0x01,  0x00,

    /* Functional Descriptors */
    0x05, 0x24, 0x00, 0x10, 0x01,

    /* Length/management descriptor (data class interface 1) */
    0x05, 0x24, 0x01, 0x00, 0x01,
    0x04, 0x24, 0x02, 0x02,
    0x05, 0x24, 0x06, 0x00, 0x01,

    /* Interrupt upload endpoint descriptor */
    0x07, 0x05, 0x81, 0x03, (uint8_t)DEF_USBD_ENDP1_SIZE, (uint8_t)( DEF_USBD_ENDP1_SIZE >> 8 ), 0x01,

    /* Interface 1 (data interface) descriptor */
    0x09, 0x04, 0x01, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x00,

    /* Endpoint descriptor */
    0x07, 0x05, 0x02, 0x02, (uint8_t)DEF_USBD_ENDP2_SIZE, (uint8_t)( DEF_USBD_ENDP2_SIZE >> 8 ), 0x00,

    /* Endpoint descriptor */
    0x07, 0x05, 0x83, 0x02, (uint8_t)DEF_USBD_ENDP3_SIZE, (uint8_t)( DEF_USBD_ENDP3_SIZE >> 8 ), 0x00,

    /* interface 2 (HID interface) descriptor */
    0x09, 0x04, 0x02, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00,

    /* interface 2 HID descriptor */
    0x09, 0x21, 0x00, 0x01, 0x00, 0x01, 0x22, DEF_USBD_REPORT_DESC_LEN, 0x00,

    /* interface 2 endpoint descriptor*/
    0x07, 0x05, 0x84, 0x03, (uint8_t)DEF_USBD_ENDP4_SIZE, (uint8_t)( DEF_USBD_ENDP4_SIZE >> 8 ), 0x01,

    /* interface 2 endpoint descriptor */
    0x07, 0x05, 0x04, 0x03, (uint8_t)DEF_USBD_ENDP4_SIZE, (uint8_t)( DEF_USBD_ENDP4_SIZE >> 8 ), 0x01,
};

const uint8_t  MyHIDReportDesc[ ] =
{
    0x06, 0x00, 0xFF,               // Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,                     // Usage (0x01)
    0xA1, 0x01,                     // Collection (Application)
    0x09, 0x02,                     //   Usage (0x02)
    0x26, 0xFF, 0x00,               //   Logical Maximum (255)
    0x75, 0x08,                     //   Report Size (8)
    0x15, 0x00,                     //   Logical Minimum (0)
    0x95, 0x40,                     //   Report Count (64)
    0x81, 0x06,                     //   Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x02,                     //   Usage (0x02)
    0x15, 0x00,                     //   Logical Minimum (0)
    0x26, 0xFF, 0x00,               //   Logical Maximum (255)
    0x75, 0x08,                     //   Report Size (8)
    0x95, 0x40,                     //   Report Count (64)
    0x91, 0x06,                     //   Output (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,                           // End Collection
};

/* Language Descriptor */
const uint8_t  MyLangDescr[] =
{
    0x04, 0x03, 0x09, 0x04
};

/* Manufacturer Descriptor */
const uint8_t  MyManuInfo[] =
{
    0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0
};

/* Product Information */
const uint8_t  MyProdInfo[] =
{
    0x12, 0x03, 'C', 0, 'H', 0, '3', 0, '2', 0, 'V', 0, '3', 0
              , '0', 0, 'x', 0
};

/* Serial Number Information */
const uint8_t  MySerNumInfo[] =
{
    0x16, 0x03, '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0
              , '6', 0, '7', 0, '8', 0, '9', 0
};

