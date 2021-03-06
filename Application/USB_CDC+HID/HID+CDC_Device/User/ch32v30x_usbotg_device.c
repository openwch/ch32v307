/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_usbotg_device.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the USBOTG firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_usbotg_device.h"
#include "stdarg.h"
/* Global define */
/* OTH */
#define pMySetupReqPakHD        ((PUSB_SETUP_REQ)EP0_DatabufHD)
#define RepDescSize             62
#define DevEP0SIZE              8
#define PID_OUT                 0
#define PID_SOF                 1
#define PID_IN                  2
#define PID_SETUP               3


/******************************************************************************/
/* ćšć±ćé */
/* Endpoint Buffer */
__attribute__ ((aligned(4))) UINT8 EP0_DatabufHD[64]; //ep0(64)
__attribute__ ((aligned(4))) UINT8 EP1_DatabufHD[64+64];  //ep1_out(64)+ep1_in(64)
__attribute__ ((aligned(4))) UINT8 EP2_DatabufHD[64+64];  //ep2_out(64)+ep2_in(64)
__attribute__ ((aligned(4))) UINT8 EP3_DatabufHD[64+64];  //ep3_out(64)+ep3_in(64)
__attribute__ ((aligned(4))) UINT8 EP4_DatabufHD[64+64];  //ep4_out(64)+ep4_in(64)
__attribute__ ((aligned(4))) UINT8 EP5_DatabufHD[64+64];  //ep5_out(64)+ep5_in(64)
__attribute__ ((aligned(4))) UINT8 EP6_DatabufHD[64+64];  //ep6_out(64)+ep6_in(64)
__attribute__ ((aligned(4))) UINT8 EP7_DatabufHD[64+64];  //ep7_out(64)+ep7_in(64)

PUINT8  pEP0_RAM_Addr;                       //ep0(64)
PUINT8  pEP1_RAM_Addr;                       //ep1_out(64)+ep1_in(64)
PUINT8  pEP2_RAM_Addr;                       //ep2_out(64)+ep2_in(64)
PUINT8  pEP3_RAM_Addr;                       //ep3_out(64)+ep3_in(64)
PUINT8  pEP4_RAM_Addr;                       //ep4_out(64)+ep4_in(64)
PUINT8  pEP5_RAM_Addr;                       //ep5_out(64)+ep5_in(64)
PUINT8  pEP6_RAM_Addr;                       //ep6_out(64)+ep6_in(64)
PUINT8  pEP7_RAM_Addr;                       //ep7_out(64)+ep7_in(64)


const UINT8 *pDescr;
volatile UINT8  USBHD_Dev_SetupReqCode = 0xFF;                                  /* USB2.0é«éèźŸć€Setupććœä»€ç  */
volatile UINT16 USBHD_Dev_SetupReqLen = 0x00;                                   /* USB2.0é«éèźŸć€SetupćéżćșŠ */
volatile UINT8  USBHD_Dev_SetupReqValueH = 0x00;                                /* USB2.0é«éèźŸć€SetupćValueé«ć­è */
volatile UINT8  USBHD_Dev_Config = 0x00;                                        /* USB2.0é«éèźŸć€éçœźćŒ */
volatile UINT8  USBHD_Dev_Address = 0x00;                                       /* USB2.0é«éèźŸć€ć°ććŒ */
volatile UINT8  USBHD_Dev_SleepStatus = 0x00;                                   /* USB2.0é«éèźŸć€çĄç ç¶æ */
volatile UINT8  USBHD_Dev_EnumStatus = 0x00;                                    /* USB2.0é«éèźŸć€æäžŸç¶æ */
volatile UINT8  USBHD_Dev_Endp0_Tog = 0x01;                                     /* USB2.0é«éèźŸć€ç«Żçč0ćæ­„æ ćż */
volatile UINT8  USBHD_Dev_Speed = 0x01;                                         /* USB2.0é«éèźŸć€éćșŠ */

volatile UINT16 USBHD_Endp1_Up_Flag = 0x00;                                     /* USB2.0é«éèźŸć€ç«Żçč1æ°æźäžäŒ ç¶æ: 0:ç©șéČ; 1:æ­ŁćšäžäŒ ; */
volatile UINT8  USBHD_Endp1_Down_Flag = 0x00;                                   /* USB2.0é«éèźŸć€ç«Żçč1äžäŒ æćæ ćż */
volatile UINT8  USBHD_Endp1_Down_Len = 0x00;                                    /* USB2.0é«éèźŸć€ç«Żçč1äžäŒ éżćșŠ */
volatile BOOL   USBHD_Endp1_T_Tog = 0;                                          /* USB2.0é«éèźŸć€ç«Żçč1ćétogäœçż»èœŹ */
volatile BOOL   USBHD_Endp1_R_Tog = 0;

volatile UINT16 USBHD_Endp2_Up_Flag = 0x00;                                     /* USB2.0é«éèźŸć€ç«Żçč2æ°æźäžäŒ ç¶æ: 0:ç©șéČ; 1:æ­ŁćšäžäŒ ; */
volatile UINT16 USBHD_Endp2_Up_LoadPtr = 0x00;                                  /* USB2.0é«éèźŸć€ç«Żçč2æ°æźäžäŒ èŁèœœćç§» */
volatile UINT8  USBHD_Endp2_Down_Flag = 0x00;                                   /* USB2.0é«éèźŸć€ç«Żçč2äžäŒ æćæ ćż */

volatile UINT16 USBHD_Endp3_Up_Flag = 0x00;
volatile UINT8  USBHD_Endp3_Down_Flag = 0x00;
volatile UINT8  USBHD_Endp3_Down_Len = 0x00;
volatile BOOL   USBHD_Endp3_T_Tog = 0;
volatile BOOL   USBHD_Endp3_R_Tog = 0;

volatile UINT32V Endp2_send_seq = 0x00;
volatile UINT8   DevConfig;
volatile UINT8   SetupReqCode;
volatile UINT16  SetupReqLen;

volatile UINT16 USBHD_Endp4_Up_Flag = 0x00;                                     /* USB2.0é«éèźŸć€ç«Żçč4æ°æźäžäŒ ç¶æ: 0:ç©șéČ; 1:æ­ŁćšäžäŒ ; */
volatile UINT16 USBHD_Endp4_Up_LoadPtr = 0x00;                                  /* USB2.0é«éèźŸć€ç«Żçč4æ°æźäžäŒ èŁèœœćç§» */
volatile UINT8  USBHD_Endp4_Down_Flag = 0x00;                                   /* USB2.0é«éèźŸć€ç«Żçč4äžäŒ æćæ ćż */

struct{
    UINT8 dataRat[4];   //æłąçčç
    UINT8 stopBit;      //ćæ­ąäœ
    UINT8 parityType;   //æ ĄéȘäœ
    UINT8 dataBit;      //æææ°æźäœ
}LINECODINGST,*linecodingst=&LINECODINGST;

UINT32 volatile  Boundrate;

#define  SET_LINE_CODING                0X20            // Configures DTE rate, stop-bits, parity, and number-of-character
#define  GET_LINE_CODING                0X21            // This request allows the host to find out the currently configured line coding.
#define  SET_CONTROL_LINE_STATE         0X22            // This request generates RS-232/V.24 style control signals.


