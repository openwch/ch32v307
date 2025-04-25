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

/* Configuration Descriptor */
const uint8_t MyCfgDescr[] = 
{
    // Configuration Descriptor
    0x09,           // bLength
    0x02,           // bDescriptorType (Configuration)
    0x6E,0x00,      // wTotalLength (to be calculated based on the full length of the descriptors)
    0x02,           // bNumInterfaces (number of interfaces)
    0x01,           // bConfigurationValue
    0x00,           // iConfiguration
    0x80,           // bmAttributes: Bus-powered; Remote wakeup
    0x32,           // MaxPower: 100mA

    // Audio Control Interface Descriptor (Audio Control Interface)
    0x09,           // bLength
    0x04,           // bDescriptorType (Interface)
    0x00,           // bInterfaceNumber
    0x00,           // bAlternateSetting
    0x00,           // bNumEndpoints
    0x01,           // bInterfaceClass (Audio)
    0x01,           // bInterfaceSubClass (Audio Control)
    0x00,           // bInterfaceProtocol
    0x00,           // iInterface

    // Audio Control Interface Header Descriptor
    0x09,           // bLength
    0x24,           // bDescriptorType (CS Interface)
    0x01,           // bDescriptorSubtype (Header)
    0x00, 0x01,     // bcdADC (Audio Device Class Specification Version)
    0x28, 0x00,     // wTotalLength (Length of this descriptor plus all following descriptors in the control interface)
    0x01,           // bInCollection (Number of Audio Control Interfaces in this collection)
    0x01,           // baInterfaceNr[1] (Interface number of first audio control interface)

    // Input Terminal Descriptor
    0x0C,           // bLength
    0x24,           // bDescriptorType (CS Interface)
    0x02,           // bDescriptorSubtype (Input Terminal)
    0x04,           // bTerminalID (ID for this terminal)
    0x01, 0x02,     // wTerminalType (Microphone)
    0x00,           // bAssocTerminal (No associated terminal)
    0x02,           // bNrChannels (Number of channels: 2 channels)
    0x03, 0x00,     // wChannelConfig (Left and Right channels)
    0x00,           // iChannelNames (No channel names)
    0x00,           // iTerminal (No terminal name)

    // Feature Unit Descriptor
    0x0A,           // bLength
    0x24,           // bDescriptorType (CS Interface)
    0x06,           // bDescriptorSubtype (Feature Unit)
    0x05,           // bUnitID (Feature Unit ID)
    0x04,           // bSourceID (Source Terminal ID)
    0x01,           // bControlSize (1 byte for controls)
    0x01,           // Mute Control on Master Channel
    0x00,           // Left Front channel
    0x00,           // Right Front channel
    0x00,           // iFeature (No feature string)

    // Output Terminal Descriptor
    0x09,           // bLength
    0x24,           // bDescriptorType (CS Interface)
    0x03,           // bDescriptorSubtype (Output Terminal)
    0x06,           // bTerminalID (Output Terminal ID)
    0x01, 0x01,     // wTerminalType (USB Streaming Terminal Type)
    0x00,           // bAssocTerminal (No associated terminal)
    0x05,           // bSourceID (Source ID)
    0x00,           // iTerminal (No terminal string)


    // Audio Streaming Interface Descriptor (Alternate Setting 0)
    0x09,           // bLength
    0x04,           // bDescriptorType (Interface)
    0x01,           // bInterfaceNumber (Audio Streaming Interface)
    0x00,           // bAlternateSetting (Alternate Setting 0)
    0x00,           // bNumEndpoints (No endpoints in this setting)
    0x01,           // bInterfaceClass (Audio)
    0x02,           // bInterfaceSubClass (Audio Streaming)
    0x00,           // bInterfaceProtocol
    0x00,           // iInterface (No interface string)

    // Audio Streaming Interface Descriptor (Alternate Setting 1)
    0x09,           // bLength
    0x04,           // bDescriptorType (Interface)
    0x01,           // bInterfaceNumber
    0x01,           // bAlternateSetting (Alternate Setting 1)
    0x01,           // bNumEndpoints (1 endpoint)
    0x01,           // bInterfaceClass (Audio)
    0x02,           // bInterfaceSubClass (Audio Streaming)
    0x00,           // bInterfaceProtocol
    0x00,           // iInterface (No interface string)

    // Audio Streaming General Descriptor
    0x07,           // bLength
    0x24,           // bDescriptorType (CS Interface)
    0x01,           // bDescriptorSubtype (General)
    0x06,           // bTerminalLink (Link to terminal 6)
    0x00,           // bDelay (0 ms)
    0x01, 0x00,     // wFormatTag (PCM format)

    // Audio Streaming Format Type Descriptor
    0x0B,           // bLength
    0x24,           // bDescriptorType (CS Interface)
    0x02,           // bDescriptorSubtype (Format Type)
    0x01,           // bFormatType (Type I - PCM)
    0x02,           // bNrChannels (2 channels)
    0x02,           // bSubframeSize (2 bytes per frame)
    0x10,           // bBitResolution (16 bits per sample)
    0x01,           // bSamFreqType (1 frequency supported)
    0x80, 0xBB, 0x00, // tSamFreq (48 kHz)

    // Audio Streaming Endpoint Descriptor (ISO Data Endpoint)
    0x09,           // bLength
    0x05,           // bDescriptorType (Endpoint)
    0x83,           // bEndpointAddress (IN endpoint)
    0x0D,           // bmAttributes (Isochronous)
    0xC0, 0x00,     // wMaxPacketSize (192 bytes)
    0x01,           // bInterval (1 frame)
    0x00,           // bRefresh
    0x00,           // bSynchAddress

    // Audio Streaming Endpoint Descriptor (General Audio)
    0x07,           // bLength
    0x25,           // bDescriptorType (CS Endpoint)
    0x01,           // bDescriptorSubtype (General)
    0x00,           // bmAttributes
    0x01,           // bLockDelayUnits
    0x01, 0x00,     // wLockDelay (1 ms)
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
const uint8_t MyProdInfo[] =
{
    0x2A, 0x03, 
    'W', 0, 
    'C', 0, 
    'H', 0, 
    ' ', 0, 
    'U', 0, 
    'A', 0, 
    'C', 0, 
    '1', 0, 
    '0', 0, 
    ' ', 0, 
    'M', 0, 
    'i', 0, 
    'c', 0,
    'r', 0, 
    'o', 0,
    'p', 0, 
    'h', 0, 
    'o', 0, 
    'n', 0, 
    'e', 0
};

/* Serial Number Information */
const uint8_t  MySerNumInfo[ ] =
{
    0x16, 0x03, '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, 
    '8', 0, '9', 0
};

