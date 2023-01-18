/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_desc.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/20
 * Description        : header file of usb_desc.c
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef USER_USB_DESC_H_
#define USER_USB_DESC_H_

#include "debug.h"

/******************************************************************************/
/* global define */
/* file version */
#define DEF_FILE_VERSION             0x01
/* usb device info define  */
#define DEF_USB_VID                  0x1A86
#define DEF_USB_PID                  0x5397
/* USB device descriptor, device serial number£¨bcdDevice£© */
#define DEF_IC_PRG_VER               DEF_FILE_VERSION
/* USB Device Attributes */
#define DEF_USB_ATTRIBUTES           0x80
/* USB Device MaxPower */
#define DEF_USB_MAXPOWER             0x32

/******************************************************************************/
/* usb device endpoint size define */
#define DEF_USBD_UEP0_SIZE           64     /* usb hs/fs device end-point 0 size */
/* HS */
#define DEF_USBD_HS_PACK_SIZE        512    /* usb hs device max bluk/int pack size */
#define DEF_USBD_HS_ISO_PACK_SIZE    1024   /* usb hs device max iso pack size */
/* FS */
#define DEF_USBD_FS_PACK_SIZE        64     /* usb fs device max bluk/int pack size */
#define DEF_USBD_FS_ISO_PACK_SIZE    1023   /* usb fs device max iso pack size */
/* LS */
#define DEf_USBD_LS_UEP0_SIZE        8      /* usb ls device end-point 0 size */
#define DEF_USBD_LS_PACK_SIZE        64     /* usb ls device max int pack size */

/* HS end-point size */
#define DEF_USB_EP1_HS_SIZE          DEF_USBD_FS_PACK_SIZE
#define DEF_USB_EP2_HS_SIZE          DEF_USBD_HS_PACK_SIZE
#define DEF_USB_EP3_HS_SIZE          DEF_USBD_HS_PACK_SIZE
/* FS end-point size */
#define DEF_USB_EP1_FS_SIZE          DEF_USBD_FS_PACK_SIZE
#define DEF_USB_EP2_FS_SIZE          DEF_USBD_FS_PACK_SIZE
#define DEF_USB_EP3_FS_SIZE          DEF_USBD_FS_PACK_SIZE
/* LS end-point size */
/* ... */

/******************************************************************************/
/* usb device Descriptor length, length of usb descriptors, if one descriptor not
 * exists , set the length to 0  */
#define DEF_USBD_DEVICE_DESC_LEN     ((uint8_t)MyDevDescr[0])
#define DEF_USBD_CONFIG_FS_DESC_LEN  ((uint16_t)MyCfgDescr_FS[2] + (uint16_t)(MyCfgDescr_FS[3] << 8))
#define DEF_USBD_CONFIG_HS_DESC_LEN  ((uint16_t)MyCfgDescr_HS[2] + (uint16_t)(MyCfgDescr_HS[3] << 8))
#define DEF_USBD_REPORT_DESC_LEN     0
#define DEF_USBD_LANG_DESC_LEN       ((uint16_t)MyLangDescr[0])
#define DEF_USBD_MANU_DESC_LEN       ((uint16_t)MyManuInfo[0])
#define DEF_USBD_PROD_DESC_LEN       ((uint16_t)MyProdInfo[0])
#define DEF_USBD_SN_DESC_LEN         ((uint16_t)MySerNumInfo[0])
#define DEF_USBD_QUALFY_DESC_LEN     ((uint16_t)MyQuaDesc[0])
#define DEF_USBD_BOS_DESC_LEN        ((uint16_t)MyBOSDesc[2] + (uint16_t)(MyBOSDesc[3] << 8))
#define DEF_USBD_FS_OTH_DESC_LEN     (DEF_USBD_CONFIG_HS_DESC_LEN)
#define DEF_USBD_HS_OTH_DESC_LEN     (DEF_USBD_CONFIG_FS_DESC_LEN)
#define DEF_ECM_SN_LEN               ((uint16_t)ECM_StrDesc_Sn[0])
#define DEF_CDD_CTRL_LEN             ((uint16_t)MyStrDesc_CDC_CTRL[0])

/******************************************************************************/
/* external variables */
extern const uint8_t MyDevDescr[ ];
extern const uint8_t MyCfgDescr_FS[ ];
extern const uint8_t MyCfgDescr_HS[ ];
extern const uint8_t MyLangDescr[ ];
extern const uint8_t MyManuInfo[ ];
extern const uint8_t MyProdInfo[ ];
extern const uint8_t MySerNumInfo[ ];
extern const uint8_t MyQuaDesc[ ];
extern const uint8_t MyBOSDesc[ ];
extern uint8_t TAB_USB_FS_OSC_DESC[ ];
extern uint8_t TAB_USB_HS_OSC_DESC[ ];
extern uint8_t ECM_StrDesc_Sn[ ];
extern const uint8_t MyStrDesc_CDC_CTRL[ ];

#endif /* USER_USB_DESC_H_ */
