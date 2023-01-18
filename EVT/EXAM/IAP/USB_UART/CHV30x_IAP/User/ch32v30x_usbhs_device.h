/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v30x_usbhs_device.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : USB2.0�����豸�������ͷ�ļ�
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32V30X_USBHS_DEVICE_H__
#define __CH32V30X_USBHS_DEVICE_H__

#include "debug.h"
#include "string.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL                    0
#endif

//#define pEP2_OUT_DataBuf    (Ep2Buffer)
//#define pEP2_IN_DataBuf     (Ep2Buffer[64])

#ifndef VOID
#define VOID                    void
#endif
#ifndef CONST
#define CONST                   const
#endif
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char           BOOLEAN;
#endif
#ifndef CHAR
typedef char                    CHAR;
#endif
#ifndef INT8
typedef char                    INT8;
#endif
#ifndef INT16
typedef short                   INT16;
#endif
#ifndef INT32
typedef long                    INT32;
#endif
#ifndef UINT8
typedef unsigned char           UINT8;
#endif
#ifndef UINT16
typedef unsigned short          UINT16;
#endif
#ifndef UINT32
typedef unsigned long           UINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile  UINT8V;
#endif
#ifndef UINT16V
typedef unsigned short volatile UINT16V;
#endif
#ifndef UINT32V
typedef unsigned long volatile  UINT32V;
#endif

#ifndef PVOID
typedef void                    *PVOID;
#endif
#ifndef PCHAR
typedef char                    *PCHAR;
#endif
#ifndef PCHAR
typedef const char              *PCCHAR;
#endif
#ifndef PINT8
typedef char                    *PINT8;
#endif
#ifndef PINT16
typedef short                   *PINT16;
#endif
#ifndef PINT32
typedef long                    *PINT32;
#endif
#ifndef PUINT8
typedef unsigned char           *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short          *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long           *PUINT32;
#endif
#ifndef PUINT8V
typedef volatile unsigned char  *PUINT8V;
#endif
#ifndef PUINT16V
typedef volatile unsigned short *PUINT16V;
#endif
#ifndef PUINT32V
typedef volatile unsigned long  *PUINT32V;
#endif


/* USB constant and structure define */

/* USB PID */
#ifndef USB_PID_SETUP
#define USB_PID_NULL            0x00    /* reserved PID */
#define USB_PID_SOF             0x05
#define USB_PID_SETUP           0x0D
#define USB_PID_IN              0x09
#define USB_PID_OUT             0x01
#define USB_PID_ACK             0x02
#define USB_PID_NAK             0x0A
#define USB_PID_STALL           0x0E
#define USB_PID_DATA0           0x03
#define USB_PID_DATA1           0x0B
#define USB_PID_PRE             0x0C
#endif

/* USB standard device request code */
#ifndef USB_GET_DESCRIPTOR
#define USB_GET_STATUS          0x00
#define USB_CLEAR_FEATURE       0x01
#define USB_SET_FEATURE         0x03
#define USB_SET_ADDRESS         0x05
#define USB_GET_DESCRIPTOR      0x06
#define USB_SET_DESCRIPTOR      0x07
#define USB_GET_CONFIGURATION   0x08
#define USB_SET_CONFIGURATION   0x09
#define USB_GET_INTERFACE       0x0A
#define USB_SET_INTERFACE       0x0B
#define USB_SYNCH_FRAME         0x0C
#endif

/* USB hub class request code */
#ifndef HUB_GET_DESCRIPTOR
#define HUB_GET_STATUS          0x00
#define HUB_CLEAR_FEATURE       0x01
#define HUB_GET_STATE           0x02
#define HUB_SET_FEATURE         0x03
#define HUB_GET_DESCRIPTOR      0x06
#define HUB_SET_DESCRIPTOR      0x07
#endif

/* USB HID class request code */
#ifndef HID_GET_REPORT
#define HID_GET_REPORT          0x01
#define HID_GET_IDLE            0x02
#define HID_GET_PROTOCOL        0x03
#define HID_SET_REPORT          0x09
#define HID_SET_IDLE            0x0A
#define HID_SET_PROTOCOL        0x0B
#endif