/******************************************************************************/
/* Device Descriptor */
const UINT8  MyDevDescrHD[] =
{
        0x12, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, DevEP0SIZE,
        0x86, 0x1A, 0xE1, 0xE6,
        0x00, 0x01, 0x01, 0x02, 0x00, 0x01,
};

/* USBæ„ćæèż°çŹŠ */
const UINT8  MyReportDescHD[ ] =
{
    0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x15, 0x00, 0x25, 0x01,
    0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x01, 0x95, 0x03, 0x75, 0x01,
    0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02, 0x95, 0x05, 0x75, 0x01, 0x91, 0x01, 0x95, 0x06,
    0x75, 0x08, 0x26, 0xFF, 0x00, 0x05, 0x07, 0x19, 0x00, 0x29, 0x91, 0x81, 0x00, 0xC0
};

/* Configration Descriptor */
const UINT8  MyCfgDescrHD[] =
{
        0x09, 0x02, 0x64, 0x00, 0x03, 0x01, 0x00, 0xA0, 0x32,/* Config Desc */

         /******************** IAD ********************/
        0x08,        // bLength: Interface Descriptor size
        0x0B,        // bDescriptorType: IAD
        0x00,        // bFirstInterface
        0x02,        // bInterfaceCount
        0x02,        // bFunctionClass: CDC
        0x02,        // bFunctionSubClass
        0x01,        // bFunctionProtocol
        0x00,        //!! string descriptor for this interface

        //CDC
        //æ„ćŁ0(CDC)æèż°çŹŠ
        0x09,0x04,0x00,0x00,0x01,0x02,0x02,0x01,0x00,             //CDCæ„ćŁæèż°çŹŠ(äžäžȘç«Żçč)
        //ćèœæèż°çŹŠ
        0x05,0x24,0x00,0x10,0x01,                                 //ćèœæèż°çŹŠïŒć€ŽïŒ
        0x05,0x24,0x01,0x00,0x00,                                 //çźĄçæèż°çŹŠ(æČĄææ°æźç±»æ„ćŁ) 03 01
        0x04,0x24,0x02,0x02,                                      //æŻæSet_Line_CodingăSet_Control_Line_StateăGet_Line_CodingÂĄÂąSerial_State
        0x05,0x24,0x06,0x00,0x01,                                 //çŒć·äžș0çCDCæ„ćŁïŒæ ć·äžș1çæ°æźç±»ćæ„ćŁ
        0x07,0x05,0x81,0x03,0x08,0x00,0xff,                       //äž­æ­äžäŒ ç«Żçčæèż°çŹŠ
        //æ„ćŁ1æèż°çŹŠ
        0x09,0x04,0x01,0x00,0x02,0x0a,0x00,0x00,0x00,             //æ°æźæ„ćŁæèż°çŹŠ
        0x07,0x05,0x04,0x02,0x40,0x00,0x00,                       //èŸćșç«Żçčæèż°çŹŠ
        0x07,0x05,0x84,0x02,0x40,0x00,0x00,                       //èŸć„ç«Żçčæèż°çŹŠ

        //éźç
        0x09, 0x04, 0x02, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,/* Interface Desc */
        0x09, 0x21, 0x10, 0x01, 0x00, 0x01, 0x22, RepDescSize , 0x00,/* HID Desc */
        0x07, 0x05, 0x83, 0x03, (UINT8)DEF_USB_FS_EP_SIZE, (UINT8)( DEF_USB_FS_EP_SIZE >> 8 ), 0x01,
};



/* Language Descriptor */
const UINT8  MyLangDescrHD[] =
{
    0x04, 0x03, 0x09, 0x04
};

/* Manufactor Descriptor */
const UINT8  MyManuInfoHD[] =
{
    0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0
};

/* Product Information */
const UINT8  MyProdInfoHD[] =
{
    0x0C, 0x03, 'C', 0, 'H', 0, '1', 0, '0', 0, 'x', 0
};

/* USBćșćć·ć­çŹŠäžČæèż°çŹŠ */
const UINT8  MySerNumInfoHD[ ] =
{
    /* 0123456789 */
    22,03,48,0,49,0,50,0,51,0,52,0,53,0,54,0,55,0,56,0,57,0
};

/* USBèźŸć€éćźæèż°çŹŠ */
const UINT8 MyUSBQUADescHD[ ] =
{
    0x0A, 0x06, 0x00, 0x02, 0xFF, 0x00, 0xFF, 0x40, 0x01, 0x00,
};

/* USBćšéæšĄćŒ,ć¶ä»éćșŠéçœźæèż°çŹŠ */
UINT8 TAB_USB_FS_OSC_DESC[ sizeof( MyCfgDescrHD ) ] =
{
    0x09, 0x07,                                                                 /* ć¶ä»éšćéèżçšćșć€ć¶ */
};

//CDCćæ°
UINT8 LineCoding[7]={0x00,0xe1,0x00,0x00,0x00,0x00,0x08};   //ćć§ćæłąçčçäžș57600ïŒ1ćæ­ąäœïŒæ æ ĄéȘäœïŒ8æ°æźäœ

UINT8 Receive_Uart_Buf[UART_REV_LEN];   //äžČćŁæ„æ¶çŒćČćș
UINT8 Uart_Input_Point = 0;   //ćŸȘçŻçŒćČćșćć„æéïŒæ»çșżć€äœéèŠćć§ćäžș0
UINT8 Uart_Output_Point = 0;  //ćŸȘçŻçŒćČćșććșæéïŒæ»çșżć€äœéèŠćć§ćäžș0
UINT8 UartByteCount = 0;     //ćœćçŒćČćșć©äœćŸćć­èæ°


UINT8 USBByteCount = 0;     //ä»ŁèĄšUSBç«Żçčæ„æ¶ć°çæ°æź
UINT8 USBBufOutPoint = 0;    //ćæ°æźæé

UINT8 UpPoint4_Busy  = 0;    //äžäŒ ç«ŻçčæŻćŠćżæ ćż

void OTG_FS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      USBOTG_FS_DeviceInit
 *
 * @brief   Initializes USB device.
 *
 * @return  none
 */
