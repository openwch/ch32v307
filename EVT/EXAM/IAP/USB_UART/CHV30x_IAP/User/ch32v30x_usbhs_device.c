/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v30x_usbhs_device.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : USB高速操作相关文件
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x_usbhs_device.h"
#include "iap.h"

/******************************************************************************/
/* 常、变量定义 */
#define Version_Num   0x0100   //V0100
/* 函数声明 */
void USBHS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/* USB缓冲区定义 */
__attribute__ ((aligned(4))) UINT8 Ep0Buffer[USBHS_UEP0_SIZE]; /* 端点0数据收发缓冲区 */
__attribute__ ((aligned(4))) UINT8 Ep1Buffer[USBHS_MAX_PACK_SIZE * 2]; /* 端点1数据接收缓冲区 */
__attribute__ ((aligned(4))) UINT8 Ep2Buffer[USBHS_MAX_PACK_SIZE * 2]; /* 端点1数据发送缓冲区 */
__attribute__ ((aligned(4))) UINT8 Ep3Buffer[USBHS_MAX_PACK_SIZE * 2]; /* 端点2数据接收缓冲区 */
#define pMySetupReqPak        ((PUSB_SETUP_REQ)Ep0Buffer)
const UINT8 *pDescr;
volatile UINT8 USBHS_Dev_SetupReqCode = 0xFF; /* USB2.0高速设备Setup包命令码 */
volatile UINT16 USBHS_Dev_SetupReqLen = 0x00; /* USB2.0高速设备Setup包长度 */
volatile UINT8 USBHS_Dev_SetupReqValueH = 0x00; /* USB2.0高速设备Setup包Value高字节 */
volatile UINT8 USBHS_Dev_Config = 0x00; /* USB2.0高速设备配置值 */
volatile UINT8 USBHS_Dev_Address = 0x00; /* USB2.0高速设备地址值 */
volatile UINT8 USBHS_Dev_SleepStatus = 0x00; /* USB2.0高速设备睡眠状态 */
volatile UINT8 USBHS_Dev_EnumStatus = 0x00; /* USB2.0高速设备枚举状态 */
volatile UINT8 USBHS_Dev_Endp0_Tog = 0x01; /* USB2.0高速设备端点0同步标志 */
volatile UINT8 USBHS_Dev_Speed = 0x01; /* USB2.0高速设备速度 */
volatile UINT8 USBHS_Int_Flag = 0x00; /* USB2.0高速设备中断标志 */

volatile UINT16 USBHS_Endp1_Up_Flag = 0x00; /* USB2.0高速设备端点1数据上传状态: 0:空闲; 1:正在上传; */
volatile UINT8 USBHS_Endp1_Down_Flag = 0x00; /* USB2.0高速设备端点1下传成功标志 */
volatile UINT8 USBHS_Endp1_Down_Len = 0x00; /* USB2.0高速设备端点1下传长度 */
volatile UINT8 USBHS_Endp1_T_Tog = 0x00;
volatile UINT8 USBHS_Endp1_R_Tog = 0x00;

volatile UINT16 USBHS_Endp2_Up_Flag = 0x00; /* USB2.0高速设备端点2数据上传状态: 0:空闲; 1:正在上传; */
volatile UINT16 USBHS_Endp2_Up_LoadPtr = 0x00; /* USB2.0高速设备端点2数据上传装载偏移 */
volatile UINT8 USBHS_Endp2_Down_Flag = 0x00; /* USB2.0高速设备端点2下传成功标志 */

u8 Endp1Busy = 0;

u8 EP1_OUT_Flag = 0;
u8 EP2_OUT_Flag = 0;
u8 Endp3Busy = 0;
u8 Flag_LED = 0;
u8 EP1_Rx_Cnt, EP2_Rx_Cnt;
/******************************************************************************/
/* USB设备描述符 */
UINT8 MyDevDescr[18] = { 0x12, 0x01, 0x10, 0x01, 0xFF, 0x80, 0x55, 0x40, 0x48,
        0x43, 0xe0, 0x55,  //USB MODULE
        (u8) Version_Num, (u8) (Version_Num >> 8), //°æ±¾ºÅ V0100
        0x00, 0x00, 0x00, 0x01 };