/* Bit define for USB request type */
#ifndef USB_REQ_TYP_MASK
#define USB_REQ_TYP_IN          0x80            /* control IN, device to host */
#define USB_REQ_TYP_OUT         0x00            /* control OUT, host to device */
#define USB_REQ_TYP_READ        0x80            /* control read, device to host */
#define USB_REQ_TYP_WRITE       0x00            /* control write, host to device */
#define USB_REQ_TYP_MASK        0x60            /* bit mask of request type */
#define USB_REQ_TYP_STANDARD    0x00
#define USB_REQ_TYP_CLASS       0x20
#define USB_REQ_TYP_VENDOR      0x40
#define USB_REQ_TYP_RESERVED    0x60
#define USB_REQ_RECIP_MASK      0x1F            /* bit mask of request recipient */
#define USB_REQ_RECIP_DEVICE    0x00
#define USB_REQ_RECIP_INTERF    0x01
#define USB_REQ_RECIP_ENDP      0x02
#define USB_REQ_RECIP_OTHER     0x03
#endif

/* USB request type for hub class request */
#ifndef HUB_GET_HUB_DESCRIPTOR
#define HUB_CLEAR_HUB_FEATURE   0x20
#define HUB_CLEAR_PORT_FEATURE  0x23
#define HUB_GET_BUS_STATE       0xA3
#define HUB_GET_HUB_DESCRIPTOR  0xA0
#define HUB_GET_HUB_STATUS      0xA0
#define HUB_GET_PORT_STATUS     0xA3
#define HUB_SET_HUB_DESCRIPTOR  0x20
#define HUB_SET_HUB_FEATURE     0x20
#define HUB_SET_PORT_FEATURE    0x23
#endif

/* Hub class feature selectors */
#ifndef HUB_PORT_RESET
#define HUB_C_HUB_LOCAL_POWER   0
#define HUB_C_HUB_OVER_CURRENT  1
#define HUB_PORT_CONNECTION     0
#define HUB_PORT_ENABLE         1
#define HUB_PORT_SUSPEND        2
#define HUB_PORT_OVER_CURRENT   3
#define HUB_PORT_RESET          4
#define HUB_PORT_POWER          8
#define HUB_PORT_LOW_SPEED      9
#define HUB_C_PORT_CONNECTION   16
#define HUB_C_PORT_ENABLE       17
#define HUB_C_PORT_SUSPEND      18
#define HUB_C_PORT_OVER_CURRENT 19
#define HUB_C_PORT_RESET        20
#endif

/* USB descriptor type */
#ifndef USB_DESCR_TYP_DEVICE
#define USB_DESCR_TYP_DEVICE    0x01
#define USB_DESCR_TYP_CONFIG    0x02
#define USB_DESCR_TYP_STRING    0x03
#define USB_DESCR_TYP_INTERF    0x04
#define USB_DESCR_TYP_ENDP      0x05
#define USB_DESCR_TYP_QUALIF    0x06
#define USB_DESCR_TYP_SPEED     0x07
#define USB_DESCR_TYP_OTG       0x09
#define USB_DESCR_TYP_BOS       0X0F
#define USB_DESCR_TYP_HID       0x21
#define USB_DESCR_TYP_REPORT    0x22
#define USB_DESCR_TYP_PHYSIC    0x23
#define USB_DESCR_TYP_CS_INTF   0x24
#define USB_DESCR_TYP_CS_ENDP   0x25
#define USB_DESCR_TYP_HUB       0x29
#endif

/* USB device class */
#ifndef USB_DEV_CLASS_HUB
#define USB_DEV_CLASS_RESERVED  0x00
#define USB_DEV_CLASS_AUDIO     0x01
#define USB_DEV_CLASS_COMMUNIC  0x02
#define USB_DEV_CLASS_HID       0x03
#define USB_DEV_CLASS_MONITOR   0x04
#define USB_DEV_CLASS_PHYSIC_IF 0x05
#define USB_DEV_CLASS_POWER     0x06
#define USB_DEV_CLASS_PRINTER   0x07
#define USB_DEV_CLASS_STORAGE   0x08
#define USB_DEV_CLASS_HUB       0x09
#define USB_DEV_CLASS_VEN_SPEC  0xFF
#endif