void USBDeviceInit( void )
{
    USBOTG_FS->BASE_CTRL = 0x00;

    USBOTG_FS->UEP4_1_MOD = USBHD_UEP4_RX_EN|USBHD_UEP4_TX_EN|USBHD_UEP1_RX_EN|USBHD_UEP1_TX_EN;
    USBOTG_FS->UEP2_3_MOD = USBHD_UEP2_RX_EN|USBHD_UEP2_TX_EN|USBHD_UEP3_RX_EN|USBHD_UEP3_TX_EN;
    USBOTG_FS->UEP5_6_MOD = USBHD_UEP5_RX_EN|USBHD_UEP5_TX_EN|USBHD_UEP6_RX_EN|USBHD_UEP6_TX_EN;
    USBOTG_FS->UEP7_MOD   = USBHD_UEP7_RX_EN|USBHD_UEP7_TX_EN;

    USBOTG_FS->UEP0_DMA = (UINT32)pEP0_RAM_Addr;
    USBOTG_FS->UEP1_DMA = (UINT32)pEP1_RAM_Addr;
    USBOTG_FS->UEP2_DMA = (UINT32)pEP2_RAM_Addr;
    USBOTG_FS->UEP3_DMA = (UINT32)pEP3_RAM_Addr;
    USBOTG_FS->UEP4_DMA = (UINT32)pEP4_RAM_Addr;
    USBOTG_FS->UEP5_DMA = (UINT32)pEP5_RAM_Addr;
    USBOTG_FS->UEP6_DMA = (UINT32)pEP6_RAM_Addr;
    USBOTG_FS->UEP7_DMA = (UINT32)pEP7_RAM_Addr;

    USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_RES_ACK;
    USBOTG_FS->UEP1_RX_CTRL = USBHD_UEP_R_RES_ACK;
    USBOTG_FS->UEP2_RX_CTRL = USBHD_UEP_R_RES_ACK;
    USBOTG_FS->UEP3_RX_CTRL = USBHD_UEP_R_RES_ACK;
    USBOTG_FS->UEP4_RX_CTRL = USBHD_UEP_R_RES_ACK;
    USBOTG_FS->UEP5_RX_CTRL = USBHD_UEP_R_RES_ACK;
    USBOTG_FS->UEP6_RX_CTRL = USBHD_UEP_R_RES_ACK;
    USBOTG_FS->UEP7_RX_CTRL = USBHD_UEP_R_RES_ACK;


    USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_RES_NAK;
    USBOTG_FS->UEP1_TX_LEN = 8;
    USBOTG_FS->UEP2_TX_LEN = 8;
    USBOTG_FS->UEP3_TX_LEN = 8;
    USBOTG_FS->UEP4_TX_LEN = 8;
    USBOTG_FS->UEP5_TX_LEN = 8;
    USBOTG_FS->UEP6_TX_LEN = 8;
    USBOTG_FS->UEP7_TX_LEN = 8;

    USBOTG_FS->UEP1_TX_CTRL = USBHD_UEP_T_RES_ACK;
    USBOTG_FS->UEP2_TX_CTRL = USBHD_UEP_T_RES_ACK|USBHD_UEP_AUTO_TOG;
    USBOTG_FS->UEP3_TX_CTRL = USBHD_UEP_T_RES_ACK|USBHD_UEP_AUTO_TOG;
    USBOTG_FS->UEP4_TX_CTRL = USBHD_UEP_T_RES_ACK|USBHD_UEP_AUTO_TOG;
    USBOTG_FS->UEP5_TX_CTRL = USBHD_UEP_T_RES_ACK|USBHD_UEP_AUTO_TOG;
    USBOTG_FS->UEP6_TX_CTRL = USBHD_UEP_T_RES_ACK|USBHD_UEP_AUTO_TOG;
    USBOTG_FS->UEP7_TX_CTRL = USBHD_UEP_T_RES_ACK|USBHD_UEP_AUTO_TOG;


    USBOTG_FS->INT_FG   = 0xFF;
    USBOTG_FS->INT_EN   = USBHD_UIE_SUSPEND | USBHD_UIE_BUS_RST | USBHD_UIE_TRANSFER;
    USBOTG_FS->DEV_ADDR = 0x00;
#if 0
    *(UINT8 *)( pEP1_IN_DataBuf + 0 ) = (UINT8)( 0 >> 24 );
    *(UINT8 *)( pEP1_IN_DataBuf + 1 ) = (UINT8)( 0 >> 16 );
    *(UINT8 *)( pEP1_IN_DataBuf + 2 ) = (UINT8)( 0 >> 8 );
    *(UINT8 *)( pEP1_IN_DataBuf + 3 ) = (UINT8)( 0 );

    R8_USB_CTRL = RB_UC_DEV_PU_EN | RB_UC_INT_BUSY | RB_UC_DMA_EN | RB_UC_LOW_SPEED;
    R8_UDEV_CTRL = RB_UD_PD_DIS|RB_UD_PORT_EN|RB_UD_LOW_SPEED;
#else
    USBOTG_FS->BASE_CTRL = USBHD_UC_DEV_PU_EN | USBHD_UC_INT_BUSY | USBHD_UC_DMA_EN;
    USBOTG_FS->UDEV_CTRL = USBHD_UD_PD_DIS|USBHD_UD_PORT_EN;
#endif

}

/*********************************************************************
 * @fn      USBOTG_RCC_Init
 *
 * @brief   Initializes the usbotg clock configuration.
 *
 * @return  none
 */
void USBOTG_RCC_Init( void )
{
    RCC_USBCLK48MConfig( RCC_USBCLK48MCLKSource_USBPHY );
    RCC_USBHSPLLCLKConfig( RCC_HSBHSPLLCLKSource_HSE );
    RCC_USBHSConfig( RCC_USBPLL_Div2 );
    RCC_USBHSPLLCKREFCLKConfig( RCC_USBHSPLLCKREFCLK_4M );
    RCC_USBHSPHYPLLALIVEcmd( ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_USBHS, ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_OTG_FS, ENABLE );
}

/*********************************************************************
 * @fn      USBOTG_Init
 *
 * @brief   Initializes the USBOTG full speed device.
 *
 * @return  none
 */
void USBOTG_Init( void )
{
    /* ç«ŻçčçŒćČćșćć§ć */
    pEP0_RAM_Addr = EP0_DatabufHD;
    pEP1_RAM_Addr = EP1_DatabufHD;
    pEP2_RAM_Addr = EP2_DatabufHD;
    pEP3_RAM_Addr = EP3_DatabufHD;
    pEP4_RAM_Addr = EP4_DatabufHD;
    pEP5_RAM_Addr = EP5_DatabufHD;
    pEP6_RAM_Addr = EP6_DatabufHD;
    pEP7_RAM_Addr = EP7_DatabufHD;
    /* äœżèœusbæ¶é */
    USBOTG_RCC_Init( );
    Delay_Us(100);
    /* usbèźŸć€ćć§ć */
    USBDeviceInit( );
    EXTEN->EXTEN_CTR |= EXTEN_USBD_PU_EN;
    /* äœżèœusbäž­æ­ */
    NVIC_EnableIRQ( OTG_FS_IRQn );
}


/*********************************************************************
 * @fn      OTG_FS_IRQHandler
 *
 * @brief   This function handles OTG_FS exception.
 *
 * @return  none
 */
