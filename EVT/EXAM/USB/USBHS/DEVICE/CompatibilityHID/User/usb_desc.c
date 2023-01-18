/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2020/04/30
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
    0x12,                                               // bLength
    0x01,                                               // bDescriptorType (Device)
    0x00, 0x02,                                         // bcdUSB 2.00
    0x00,                                               // bDeviceClass (Use class information in the Interface Descriptors)
    0x00,                                               // bDeviceSubClass
    0x00,                                               // bDeviceProtocol
    DEF_USBD_UEP0_SIZE,                                 // bMaxPacketSize0
    (uint8_t)DEF_USB_VID, (uint8_t)(DEF_USB_VID >> 8),  // idVendor 0x1A86
    (uint8_t)DEF_USB_PID, (uint8_t)(DEF_USB_PID >> 8),  // idProduct 0xE6E1
    0x00, DEF_IC_PRG_VER,                               // bcdDevice 1.00
    0x01,                                               // iManufacturer (String Index)
    0x02,                                               // iProduct (String Index)
    0x03,                                               // iSerialNumber (String Index)
    0x01,                                               // bNumConfigurations 2
};

/* Configuration Descriptor(HS) */
const uint8_t  MyCfgDescr_HS[ ] =
{
    /* Configuration Descriptor */
    0x09,                           // bLength
    0x02,                           // bDescriptorType
    0x29, 0x00,                     // wTotalLength
    0x01,                           // bNumInterfaces
    0x01,                           // bConfigurationValue
    0x00,                           // iConfiguration (String Index)
    0x80,                           // bmAttributes
    0x23,                           // bMaxPower 70mA

    /* Interface Descriptor */
    0x09,                           // bLength
    0x04,                           // bDescriptorType (Interface)
    0x00,                           // bInterfaceNumber 0
    0x00,                           // bAlternateSetting
    0x02,                           // bNumEndpoints 2
    0x03,                           // bInterfaceClass
    0x00,                           // bInterfaceSubClass
    0x00,                           // bInterfaceProtocol
    0x00,                           // iInterface (String Index)

    /* HID Descriptor */
    0x09,                           // bLength
    0x21,                           // bDescriptorType
    0x11, 0x01,                     // bcdHID
    0x00,                           // bCountryCode
    0x01,                           // bNumDescriptors
    0x22,                           // bDescriptorType
    0x22, 0x00,                     // wDescriptorLength

    /* Endpoint Descriptor */
    0x07,                           // bLength
    0x05,                           // bDescriptorType
    0x01,                           // bEndpointAddress: OUT Endpoint 1
    0x03,                           // bmAttributes
    0x00, 0x02,                     // wMaxPacketSize
    0x01,                           // bInterval: 1mS

    /* Endpoint Descriptor */
    0x07,                           // bLength
    0x05,                           // bDescriptorType
    0x82,                           // bEndpointAddress: IN Endpoint 2
    0x03,                           // bmAttributes
    0x00, 0x02,                     // wMaxPacketSize
    0x01,                           // bInterval: 1mS
};


/* Configuration Descriptor */
const uint8_t  MyCfgDescr_FS[ ] =
{
    /* Configuration Descriptor */
    0x09,                           // bLength
    0x02,                           // bDescriptorType
    0x29, 0x00,                     // wTotalLength
    0x01,                           // bNumInterfaces
    0x01,                           // bConfigurationValue
    0x00,                           // iConfiguration (String Index)
    0x80,                           // bmAttributes Remote Wakeup
    0x23,                           // bMaxPower 70mA

    /* Interface Descriptor */
    0x09,                           // bLength
    0x04,                           // bDescriptorType (Interface)
    0x00,                           // bInterfaceNumber 0
    0x00,                           // bAlternateSetting
    0x02,                           // bNumEndpoints 2
    0x03,                           // bInterfaceClass
    0x00,                           // bInterfaceSubClass
    0x00,                           // bInterfaceProtocol
    0x00,                           // iInterface (String Index)

    /* HID Descriptor */
    0x09,                           // bLength
    0x21,                           // bDescriptorType
    0x11, 0x01,                     // bcdHID
    0x00,                           // bCountryCode
    0x01,                           // bNumDescriptors
    0x22,                           // bDescriptorType
    0x22, 0x00,                     // wDescriptorLength

    /* Endpoint Descriptor */
    0x07,                           // bLength
    0x05,                           // bDescriptorType
    0x01,                           // bEndpointAddress: OUT Endpoint 1
    0x03,                           // bmAttributes
    0x40, 0x00,                     // wMaxPacketSize
    0x01,                           // bInterval: 1mS

    /* Endpoint Descriptor */
    0x07,                           // bLength
    0x05,                           // bDescriptorType
    0x82,                           // bEndpointAddress: IN Endpoint 2
    0x03,                           // bmAttributes
    0x40, 0x00,                     // wMaxPacketSize
    0x01,                           // bInterval: 1mS
};

/* HID Report Descriptor */
const uint8_t  MyHIDReportDesc_HS[ ] =
{
    0x06, 0x00, 0xFF,               // Usage Page (Vendor Defined 0xFF00)
    0x09, 0x01,                     // Usage (0x01)
    0xA1, 0x01,                     // Collection (Application)
    0x09, 0x02,                     //   Usage (0x02)
    0x26, 0xFF, 0x00,               //   Logical Maximum (255)
    0x75, 0x20,                     //   Report Size (32)
    0x15, 0x00,                     //   Logical Minimum (0)
    0x95, 0x80,                     //   Report Count (128)
    0x81, 0x06,                     //   Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x02,                     //   Usage (0x02)
    0x15, 0x00,                     //   Logical Minimum (0)
    0x26, 0xFF, 0x00,               //   Logical Maximum (255)
    0x75, 0x20,                     //   Report Size (32)
    0x95, 0x80,                     //   Report Count (128)
    0x91, 0x06,                     //   Output (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,                           // End Collection
};

const uint8_t  MyHIDReportDesc_FS[ ] =
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
    0x12, 0x03, 'C', 0, 'H', 0, '3', 0, '2', 0, 'V', 0, '3', 0, '0', 0, 'x', 0
};

/* Serial Number Information */
const uint8_t  MySerNumInfo[] =
{
    0x16, 0x03, '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0
              , '6', 0, '7', 0, '8', 0, '9', 0
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
