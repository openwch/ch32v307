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

/* Global define */
/* OTH */
#define pMySetupReqPakHD        ((PUSB_SETUP_REQ)EP0_DatabufHD)
#define RepDescSize             62

#if DEF_USBD_SPEED == DEF_USBD_SPEED_LOW
#define RepDataLoadLen          8
#else
#define RepDataLoadLen          64
#endif

/******************************************************************************/
/* 全局变量 */
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
volatile UINT8  USBHD_Dev_SetupReqCode = 0xFF;                                  /* USB2.0全速设备Setup包命令码 */
volatile UINT16 USBHD_Dev_SetupReqLen = 0x00;                                   /* USB2.0全速设备Setup包长度 */
volatile UINT8  USBHD_Dev_SetupReqValueH = 0x00;                                /* USB2.0全速设备Setup包Value高字节 */
volatile UINT8  USBHD_Dev_Config = 0x00;                                        /* USB2.0全速设备配置值 */
volatile UINT8  USBHD_Dev_Address = 0x00;                                       /* USB2.0全速设备地址值 */
volatile UINT8  USBHD_Dev_SleepStatus = 0x00;                                   /* USB2.0全速设备睡眠状态 */
volatile UINT8  USBHD_Dev_EnumStatus = 0x00;                                    /* USB2.0全速设备枚举状态 */
volatile UINT8  USBHD_Dev_Endp0_Tog = 0x01;                                     /* USB2.0全速设备端点0同步标志 */
volatile UINT8  USBHD_Dev_Speed = 0x01;                                         /* USB2.0全速设备速度 */
																						 
volatile UINT16 USBHD_Endp1_Up_Flag = 0x00;                                     /* USB2.0全速设备端点1数据上传状态: 0:空闲; 1:正在上传; */
volatile UINT8  USBHD_Endp1_Down_Flag = 0x00;                                   /* USB2.0全速设备端点1下传成功标志 */
volatile UINT8  USBHD_Endp1_Down_Len = 0x00;                                    /* USB2.0全速设备端点1下传长度 */
volatile BOOL   USBHD_Endp1_T_Tog = 0;                                          /* USB2.0全速设备端点1发送tog位翻转 */
volatile BOOL   USBHD_Endp1_R_Tog = 0;                                                   
																						 
volatile UINT16 USBHD_Endp2_Up_Flag = 0x00;                                     /* USB2.0全速设备端点2数据上传状态: 0:空闲; 1:正在上传; */
volatile UINT16 USBHD_Endp2_Up_LoadPtr = 0x00;                                  /* USB2.0全速设备端点2数据上传装载偏移 */
volatile UINT8  USBHD_Endp2_Down_Flag = 0x00;                                   /* USB2.0全速设备端点2下传成功标志 */
volatile BOOL   USBHD_Endp2_T_Tog = 0;                                          /* USB2.0全速设备端点2发送tog位翻转 */
volatile BOOL   USBHD_Endp2_R_Tog = 0;   

volatile UINT32V Endp2_send_seq = 0x00;
volatile UINT8   DevConfig;
volatile UINT8   SetupReqCode;
volatile UINT16  SetupReqLen;

/******************************************************************************/
/* Device Descriptor */
const UINT8  MyDevDescrHD[] =
{
    0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, DEF_USBD_UEP0_SIZE,
    0x86, 0x1A, 0xE1, 0xE6,
    0x00, 0x01, 0x01, 0x02, 0x00, 0x01,
};

/* Configration Descriptor */
/* USB配置描述符(低速) */
UINT8  MyCfgDescr_LS[ ] =
{
    0x09,0x02,0x29,0x00,0x01,0x01,0x04,0xA0,0x23,               //配置描述符
    0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x05,               //接口描述符
    0x09,0x21,0x00,0x01,0x00,0x01,0x22,0x22,0x00,               //HID类描述符
    0x07,0x05,0x81,0x03,0x08,0x00,0x0A,                         //端点描述符(全速间隔时间改成1ms)
    0x07,0x05,0x01,0x03,0x08,0x00,0x0A,                         //端点描述符
};
/* USB配置描述符(全速) */
UINT8  MyCfgDescr_FS[ ] =
{
    0x09,0x02,0x29,0x00,0x01,0x01,0x04,0xA0,0x23,               //配置描述符
    0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x05,               //接口描述符
    0x09,0x21,0x00,0x01,0x00,0x01,0x22,0x22,0x00,               //HID类描述符
    0x07,0x05,0x81,0x03,0x40,0x00,0x01,                         //端点描述符(全速间隔时间改成1ms)
    0x07,0x05,0x01,0x03,0x40,0x00,0x01,                         //端点描述符
};