/* USB endpoint type and attributes */
#ifndef USB_ENDP_TYPE_MASK
#define USB_ENDP_DIR_MASK       0x80
#define USB_ENDP_ADDR_MASK      0x0F
#define USB_ENDP_TYPE_MASK      0x03
#define USB_ENDP_TYPE_CTRL      0x00
#define USB_ENDP_TYPE_ISOCH     0x01
#define USB_ENDP_TYPE_BULK      0x02
#define USB_ENDP_TYPE_INTER     0x03
#endif

#ifndef USB_DEVICE_ADDR
#define USB_DEVICE_ADDR             0x02
#endif
#ifndef DEFAULT_ENDP0_SIZE
#define DEFAULT_ENDP0_SIZE      8       /* default maximum packet size for endpoint 0 */
#endif
#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE         64      /* maximum packet size */
#endif
#ifndef USB_BO_CBW_SIZE
#define USB_BO_CBW_SIZE             0x1F
#define USB_BO_CSW_SIZE             0x0D
#endif
#ifndef USB_BO_CBW_SIG0
#define USB_BO_CBW_SIG0         0x55
#define USB_BO_CBW_SIG1         0x53
#define USB_BO_CBW_SIG2         0x42
#define USB_BO_CBW_SIG3         0x43
#define USB_BO_CSW_SIG0         0x55
#define USB_BO_CSW_SIG1         0x53
#define USB_BO_CSW_SIG2         0x42
#define USB_BO_CSW_SIG3         0x53
#endif

#ifndef __PACKED
#define __PACKED            __attribute__((packed))
#endif

/* Global define */
#define IF_FULL_SPEED       1

#define USBHS_UEP0_SIZE     64
#if IF_FULL_SPEED
#define USBHS_MAX_PACK_SIZE 64
#else
#define USBHS_MAX_PACK_SIZE 64
#endif
/******************************************************************************/
/* USB�豸������غ궨�� */
// USB CONTROL
#define USBHS_DMA_EN        (1<<0)
#define USBHS_ALL_CLR       (1<<1)
#define USBHS_FORCE_RST     (1<<2)
#define USBHS_INT_BUSY_EN   (1<<3)
#define USBHS_DEV_PU_EN     (1<<4)
#define USBHS_SPEED_MASK    (3<<5)
#define USBHS_FULL_SPEED    (0<<5)
#define USBHS_HIGH_SPEED    (1<<5)
#define USBHS_LOW_SPEED     (2<<5)
#define USBHS_HOST_MODE     (1<<7)

// USB HOST_CTRL
#define USBHS_HOST_TX_EN          (1<<3)
#define USBHS_HOST_RX_EN          (1<<18)

#define USBHS_SEND_BUS_RESET      ((UINT32)(1<<0))
#define USBHS_SEND_BUS_SUSPEND    ((UINT32)(1<<1))
#define USBHS_SEND_BUS_RESUME     ((UINT32)(1<<2))
#define USBHS_REMOTE_WAKE         ((UINT32)(1<<3))
#define USBHS_SUSPENDM            ((UINT32)(1<<4))
#define USBHS_LINK_READY          ((UINT32)(1<<6))
#define USBHS_SEND_SOF_EN         ((UINT32)(1<<7))

// USB_INT_EN
#define USBHS_BUS_RST_EN          (1<<0)
#define USBHS_DETECT_EN           (1<<0)
#define USBHS_TRANSFER_EN         (1<<1)
#define USBHS_SUSPEND_EN          (1<<2)
#define USBHS_SOF_ACT_EN          (1<<3)
#define USBHS_FIFO_OV_EN          (1<<4)
#define USBHS_SETUP_ACT_EN        (1<<5)
#define USBHS_ISO_ACT_EN          (1<<6)
#define USBHS_DEV_NAK_EN          (1<<7)

