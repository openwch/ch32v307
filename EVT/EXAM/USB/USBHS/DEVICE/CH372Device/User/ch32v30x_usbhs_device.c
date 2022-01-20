/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_usbhs_device.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : USB高速操作相关文件
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_usbhs_device.h"

/******************************************************************************/
/* 常、变量定义 */

/* 函数声明 */
void USBHS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/* USB缓冲区定义 */
__attribute__ ((aligned(4))) UINT8 EP0_Databuf[ USBHS_UEP0_SIZE ]; /* 端点0数据收发缓冲区 */
__attribute__ ((aligned(4))) UINT8 EP1_Rx_Databuf[ USBHS_MAX_PACK_SIZE ]; /* 端点1数据接收缓冲区 */
__attribute__ ((aligned(4))) UINT8 EP1_Tx_Databuf[ USBHS_MAX_PACK_SIZE ]; /* 端点1数据发送缓冲区 */
__attribute__ ((aligned(4))) UINT8 EP2_Rx_Databuf[ USBHS_MAX_PACK_SIZE ]; /* 端点2数据接收缓冲区 */
__attribute__ ((aligned(4))) UINT8 EP2_Tx_Databuf[ USBHS_MAX_PACK_SIZE ]; /* 端点2数据发送缓冲区 */

#define pMySetupReqPak        ((PUSB_SETUP_REQ)EP0_Databuf)
const UINT8 *pDescr;
volatile UINT8  USBHS_Dev_SetupReqCode = 0xFF;                                  /* USB2.0高速设备Setup包命令码 */
volatile UINT16 USBHS_Dev_SetupReqLen = 0x00;                                   /* USB2.0高速设备Setup包长度 */
volatile UINT8  USBHS_Dev_SetupReqValueH = 0x00;                                /* USB2.0高速设备Setup包Value高字节 */
volatile UINT8  USBHS_Dev_Config = 0x00;                                        /* USB2.0高速设备配置值 */
volatile UINT8  USBHS_Dev_Address = 0x00;                                       /* USB2.0高速设备地址值 */
volatile UINT8  USBHS_Dev_SleepStatus = 0x00;                                   /* USB2.0高速设备睡眠状态 */
volatile UINT8  USBHS_Dev_EnumStatus = 0x00;                                    /* USB2.0高速设备枚举状态 */
volatile UINT8  USBHS_Dev_Endp0_Tog = 0x01;                                     /* USB2.0高速设备端点0同步标志 */
volatile UINT8  USBHS_Dev_Speed = 0x01;                                         /* USB2.0高速设备速度 */
volatile UINT8  USBHS_Int_Flag  = 0x00;                                         /* USB2.0高速设备中断标志 */

volatile UINT16 USBHS_Endp1_Up_Flag = 0x00;                                     /* USB2.0高速设备端点1数据上传状态: 0:空闲; 1:正在上传; */
volatile UINT8  USBHS_Endp1_Down_Flag = 0x00;                                   /* USB2.0高速设备端点1下传成功标志 */
volatile UINT8  USBHS_Endp1_Down_Len = 0x00;                                    /* USB2.0高速设备端点1下传长度 */
volatile UINT8  USBHS_Endp1_T_Tog = 0x00;
volatile UINT8  USBHS_Endp1_R_Tog = 0x00;

volatile UINT16 USBHS_Endp2_Up_Flag = 0x00;                                     /* USB2.0高速设备端点2数据上传状态: 0:空闲; 1:正在上传; */
volatile UINT16 USBHS_Endp2_Up_LoadPtr = 0x00;                                  /* USB2.0高速设备端点2数据上传装载偏移 */
volatile UINT8  USBHS_Endp2_Down_Flag = 0x00;                                   /* USB2.0高速设备端点2下传成功标志 */

/******************************************************************************/
/* USB设备描述符 */
UINT8  MyDevDescr[ 18 ] =
{
    0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40,
    0x86, 0x1A, 0x37, 0x55,
    DEF_IC_PRG_VER2, DEF_IC_PRG_VER, 0x01, 0x02, 0x00, 0x01
};