/* USB报告描述符 */
const UINT8  CompatibilityHIDRepDesc[ ] =
{
        0x06, 0x00,0xff,
        0x09, 0x01,
        0xa1, 0x01,                                                   //集合开始
        0x09, 0x02,                                                   //Usage Page  用法
        0x15, 0x00,                                                   //Logical  Minimun
        0x26, 0x00,0xff,                                              //Logical  Maximun
        0x75, 0x08,                                                   //Report Size
        0x95, RepDataLoadLen,                                         //Report Counet
        0x81, 0x06,                                                   //Input
        0x09, 0x02,                                                   //Usage Page  用法
        0x15, 0x00,                                                   //Logical  Minimun
        0x26, 0x00,0xff,                                              //Logical  Maximun
        0x75, 0x08,                                                   //Report Size
        0x95, RepDataLoadLen,                                         //Report Counet
        0x91, 0x06,                                                   //Output
        0xC0
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

/* USB序列号字符串描述符 */
const UINT8  MySerNumInfoHD[ ] =
{
    /* 0123456789 */
    22,03,48,0,49,0,50,0,51,0,52,0,53,0,54,0,55,0,56,0,57,0
};

/* USB设备限定描述符 */
const UINT8 MyUSBQUADescHD[ ] =
{
    0x0A, 0x06, 0x00, 0x02, 0xFF, 0x00, 0xFF, 0x40, 0x01, 0x00,
};

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
#ifdef CH32V30x_D8C
    RCC_USBCLK48MConfig( RCC_USBCLK48MCLKSource_USBPHY );
    RCC_USBHSPLLCLKConfig( RCC_HSBHSPLLCLKSource_HSE );
    RCC_USBHSConfig( RCC_USBPLL_Div2 );
    RCC_USBHSPLLCKREFCLKConfig( RCC_USBHSPLLCKREFCLK_4M );
    RCC_USBHSPHYPLLALIVEcmd( ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_USBHS, ENABLE );

#else
    RCC_OTGFSCLKConfig(RCC_OTGFSCLKSource_PLLCLK_Div1);;

#endif

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
    /* 端点缓冲区初始化 */
    pEP0_RAM_Addr = EP0_DatabufHD;
    pEP1_RAM_Addr = EP1_DatabufHD;
    pEP2_RAM_Addr = EP2_DatabufHD;
    pEP3_RAM_Addr = EP3_DatabufHD;
    pEP4_RAM_Addr = EP4_DatabufHD;
    pEP5_RAM_Addr = EP5_DatabufHD;
    pEP6_RAM_Addr = EP6_DatabufHD;
    pEP7_RAM_Addr = EP7_DatabufHD;
    /* 使能usb时钟 */
    USBOTG_RCC_Init( );
    Delay_Us(100);
    /* usb设备初始化 */
    USBDeviceInit( );
    EXTEN->EXTEN_CTR |= EXTEN_USBD_PU_EN;
    /* 使能usb中断 */
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

    if( intflag & USBHD_UIF_TRANSFER )
    {
        switch ( USBOTG_FS->INT_ST & USBHD_UIS_TOKEN_MASK )
        {
            /* SETUP包处理 */
            case USBHD_UIS_TOKEN_SETUP:
#if 0
                /* 打印当前Usbsetup命令  */
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
                /* 判断当前是标准请求还是其他请求 */
                if ( ( pSetupReqPakHD->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                {
                    /* 其它请求,如类请求,产商请求等 */
                    if( pSetupReqPakHD->bRequestType & 0x40 )
                    {
                        /* 厂商请求 */
                        switch( pSetupReqPakHD->bRequest )
                        {
                            default:
                                errflag = 0xFF;/* 操作失败 */
                                break;
                        }
                    }
                    else if( pSetupReqPakHD->bRequestType & 0x20 )
                    {
                        /* HID类请求 */
                        switch( pSetupReqPakHD->bRequest )
                        {
                        case 0x01: //GetReport
                            break;
                        case 0x02: //GetIdle
                            break;
                        case 0x03: //GetProtocol
                            break;
                        case 0x09: //SetReport
                            break;
                        case 0x0A: //SetIdle
                            break;
                        case 0x0B: //SetProtocol
                            break;
                        default:
                            errflag = 0xFF;
                            break;
                        }
                    }

                    /* 判断是否可以正常处理 */
                    if( errflag != 0xFF )
                    {
                        if( SetupReqLen > len )
                        {
                            SetupReqLen = len;
                        }
                        len = ( USBHD_Dev_SetupReqLen >= DEF_USBD_UEP0_SIZE ) ? DEF_USBD_UEP0_SIZE : USBHD_Dev_SetupReqLen;
                        memcpy( EP0_DatabufHD, pDescr, len );
                        pDescr += len;
                    }
                }
                else
                {
                    /* 处理标准USB请求包 */
                    switch( SetupReqCode )
                    {
                        case USB_GET_DESCRIPTOR:
                        {
                            switch( ((pSetupReqPakHD->wValue)>>8) )
                            {
                                case USB_DESCR_TYP_DEVICE:
                                    /* 获取设备描述符 */
                                    pDescr = MyDevDescrHD;
                                    len = MyDevDescrHD[0];
                                    break;

                                case USB_DESCR_TYP_CONFIG:
                                    /* 获取配置描述符 */
#if DEF_USBD_SPEED == DEF_USBD_SPEED_FULL
                                    pDescr = MyCfgDescr_FS;
                                    len = sizeof( MyCfgDescr_FS );
#elif DEF_USBD_SPEED == DEF_USBD_SPEED_LOW
                                    pDescr = MyCfgDescr_LS;
                                    len = sizeof( MyCfgDescr_LS );
#endif
                                    break;

                                case USB_DESCR_TYP_STRING:
                                    /* 获取字符串描述符 */
                                    switch( (pSetupReqPakHD->wValue)&0xff )
                                    {
                                        case 0:
                                            /* 语言字符串描述符 */
                                        pDescr = MyLangDescrHD;
                                        len = MyLangDescrHD[0];
                                            break;

                                        case 1:
                                            /* USB产商字符串描述符 */
                                            pDescr = MyManuInfoHD;
                                            len = MyManuInfoHD[0];
                                            break;

                                        case 2:
                                            /* USB产品字符串描述符 */
                                            pDescr = MyProdInfoHD;
                                            len = MyProdInfoHD[0];
                                            break;

                                        case 3:
                                            /* USB序列号字符串描述符 */
                                            pDescr = MySerNumInfoHD;
                                            len = sizeof( MySerNumInfoHD );
                                            break;

                                        default:
                                            errflag = 0xFF;
                                            break;
                                    }
                                    break;

                                case USB_DESCR_TYP_REPORT:
                                    /* USB设备报告描述符 */
                                    pDescr = CompatibilityHIDRepDesc;
                                    len = sizeof( CompatibilityHIDRepDesc );
                                    break;

                                case USB_DESCR_TYP_QUALIF:
                                    /* 设备限定描述符 */
                                    pDescr = ( PUINT8 )&MyUSBQUADescHD[ 0 ];
                                    len = sizeof( MyUSBQUADescHD );
                                    break;

                                case USB_DESCR_TYP_SPEED:
                                    /* 其他速度配置描述符 */
                                    errflag = 0xFF;
                                    break;

                                case USB_DESCR_TYP_BOS:
                                    /* BOS描述符 */
                                    /* USB2.0设备不支持BOS描述符 */
                                    errflag = 0xFF;
                                    break;

                                default :
                                    errflag = 0xff;
                                    break;

                            }

                            if( SetupReqLen>len )   SetupReqLen = len;
                            len = (SetupReqLen >= DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : SetupReqLen;
                            memcpy( pEP0_DataBuf, pDescr, len );
                            pDescr += len;
                        }
                            break;

                        case USB_SET_ADDRESS:
                            /* 设置地址 */
                            SetupReqLen = (pSetupReqPakHD->wValue)&0xff;
                            break;

                        case USB_GET_CONFIGURATION:
                            /* 获取配置值 */
                            pEP0_DataBuf[0] = DevConfig;
                            if ( SetupReqLen > 1 ) SetupReqLen = 1;
                            break;

                        case USB_SET_CONFIGURATION:
                            /* 设置配置值 */
                            DevConfig = (pSetupReqPakHD->wValue)&0xff;
                            break;

                        case USB_CLEAR_FEATURE:
                            /* 清除特性 */
                            if ( ( pSetupReqPakHD->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                            {
                                /* 清除端点 */
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

                                default:
                                    errflag = 0xFF;
                                    break;

                                }
                            }
                            else    errflag = 0xFF;
                            break;

                        case USB_SET_FEATURE:
                            /* 设置特性 */
                            if( ( pMySetupReqPakHD->bRequestType & 0x1F ) == 0x00 )
                            {
                                /* 设置设备 */
                                if( pMySetupReqPakHD->wValue == 0x01 )
                                {
                                    if( MyCfgDescr_FS[ 7 ] & 0x20 )
                                    {
                                        /* 设置唤醒使能标志 */
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
                                /* 设置端点 */
                                if( pMySetupReqPakHD->wValue == 0x00 )
                                {
                                    /* 设置指定端点STALL */
                                    switch( ( pMySetupReqPakHD->wIndex ) & 0xff )
                                    {
                                        case 0x82:
                                            /* 设置端点2 IN STALL */
                                            USBOTG_FS->UEP2_TX_CTRL = ( USBOTG_FS->UEP2_TX_CTRL &= ~USBHD_UEP_T_RES_MASK ) | USBHD_UEP_T_RES_STALL;
                                            //USBHS->UEP2_CTRL  = ( USBHS->UEP2_CTRL & ~USBHS_EP_T_RES_MASK ) | USBHS_EP_T_RES_STALL;
                                            break;

                                        case 0x02:
                                            /* 设置端点2 OUT Stall */
                                            USBOTG_FS->UEP2_RX_CTRL = ( USBOTG_FS->UEP2_RX_CTRL &= ~USBHD_UEP_R_RES_MASK ) | USBHD_UEP_R_RES_STALL;
                                            //USBHS->UEP2_CTRL  = ( USBHS->UEP2_CTRL & ~USBHS_EP_R_RES_MASK ) | USBHS_EP_R_RES_STALL;
                                            break;

                                        case 0x81:
                                            /* 设置端点1 IN STALL */
                                            USBOTG_FS->UEP1_TX_CTRL = ( USBOTG_FS->UEP1_TX_CTRL &= ~USBHD_UEP_T_RES_MASK ) | USBHD_UEP_T_RES_STALL;
                                            //USBHS->UEP1_CTRL  = ( USBHS->UEP1_CTRL & ~USBHS_EP_T_RES_MASK ) | USBHS_EP_T_RES_STALL;
                                            break;

                                        case 0x01:
                                            /* 设置端点1 OUT STALL */
                                            USBOTG_FS->UEP1_RX_CTRL = ( USBOTG_FS->UEP1_RX_CTRL &= ~USBHD_UEP_R_RES_MASK ) | USBHD_UEP_R_RES_STALL;
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
                            /* 获取接口 */
                            pEP0_DataBuf[0] = 0x00;
                            if ( SetupReqLen > 1 ) SetupReqLen = 1;
                            break;

                        case USB_SET_INTERFACE:
                            /* 设置接口 */
                            EP0_DatabufHD[ 0 ] = 0x00;
                            if( USBHD_Dev_SetupReqLen > 1 )
                            {
                                USBHD_Dev_SetupReqLen = 1;
                            }
                            break;

                        case USB_GET_STATUS:
                            /* 根据当前端点实际状态进行应答 */
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
                        len = (SetupReqLen>DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : SetupReqLen;
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
                                    len = SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : SetupReqLen;
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
                    USBOTG_FS->UEP1_TX_CTRL = (USBOTG_FS->UEP1_TX_CTRL & ~USBHD_UEP_T_RES_MASK) | USBHD_UEP_T_RES_NAK;
                    USBHD_Endp1_Up_Flag = 0x00;
                    break;

                case USBHD_UIS_TOKEN_IN | 2:
                    USBOTG_FS->UEP2_TX_CTRL = (USBOTG_FS->UEP2_TX_CTRL & ~USBHD_UEP_T_RES_MASK) | USBHD_UEP_T_RES_NAK;
                    USBHD_Endp2_Up_Flag = 0x00;
                    break;

                default :
                    break;

                }
                break;

            case USBHD_UIS_TOKEN_OUT:
                switch ( USBOTG_FS->INT_ST & ( USBHD_UIS_TOKEN_MASK | USBHD_UIS_ENDP_MASK ) )
                {
                    case USBHD_UIS_TOKEN_OUT:
                            len = USBOTG_FS->RX_LEN;
                            break;

                    case USBHD_UIS_TOKEN_OUT | 1:
                        if ( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK )
                        {
                            USBHD_Endp1_Down_Len = USBOTG_FS->RX_LEN;
                            if( USBHD_Endp1_Down_Len )
                            {
                                USBHD_Endp1_Down_Flag = 1;
                            }
                            USBOTG_FS->UEP1_RX_CTRL = (USBOTG_FS->UEP1_RX_CTRL & ~USBHD_UEP_R_RES_MASK) | USBHD_UEP_R_RES_NAK;
                            USBOTG_FS->UEP1_RX_CTRL ^= USBHD_UEP_R_TOG;
                        }
                        break;

                    case USBHD_UIS_TOKEN_OUT | 2:
                        if ( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK )
                        {

                        }
                        break;

                    default :
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
    }
    else if( intflag & USBHD_UIF_SUSPEND )
    {
        if ( USBOTG_FS->MIS_ST & USBHD_UMS_SUSPEND ) {;}
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
    USBOTG_FS->UEP1_TX_CTRL  ^= USBHD_UEP_T_TOG;
    USBHD_Endp1_Up_Flag = 0x01;
}

/*********************************************************************
 * @fn      Ep1_Tx
 *
 * @brief   USBHS 端点1上传
 *
 * @return  none
 */
void Ep1_Tx( PUINT8 data, UINT8 len )
{
    while( USBHD_Endp1_Up_Flag );
    memcpy( pEP1_IN_DataBuf, data, len );
    DevEP1_IN_Deal( len );
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
    /* Your Code About endp2 Tx Here */
    USBOTG_FS->UEP2_TX_LEN = l;
    USBOTG_FS->UEP2_TX_CTRL  = (USBOTG_FS->UEP2_TX_CTRL & ~USBHD_UEP_T_RES_MASK)| USBHD_UEP_T_RES_ACK;
    USBOTG_FS->UEP2_TX_CTRL  ^= USBHD_UEP_T_TOG;
    USBHD_Endp2_Up_Flag = 0x01;
}

/*********************************************************************
 * @fn      Ep2_Tx
 *
 * @brief   USBHS 端点2上传
 *
 * @return  none
 */
void Ep2_Tx( PUINT8 data, UINT8 len )
{
    while( USBHD_Endp2_Up_Flag );
    memcpy( pEP2_IN_DataBuf, data, len );
    DevEP2_IN_Deal( len );
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
    /* Your Code About endp3 Tx Here */
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
    /* Your Code About endp4 Tx Here */
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
    /* Your Code About endp5 Tx Here */
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
    /* Your Code About endp6 Tx Here */
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
    /* Your Code About endp7 Tx Here */
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
    /* Your Code About endp1 Rx Here */
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
    /* Your Code About endp2 Rx Here */
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
    /* Your Code About endp3 Rx Here */
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
    /* Your Code About endp4 Rx Here */
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
    /* Your Code About endp5 Rx Here */
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
    /* Your Code About endp6 Rx Here */
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
    /* Your Code About endp7 Rx Here */
}