// USB DEV AD

// USB FRAME_NO

// USB SUSPEND
#define USBHS_DEV_REMOTE_WAKEUP   (1<<2)
#define USBHS_LINESTATE_MASK      (2<<4)          /* Read Only */

// RESERVED0

// USB SPEED TYPE

// USB_MIS_ST
#define USBHS_SPLIT_CAN           (1<<0)
#define USBHS_ATTACH              (1<<1)
#define USBHS_SUSPEND             (1<<2)
#define USBHS_BUS_RESET           (1<<3)
#define USBHS_R_FIFO_RDY          (1<<4)
#define USBHS_SIE_FREE            (1<<5)
#define USBHS_SOF_ACT             (1<<6)
#define USBHS_SOF_PRES            (1<<7)

// INT_FLAG
#define USBHS_BUS_RST_FLAG        (1<<0)
#define USBHS_DETECT_FLAG         (1<<0)
#define USBHS_TRANSFER_FLAG       (1<<1)
#define USBHS_SUSPEND_FLAG        (1<<2)
#define USBHS_HST_SOF_FLAG        (1<<3)
#define USBHS_FIFO_OV_FLAG        (1<<4)
#define USBHS_SETUP_FLAG          (1<<5)
#define USBHS_ISO_ACT_FLAG        (1<<6)

// INT_ST
#define USBHS_DEV_UIS_IS_NAK      (1<<7)
#define USBHS_DEV_UIS_TOG_OK      (1<<6)
#define MASK_UIS_TOKEN            (3<<4)
#define MASK_UIS_ENDP             (0x0F)
#define MASK_UIS_H_RES            (0x0F)

//USB_RX_LEN

//UEP_CONFIG
#define USBHS_EP0_T_EN          (1<<0)
#define USBHS_EP0_R_EN          (1<<16)

#define USBHS_EP1_T_EN          (1<<1)
#define USBHS_EP1_R_EN          (1<<17)

#define USBHS_EP2_T_EN          (1<<2)
#define USBHS_EP2_R_EN          (1<<18)

#define USBHS_EP3_T_EN          (1<<3)
#define USBHS_EP3_R_EN          (1<<19)

#define USBHS_EP4_T_EN          (1<<4)
#define USBHS_EP4_R_EN          (1<<20)

#define USBHS_EP5_T_EN          (1<<5)
#define USBHS_EP5_R_EN          (1<<21)

#define USBHS_EP6_T_EN          (1<<6)
#define USBHS_EP6_R_EN          (1<<22)

#define USBHS_EP7_T_EN          (1<<7)
#define USBHS_EP7_R_EN          (1<<23)

#define USBHS_EP8_T_EN          (1<<8)
#define USBHS_EP8_R_EN          (1<<24)

#define USBHS_EP9_T_EN          (1<<9)
#define USBHS_EP9_R_EN          (1<<25)

#define USBHS_EP10_T_EN         (1<<10)
#define USBHS_EP10_R_EN         (1<<26)

#define USBHS_EP11_T_EN         (1<<11)
#define USBHS_EP11_R_EN         (1<<27)

#define USBHS_EP12_T_EN         (1<<12)
#define USBHS_EP12_R_EN         (1<<28)

#define USBHS_EP13_T_EN         (1<<13)
#define USBHS_EP13_R_EN         (1<<29)

#define USBHS_EP14_T_EN         (1<<14)
#define USBHS_EP14_R_EN         (1<<30)

#define USBHS_EP15_T_EN         (1<<15)
#define USBHS_EP15_R_EN         (1<<31)

//UEP_TYPE
#define USBHS_EP0_T_TYP        (1<<0)
#define USBHS_EP0_R_TYP        (1<<16)

#define USBHS_EP1_T_TYP        (1<<1)
#define USBHS_EP1_R_TYP        (1<<17)