/* USB配置描述符(全速) */
const UINT8  MyCfgDescr_FS[ ] =
{
    0x09, 0x02, 0x2E, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
    0x09, 0x04, 0x00, 0x00, 0x04, 0xFF, 0xFF, 0xFF, 0x00,
    0x07, 0x05, 0x81, 0x02, (UINT8)DEF_USB_FS_EP_SIZE, (UINT8)( DEF_USB_FS_EP_SIZE >> 8 ), 0x00,
    0x07, 0x05, 0x01, 0x02, (UINT8)DEF_USB_FS_EP_SIZE, (UINT8)( DEF_USB_FS_EP_SIZE >> 8 ), 0x00,
    0x07, 0x05, 0x82, 0x02, (UINT8)DEF_USB_FS_EP_SIZE, (UINT8)( DEF_USB_FS_EP_SIZE >> 8 ), 0x00,
    0x07, 0x05, 0x02, 0x02, (UINT8)DEF_USB_FS_EP_SIZE, (UINT8)( DEF_USB_FS_EP_SIZE >> 8 ), 0x00,
};

/* USB配置描述符(高速) */
const UINT8  MyCfgDescr_HS[ ] =
{
    0x09, 0x02, 0x2E, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
    0x09, 0x04, 0x00, 0x00, 0x04, 0xFF, 0xFF, 0xFF, 0x00,
    0x07, 0x05, 0x81, 0x02, (UINT8)DEF_USB_HS_EP_SIZE, (UINT8)( DEF_USB_HS_EP_SIZE >> 8 ), 0x00,
    0x07, 0x05, 0x01, 0x02, (UINT8)DEF_USB_HS_EP_SIZE, (UINT8)( DEF_USB_HS_EP_SIZE >> 8 ), 0x00,
    0x07, 0x05, 0x82, 0x02, (UINT8)DEF_USB_HS_EP_SIZE, (UINT8)( DEF_USB_HS_EP_SIZE >> 8 ), 0x00,
    0x07, 0x05, 0x02, 0x02, (UINT8)DEF_USB_HS_EP_SIZE, (UINT8)( DEF_USB_HS_EP_SIZE >> 8 ), 0x00,
};

/* USB语言字符串描述符 */
const UINT8  MyLangDescr[ ] =
{
    0x04, 0x03, 0x09, 0x04
};

/* USB产商字符串描述符 */
const UINT8  MyManuInfo[ ] =
{
    /* wch.cn */
    0x0E, 0x03,
    'w',0x00, 'c',0x00, 'h',0x00, '.',0x00, 'c',0x00, 'n',0x00
};

/* USB产品字符串描述符 */
const UINT8  MyProdInfo[ ] =
{
    /* WCH USB2.0 DEVICE */
    0x26, 0x03,
    'W',0x00, 'C',0x00, 'H',0x00, ' ',0x00, 'U',0x00, 'S',0x00,
    'B',0x00, '2',0x00, '.',0x00, '0',0x00, ' ',0x00, 'D',0x00,
    'E',0x00, 'V',0x00, 'I',0x00, 'C',0x00, 'E',0x00, ' ',0x00
};

/* USB序列号字符串描述符 */
const UINT8  MySerNumInfo[ ] =
{
    /* 0123456789 */
    0x16, 0x03,
    '0',0x00, '1',0x00, '2',0x00, '3',0x00, '4',0x00, '5',0x00,
    '6',0x00, '7',0x00, '8',0x00, '9',0x00,
};

/* USB设备限定描述符 */
const UINT8 MyUSBQUADesc[ ] =
{
    0x0A, 0x06, 0x00, 0x02, 0xFF, 0x00, 0xFF, 0x40, 0x01, 0x00,
};
const UINT8 MyBOSDesc[ ] =
{
    0x05, 0x0f, 0x16, 0x00, 0x02,
    0x07, 0x10, 0x02, 0x02, 0x00, 0x00, 0x00,
    0x0a, 0x10, 0x03, 0x00, 0x0e, 0x00, 0x01, 0x0a, 0xff, 0x07,
};
/* USB全速模式,其他速度配置描述符 */
UINT8 TAB_USB_FS_OSC_DESC[ sizeof( MyCfgDescr_HS ) ] =
{
    0x09, 0x07,                                                                 /* 其他部分通过程序复制 */
};