/* USB配置描述符(全速) */
const UINT8 MyCfgDescr_FS[] = { 0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80,
        0x32, 0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x80, 0x55, 0x00, 0x07, 0x05,
        0x82, 0x02, 0x40, 0x00, 0x00, 0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00 };

/* USB配置描述符(高速) */
const UINT8 MyCfgDescr_HS[] = { 0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80,
        0x32, 0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x80, 0x55, 0x00, 0x07, 0x05,
        0x82, 0x02, 0x40, 0x00, 0x00, 0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00 };

/* USB语言字符串描述符 */
const UINT8 MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };

/* USB产商字符串描述符 */
const UINT8 MyManuInfo[] = {
/* wch.cn */
0x0E, 0x03, 'w', 0x00, 'c', 0x00, 'h', 0x00, '.', 0x00, 'c', 0x00, 'n', 0x00 };

/* USB产品字符串描述符 */
const UINT8 MyProdInfo[] = {
/* WCH USB2.0 DEVICE */
0x26, 0x03, 'W', 0x00, 'C', 0x00, 'H', 0x00, ' ', 0x00, 'U', 0x00, 'S', 0x00,
        'B', 0x00, '2', 0x00, '.', 0x00, '0', 0x00, ' ', 0x00, 'D', 0x00, 'E',
        0x00, 'V', 0x00, 'I', 0x00, 'C', 0x00, 'E', 0x00, ' ', 0x00 };

/* USB序列号字符串描述符 */
const UINT8 MySerNumInfo[] = {
/* 0123456789 */
0x16, 0x03, '0', 0x00, '1', 0x00, '2', 0x00, '3', 0x00, '4', 0x00, '5', 0x00,
        '6', 0x00, '7', 0x00, '8', 0x00, '9', 0x00, };

/* USB设备限定描述符 */
const UINT8 MyUSBQUADesc[] = { 0x0A, 0x06, 0x00, 0x02, 0xFF, 0x00, 0xFF, 0x40,
        0x01, 0x00, };
const UINT8 MyBOSDesc[] = { 0x05, 0x0f, 0x16, 0x00, 0x02, 0x07, 0x10, 0x02,
        0x02, 0x00, 0x00, 0x00, 0x0a, 0x10, 0x03, 0x00, 0x0e, 0x00, 0x01, 0x0a,
        0xff, 0x07, };

/* USB高速模式,其他速度配置描述符 */
UINT8 TAB_USB_HS_OSC_DESC[sizeof(MyCfgDescr_FS)] = { 0x09, 0x07, /* 其他部分通过程序复制 */
};

/*********************************************************************
 * @fn      USBHS_RCC_Init
 *
 * @brief   Initializes the clock for USB2.0 High speed device.
 *
 * @return  none
 */
void USBHS_RCC_Init(void) {
    RCC_USBHSPLLCLKConfig(RCC_HSBHSPLLCLKSource_HSE);
    RCC_USBHSConfig(RCC_USBPLL_Div2);
    RCC_USBHSPLLCKREFCLKConfig(RCC_USBHSPLLCKREFCLK_4M);
    RCC_USBCLK48MConfig(RCC_USBCLK48MCLKSource_USBPHY);
    RCC_USBHSPHYPLLALIVEcmd(ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHS, ENABLE);
}

/*********************************************************************
 * @fn      USBHS_Device_Endp_Init
 *
 * @brief   USB2.0高速设备端点初始化
 *
 * @return  none
 */
