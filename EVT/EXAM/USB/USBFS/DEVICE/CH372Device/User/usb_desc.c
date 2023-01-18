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
    0xFF,       // bDeviceClass
    0xFF,       // bDeviceSubClass
    0xFF,       // bDeviceProtocol
    DEF_USBD_UEP0_SIZE,   // bMaxPacketSize0 64
    (uint8_t)DEF_USB_VID, (uint8_t)(DEF_USB_VID >> 8),  // idVendor 0x1A86
    (uint8_t)DEF_USB_PID, (uint8_t)(DEF_USB_PID >> 8),  // idProduct 0x5537
    DEF_IC_PRG_VER, 0x00, // bcdDevice 0.01
    0x01,       // iManufacturer (String Index)
    0x02,       // iProduct (String Index)
    0x03,       // iSerialNumber (String Index)
    0x01,       // bNumConfigurations 1
};

/* Configuration Descriptor */
const uint8_t  MyCfgDescr[ ] =
{
    0x09,        // bLength
    0x02,        // bDescriptorType (Configuration)
    0x3C, 0x00,  // wTotalLength 60
    0x01,        // bNumInterfaces 1
    0x01,        // bConfigurationValue
    0x00,        // iConfiguration (String Index)
    0x80,        // bmAttributes
    0x32,        // bMaxPower 100mA

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x00,        // bInterfaceNumber 0
    0x00,        // bAlternateSetting
    0x06,        // bNumEndpoints 6
    0xFF,        // bInterfaceClass
    0x80,        // bInterfaceSubClass
    0x55,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x86,        // bEndpointAddress (IN/D2H)
    0x02,        // bmAttributes (Bulk)
    (uint8_t)DEF_USBD_EP6_FS_SIZE, (uint8_t)( DEF_USBD_EP6_FS_SIZE >> 8 ), // wMaxPacketSize 8
    0x00,        // bInterval 0 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x05,        // bEndpointAddress (OUT/H2D)
    0x02,        // bmAttributes (Bulk)
    (uint8_t)DEF_USBD_EP5_FS_SIZE, (uint8_t)( DEF_USBD_EP5_FS_SIZE >> 8 ), // wMaxPacketSize 8
    0x00,        // bInterval 0 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x84,        // bEndpointAddress (IN/D2H)
    0x02,        // bmAttributes (Bulk)
    (uint8_t)DEF_USBD_EP4_FS_SIZE, (uint8_t)( DEF_USBD_EP4_FS_SIZE >> 8 ), // wMaxPacketSize 8
    0x00,        // bInterval 0 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x03,        // bEndpointAddress (OUT/H2D)
    0x02,        // bmAttributes (Bulk)
    (uint8_t)DEF_USBD_EP3_FS_SIZE, (uint8_t)( DEF_USBD_EP3_FS_SIZE >> 8 ), // wMaxPacketSize 8
    0x00,        // bInterval 0 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x82,        // bEndpointAddress (IN/D2H)
    0x02,        // bmAttributes (Bulk)
    (uint8_t)DEF_USBD_EP2_FS_SIZE, (uint8_t)( DEF_USBD_EP2_FS_SIZE >> 8 ), // wMaxPacketSize 8
    0x00,        // bInterval 0 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x01,        // bEndpointAddress (OUT/H2D)
    0x02,        // bmAttributes (Bulk)
    (uint8_t)DEF_USBD_EP1_FS_SIZE, (uint8_t)( DEF_USBD_EP1_FS_SIZE >> 8 ), // wMaxPacketSize 8
    0x00,        // bInterval 0 (unit depends on device speed)
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