/* USB高速模式,其他速度配置描述符 */
UINT8 TAB_USB_HS_OSC_DESC[ sizeof( MyCfgDescr_FS ) ] =
{
    0x09, 0x07,                                                                 /* 其他部分通过程序复制 */
};

/*********************************************************************
 * @fn      USBHS_RCC_Init
 *
 * @brief   Initializes the clock for USB2.0 High speed device.
 *
 * @return  none
 */
void USBHS_RCC_Init( void )
{
    RCC->CFGR2 = USBHS_PLL_SRC_HSE | USBHS_PLL_SRC_PRE_DIV2 | USBHS_PLL_CKREF_4M; /* PLL REF = HSE/2 = 4MHz */
    RCC->CFGR2 |= USB_48M_CLK_SRC_PHY | USBHS_PLL_ALIVE;
    RCC->AHBPCENR |= ( (uint32_t)( 1 << 11 ) );
    Delay_Us( 200 );
}

/*********************************************************************
 * @fn      USBHS_Device_Endp_Init
 *
 * @brief   USB2.0高速设备端点初始化
 *
 * @return  none
 */
void USBHS_Device_Endp_Init ( void )
{
    /* 使能端点1、端点2发送和接收  */
    USBHSD->ENDP_CONFIG = USBHS_EP0_T_EN | USBHS_EP0_R_EN | USBHS_EP1_T_EN | USBHS_EP2_T_EN | USBHS_EP1_R_EN | USBHS_EP2_R_EN;

    /* 端点非同步端点 */
    USBHSD->ENDP_TYPE = 0x00;

    /* 端点缓冲区模式，非双缓冲区，ISO传输BUF模式需要指定0  */
    USBHSD->BUF_MODE = 0x00;

    /* 端点最大长度包配置 */
    USBHSD->UEP0_MAX_LEN = 64;
    USBHSD->UEP1_MAX_LEN = 512;
    USBHSD->UEP2_MAX_LEN = 512;
    USBHSD->UEP3_MAX_LEN = 512;
    USBHSD->UEP4_MAX_LEN = 512;
    USBHSD->UEP5_MAX_LEN = 512;
    USBHSD->UEP6_MAX_LEN = 512;
    USBHSD->UEP7_MAX_LEN = 512;
    USBHSD->UEP8_MAX_LEN = 512;
    USBHSD->UEP9_MAX_LEN = 512;
    USBHSD->UEP10_MAX_LEN = 512;
    USBHSD->UEP11_MAX_LEN = 512;
    USBHSD->UEP12_MAX_LEN = 512;
    USBHSD->UEP13_MAX_LEN = 512;
    USBHSD->UEP14_MAX_LEN = 512;
    USBHSD->UEP15_MAX_LEN = 512;

    /* 端点DMA地址配置 */
    USBHSD->UEP0_DMA    = (UINT32)(UINT8 *)EP0_Databuf;
    USBHSD->UEP1_TX_DMA = (UINT32)(UINT8 *)EP1_Tx_Databuf;
    USBHSD->UEP1_RX_DMA = (UINT32)(UINT8 *)EP1_Rx_Databuf;
    USBHSD->UEP2_TX_DMA = (UINT32)(UINT8 *)EP2_Tx_Databuf;
    USBHSD->UEP2_RX_DMA = (UINT32)(UINT8 *)EP2_Rx_Databuf;

    /* 端点控制寄存器配置 */
    USBHSD->UEP0_TX_LEN  = 0;
    USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_NAK;
    USBHSD->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK;

    USBHSD->UEP1_TX_LEN  = 0;
    USBHSD->UEP1_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP1_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP2_TX_LEN  = 0;
    USBHSD->UEP2_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP2_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP3_TX_LEN  = 0;
    USBHSD->UEP3_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP3_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP4_TX_LEN  = 0;
    USBHSD->UEP4_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP4_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP5_TX_LEN  = 0;
    USBHSD->UEP5_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP5_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP6_TX_LEN  = 0;
    USBHSD->UEP6_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP6_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP7_TX_LEN  = 0;
    USBHSD->UEP7_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP7_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP8_TX_LEN  = 0;
    USBHSD->UEP8_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP8_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP9_TX_LEN  = 0;
    USBHSD->UEP9_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP9_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP10_TX_LEN  = 0;
    USBHSD->UEP10_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP10_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP11_TX_LEN  = 0;
    USBHSD->UEP11_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP11_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP12_TX_LEN  = 0;
    USBHSD->UEP12_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP12_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP13_TX_LEN  = 0;
    USBHSD->UEP13_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP13_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP14_TX_LEN  = 0;
    USBHSD->UEP14_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP14_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;

    USBHSD->UEP15_TX_LEN  = 0;
    USBHSD->UEP15_TX_CTRL = USBHS_EP_T_AUTOTOG | USBHS_EP_T_RES_NAK;
    USBHSD->UEP15_RX_CTRL = USBHS_EP_R_AUTOTOG | USBHS_EP_R_RES_ACK;
}