void USBHS_Device_Endp_Init(void) {
    /* 使能端点1、端点2发送和接收  */
    USBHSD->ENDP_CONFIG = USBHS_EP0_T_EN | USBHS_EP0_R_EN | USBHS_EP1_T_EN
            | USBHS_EP2_T_EN | USBHS_EP1_R_EN | USBHS_EP2_R_EN | USBHS_EP3_T_EN
            | USBHS_EP3_R_EN | USBHS_EP4_T_EN | USBHS_EP4_R_EN;

    /* 端点非同步端点 */
    USBHSD->ENDP_TYPE = 0x00;

    /* 端点缓冲区模式，非双缓冲区，ISO传输BUF模式需要指定0  */
    USBHSD->BUF_MODE = 0x00;

    /* 端点最大长度包配置 */
    USBHSD->UEP0_MAX_LEN = 64;
    USBHSD->UEP1_MAX_LEN = 64;
    USBHSD->UEP2_MAX_LEN = 64;
    USBHSD->UEP3_MAX_LEN = 64;
    USBHSD->UEP4_MAX_LEN = 64;
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
    USBHSD->UEP0_DMA = (UINT32) (UINT8 *) Ep0Buffer;
    USBHSD->UEP1_RX_DMA = (UINT32) (UINT8 *) Ep1Buffer;
    USBHSD->UEP1_TX_DMA = (UINT32) (UINT8 *) (&Ep1Buffer[64]);
    USBHSD->UEP2_RX_DMA = (UINT32) (UINT8 *) Ep2Buffer;
    USBHSD->UEP2_TX_DMA = (UINT32) (UINT8 *) (&Ep2Buffer[64]);
    USBHSD->UEP3_RX_DMA = (UINT32) (UINT8 *) Ep3Buffer;
    USBHSD->UEP3_TX_DMA = (UINT32) (UINT8 *) (&Ep3Buffer[64]);

    /* 端点控制寄存器配置 */
    USBHSD->UEP0_TX_LEN = 0;
    USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_NAK;
    USBHSD->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK;

    USBHSD->UEP1_TX_LEN = 0;
    USBHSD->UEP1_TX_CTRL = USBHS_EP_T_RES_NAK;
    USBHSD->UEP1_RX_CTRL = USBHS_EP_R_RES_ACK;

    USBHSD->UEP2_TX_LEN = 0;
    USBHSD->UEP2_TX_CTRL = USBHS_EP_T_RES_NAK;
    USBHSD->UEP2_RX_CTRL = USBHS_EP_R_RES_ACK;

    USBHSD->UEP3_TX_LEN = 0;
    USBHSD->UEP3_TX_CTRL = USBHS_EP_T_RES_NAK;
    USBHSD->UEP3_RX_CTRL = USBHS_EP_R_RES_ACK;

    USBHSD->UEP4_TX_LEN = 0;
    USBHSD->UEP4_TX_CTRL = USBHS_EP_T_RES_NAK;
    USBHSD->UEP4_RX_CTRL = USBHS_EP_R_RES_ACK;
}

/*********************************************************************
 * @fn      USBHS_Device_Init
 *
 * @brief   USB2.0高速设备初始化
 *
 * @return  none
 */