#define USBHS_EP2_T_TYP        (1<<2)
#define USBHS_EP2_R_TYP        (1<<18)

#define USBHS_EP3_T_TYP        (1<<3)
#define USBHS_EP3_R_TYP        (1<<19)

#define USBHS_EP4_T_TYP        (1<<4)
#define USBHS_EP4_R_TYP        (1<<20)

#define USBHS_EP5_T_TYP        (1<<5)
#define USBHS_EP5_R_TYP        (1<<21)

#define USBHS_EP6_T_TYP        (1<<6)
#define USBHS_EP6_R_TYP        (1<<22)

#define USBHS_EP7_T_TYP        (1<<7)
#define USBHS_EP7_R_TYP        (1<<23)

#define USBHS_EP8_T_TYP        (1<<8)
#define USBHS_EP8_R_TYP        (1<<24)

#define USBHS_EP9_T_TYP        (1<<8)
#define USBHS_EP9_R_TYP        (1<<25)

#define USBHS_EP10_T_TYP       (1<<10)
#define USBHS_EP10_R_TYP       (1<<26)

#define USBHS_EP11_T_TYP       (1<<11)
#define USBHS_EP11_R_TYP       (1<<27)

#define USBHS_EP12_T_TYP       (1<<12)
#define USBHS_EP12_R_TYP       (1<<28)

#define USBHS_EP13_T_TYP       (1<<13)
#define USBHS_EP13_R_TYP       (1<<29)

#define USBHS_EP14_T_TYP       (1<<14)
#define USBHS_EP14_R_TYP       (1<<30)

#define USBHS_EP15_T_TYP       (1<<15)
#define USBHS_EP15_R_TYP       (1<<31)


/* BUF_MOD UEP1~15 */
#define USBHS_EP0_BUF_MOD      (1<<0)
#define USBHS_EP0_ISO_BUF_MOD  (1<<16)

#define USBHS_EP1_BUF_MOD      (1<<1)
#define USBHS_EP1_ISO_BUF_MOD  (1<<17)

#define USBHS_EP2_BUF_MOD      (1<<2)
#define USBHS_EP2_ISO_BUF_MOD  (1<<18)

#define USBHS_EP3_BUF_MOD      (1<<3)
#define USBHS_EP3_ISO_BUF_MOD  (1<<19)

#define USBHS_EP4_BUF_MOD      (1<<4)
#define USBHS_EP4_ISO_BUF_MOD  (1<<20)

#define USBHS_EP5_BUF_MOD      (1<<5)
#define USBHS_EP5_ISO_BUF_MOD  (1<<21)

#define USBHS_EP6_BUF_MOD      (1<<6)
#define USBHS_EP6_ISO_BUF_MOD  (1<<22)

#define USBHS_EP7_BUF_MOD      (1<<7)
#define USBHS_EP7_ISO_BUF_MOD  (1<<23)

#define USBHS_EP8_BUF_MOD      (1<<8)
#define USBHS_EP8_ISO_BUF_MOD  (1<<24)

#define USBHS_EP9_BUF_MOD      (1<<9)
#define USBHS_EP9_ISO_BUF_MOD  (1<<25)

#define USBHS_EP10_BUF_MOD     (1<<10)
#define USBHS_EP10_ISO_BUF_MOD (1<<26)

#define USBHS_EP11_BUF_MOD     (1<<11)
#define USBHS_EP11_ISO_BUF_MOD (1<<27)

#define USBHS_EP12_BUF_MOD     (1<<12)
#define USBHS_EP12_ISO_BUF_MOD (1<<28)

#define USBHS_EP13_BUF_MOD     (1<<13)
#define USBHS_EP13_ISO_BUF_MOD (1<<29)

#define USBHS_EP14_BUF_MOD     (1<<14)
#define USBHS_EP14_ISO_BUF_MOD (1<<30)

#define USBHS_EP15_BUF_MOD     (1<<15)
#define USBHS_EP15_ISO_BUF_MOD (1<<31)
//USBHS_EPn_T_EN  USBHS_EPn_R_EN  USBHS_EPn_BUF_MOD
//      0               0               x
//      1               0               0
//      1               0               1
//      0               1               0
//      0               1               1