/*********************************************************************
 * @fn      USBHS_Device_Init
 *
 * @brief   USB2.0高速设备初始化
 *
 * @return  none
 */
void USBHS_Device_Init ( FunctionalState sta )
{
    if( sta )
    {
        /* 配置DMA、速度、端点使能等 */
        USBHSD->HOST_CTRL = 0x00;
        USBHSD->HOST_CTRL = USBHS_SUSPENDM;

        USBHSD->CONTROL   = 0x00;
        USBHSD->CONTROL   = USBHS_DMA_EN | USBHS_INT_BUSY_EN | USBHS_HIGH_SPEED;
//        USBHSD->CONTROL   = USBHS_DMA_EN | USBHS_INT_BUSY_EN | USBHS_FULL_SPEED;
//        USBHSD->CONTROL   = USBHS_DMA_EN | USBHS_INT_BUSY_EN | USBHS_LOW_SPEED;

        USBHSD->INT_EN    = 0;
        USBHSD->INT_EN    = USBHS_SETUP_ACT_EN | USBHS_TRANSFER_EN | USBHS_DETECT_EN | USBHS_SUSPEND_EN;

        /* ALL endpoint enable */
        USBHSD->ENDP_CONFIG = 0xffffffff;

        /* USB2.0高速设备端点初始化 */
        USBHS_Device_Endp_Init( );
        Delay_Us(10);

        /* 使能USB连接 */
        USBHSD->CONTROL |= USBHS_DEV_PU_EN;
    }
    else
    {
        USBHSD->CONTROL &= ~USBHS_DEV_PU_EN;
        USBHSD->CONTROL |= USBHS_ALL_CLR | USBHS_FORCE_RST;
    }
}

/*********************************************************************
 * @fn      USBHS_Device_SetAddress
 *
 * @brief   USB2.0高速设备设置设备地址
 *
 * @return  none
 */
void USBHS_Device_SetAddress( UINT32 address )
{
    USBHSD->DEV_AD = 0;
    USBHSD->DEV_AD = address & 0xff;
}

/*********************************************************************
 * @fn      USBHS_IRQHandler
 *
 * @brief   This function handles USBHS exception.
 *
 * @return  none
 */