void USBHS_Device_Init(FunctionalState sta) {
    if (sta) {
        /* 配置DMA、速度、端点使能等 */
        USBHSD->HOST_CTRL = 0x00;
        USBHSD->HOST_CTRL = USBHS_SUSPENDM;

        USBHSD->CONTROL = 0x00;
        // USBHSD->CONTROL   = USBHS_DMA_EN | USBHS_INT_BUSY_EN | USBHS_HIGH_SPEED;
        USBHSD->CONTROL = USBHS_DMA_EN | USBHS_INT_BUSY_EN | USBHS_FULL_SPEED;
//        USBHSD->CONTROL   = USBHS_DMA_EN | USBHS_INT_BUSY_EN | USBHS_LOW_SPEED;

        USBHSD->INT_EN = 0;
        USBHSD->INT_EN = USBHS_SETUP_ACT_EN | USBHS_TRANSFER_EN
                | USBHS_DETECT_EN | USBHS_SUSPEND_EN;

        /* ALL endpoint enable */
        USBHSD->ENDP_CONFIG = 0xffffffff;

        /* USB2.0高速设备端点初始化 */
        USBHS_Device_Endp_Init();
        Delay_Us(10);

        /* 使能USB连接 */
        USBHSD->CONTROL |= USBHS_DEV_PU_EN;
    } else {
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
void USBHS_Device_SetAddress(UINT32 address) {
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
void USBHS_IRQHandler(void) {
    UINT32 end_num;
    UINT32 rx_token;
    UINT16 len = 0x00;
    UINT8 errflag = 0x00;
    UINT8 chtype;

    USBHS_Int_Flag = USBHSD->INT_FG;

    if (USBHS_Int_Flag & USBHS_TRANSFER_FLAG) {
        /* 端点传输处理 */
        end_num = (USBHSD->INT_ST) & MASK_UIS_ENDP;
        rx_token = (((USBHSD->INT_ST) & MASK_UIS_TOKEN) >> 4) & 0x03;
        /* 00: OUT, 01:SOF, 10:IN, 11:SETUP */
        if (end_num == 0) {
            /* 端点0处理 */
            if (rx_token == PIDhs_IN) {
                /* 端点0上传成功中断 */
                switch (USBHS_Dev_SetupReqCode) {
                case USB_GET_DESCRIPTOR:
                case 0x20: {
                    len = USBHS_Dev_SetupReqLen >= USBHS_UEP0_SIZE ?
                            USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
                    memcpy(Ep0Buffer, pDescr, len);
                    USBHS_Dev_SetupReqLen -= len;
                    pDescr += len;
                    USBHSD->UEP0_TX_LEN = len;
                    USBHSD->UEP0_TX_CTRL ^= USBHS_EP_T_TOG_1;
                    break;
                }

                case USB_SET_ADDRESS:
                    USBHS_Device_SetAddress(USBHS_Dev_Address);
                    USBHSD->UEP0_TX_LEN = 0;
                    USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_NAK;
                    USBHSD->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK;
                    break;

                default:
                    /* 状态阶段完成中断或者是强制上传0长度数据包结束控制传输 */
                    USBHSD->UEP0_TX_LEN = 0;
                    USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_NAK;
                    USBHSD->UEP0_RX_CTRL = USBHS_EP_R_RES_ACK;
                    pDescr = NULL;
                    break;
                }
            } else if (rx_token == PIDhs_OUT) {
                USBHSD->UEP0_TX_LEN = 0;
                USBHSD->UEP0_TX_CTRL |= USBHS_EP_T_RES_ACK;
                USBHSD->UEP0_RX_CTRL |= USBHS_EP_R_RES_ACK;
            }
        } else if (end_num == 1) {
            if (rx_token == PIDhs_IN) {
                USBHSD->UEP1_TX_LEN = 0;
                USBHSD->UEP1_TX_CTRL ^= USBHS_EP_T_TOG_1;
                Endp1Busy = 0;
                USBHSD->UEP1_TX_CTRL = (USBHSD->UEP1_TX_CTRL
                        & (~USBHS_EP_T_RES_MASK)) | USBHS_EP_T_RES_NAK;
            } else if (rx_token == PIDhs_OUT) {
                if (USBHSD->INT_ST & USBHS_DEV_UIS_TOG_OK) {
                    EP1_Rx_Cnt = USBHSD->RX_LEN;
                    EP1_OUT_Flag = 1;
                    USBHSD->UEP1_RX_CTRL = (USBHSD->UEP1_RX_CTRL
                            & (~USBHS_EP_R_RES_MASK)) | USBHS_EP_R_RES_NAK;
                }

            }
        } else if (end_num == 2) {
            if (rx_token == PIDhs_IN) {

                len = USBHSD->UEP2_TX_LEN;

                USBHSD->UEP2_TX_CTRL ^= USBHS_EP_T_TOG_1;

                USBHSD->UEP2_TX_CTRL = (USBHSD->UEP2_TX_CTRL
                        & (~USBHS_EP_T_RES_MASK)) | USBHS_EP_T_RES_NAK;

            } else if (rx_token == PIDhs_OUT) {

                if (USBHSD->INT_ST & USBHS_DEV_UIS_TOG_OK) {
                    USBHSD->UEP2_RX_CTRL ^= USBHS_EP_R_TOG_1;
                    EP2_Rx_Cnt = USBHSD->RX_LEN;
                    EP2_OUT_Flag = 1;

                    DevEPhs_OUT_Deal(EP2_Rx_Cnt);
                }

            }
        }
        USBHSD->INT_FG = USBHS_TRANSFER_FLAG;
    } else if (USBHS_Int_Flag & USBHS_SETUP_FLAG) {
        /* SETUP包处理 */
        USBHS_Dev_SetupReqLen = pMySetupReqPak->wLength;
        USBHS_Dev_SetupReqCode = pMySetupReqPak->bRequest;
        chtype = pMySetupReqPak->bRequestType;
        len = 0x00;
        errflag = 0x00;

        /* 判断当前是标准请求还是其他请求 */
        if (( pMySetupReqPak->bRequestType & USB_REQ_TYP_MASK)
                != USB_REQ_TYP_STANDARD) {
            /* 其它请求,如类请求,产商请求等 */
            if ( pMySetupReqPak->bRequestType & 0x40) {
                /* 厂商请求 */
                switch (USBHS_Dev_SetupReqCode) {
                default:
                    errflag = 0xFF; /* 操作失败 */
                    break;
                }
            } else if ( pMySetupReqPak->bRequestType & 0x20) {
                /* 类请求 */
            }

            /* 判断是否可以正常处理 */
            if (errflag != 0xFF) {
                if (USBHS_Dev_SetupReqLen > len) {
                    USBHS_Dev_SetupReqLen = len;
                }
                len = (USBHS_Dev_SetupReqLen >= USBHS_UEP0_SIZE) ?
                        USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
                memcpy(Ep0Buffer, pDescr, len);
                pDescr += len;
            }
        } else {
            /* 处理标准USB请求包 */
            switch (USBHS_Dev_SetupReqCode) {
            case USB_GET_DESCRIPTOR: {
                switch ((( pMySetupReqPak->wValue) >> 8)) {
                case USB_DESCR_TYP_DEVICE:
                    /* 获取设备描述符 */
                    pDescr = MyDevDescr;
                    len = MyDevDescr[0];
                    break;

                case USB_DESCR_TYP_CONFIG:
                    /* 获取配置描述符 */
                    pDescr = MyCfgDescr_HS;
                    len = MyCfgDescr_HS[2];
                    break;

                case USB_DESCR_TYP_STRING:
                    /* 获取字符串描述符 */
                    switch (( pMySetupReqPak->wValue) & 0xff) {
                    case 0:
                        /* 语言字符串描述符 */
                        pDescr = MyLangDescr;
                        len = MyLangDescr[0];
                        break;

                    case 1:
                        /* USB产商字符串描述符 */
                        pDescr = MyManuInfo;
                        len = sizeof(MyManuInfo);
                        break;

                    case 2:
                        /* USB产品字符串描述符 */
                        pDescr = MyProdInfo;
                        len = sizeof(MyProdInfo);
                        break;

                    case 3:
                        /* USB序列号字符串描述符 */
                        pDescr = MySerNumInfo;
                        len = sizeof(MySerNumInfo);
                        break;

                    default:
                        errflag = 0xFF;
                        break;
                    }
                    break;

                case 6:
                    /* 设备限定描述符 */
                    pDescr = (PUINT8) &MyUSBQUADesc[0];
                    len = sizeof(MyUSBQUADesc);
                    break;

                case 7:
                    /* 其他速度配置描述符 */
                    if (USBHS_Dev_Speed == 0x01)
                        errflag = 0xFF;
                    break;

                case 0x0f:
                    /* BOS描述符 */
                    /* USB2.0设备不支持BOS描述符 */
                    errflag = 0xFF;
                    break;
                default:
                    errflag = 0xFF;
                    break;
                }

                /* 判断是否可以正常处理 */
                if (errflag != 0xFF) {
                    if (USBHS_Dev_SetupReqLen > len) {
                        USBHS_Dev_SetupReqLen = len;
                    }
                    len = (USBHS_Dev_SetupReqLen >= USBHS_UEP0_SIZE) ?
                            USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
                    memcpy(Ep0Buffer, pDescr, len);
                    pDescr += len;
                }
            }
                break;

            case USB_SET_ADDRESS:
                /* 设置地址 */
                USBHS_Dev_Address = ( pMySetupReqPak->wValue) & 0xff;
                break;

            case USB_GET_CONFIGURATION:
                /* 获取配置值 */
                Ep0Buffer[0] = USBHS_Dev_Config;
                if (USBHS_Dev_SetupReqLen > 1) {
                    USBHS_Dev_SetupReqLen = 1;
                }
                break;

            case USB_SET_CONFIGURATION:
                /* 设置配置值 */
                USBHS_Dev_Config = ( pMySetupReqPak->wValue) & 0xff;
                USBHS_Dev_EnumStatus = 0x01;
                break;

            case USB_CLEAR_FEATURE: {
                /* 清除特性 */
                if (( pMySetupReqPak->bRequestType & USB_REQ_RECIP_MASK)
                        == USB_REQ_RECIP_ENDP) {
                    /* 清除端点 */
                    switch (( pMySetupReqPak->wIndex) & 0xff)/* wIndexL */
                    {
                    case 0x82:
                        /* SET Endp2 Tx to USBHS_EP_T_RES_NAK;USBHS_EP_T_TOG_0;len = 0 */
                        USBHSD->UEP2_TX_LEN = 0;
                        USBHSD->UEP2_TX_CTRL = USBHS_EP_T_RES_NAK
                                | USBHS_EP_T_TOG_0;
                        break;

                    case 0x02:
                        /* SET Endp2 Rx to USBHS_EP_R_RES_ACK;USBHS_EP_R_TOG_0 */
                        USBHSD->UEP2_TX_CTRL = USBHS_EP_R_RES_ACK
                                | USBHS_EP_R_TOG_0;
                        break;

                    case 0x81:
                        /* SET Endp1 Tx to USBHS_EP_T_RES_NAK;USBHS_EP_T_TOG_0;len = 0 */
                        USBHSD->UEP1_TX_LEN = 0;
                        USBHSD->UEP1_TX_CTRL = USBHS_EP_T_RES_NAK
                                | USBHS_EP_T_TOG_0;
                        break;

                    case 0x01:
                        /* SET Endp1 Rx to USBHS_EP_R_RES_NAK;USBHS_EP_R_TOG_0 */
                        USBHSD->UEP1_RX_CTRL = USBHS_EP_R_RES_ACK
                                | USBHS_EP_R_TOG_0;
                        break;

                    default:
                        errflag = 0xFF;
                        break;
                    }
                }

                else {
                    errflag = 0xFF;

                }
                break;
                case USB_SET_FEATURE:
                /* 设置特性 */
                if (( pMySetupReqPak->bRequestType & 0x1F) == 0x00) {
                    /* 设置设备 */
                    if ( pMySetupReqPak->wValue == 0x01) {
                        errflag = 0xFF;

                    } else {
                        errflag = 0xFF;
                    }
                } else if (( pMySetupReqPak->bRequestType & 0x1F) == 0x02) {
                    /* 设置端点 */
                    if ( pMySetupReqPak->wValue == 0x00) {
                        /* 设置指定端点STALL */
                        switch (( pMySetupReqPak->wIndex) & 0xff) {
                        case 0x82:
                            /* 设置端点2 IN STALL */
                            USBHSD->UEP2_TX_CTRL = ( USBHSD->UEP2_TX_CTRL
                                    & ~USBHS_EP_T_RES_MASK)
                                    | USBHS_EP_T_RES_STALL;
                            break;

                        case 0x02:
                            /* 设置端点2 OUT Stall */
                            USBHSD->UEP2_RX_CTRL = ( USBHSD->UEP2_RX_CTRL
                                    & ~USBHS_EP_R_RES_MASK)
                                    | USBHS_EP_R_RES_STALL;
                            break;

                        case 0x81:
                            /* 设置端点1 IN STALL */
                            USBHSD->UEP1_TX_CTRL = ( USBHSD->UEP1_TX_CTRL
                                    & ~USBHS_EP_T_RES_MASK)
                                    | USBHS_EP_T_RES_STALL;
                            break;

                        case 0x01:
                            /* 设置端点1 OUT STALL */
                            USBHSD->UEP1_RX_CTRL = ( USBHSD->UEP1_RX_CTRL
                                    & ~USBHS_EP_R_RES_MASK)
                                    | USBHS_EP_R_RES_STALL;
                            break;

                        default:
                            errflag = 0xFF;
                            break;
                        }
                    } else {
                        errflag = 0xFF;
                    }
                } else {
                    errflag = 0xFF;
                }
                break;
            }

            case USB_GET_INTERFACE:
                Ep0Buffer[0] = 0x00;
                if (USBHS_Dev_SetupReqLen > 1) {
                    USBHS_Dev_SetupReqLen = 1;
                }
                break;

            case USB_SET_INTERFACE:
                Ep0Buffer[0] = 0x00;
                if (USBHS_Dev_SetupReqLen > 1) {
                    USBHS_Dev_SetupReqLen = 1;
                }
                break;

            case USB_GET_STATUS:
                /* 根据当前端点实际状态进行应答 */
                Ep0Buffer[0] = 0x00;
                Ep0Buffer[1] = 0x00;
                if ( pMySetupReqPak->wIndex == 0x81) {
                    if (( USBHSD->UEP1_TX_CTRL & USBHS_EP_T_RES_MASK)
                            == USBHS_EP_T_RES_STALL) {
                        Ep0Buffer[0] = 0x01;
                    }
                } else if ( pMySetupReqPak->wIndex == 0x01) {
                    if (( USBHSD->UEP1_RX_CTRL & USBHS_EP_R_RES_MASK)
                            == USBHS_EP_R_RES_STALL) {
                        Ep0Buffer[0] = 0x01;
                    }
                } else if ( pMySetupReqPak->wIndex == 0x82) {
                    if (( USBHSD->UEP2_TX_CTRL & USBHS_EP_T_RES_MASK)
                            == USBHS_EP_T_RES_STALL) {
                        Ep0Buffer[0] = 0x01;
                    }
                } else if ( pMySetupReqPak->wIndex == 0x02) {
                    if (( USBHSD->UEP2_RX_CTRL & USBHS_EP_R_RES_MASK)
                            == USBHS_EP_R_RES_STALL) {
                        Ep0Buffer[0] = 0x01;
                    }
                }
                if (USBHS_Dev_SetupReqLen > 2) {
                    USBHS_Dev_SetupReqLen = 2;
                }
                break;

            default:
                errflag = 0xff;
                break;
            }
        }

        /* 端点0处理 */
        if (errflag == 0xFF) {
            /* IN - STALL / OUT - DATA - STALL */
            USBHS_Dev_SetupReqCode = 0xFF;
            USBHSD->UEP0_TX_LEN = 0;
            USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_STALL;
            USBHSD->UEP0_RX_CTRL = USBHS_EP_R_RES_STALL;
        } else {
            /* DATA stage (IN -DATA1-ACK) */
            if (chtype & 0x80) {
                len = (USBHS_Dev_SetupReqLen > USBHS_UEP0_SIZE) ?
                        USBHS_UEP0_SIZE : USBHS_Dev_SetupReqLen;
                USBHS_Dev_SetupReqLen -= len;
            } else {
                len = 0;
            }
            USBHSD->UEP0_TX_LEN = len;
            USBHSD->UEP0_TX_CTRL = USBHS_EP_T_RES_ACK | USBHS_EP_T_TOG_1;
        }
        USBHSD->INT_FG = USBHS_SETUP_FLAG;
    } else if (USBHS_Int_Flag & USBHS_DETECT_FLAG) {
        USBHS_Dev_Address = 0x00;
        USBHS_Device_Endp_Init(); /* USB2.0高速设备端点初始化 */
        USBHS_Device_SetAddress(USBHS_Dev_Address); /* USB2.0高速设备设置设备地址 */
        USBHSD->INT_FG = USBHS_DETECT_FLAG;
    } else if (USBHS_Int_Flag & USBHS_SUSPEND_FLAG) {
        USBHS_Dev_SleepStatus &= ~0x02;
        USBHS_Dev_EnumStatus = 0x01;
        USBHSD->INT_FG = USBHS_SUSPEND_FLAG;
    }
}

/*********************************************************************
 * @fn      void DevEPhs_IN_Deal
 *
 * @brief   Device endpoint2 IN.
 *
 * @param   l - IN length(<64B)
 *
 * @return  none
 */
void DevEPhs_IN_Deal(UINT8 l) {
    USBHSD->UEP2_TX_LEN = l;
    USBHSD->UEP2_TX_CTRL = (USBHSD->UEP2_TX_CTRL & (~USBHS_EP_T_RES_MASK))
            | USBHS_EP_T_RES_ACK;
}

/*********************************************************************
 * @fn      DevEPhs_OUT_Deal
 *
 * @brief   Deal device Endpoint 2 OUT.
 *
 * @param   l - Data length.
 *
 * @return  none
 */
void DevEPhs_OUT_Deal(UINT8 l) {
    UINT8 s, EP2_Tx_Cnt;
    memcpy(EP2_Rx_Buffer, Ep2Buffer, l);
    s = RecData_Deal();
    if (s != ERR_End) {
        Ep2Buffer[64] = 0x00;
        if (s == ERR_ERROR)
            Ep2Buffer[65] = 0x01;
        else
            Ep2Buffer[65] = 0x00;
        EP2_Tx_Cnt = 2;
        DevEPhs_IN_Deal(EP2_Tx_Cnt);

    }
}