// UEPn_T_LEN
#define USBHS_EP_T_LEN_MASK       (0x7FF)

//UEPn_TX_CTRL
#define USBHS_EP_T_RES_MASK       (3<<0)
    #define USBHS_EP_T_RES_ACK        (0<<0)
    #define USBHS_EP_T_RES_NYET       (1<<0)
    #define USBHS_EP_T_RES_NAK        (2<<0)
    #define USBHS_EP_T_RES_STALL      (3<<0)

#define USBHS_EP_T_TOG_MASK       (3<<3)
    #define USBHS_EP_T_TOG_0          (0<<3)
    #define USBHS_EP_T_TOG_1          (1<<3)
    #define USBHS_EP_T_TOG_2          (2<<3)
    #define USBHS_EP_T_TOG_M          (3<<3)

#define USBHS_EP_T_AUTOTOG        (1<<5)

//UEPn_RX_CTRL
#define USBHS_EP_R_RES_MASK       (3<<0)
    #define USBHS_EP_R_RES_ACK        (0<<0)
    #define USBHS_EP_R_RES_NYET       (1<<0)
    #define USBHS_EP_R_RES_NAK        (2<<0)
    #define USBHS_EP_R_RES_STALL      (3<<0)

#define USBHS_EP_R_TOG_MASK       (3<<3)
    #define USBHS_EP_R_TOG_0          (0<<3)
    #define USBHS_EP_R_TOG_1          (1<<3)
    #define USBHS_EP_R_TOG_2          (2<<3)
    #define USBHS_EP_R_TOG_M          (3<<3)

#define USBHS_EP_R_AUTOTOG       (1<<5)

#define USBHS_TOG_MATCH               (1<<6)

// 00: OUT, 01:SOF, 10:IN, 11:SETUP
#define PIDhs_OUT             0
#define PIDhs_SOF             1
#define PIDhs_IN              2
#define PIDhs_SETUP           3

#define USBHSD              ((USBHSD_TypeDef *) USBHS_BASE)
/******************************************************************************/
/* USBHS PHY Clock Config (RCC_CFGR2) */
#define USB_48M_CLK_SRC_MASK   (1<<31)
#define USB_48M_CLK_SRC_SYS    (0<<31)
#define USB_48M_CLK_SRC_PHY    (1<<31)

#define USBHS_PLL_ALIVE        (1<<30)

#define USBHS_PLL_CKREF_MASK   (3<<28)
#define USBHS_PLL_CKREF_3M     (0<<28)
#define USBHS_PLL_CKREF_4M     (1<<28)
#define USBHS_PLL_CKREF_8M     (2<<28)
#define USBHS_PLL_CKREF_5M     (3<<28)

#define USBHS_PLL_SRC_MASK     (1<<27)
#define USBHS_PLL_SRC_HSE      (0<<27)
#define USBHS_PLL_SRC_HSI      (1<<27)

#define USBHS_PLL_SRC_PRE_MASK (7<<24)
#define USBHS_PLL_SRC_PRE_DIV1 (0<<24)
#define USBHS_PLL_SRC_PRE_DIV2 (1<<24)
#define USBHS_PLL_SRC_PRE_DIV3 (2<<24)
#define USBHS_PLL_SRC_PRE_DIV4 (3<<24)
#define USBHS_PLL_SRC_PRE_DIV5 (4<<24)
#define USBHS_PLL_SRC_PRE_DIV6 (5<<24)
#define USBHS_PLL_SRC_PRE_DIV7 (6<<24)
#define USBHS_PLL_SRC_PRE_DIV8 (7<<24)

/******************************************************************************/
#define DEF_USB_EP0_SIZE           64
#define DEF_USB_EP1_SIZE           64
#define DEF_USB_FS_EP2_SIZE        64
#define DEF_USB_HS_EP2_SIZE        512
#define DEF_USB_FS_EP_SIZE         64
#define DEF_USB_HS_EP_SIZE         512