void USBHS_IRQHandler( void )
{
    UINT32 end_num;
    UINT32 rx_token;
    UINT16 len = 0x00;
    UINT16 rx_len = 0;
    UINT16 i;
    UINT8  errflag = 0x00;
    UINT8  chtype;

    USBHS_Int_Flag = USBHSD->INT_FG;

    if( USBHS_Int_Flag & USBHS_TRANSFER_FLAG )
    {
        /* 端点传输处理 */
        end_num  = (USBHSD->INT_ST) & MASK_UIS_ENDP;
        rx_token = ( ( (USBHSD->INT_ST) & MASK_UIS_TOKEN ) >> 4 ) & 0x03;
        /* 00: OUT, 01:SOF, 10:IN, 11:SETUP */
        if( end_num == 0 )
        {
            /* 端点0处理 */
            if( rx_token == PID_IN )
            {
                /* 端点0上传成功中断 */
                switch( USBHS_Dev_SetupReqCode )
                {
                    case USB_GET_DESCRIPTOR:
                        len = USBHS_Dev_SetupReqLen >= USBHS_UEP0_SIZE ? USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
                        memcpy( EP0_Databuf, pDescr, len );
                        USBHS_Dev_SetupReqLen -= len;
                        pDescr += len;
                        USBHS_Dev_Endp0_Tog ^= 1;
                        USBHSD->UEP0_TX_LEN  = len;
                        USBHSD->UEP0_TX_CTRL =  USBHS_EP_T_RES_ACK | ( USBHS_Dev_Endp0_Tog ? USBHS_EP_T_TOG_0 : USBHS_EP_T_TOG_1 );
                        break;

                    case USB_SET_ADDRESS:
                        USBHS_Device_SetAddress( USBHS_Dev_Address );
                        USBHSD->UEP0_TX_LEN = 0;
                        USBHSD->UEP0_TX_CTRL = 0;
                        USBHSD->UEP0_RX_CTRL = 0;
                        break;

                    default:
                        /* 状态阶段完成中断或者是强制上传0长度数据包结束控制传输 */
                        USBHSD->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK | USBHS_EP_R_TOG_1;
                        pDescr = NULL;
                        break;
                }
            }
            else if( rx_token == PID_OUT )
            {
                USBHSD->UEP0_TX_LEN  = 0;
                USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_ACK | USBHS_EP_T_TOG_1;
            }
        }
        else if( end_num == 1 )
        {
            if( rx_token == PID_IN )
            {

            }
            else if( rx_token == PID_OUT )
            {
                rx_len = USBHSD->RX_LEN;
                for( i=0; i<rx_len; i++)
                {
                    EP1_Tx_Databuf[i] = EP1_Rx_Databuf[i];
                }
                USBHSD->UEP1_TX_LEN  = rx_len;
                USBHSD->UEP1_TX_CTRL &= ~USBHS_EP_T_RES_MASK;
                USBHSD->UEP1_TX_CTRL |= USBHS_EP_T_RES_ACK;
                USBHSD->UEP1_RX_CTRL &= ~ USBHS_EP_R_RES_MASK;
                USBHSD->UEP1_RX_CTRL |= USBHS_EP_R_RES_ACK;

            }
        }
        else if( end_num == 2 )
        {
            if( rx_token == PID_IN )
            {

            }
            else if( rx_token == PID_OUT )
            {
                rx_len = USBHSD->RX_LEN;
                for( i=0; i<rx_len; i++)
                {
                    EP2_Tx_Databuf[i] = EP2_Rx_Databuf[i];
                }
                USBHSD->UEP2_TX_LEN  = rx_len;
                USBHSD->UEP2_TX_CTRL &= ~USBHS_EP_T_RES_MASK;
                USBHSD->UEP2_TX_CTRL |= USBHS_EP_T_RES_ACK;
                USBHSD->UEP2_RX_CTRL &= ~ USBHS_EP_R_RES_MASK;
                USBHSD->UEP2_RX_CTRL |= USBHS_EP_R_RES_ACK;
            }
        }
        USBHSD->INT_FG = USBHS_TRANSFER_FLAG;
    }
    else if( USBHS_Int_Flag & USBHS_SETUP_FLAG )
    {
        /* SETUP包处理 */
        USBHS_Dev_SetupReqLen = pMySetupReqPak->wLength;
        USBHS_Dev_SetupReqCode = pMySetupReqPak->bRequest;
        chtype = pMySetupReqPak->bRequestType;
        len = 0x00;
        errflag = 0x00;

        /* 判断当前是标准请求还是其他请求 */
        if( ( pMySetupReqPak->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
        {
            /* 其它请求,如类请求,产商请求等 */
            if( pMySetupReqPak->bRequestType & 0x40 )
            {
                /* 厂商请求 */
                switch( USBHS_Dev_SetupReqCode )
                {
                    default:
                        errflag = 0xFF;                                         /* 操作失败 */
                        break;
                }
            }
            else if( pMySetupReqPak->bRequestType & 0x20 )
            {
                /* 类请求 */
            }

            /* 判断是否可以正常处理 */
            if( errflag != 0xFF )
            {
                if( USBHS_Dev_SetupReqLen > len )
                {
                    USBHS_Dev_SetupReqLen = len;
                }
                len = ( USBHS_Dev_SetupReqLen >= USBHS_UEP0_SIZE ) ? USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
                memcpy( EP0_Databuf, pDescr, len );
                pDescr += len;
            }
        }
        else
        {
            /* 处理标准USB请求包 */
            switch( USBHS_Dev_SetupReqCode )
            {
                case USB_GET_DESCRIPTOR:
                {
                    switch( ( ( pMySetupReqPak->wValue ) >> 8 ) )
                    {
                        case USB_DESCR_TYP_DEVICE:
                            /* 获取设备描述符 */
                            pDescr = MyDevDescr;
                            len = MyDevDescr[ 0 ];
                            break;

                        case USB_DESCR_TYP_CONFIG:
                            /* 获取配置描述符 */
                            pDescr = MyCfgDescr_HS;
                            len = MyCfgDescr_HS[ 2 ] | ( (UINT16)MyCfgDescr_HS[ 3 ] << 8 );
                            break;

                        case USB_DESCR_TYP_STRING:
                            /* 获取字符串描述符 */
                            switch( ( pMySetupReqPak->wValue ) & 0xff )
                            {
                                case 0:
                                    /* 语言字符串描述符 */
                                    pDescr = MyLangDescr;
                                    len = MyLangDescr[ 0 ];
                                    break;

                                case 1:
                                    /* USB产商字符串描述符 */
                                    pDescr = MyManuInfo;
                                    len = sizeof( MyManuInfo );
                                    break;

                                case 2:
                                    /* USB产品字符串描述符 */
                                    pDescr = MyProdInfo;
                                    len = sizeof( MyProdInfo );
                                    break;

                                case 3:
                                    /* USB序列号字符串描述符 */
                                    pDescr = MySerNumInfo;
                                    len = sizeof( MySerNumInfo );
                                    break;

                                default:
                                    errflag = 0xFF;
                                    break;
                            }
                            break;

                        case 6:
                            /* 设备限定描述符 */
                            pDescr = ( PUINT8 )&MyUSBQUADesc[ 0 ];
                            len = sizeof( MyUSBQUADesc );
                            break;

                        case 7:
                            /* 其他速度配置描述符 */
                            if( USBHS_Dev_Speed == 0x01 )
                            {
                                /* 高速模式 */
                                memcpy( &TAB_USB_HS_OSC_DESC[ 2 ], &MyCfgDescr_FS[ 2 ], sizeof( MyCfgDescr_FS ) - 2 );
                                pDescr = ( PUINT8 )&TAB_USB_HS_OSC_DESC[ 0 ];
                                len = sizeof( TAB_USB_HS_OSC_DESC );
                            }
                            else if( USBHS_Dev_Speed == 0x00 )
                            {
                                /* 全速模式 */
                                memcpy( &TAB_USB_FS_OSC_DESC[ 2 ], &MyCfgDescr_HS[ 2 ], sizeof( MyCfgDescr_HS ) - 2 );
                                pDescr = ( PUINT8 )&TAB_USB_FS_OSC_DESC[ 0 ];
                                len = sizeof( TAB_USB_FS_OSC_DESC );
                            }
                            else
                            {
                                errflag = 0xFF;
                            }
                            break;

                        case 0x0f:
                            /* BOS描述符 */
                            /* USB2.0设备不支持BOS描述符 */
                            errflag = 0xFF;
                            break;
                        default :
                            errflag = 0xFF;
                            break;
                    }

                    /* 判断是否可以正常处理 */
                    if( errflag != 0xFF )
                    {
                        if( USBHS_Dev_SetupReqLen > len )
                        {
                            USBHS_Dev_SetupReqLen = len;
                        }
                        len = ( USBHS_Dev_SetupReqLen >= USBHS_UEP0_SIZE ) ? USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
                        memcpy( EP0_Databuf, pDescr, len );
                        pDescr += len;
                    }
                }
                break;

                case USB_SET_ADDRESS:
                    /* 设置地址 */
                    USBHS_Dev_Address = ( pMySetupReqPak->wValue )& 0xff;
                    break;

                case USB_GET_CONFIGURATION:
                    /* 获取配置值 */
                    EP0_Databuf[ 0 ] = USBHS_Dev_Config;
                    if( USBHS_Dev_SetupReqLen > 1 )
                    {
                        USBHS_Dev_SetupReqLen = 1;
                    }
                    break;

                case USB_SET_CONFIGURATION:
                    /* 设置配置值 */
                    USBHS_Dev_Config = ( pMySetupReqPak->wValue ) & 0xff;
                    USBHS_Dev_EnumStatus = 0x01;
                    break;

                case USB_CLEAR_FEATURE:
                    /* 清除特性 */
                    if( ( pMySetupReqPak->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                    {
                        /* 清除端点 */
                        switch( ( pMySetupReqPak->wIndex ) & 0xff )/* wIndexL */
                        {
                            case 0x82:
                                /* SET Endp2 Tx to USBHS_EP_T_RES_NAK;USBHS_EP_T_TOG_0;len = 0 */
                                USBHSD->UEP2_TX_LEN = 0;
                                USBHSD->UEP2_TX_CTRL = USBHS_EP_T_RES_NAK | USBHS_EP_T_TOG_0;
                                break;

                            case 0x02:
                                /* SET Endp2 Rx to USBHS_EP_R_RES_ACK;USBHS_EP_R_TOG_0 */
                                USBHSD->UEP2_TX_CTRL = USBHS_EP_R_RES_ACK | USBHS_EP_R_TOG_0;
                                break;

                            case 0x81:
                                /* SET Endp1 Tx to USBHS_EP_T_RES_NAK;USBHS_EP_T_TOG_0;len = 0 */
                                USBHSD->UEP1_TX_LEN = 0;
                                USBHSD->UEP1_TX_CTRL = USBHS_EP_T_RES_NAK | USBHS_EP_T_TOG_0;
                                break;

                            case 0x01:
                                /* SET Endp1 Rx to USBHS_EP_R_RES_NAK;USBHS_EP_R_TOG_0 */
                                USBHSD->UEP1_RX_CTRL = USBHS_EP_R_RES_ACK | USBHS_EP_R_TOG_0;
                                break;

                            default:
                                errflag = 0xFF;
                                break;
                        }
                    }
                    else if( ( pMySetupReqPak->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                    {
                        if( ( ( pMySetupReqPak->wValue ) & 0xff ) == 1 )/* wIndexL */
                        {
                            USBHS_Dev_SleepStatus &= ~0x01;
                        }
                    }
                    else
                    {
                        errflag = 0xFF;
                    }
                    break;

                case USB_SET_FEATURE:
                    /* 设置特性 */
                    if( ( pMySetupReqPak->bRequestType & 0x1F ) == 0x00 )
                    {
                        /* 设置设备 */
                        if( pMySetupReqPak->wValue == 0x01 )
                        {
                            if( MyCfgDescr_HS[ 7 ] & 0x20 )
                            {
                                /* 设置唤醒使能标志 */
                                USBHS_Dev_SleepStatus = 0x01;
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
                    else if( ( pMySetupReqPak->bRequestType & 0x1F ) == 0x02 )
                    {
                        /* 设置端点 */
                        if( pMySetupReqPak->wValue == 0x00 )
                        {
                            /* 设置指定端点STALL */
                            switch( ( pMySetupReqPak->wIndex ) & 0xff )
                            {
                                case 0x82:
                                    /* 设置端点2 IN STALL */
                                    USBHSD->UEP2_TX_CTRL = ( USBHSD->UEP2_TX_CTRL & ~USBHS_EP_T_RES_MASK ) | USBHS_EP_T_RES_STALL;
                                    break;

                                case 0x02:
                                    /* 设置端点2 OUT Stall */
                                    USBHSD->UEP2_RX_CTRL = ( USBHSD->UEP2_RX_CTRL & ~USBHS_EP_R_RES_MASK ) | USBHS_EP_R_RES_STALL;
                                    break;

                                case 0x81:
                                    /* 设置端点1 IN STALL */
                                    USBHSD->UEP1_TX_CTRL = ( USBHSD->UEP1_TX_CTRL & ~USBHS_EP_T_RES_MASK ) | USBHS_EP_T_RES_STALL;
                                    break;

                                case 0x01:
                                    /* 设置端点1 OUT STALL */
                                    USBHSD->UEP1_RX_CTRL = ( USBHSD->UEP1_RX_CTRL & ~USBHS_EP_R_RES_MASK ) | USBHS_EP_R_RES_STALL;
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
                    EP0_Databuf[ 0 ] = 0x00;
                    if( USBHS_Dev_SetupReqLen > 1 )
                    {
                        USBHS_Dev_SetupReqLen = 1;
                    }
                    break;

                case USB_SET_INTERFACE:
                    EP0_Databuf[ 0 ] = 0x00;
                    if( USBHS_Dev_SetupReqLen > 1 )
                    {
                        USBHS_Dev_SetupReqLen = 1;
                    }
                    break;

                case USB_GET_STATUS:
                    /* 根据当前端点实际状态进行应答 */
                    EP0_Databuf[ 0 ] = 0x00;
                    EP0_Databuf[ 1 ] = 0x00;
                    if( pMySetupReqPak->wIndex == 0x81 )
                    {
                        if( ( USBHSD->UEP1_TX_CTRL & USBHS_EP_T_RES_MASK ) == USBHS_EP_T_RES_STALL )
                        {
                            EP0_Databuf[ 0 ] = 0x01;
                        }
                    }
                    else if( pMySetupReqPak->wIndex == 0x01 )
                    {
                        if( ( USBHSD->UEP1_RX_CTRL & USBHS_EP_R_RES_MASK ) == USBHS_EP_R_RES_STALL )
                        {
                            EP0_Databuf[ 0 ] = 0x01;
                        }
                    }
                    else if( pMySetupReqPak->wIndex == 0x82 )
                    {
                        if( ( USBHSD->UEP2_TX_CTRL & USBHS_EP_T_RES_MASK ) == USBHS_EP_T_RES_STALL )
                        {
                            EP0_Databuf[ 0 ] = 0x01;
                        }
                    }
                    else if( pMySetupReqPak->wIndex == 0x02 )
                    {
                        if( ( USBHSD->UEP2_RX_CTRL & USBHS_EP_R_RES_MASK ) == USBHS_EP_R_RES_STALL )
                        {
                            EP0_Databuf[ 0 ] = 0x01;
                        }
                    }
                    if( USBHS_Dev_SetupReqLen > 2 )
                    {
                        USBHS_Dev_SetupReqLen = 2;
                    }
                    break;

                default:
                    errflag = 0xff;
                    break;
            }
        }

        /* 端点0处理 */
        if( errflag == 0xFF )
        {
            /* IN - STALL / OUT - DATA - STALL */
            USBHS_Dev_SetupReqCode = 0xFF;
            USBHSD->UEP0_TX_LEN  = 0;
            USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_STALL;
            USBHSD->UEP0_RX_CTRL = USBHS_EP_R_RES_STALL;
        }
        else
        {
            /* DATA stage (IN -DATA1-ACK) */
            if( chtype & 0x80 )
            {
                len = ( USBHS_Dev_SetupReqLen> USBHS_UEP0_SIZE ) ? USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
                USBHS_Dev_SetupReqLen -= len;
            }
            else
            {
                len = 0;
            }
            USBHSD->UEP0_TX_LEN  = len;
            USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_ACK | USBHS_EP_T_TOG_1;
        }
        USBHSD->INT_FG = USBHS_SETUP_FLAG;
    }
    else if( USBHS_Int_Flag & USBHS_DETECT_FLAG )
    {
        /* USB总线复位中断 */
#if 0
        printf("USB ReSet!!!\n");
#endif
        USBHS_Dev_Address = 0x00;
        USBHS_Device_Endp_Init( );                                              /* USB2.0高速设备端点初始化 */
        USBHS_Device_SetAddress( USBHS_Dev_Address );                           /* USB2.0高速设备设置设备地址 */
        USBHSD->INT_FG = USBHS_DETECT_FLAG;
    }
    else if( USBHS_Int_Flag & USBHS_SUSPEND_FLAG )
    {
        /* USB总线挂起/唤醒完成中断 */
        /* 唤醒 */
#if 0
        printf("USB SUSPEND!!!\n");

#endif
        USBHS_Dev_SleepStatus &= ~0x02;
        USBHS_Dev_EnumStatus = 0x01;
        USBHSD->INT_FG = USBHS_SUSPEND_FLAG;
    }
}