void OTG_FS_IRQHandler( void )
{
    UINT8  len, chtype;
    UINT8  intflag, errflag = 0;

    intflag = USBOTG_FS->INT_FG;

    if( intflag & USBHD_UIF_TRANSFER ) //äŒ èŸćźæçæ ćż
    {
        switch ( USBOTG_FS->INT_ST & USBHD_UIS_TOKEN_MASK )
        {
            /* SETUPćć€ç */
            case USBHD_UIS_TOKEN_SETUP:
#if 0
                /* æć°ćœćUsbsetupćœä»€  */
                printf( "Setup Req :\n" );
                printf( "%02X ", pSetupReqPakHD->bRequestType );
                printf( "%02X ", pSetupReqPakHD->bRequest );
                printf( "%04X ", pSetupReqPakHD->wValue );
                printf( "%04X ", pSetupReqPakHD->wIndex );
                printf( "%04X ", pSetupReqPakHD->wLength );
                printf( "\n" );

#endif
                USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_TOG|USBHD_UEP_T_RES_NAK;
                USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_TOG|USBHD_UEP_R_RES_ACK;
                SetupReqLen  = pSetupReqPakHD->wLength;
                SetupReqCode = pSetupReqPakHD->bRequest;
                chtype = pSetupReqPakHD->bRequestType;
                len = 0;
                errflag = 0;
                /* ć€æ­ćœćæŻæ ćèŻ·æ±èżæŻć¶ä»èŻ·æ± */
                if ( ( pSetupReqPakHD->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                {
                    switch( SetupReqCode )
                  {
                   case GET_LINE_CODING://0x21 currently configured
                       pDescr=LineCoding;
                       len=sizeof(LineCoding);
                       len=SetupReqLen>=DEFAULT_ENDP0_SIZE ? DEFAULT_ENDP0_SIZE : SetupReqLen;//æŹæŹĄäŒ èŸéżćșŠ
                       memcpy(EP0_DatabufHD,pDescr,len);
                       pDescr += len;
                       break;
                   case SET_CONTROL_LINE_STATE:  //0x22  generates RS-232/V.24 style control signals
                       break;
                   case SET_LINE_CODING:      //0x20  Configure

                       break;
                   default:
                       len = 0xFF;         //ćœä»€äžæŻæ
                       break;
                   }
                }
                else
                {
                    /* ć€çæ ćUSBèŻ·æ±ć */
                    switch( SetupReqCode )
                    {



                     case USB_GET_DESCRIPTOR:
                        {
                            switch( ((pSetupReqPakHD->wValue)>>8) )
                            {
                                case USB_DESCR_TYP_DEVICE:
                                    /* è·ćèźŸć€æèż°çŹŠ */
                                    pDescr = MyDevDescrHD;
                                    len = MyDevDescrHD[0];
                                    break;

                                case USB_DESCR_TYP_CONFIG:
                                    /* è·ćéçœźæèż°çŹŠ */
                                    pDescr = MyCfgDescrHD;
                                    len = MyCfgDescrHD[2];
                                    break;

                                case USB_DESCR_TYP_STRING:
                                    /* è·ćć­çŹŠäžČæèż°çŹŠ */
                                    switch( (pSetupReqPakHD->wValue)&0xff )
                                    {
                                        case 0:
                                            /* èŻ­èšć­çŹŠäžČæèż°çŹŠ */
                                        pDescr = MyLangDescrHD;
                                        len = MyLangDescrHD[0];
                                            break;

                                        case 1:
                                            /* USBäș§ćć­çŹŠäžČæèż°çŹŠ */
                                            pDescr = MyManuInfoHD;
                                            len = MyManuInfoHD[0];
                                            break;

                                        case 2:
                                            /* USBäș§ćć­çŹŠäžČæèż°çŹŠ */
                                            pDescr = MyProdInfoHD;
                                            len = MyProdInfoHD[0];
                                            break;

                                        case 3:
                                            /* USBćșćć·ć­çŹŠäžČæèż°çŹŠ */
                                            pDescr = MySerNumInfoHD;
                                            len = sizeof( MySerNumInfoHD );
                                            break;

                                        default:
                                            errflag = 0xFF;
                                            break;
                                    }
                                    break;

                                case USB_DESCR_TYP_REPORT:
                                    /* USBèźŸć€æ„ćæèż°çŹŠ */
                                    pDescr = MyReportDescHD;
                                    len = sizeof( MyReportDescHD );
                                    break;

                                case USB_DESCR_TYP_QUALIF:
                                    /* èźŸć€éćźæèż°çŹŠ */
                                    pDescr = ( PUINT8 )&MyUSBQUADescHD[ 0 ];
                                    len = sizeof( MyUSBQUADescHD );
                                    break;

                                case USB_DESCR_TYP_SPEED:
                                    /* ć¶ä»éćșŠéçœźæèż°çŹŠ */
                                    /* ć¶ä»éćșŠéçœźæèż°çŹŠ */
                                    if( USBHD_Dev_Speed == 0x00 )
                                    {
                                      /* ćšéæšĄćŒ */
                                      memcpy( &TAB_USB_FS_OSC_DESC[ 2 ], &MyCfgDescrHD[ 2 ], sizeof( MyCfgDescrHD ) - 2 );
                                      pDescr = ( PUINT8 )&TAB_USB_FS_OSC_DESC[ 0 ];
                                      len = sizeof( TAB_USB_FS_OSC_DESC );
                                    }
                                    else
                                    {
                                      errflag = 0xFF;
                                    }
                                    break;

                                case USB_DESCR_TYP_BOS:
                                    /* BOSæèż°çŹŠ */
                                    /* USB2.0èźŸć€äžæŻæBOSæèż°çŹŠ */
                                    pDescr = MyReportDescHD;
                                    len = sizeof( MyReportDescHD );
                                    break;

                                default :
                                    errflag = 0xff;
                                    break;

                            }

                            if( SetupReqLen>len )
                            SetupReqLen = len;
                            len = (SetupReqLen >= DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                            memcpy( pEP0_DataBuf, pDescr, len );
                            pDescr += len;
                        }
                            break;

                        case USB_SET_ADDRESS:
                            /* èźŸçœźć°ć */
                            SetupReqLen = (pSetupReqPakHD->wValue)&0xff;
                            break;

                        case USB_GET_CONFIGURATION:
                            /* è·ćéçœźćŒ */
                            pEP0_DataBuf[0] = DevConfig;
                            if ( SetupReqLen > 1 ) SetupReqLen = 1;
                            break;

                        case USB_SET_CONFIGURATION:
                            /* èźŸçœźéçœźćŒ */
                            DevConfig = (pSetupReqPakHD->wValue)&0xff;
                            break;

                        case USB_CLEAR_FEATURE:
                            /* æžé€çčæ§ */
                            if ( ( pSetupReqPakHD->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                            {
                                /* æžé€ç«Żçč */
                                switch( (pSetupReqPakHD->wIndex)&0xff )
                                {
                                case 0x82:
                                    USBOTG_FS->UEP2_TX_CTRL = (USBOTG_FS->UEP2_TX_CTRL & ~( USBHD_UEP_T_TOG|USBHD_UEP_T_RES_MASK )) | USBHD_UEP_T_RES_NAK;
                                    break;

                                case 0x02:
                                    USBOTG_FS->UEP2_RX_CTRL = (USBOTG_FS->UEP2_RX_CTRL & ~( USBHD_UEP_R_TOG|USBHD_UEP_R_RES_MASK )) | USBHD_UEP_R_RES_ACK;
                                    break;

                                case 0x81:
                                    USBOTG_FS->UEP1_TX_CTRL = (USBOTG_FS->UEP1_TX_CTRL & ~( USBHD_UEP_T_TOG|USBHD_UEP_T_RES_MASK )) | USBHD_UEP_T_RES_NAK;
                                    break;

                                case 0x01:
                                    USBOTG_FS->UEP1_RX_CTRL = (USBOTG_FS->UEP1_RX_CTRL & ~( USBHD_UEP_R_TOG|USBHD_UEP_R_RES_MASK )) | USBHD_UEP_R_RES_ACK;
                                    break;
                                case 0x83:
                                    USBOTG_FS->UEP3_TX_CTRL = (USBOTG_FS->UEP3_TX_CTRL & ~( USBHD_UEP_T_TOG|USBHD_UEP_T_RES_MASK )) | USBHD_UEP_T_RES_NAK;
                                    break;

                                case 0x03:
                                    USBOTG_FS->UEP3_RX_CTRL = (USBOTG_FS->UEP3_RX_CTRL & ~( USBHD_UEP_R_TOG|USBHD_UEP_R_RES_MASK )) | USBHD_UEP_R_RES_ACK;
                                    break;

                                case 0x84:
                                    USBOTG_FS->UEP4_TX_CTRL = (USBOTG_FS->UEP4_TX_CTRL & ~( USBHD_UEP_T_TOG|USBHD_UEP_T_RES_MASK )) | USBHD_UEP_T_RES_NAK;
                                    break;

                               case 0x04:
                                    USBOTG_FS->UEP4_RX_CTRL = (USBOTG_FS->UEP4_RX_CTRL & ~( USBHD_UEP_R_TOG|USBHD_UEP_R_RES_MASK )) | USBHD_UEP_R_RES_ACK;
                                    break;
                                default:
                                    errflag = 0xFF;
                                    break;

                                }
                            }
                            else    errflag = 0xFF;
                            break;

                        case USB_SET_FEATURE:
                            /* èźŸçœźçčæ§ */
                            if( ( pMySetupReqPakHD->bRequestType & 0x1F ) == 0x00 )
                            {
                                /* èźŸçœźèźŸć€ */
                                if( pMySetupReqPakHD->wValue == 0x01 )
                                {
                                    if( MyCfgDescrHD[ 7 ] & 0x20 )
                                    {
                                        /* èźŸçœźć€éäœżèœæ ćż */
                                        USBHD_Dev_SleepStatus = 0x01;
                                    }
                                    else
                                    {
                                        errflag = 0xFF;
                                    }
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                            }
                            else if( ( pMySetupReqPakHD->bRequestType & 0x1F ) == 0x02 )
                            {
                                /* èźŸçœźç«Żçč */
                                if( pMySetupReqPakHD->wValue == 0x00 )
                                {
                                    /* èźŸçœźæćźç«ŻçčSTALL */
                                    switch( ( pMySetupReqPakHD->wIndex ) & 0xff )
                                    {
                                        case 0x82:
                                            /* èźŸçœźç«Żçč2 IN STALL */
                                            USBOTG_FS->UEP2_TX_CTRL = ( USBOTG_FS->UEP2_TX_CTRL &= ~USBHD_UEP_T_RES_MASK ) | USBHD_UEP_T_RES_STALL;
                                            //USBHS->UEP2_CTRL  = ( USBHS->UEP2_CTRL & ~USBHS_EP_T_RES_MASK ) | USBHS_EP_T_RES_STALL;
                                            break;

                                        case 0x02:
                                            /* èźŸçœźç«Żçč2 OUT Stall */
                                            USBOTG_FS->UEP2_RX_CTRL = ( USBOTG_FS->UEP2_RX_CTRL &= ~USBHD_UEP_R_RES_MASK ) | USBHD_UEP_R_RES_STALL;
                                            //USBHS->UEP2_CTRL  = ( USBHS->UEP2_CTRL & ~USBHS_EP_R_RES_MASK ) | USBHS_EP_R_RES_STALL;
                                            break;

                                        case 0x81:
                                            /* èźŸçœźç«Żçč1 IN STALL */
                                            USBOTG_FS->UEP1_TX_CTRL = ( USBOTG_FS->UEP1_TX_CTRL &= ~USBHD_UEP_T_RES_MASK ) | USBHD_UEP_T_RES_STALL;
                                            //USBHS->UEP1_CTRL  = ( USBHS->UEP1_CTRL & ~USBHS_EP_T_RES_MASK ) | USBHS_EP_T_RES_STALL;
                                            break;

                                        case 0x01:
                                            /* èźŸçœźç«Żçč1 OUT STALL */
                                            USBOTG_FS->UEP1_RX_CTRL = ( USBOTG_FS->UEP1_RX_CTRL &= ~USBHD_UEP_R_RES_MASK ) | USBHD_UEP_R_RES_STALL;
                                            //USBHS->UEP1_CTRL  = ( USBHS->UEP1_CTRL & ~USBHS_EP_R_RES_MASK ) | USBHS_EP_R_RES_STALL;
                                            break;

                                        case 0x83:
                                          /* èźŸçœźç«Żçč3 IN STALL */
                                           USBOTG_FS->UEP3_TX_CTRL = ( USBOTG_FS->UEP3_TX_CTRL &= ~USBHD_UEP_T_RES_MASK ) | USBHD_UEP_T_RES_STALL;
                                          //USBHS->UEP1_CTRL  = ( USBHS->UEP1_CTRL & ~USBHS_EP_T_RES_MASK ) | USBHS_EP_T_RES_STALL;
                                           break;

                                      case 0x03:
                                          /* èźŸçœźç«Żçč3 OUT STALL */
                                           USBOTG_FS->UEP3_RX_CTRL = ( USBOTG_FS->UEP3_RX_CTRL &= ~USBHD_UEP_R_RES_MASK ) | USBHD_UEP_R_RES_STALL;
                                          //USBHS->UEP1_CTRL  = ( USBHS->UEP1_CTRL & ~USBHS_EP_R_RES_MASK ) | USBHS_EP_R_RES_STALL;
                                           break;
                                      case 0x84:
                                        /* èźŸçœźç«Żçč4 IN STALL */
                                          USBOTG_FS->UEP4_TX_CTRL = ( USBOTG_FS->UEP4_TX_CTRL &= ~USBHD_UEP_T_RES_MASK ) | USBHD_UEP_T_RES_STALL;
                                        //USBHS->UEP1_CTRL  = ( USBHS->UEP1_CTRL & ~USBHS_EP_T_RES_MASK ) | USBHS_EP_T_RES_STALL;
                                          break;

                                    case 0x04:
                                        /* èźŸçœźç«Żçč4 OUT STALL */
                                         USBOTG_FS->UEP4_RX_CTRL = ( USBOTG_FS->UEP4_RX_CTRL &= ~USBHD_UEP_R_RES_MASK ) | USBHD_UEP_R_RES_STALL;
                                        //USBHS->UEP1_CTRL  = ( USBHS->UEP1_CTRL & ~USBHS_EP_R_RES_MASK ) | USBHS_EP_R_RES_STALL;
                                         break;
                                        default:
                                            errflag = 0xFF;
                                            break;
                                    }
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                            }
                            else
                            {
                                errflag = 0xFF;
                            }
                            break;

                        case USB_GET_INTERFACE:
                            /* è·ćæ„ćŁ */
                            pEP0_DataBuf[0] = 0x00;
                            if ( SetupReqLen > 1 ) SetupReqLen = 1;
                            break;

                        case USB_SET_INTERFACE:
                            /* èźŸçœźæ„ćŁ */
                            EP0_DatabufHD[ 0 ] = 0x00;
                            if( USBHD_Dev_SetupReqLen > 1 )
                            {
                                USBHD_Dev_SetupReqLen = 1;
                            }
                            break;

                        case USB_GET_STATUS:
                            /* æ čæźćœćç«Żçčćźéç¶æèżèĄćșç­ */
                            EP0_DatabufHD[ 0 ] = 0x00;
                            EP0_DatabufHD[ 1 ] = 0x00;
                            if( pMySetupReqPakHD->wIndex == 0x81 )
                            {
                                if( ( USBOTG_FS->UEP1_TX_CTRL & USBHD_UEP_T_RES_MASK ) == USBHD_UEP_T_RES_STALL )
                                {
                                    EP0_DatabufHD[ 0 ] = 0x01;
                                }
                            }
                            else if( pMySetupReqPakHD->wIndex == 0x01 )
                            {
                                if( ( USBOTG_FS->UEP1_RX_CTRL & USBHD_UEP_R_RES_MASK ) == USBHD_UEP_R_RES_STALL )
                                {
                                    EP0_DatabufHD[ 0 ] = 0x01;
                                }
                            }
                            else if( pMySetupReqPakHD->wIndex == 0x82 )
                            {
                                if( ( USBOTG_FS->UEP2_TX_CTRL & USBHD_UEP_T_RES_MASK ) == USBHD_UEP_T_RES_STALL )
                                {
                                    EP0_DatabufHD[ 0 ] = 0x01;
                                }
                            }
                            else if( pMySetupReqPakHD->wIndex == 0x02 )
                            {
                                if( ( USBOTG_FS->UEP2_RX_CTRL & USBHD_UEP_R_RES_MASK ) == USBHD_UEP_R_RES_STALL )
                                {
                                    EP0_DatabufHD[ 0 ] = 0x01;
                                }
                            }
                            else if( pMySetupReqPakHD->wIndex == 0x03 )
                           {
                               if( ( USBOTG_FS->UEP3_RX_CTRL & USBHD_UEP_R_RES_MASK ) == USBHD_UEP_R_RES_STALL )
                               {
                                   EP0_DatabufHD[ 0 ] = 0x01;
                               }
                           }
                           else if( pMySetupReqPakHD->wIndex == 0x83 )
                           {
                               if( ( USBOTG_FS->UEP3_TX_CTRL & USBHD_UEP_T_RES_MASK ) == USBHD_UEP_T_RES_STALL )
                               {
                                   EP0_DatabufHD[ 0 ] = 0x01;
                               }
                           }
                           else if( pMySetupReqPakHD->wIndex == 0x84 )
                          {
                              if( ( USBOTG_FS->UEP4_TX_CTRL & USBHD_UEP_T_RES_MASK ) == USBHD_UEP_T_RES_STALL )
                              {
                                  EP0_DatabufHD[ 0 ] = 0x01;
                              }
                          }
                           else if( pMySetupReqPakHD->wIndex == 0x04 )
                         {
                             if( ( USBOTG_FS->UEP4_RX_CTRL & USBHD_UEP_R_RES_MASK ) == USBHD_UEP_R_RES_STALL )
                             {
                                EP0_DatabufHD[ 0 ] = 0x01;
                             }
                         }
                            if( USBHD_Dev_SetupReqLen > 2 )
                            {
                                USBHD_Dev_SetupReqLen = 2;
                            }
                            break;

                        default:
                            errflag = 0xff;
                            break;
                    }
                }
                if( errflag == 0xff)
                {
#if 0
                    printf("uep0 stall\n");
#endif
                    USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_TOG|USBHD_UEP_T_RES_STALL;
                    USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_TOG|USBHD_UEP_R_RES_STALL;
                }
                else
                {
                    if( chtype & 0x80 )
                    {
                        len = (SetupReqLen>DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                        SetupReqLen -= len;
                    }
                    else  len = 0;

                    USBOTG_FS->UEP0_TX_LEN  = len;
                    USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_TOG|USBHD_UEP_T_RES_ACK;
                    USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_TOG|USBHD_UEP_R_RES_ACK;
                }
                break;

            case USBHD_UIS_TOKEN_IN:
                switch ( USBOTG_FS->INT_ST & ( USBHD_UIS_TOKEN_MASK | USBHD_UIS_ENDP_MASK ) )
                {
                    case USBHD_UIS_TOKEN_IN:
                        switch( SetupReqCode )
                        {
                            case USB_GET_DESCRIPTOR:
                                    len = SetupReqLen >= DevEP0SIZE ? DevEP0SIZE : SetupReqLen;
                                    memcpy( pEP0_DataBuf, pDescr, len );
                                    SetupReqLen -= len;
                                    pDescr += len;
                                    USBOTG_FS->UEP0_TX_LEN   = len;
                                    USBOTG_FS->UEP0_TX_CTRL ^= USBHD_UEP_T_TOG;
                                    break;

                            case USB_SET_ADDRESS:
                                    USBOTG_FS->DEV_ADDR = (USBOTG_FS->DEV_ADDR&USBHD_UDA_GP_BIT) | SetupReqLen;
                                    USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_RES_NAK;
                                    USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_RES_ACK;
                                    break;

                            default:
                                    USBOTG_FS->UEP0_TX_LEN = 0;
                                    USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_RES_NAK;
                                    USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_RES_ACK;
                                    break;

                        }
                        break;

                case USBHD_UIS_TOKEN_IN | 1:
                    USBHD_UEP1_T_LEN=0;
                    USBOTG_FS->UEP1_TX_CTRL = (USBOTG_FS->UEP1_TX_CTRL & ~USBHD_UEP_T_RES_MASK) | USBHD_UEP_T_RES_NAK;
                    USBHD_Endp1_Up_Flag = 0x00;
                    break;

                case USBHD_UIS_TOKEN_IN | 2:
                    USBOTG_FS->UEP2_TX_LEN =0;
                    USBOTG_FS->UEP2_TX_CTRL = (USBOTG_FS->UEP2_TX_CTRL & ~USBHD_UEP_T_RES_MASK) | USBHD_UEP_T_RES_NAK;
                    USBHD_Endp2_Up_Flag = 0x00;
                    break;

                case USBHD_UIS_TOKEN_IN | 3:
                    USBHD_UEP3_T_LEN=0;
                    USBOTG_FS->UEP3_TX_CTRL  = (USBOTG_FS->UEP3_TX_CTRL & ~USBHD_UEP_T_RES_MASK) | USBHD_UEP_T_RES_NAK;
                    USBHD_Endp3_Up_Flag = 0x00;
                    break;

                case USBHD_UIS_TOKEN_IN | 4:
                    UpPoint4_Busy = 0;
                    USBHD_UEP4_T_LEN=0;
                    USBOTG_FS->UEP4_TX_LEN =0;
                    USBOTG_FS->UEP4_TX_CTRL  = (USBOTG_FS->UEP4_TX_CTRL & ~USBHD_UEP_T_RES_MASK) | USBHD_UEP_T_RES_NAK;
                    USBHD_Endp4_Up_Flag = 0x00;
                    break;

                case USBHD_UIS_TOKEN_IN | 5:
                    USBOTG_FS->UEP5_TX_CTRL  = (USBOTG_FS->UEP5_TX_CTRL & ~USBHD_UEP_T_RES_MASK) | USBHD_UEP_T_RES_NAK;
                    break;

                case USBHD_UIS_TOKEN_IN | 6:
                    USBOTG_FS->UEP6_TX_CTRL  = (USBOTG_FS->UEP6_TX_CTRL & ~USBHD_UEP_T_RES_MASK) | USBHD_UEP_T_RES_NAK;
                    break;

                case USBHD_UIS_TOKEN_IN | 7:
                    USBOTG_FS->UEP7_TX_CTRL  = (USBOTG_FS->UEP7_TX_CTRL & ~USBHD_UEP_T_RES_MASK) | USBHD_UEP_T_RES_NAK;
                    break;

                default :
                    break;

                }
                break;

            case USBHD_UIS_TOKEN_OUT:
                switch ( USBOTG_FS->INT_ST & ( USBHD_UIS_TOKEN_MASK | USBHD_UIS_ENDP_MASK ) )
                {
                    case USBHD_UIS_TOKEN_OUT:
                        if(SetupReqCode ==SET_LINE_CODING)  //èźŸçœźäžČćŁć±æ§
                        {
                            printf("%x\n",SetupReqCode);
                            if( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK )
                            {

                                 len = USBOTG_FS->RX_LEN;

                                 linecodingst->dataBit=pEP0_DataBuf[6];
                                 linecodingst->parityType=pEP0_DataBuf[5];
                                 linecodingst->stopBit=pEP0_DataBuf[4];
                                 linecodingst->dataRat[0]=pEP0_DataBuf[0];
                                 linecodingst->dataRat[1]=pEP0_DataBuf[1];
                                 linecodingst->dataRat[2]=pEP0_DataBuf[2];
                                 linecodingst->dataRat[3]=pEP0_DataBuf[3];

                                 USART2_CFG(*(int*)(linecodingst->dataRat),linecodingst->dataBit,linecodingst->stopBit,linecodingst->parityType);
                                memcpy(LineCoding,pMySetupReqPakHD,USBOTG_FS->RX_LEN);
                                USBHD_UEP0_T_LEN = 0;
                                USBOTG_FS->UEP0_RX_CTRL  = (USBOTG_FS->UEP0_RX_CTRL & ~USBHD_UEP_R_RES_MASK)| USBHD_UEP_R_RES_ACK; //ćć€äžäŒ 0ć
                            }
                        }
                        else
                        {
                            USBHD_UEP0_T_LEN = 0;
                            USBOTG_FS->UEP0_RX_CTRL  = (USBOTG_FS->UEP0_RX_CTRL & ~USBHD_UEP_R_RES_MASK)| USBHD_UEP_R_RES_NAK;  //ç¶æé¶æź”ïŒćŻčINććșNAK
                        }
                        break;
                            len = USBOTG_FS->RX_LEN;
                            break;

                    case USBHD_UIS_TOKEN_OUT | 1:
                        if ( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK )
                        {
                            USBOTG_FS->UEP1_RX_CTRL ^= USBHD_UEP_R_TOG;
                            len = USBOTG_FS->RX_LEN;
                            DevEP1_OUT_Deal( len );
                        }
                        break;

                    case USBHD_UIS_TOKEN_OUT | 2:
                        if ( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK )
                        {

                            USBOTG_FS->UEP2_RX_CTRL ^= USBHD_UEP_R_TOG;
                            len = USBOTG_FS->RX_LEN;
                            DevEP2_OUT_Deal( len );
                        }
                        break;

                    case USBHD_UIS_TOKEN_OUT | 3:
                        if ( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK )
                        {
                            USBOTG_FS->UEP3_RX_CTRL ^= USBHD_UEP_R_TOG;
                            len = USBOTG_FS->RX_LEN;
                            DevEP3_OUT_Deal( len );
                        }
                        break;

                    case USBHD_UIS_TOKEN_OUT | 4:
                        if ( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK )
                        {
                            USBByteCount = USBOTG_FS->RX_LEN;
                            USBOTG_FS->UEP4_RX_CTRL = (USBOTG_FS->UEP4_RX_CTRL & ~USBHD_UEP_R_RES_MASK) | USBHD_UEP_R_RES_NAK;
                            USBBufOutPoint = 0;
                        }
                        break;

                    case USBHD_UIS_TOKEN_OUT | 5:
                        if ( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK )
                        {
                            USBOTG_FS->UEP5_RX_CTRL ^= USBHD_UEP_R_TOG;
                            len = USBOTG_FS->RX_LEN;
                            DevEP5_OUT_Deal( len );
                        }
                        break;

                    case USBHD_UIS_TOKEN_OUT | 6:
                        if ( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK )
                        {
                            USBOTG_FS->UEP6_RX_CTRL ^= USBHD_UEP_R_TOG;
                            len = USBOTG_FS->RX_LEN;
                            DevEP6_OUT_Deal( len );
                        }
                        break;

                    case USBHD_UIS_TOKEN_OUT | 7:
                        if ( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK )
                        {
                            USBOTG_FS->UEP7_RX_CTRL ^= USBHD_UEP_R_TOG;
                            len = USBOTG_FS->RX_LEN;
                            DevEP7_OUT_Deal( len );
                        }
                        break;
                }

                break;

            case USBHD_UIS_TOKEN_SOF:

                break;

            default :
                break;

        }

        USBOTG_FS->INT_FG = USBHD_UIF_TRANSFER;
    }
    else if( intflag & USBHD_UIF_BUS_RST )
    {
        USBOTG_FS->DEV_ADDR = 0;

        USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_RES_ACK;
        USBOTG_FS->UEP1_RX_CTRL = USBHD_UEP_R_RES_ACK;
        USBOTG_FS->UEP2_RX_CTRL = USBHD_UEP_R_RES_ACK;
        USBOTG_FS->UEP3_RX_CTRL = USBHD_UEP_R_RES_ACK;
        USBOTG_FS->UEP4_RX_CTRL = USBHD_UEP_R_RES_ACK;
        USBOTG_FS->UEP5_RX_CTRL = USBHD_UEP_R_RES_ACK;
        USBOTG_FS->UEP6_RX_CTRL = USBHD_UEP_R_RES_ACK;
        USBOTG_FS->UEP7_RX_CTRL = USBHD_UEP_R_RES_ACK;

        USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_RES_NAK;
        USBOTG_FS->UEP1_TX_CTRL = USBHD_UEP_T_RES_NAK;
        USBOTG_FS->UEP2_TX_CTRL = USBHD_UEP_T_RES_NAK;
        USBOTG_FS->UEP3_TX_CTRL = USBHD_UEP_T_RES_NAK;
        USBOTG_FS->UEP4_TX_CTRL = USBHD_UEP_T_RES_NAK;
        USBOTG_FS->UEP5_TX_CTRL = USBHD_UEP_T_RES_NAK;
        USBOTG_FS->UEP6_TX_CTRL = USBHD_UEP_T_RES_NAK;
        USBOTG_FS->UEP7_TX_CTRL = USBHD_UEP_T_RES_NAK;

        USBOTG_FS->INT_FG |= USBHD_UIF_BUS_RST;
        USBOTG_FS->INT_FG = USBHD_UIF_TRANSFER;
        USBOTG_FS->INT_FG = USBHD_UIF_SUSPEND;


        Uart_Input_Point = 0;
        Uart_Output_Point = 0;
        UartByteCount = 0;
        USBByteCount = 0;
        UpPoint4_Busy = 0;
        DevConfig=0;
    }
    else if( intflag & USBHD_UIF_SUSPEND )
    {
        if ( USBOTG_FS->MIS_ST & USBHD_UMS_SUSPEND )
        {
           ;
        }
        else{;}
        USBOTG_FS->INT_FG = USBHD_UIF_SUSPEND;
    }
    else
    {
        USBOTG_FS->INT_FG = intflag;
    }
}


/*********************************************************************
 * @fn      DevEP1_IN_Deal
 *
 * @brief   Device endpoint1 IN.
 *
 * @param   l - IN length(<64B)
 *
 * @return  none
 */
void DevEP1_IN_Deal( UINT8 l )
{
    USBOTG_FS->UEP1_TX_LEN = l;
    USBOTG_FS->UEP1_TX_CTRL  = (USBOTG_FS->UEP1_TX_CTRL & ~USBHD_UEP_T_RES_MASK)| USBHD_UEP_T_RES_ACK;
    USBOTG_FS->UEP1_TX_CTRL ^= USBHD_UEP_T_TOG;
    USBHD_Endp2_Up_Flag = 0x01;
}
/*********************************************************************
 * @fn      DevEP2_IN_Deal
 *
 * @brief   Device endpoint2 IN.
 *
 * @param   l - IN length(<64B)
 *
 * @return  none
 */
void DevEP2_IN_Deal( UINT8 l )
{
    USBOTG_FS->UEP2_TX_LEN = l;
    USBOTG_FS->UEP2_TX_CTRL = (USBOTG_FS->UEP2_TX_CTRL & ~USBHD_UEP_T_RES_MASK)| USBHD_UEP_T_RES_ACK;
    USBOTG_FS->UEP2_TX_CTRL ^= USBHD_UEP_T_TOG;
}

/*********************************************************************
 * @fn      DevEP3_IN_Deal
 *
 * @brief   Device endpoint3 IN.
 *
 * @param   l - IN length(<64B)
 *
 * @return  none
 */
void DevEP3_IN_Deal( UINT8 l )
{
    USBOTG_FS->UEP3_TX_LEN = l;
    USBOTG_FS->UEP3_TX_CTRL = (USBOTG_FS->UEP3_TX_CTRL & ~USBHD_UEP_T_RES_MASK)| USBHD_UEP_T_RES_ACK;
}

/*********************************************************************
 * @fn      DevEP4_IN_Deal
 *
 * @brief   Device endpoint4 IN.
 *
 * @param   l - IN length(<64B)
 *
 * @return  none
 */
void DevEP4_IN_Deal( UINT8 l )
{
    USBOTG_FS->UEP4_TX_LEN = l;
    USBOTG_FS->UEP4_TX_CTRL = (USBOTG_FS->UEP4_TX_CTRL & ~USBHD_UEP_T_RES_MASK)| USBHD_UEP_T_RES_ACK;
    USBOTG_FS->UEP4_TX_CTRL ^= USBHD_UEP_T_TOG;
}

/*********************************************************************
 * @fn      DevEP5_IN_Deal
 *
 * @brief   Device endpoint5 IN.
 *
 * @param   l - IN length(<64B)
 *
 * @return  none
 */
void DevEP5_IN_Deal( UINT8 l )
{
    USBOTG_FS->UEP5_TX_LEN = l;
    USBOTG_FS->UEP5_TX_CTRL = (USBOTG_FS->UEP5_TX_CTRL & ~USBHD_UEP_T_RES_MASK)| USBHD_UEP_T_RES_ACK;
}

/*********************************************************************
 * @fn      DevEP6_IN_Deal
 *
 * @brief   Device endpoint6 IN.
 *
 * @param   l - IN length(<64B)
 *
 * @return  none
 */
void DevEP6_IN_Deal( UINT8 l )
{
    USBOTG_FS->UEP6_TX_LEN = l;
    USBOTG_FS->UEP6_TX_CTRL = (USBOTG_FS->UEP6_TX_CTRL & ~USBHD_UEP_T_RES_MASK)| USBHD_UEP_T_RES_ACK;
}

/*********************************************************************
 * @fn      DevEP7_IN_Deal
 *
 * @brief   Device endpoint7 IN.
 *
 * @param   l - IN length(<64B)
 *
 * @return  none
 */
void DevEP7_IN_Deal( UINT8 l )
{
    USBOTG_FS->UEP7_TX_LEN = l;
    USBOTG_FS->UEP7_TX_CTRL = (USBOTG_FS->UEP7_TX_CTRL & ~USBHD_UEP_T_RES_MASK)| USBHD_UEP_T_RES_ACK;
}

/*********************************************************************
 * @fn      DevEP1_OUT_Deal
 *
 * @brief   Deal device Endpoint 1 OUT.
 *
 * @param   l - Data length.
 *
 * @return  none
 */
void DevEP1_OUT_Deal( UINT8 l )
{
    UINT8 i;

   for(i=0; i<l; i++)
   {
       pEP1_IN_DataBuf[i] = ~pEP1_OUT_DataBuf[i];
   }

   DevEP1_IN_Deal( l );
}

/*********************************************************************
 * @fn      DevEP2_OUT_Deal
 *
 * @brief   Deal device Endpoint 2 OUT.
 *
 * @param   l - Data length.
 *
 * @return  none
 */
void DevEP2_OUT_Deal( UINT8 l )
{
    UINT8 i;

    for(i=0; i<l; i++)
    {
        pEP2_IN_DataBuf[i] = ~pEP2_OUT_DataBuf[i];
    }

    DevEP2_IN_Deal( l );
}

/*********************************************************************
 * @fn      DevEP3_OUT_Deal
 *
 * @brief   Deal device Endpoint 3 OUT.
 *
 * @param   l - Data length.
 *
 * @return  none
 */
void DevEP3_OUT_Deal( UINT8 l )
{
    UINT8 i;

    for(i=0; i<l; i++)
    {
        pEP3_IN_DataBuf[i] = pEP3_OUT_DataBuf[i];
    }

    DevEP3_IN_Deal( l );
}

/*********************************************************************
 * @fn      DevEP4_OUT_Deal
 *
 * @brief   Deal device Endpoint 4 OUT.
 *
 * @param   l - Data length.
 *
 * @return  none
 */
void DevEP4_OUT_Deal( UINT8 l )
{
    UINT8 i;

    for(i=0; i<l; i++)
    {
        pEP4_IN_DataBuf[i] = pEP4_OUT_DataBuf[i];
    }

    DevEP4_IN_Deal( l );
}

/*********************************************************************
 * @fn      DevEP5_OUT_Deal
 *
 * @brief   Deal device Endpoint 5 OUT.
 *
 * @param   l - Data length.
 *
 * @return  none
 */
void DevEP5_OUT_Deal( UINT8 l )
{
    UINT8 i;

    for(i=0; i<l; i++)
    {
        pEP5_IN_DataBuf[i] = pEP5_OUT_DataBuf[i];
    }

    DevEP5_IN_Deal( l );
}

/*********************************************************************
 * @fn      DevEP6_OUT_Deal
 *
 * @brief   Deal device Endpoint 6 OUT.
 *
 * @param   l - Data length.
 *
 * @return  none
 */
void DevEP6_OUT_Deal( UINT8 l )
{
    UINT8 i;

    for(i=0; i<l; i++)
    {
        pEP6_IN_DataBuf[i] = pEP6_OUT_DataBuf[i];
    }

    DevEP6_IN_Deal( l );
}

/*********************************************************************
 * @fn      DevEP7_OUT_Deal
 *
 * @brief   Deal device Endpoint 7 OUT.
 *
 * @param   l - Data length.
 *
 * @return  none
 */
void DevEP7_OUT_Deal( UINT8 l )
{
    UINT8 i;

    for(i=0; i<l; i++)
    {
        pEP7_IN_DataBuf[i] = pEP7_OUT_DataBuf[i];
    }

    DevEP7_IN_Deal( l );
}