/******************************************************************************/
#define DEF_IC_PRG_VER             0x11
#define DEF_IC_PRG_VER2            0x00
/******************************************************************************/

extern __attribute__ ((aligned(4))) UINT8 Ep0Buffer[ USBHS_UEP0_SIZE ]; /* �˵�0�����շ������� */
extern __attribute__ ((aligned(4))) UINT8 Ep1Buffer[ USBHS_MAX_PACK_SIZE*2 ]; /* �˵�1���ݽ��ջ����� */
extern __attribute__ ((aligned(4))) UINT8 Ep2Buffer[ USBHS_MAX_PACK_SIZE*2 ]; /* �˵�1���ݷ��ͻ����� */
extern __attribute__ ((aligned(4))) UINT8 Ep3Buffer[ USBHS_MAX_PACK_SIZE*2 ]; /* �˵�2���ݽ��ջ����� */
extern const UINT8 *pDescr;
extern volatile UINT8  USBHS_Dev_SetupReqCode;                                  /* USB2.0�����豸Setup�������� */
extern volatile UINT16 USBHS_Dev_SetupReqLen;                                   /* USB2.0�����豸Setup������ */
extern volatile UINT8  USBHS_Dev_SetupReqValueH;                                /* USB2.0�����豸Setup��Value���ֽ� */
extern volatile UINT8  USBHS_Dev_Config;                                        /* USB2.0�����豸����ֵ */
extern volatile UINT8  USBHS_Dev_Address;                                       /* USB2.0�����豸��ֵַ */
extern volatile UINT8  USBHS_Dev_SleepStatus;                                   /* USB2.0�����豸˯��״̬ */
extern volatile UINT8  USBHS_Dev_EnumStatus;                                    /* USB2.0�����豸ö��״̬ */
extern volatile UINT8  USBHS_Dev_Endp0_Tog;                                     /* USB2.0�����豸�˵�0ͬ����־ */

extern volatile UINT16 USBHS_Endp1_Up_Flag;                                     /* USB2.0�����豸�˵�1�����ϴ�״̬: 0:����; 1:�����ϴ�; */
extern volatile UINT8  USBHS_Endp1_Down_Flag;                                   /* USB2.0�����豸�˵�1�´��ɹ���־ */
extern volatile UINT8  USBHS_Endp1_Down_Len;                                    /* USB2.0�����豸�˵�1�´����� */

extern volatile UINT16 USBHS_Endp2_Up_Flag;                                     /* USB2.0�����豸�˵�2�����ϴ�״̬: 0:����; 1:�����ϴ�; */
extern volatile UINT16 USBHS_Endp2_Up_LoadPtr;                                  /* USB2.0�����豸�˵�2�����ϴ�װ��ƫ�� */
extern volatile UINT8  USBHS_Endp2_Down_Flag;                                   /* USB2.0�����豸�˵�2�´��ɹ���־ */
extern u8 Endp1Busy;
extern u8 EP1_OUT_Flag;
extern u8 EP2_OUT_Flag;
extern u8 Endp3Busy;
extern u8 Flag_LED;
/********************************************************************************/
/* �������� */
extern void USBHS_RCC_Init( void );                                             /* USB2.0�����豸RCC��ʼ�� */
extern void USBHS_Device_Endp_Init ( void );                                    /* USB2.0�����豸�˵��ʼ�� */
extern void USBHS_Device_Init ( FunctionalState sta );                          /* USB2.0�����豸��ʼ�� */
void DevEPhs_IN_Deal(UINT8 l);
void DevEPhs_OUT_Deal(UINT8 l);
extern void USBHS_Device_SetAddress( UINT32 address );                          /* USB2.0�����豸�����豸��ַ */
extern void USBHS_IRQHandler( void );                                           /* USB2.0�����豸�жϷ������ */
extern void USBHS_Sleep_WakeUp_Cfg( void );                                     /* USB2.0�����豸˯�߻������� */

#ifdef __cplusplus
}
#endif

#endif

