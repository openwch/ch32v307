/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v30x_usbfs_device.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/03/05
 * Description        : This file provides all the USBFS firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x_usbfs_device.h"
#include "iap.h"
/* Global define */
/* OTH */
#define pMySetupReqPakHD        ((PUSB_SETUP_REQ)EP0_DatabufHD)
#define RepDescSize             62
#define DevEP0SIZE              0x40
#define PID_OUT                 0
#define PID_SOF                 1
#define PID_IN                  2
#define PID_SETUP               3
#define Version_Num   0x0100   //V0100
u8 EP2_Tx_Buffer[2];
/******************************************************************************/
/* Endpoint Buffer */
__attribute__ ((aligned(4))) UINT8 EP0_DatabufHD[64]; //ep0(64)
__attribute__ ((aligned(4))) UINT8 EP2_DatabufHD[64 + 64]; //ep2_out(64)+ep2_in(64)

PUINT8 pEP0_RAM_Addr;                       //ep0(64)
PUINT8 pEP2_RAM_Addr;                       //ep2_out(64)+ep2_in(64)

const UINT8 *pDescr;
volatile UINT16 USBFS_Dev_SetupReqLen = 0x00;
volatile UINT8 USBFS_Dev_Config = 0x00;
volatile UINT8 USBFS_Dev_Address = 0x00;
volatile UINT8 USBFS_Dev_SleepStatus = 0x00;
volatile UINT8 DevConfig;
volatile UINT8 SetupReqCode;
volatile UINT16 SetupReqLen;

/******************************************************************************/
/* Device Descriptor */
const UINT8 MyDevDescrHD[] = { 0x12, 0x01, 0x10, 0x01, 0xFF, 0x80, 0x55,
        DevEP0SIZE, 0x48, 0x43, 0xe0, 0x55,  //USB MODULE
        (u8) Version_Num, (u8) (Version_Num >> 8),
        0x00, 0x00, 0x00, 0x01 };

/* Configration Descriptor */
const UINT8 MyCfgDescrHD[] = { 0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80,
        0x32, 0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x80, 0x55, 0x00, 0x07, 0x05,
        0x82, 0x02, 0x40, 0x00, 0x00, 0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00 };

void USBFS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      USBFSD_DeviceInit
 *
 * @brief   Initializes USB device.
 *
 * @return  none
 */
void USBDeviceInit(void) {
    USBFSD->BASE_CTRL = 0x00;

    USBFSD->UEP2_3_MOD = USBFS_UEP2_RX_EN | USBFS_UEP2_TX_EN;

    USBFSD->UEP0_DMA = (UINT32) pEP0_RAM_Addr;
    USBFSD->UEP2_DMA = (UINT32) pEP2_RAM_Addr;

    USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_RES_ACK;
    USBFSD->UEP1_RX_CTRL = USBFS_UEP_R_RES_NAK;
    USBFSD->UEP2_RX_CTRL = USBFS_UEP_R_RES_ACK;
    USBFSD->UEP3_RX_CTRL = USBFS_UEP_R_RES_NAK;
    USBFSD->UEP4_RX_CTRL = USBFS_UEP_R_RES_NAK;
    USBFSD->UEP5_RX_CTRL = USBFS_UEP_R_RES_NAK;
    USBFSD->UEP6_RX_CTRL = USBFS_UEP_R_RES_NAK;
    USBFSD->UEP7_RX_CTRL = USBFS_UEP_R_RES_NAK;


    USBFSD->UEP2_TX_LEN = 8;
    USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_RES_NAK;
    USBFSD->UEP2_TX_CTRL = USBFS_UEP_T_RES_ACK | USBFS_UEP_AUTO_TOG;

    USBFSD->INT_FG = 0xFF;
    USBFSD->INT_EN = USBFS_UIE_SUSPEND | USBFS_UIE_BUS_RST
            | USBFS_UIE_TRANSFER;
    USBFSD->DEV_ADDR = 0x00;

    USBFSD->BASE_CTRL = USBFS_UC_DEV_PU_EN | USBFS_UC_INT_BUSY
            | USBFS_UC_DMA_EN;
    USBFSD->UDEV_CTRL = USBFS_UD_PD_DIS | USBFS_UD_PORT_EN;
}

/*********************************************************************
 * @fn      USBFS_RCC_Init
 *
 * @brief   Initializes the usbfs clock configuration.
 *
 * @return  none
 */
void USBFS_RCC_Init(void) {
#ifdef CH32V30x_D8C
    RCC_USBCLK48MConfig( RCC_USBCLK48MCLKSource_USBPHY);
    RCC_USBHSPLLCLKConfig( RCC_HSBHSPLLCLKSource_HSE);
    RCC_USBHSConfig( RCC_USBPLL_Div2);
    RCC_USBHSPLLCKREFCLKConfig( RCC_USBHSPLLCKREFCLK_4M);
    RCC_USBHSPHYPLLALIVEcmd(ENABLE);
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_USBHS, ENABLE);

#else
    RCC_USBFSCLKConfig(RCC_USBFSCLKSource_PLLCLK_Div1);

#endif

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_USBFS, ENABLE);
}

/*********************************************************************
 * @fn      USBFS_Init
 *
 * @brief   Initializes the USBFS full speed device.
 *
 * @return  none
 */
void USBFS_Init(void) {
    pEP0_RAM_Addr = EP0_DatabufHD;
    pEP2_RAM_Addr = EP2_DatabufHD;

    USBFS_RCC_Init();
    Delay_Us(100);
    USBDeviceInit();
    NVIC_EnableIRQ(USBFS_IRQn);
}

/*********************************************************************
 * @fn      USBFS_IRQHandler
 *
 * @brief   This function handles USB_FS exception.
 *
 * @return  none
 */
void USBFS_IRQHandler(void) {
    UINT8 len, chtype;
    UINT8 intflag, errflag = 0;

    intflag = USBFSD->INT_FG;

    if (intflag & USBFS_UIF_TRANSFER) {
        switch ( USBFSD->INT_ST & USBFS_UIS_TOKEN_MASK) {
        /* SETUP */
        case USBFS_UIS_TOKEN_SETUP:
            USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_NAK;
            USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG | USBFS_UEP_R_RES_ACK;
            SetupReqLen = pSetupReqPakHD->wLength;
            SetupReqCode = pSetupReqPakHD->bRequest;
            chtype = pSetupReqPakHD->bRequestType;
            len = 0;
            errflag = 0;

            if (( pSetupReqPakHD->bRequestType & USB_REQ_TYP_MASK)
                    != USB_REQ_TYP_STANDARD) {
                if ( pSetupReqPakHD->bRequestType & 0x40) {
                    switch ( pSetupReqPakHD->bRequest) {
                    default:
                        errflag = 0xFF;
                        break;
                    }
                } else if ( pSetupReqPakHD->bRequestType & 0x20) {
                    switch ( pSetupReqPakHD->bRequest) {
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

                if (errflag != 0xFF) {
                    if (SetupReqLen > len) {
                        SetupReqLen = len;
                    }
                    len = (USBFS_Dev_SetupReqLen >= DevEP0SIZE) ?
                            DevEP0SIZE : USBFS_Dev_SetupReqLen;
                    memcpy(EP0_DatabufHD, pDescr, len);
                    pDescr += len;
                }
            } else {
                switch (SetupReqCode) {
                case USB_GET_DESCRIPTOR: {
                    switch (((pSetupReqPakHD->wValue) >> 8)) {
                    case USB_DESCR_TYP_DEVICE:
                        pDescr = MyDevDescrHD;
                        len = MyDevDescrHD[0];
                        break;

                    case USB_DESCR_TYP_CONFIG:
                        pDescr = MyCfgDescrHD;
                        len = MyCfgDescrHD[2];
                        break;

                    case USB_DESCR_TYP_STRING:
                        switch ((pSetupReqPakHD->wValue) & 0xff) {
                        case 0:

                            break;

                        case 1:

                            break;

                        case 2:

                            break;

                        case 3:

                            break;

                        default:
                            errflag = 0xFF;
                            break;
                        }
                        break;

                    case USB_DESCR_TYP_REPORT:
                        break;

                    case USB_DESCR_TYP_QUALIF:

                        break;

                    default:
                        errflag = 0xff;
                        break;

                    }

                    if (SetupReqLen > len)
                        SetupReqLen = len;
                    len = (SetupReqLen >= DevEP0SIZE) ?
                            DevEP0SIZE : SetupReqLen;
                    memcpy( pEP0_DataBuf, pDescr, len);
                    pDescr += len;
                }
                    break;

                case USB_SET_ADDRESS:
                    SetupReqLen = (pSetupReqPakHD->wValue) & 0xff;
                    break;

                case USB_GET_CONFIGURATION:
                    pEP0_DataBuf[0] = DevConfig;
                    if (SetupReqLen > 1)
                        SetupReqLen = 1;
                    break;

                case USB_SET_CONFIGURATION:
                    DevConfig = (pSetupReqPakHD->wValue) & 0xff;
                    break;

                case USB_CLEAR_FEATURE:
                    if (( pSetupReqPakHD->bRequestType & USB_REQ_RECIP_MASK)
                            == USB_REQ_RECIP_ENDP) {
                        switch ((pSetupReqPakHD->wIndex) & 0xff) {
                        case 0x82:
                            USBFSD->UEP2_TX_CTRL =
                                    (USBFSD->UEP2_TX_CTRL
                                            & ~( USBFS_UEP_T_TOG
                                                    | USBFS_UEP_T_RES_MASK))
                                            | USBFS_UEP_T_RES_NAK;
                            break;

                        case 0x02:
                            USBFSD->UEP2_RX_CTRL =
                                    (USBFSD->UEP2_RX_CTRL
                                            & ~( USBFS_UEP_R_TOG
                                                    | USBFS_UEP_R_RES_MASK))
                                            | USBFS_UEP_R_RES_ACK;
                            break;

                        case 0x81:
                            USBFSD->UEP1_TX_CTRL =
                                    (USBFSD->UEP1_TX_CTRL
                                            & ~( USBFS_UEP_T_TOG
                                                    | USBFS_UEP_T_RES_MASK))
                                            | USBFS_UEP_T_RES_NAK;
                            break;

                        case 0x01:
                            USBFSD->UEP1_RX_CTRL =
                                    (USBFSD->UEP1_RX_CTRL
                                            & ~( USBFS_UEP_R_TOG
                                                    | USBFS_UEP_R_RES_MASK))
                                            | USBFS_UEP_R_RES_ACK;
                            break;

                        default:
                            errflag = 0xFF;
                            break;

                        }
                    } else
                        errflag = 0xFF;
                    break;

                case USB_SET_FEATURE:
                    if (( pMySetupReqPakHD->bRequestType & 0x1F) == 0x00) {
                        if ( pMySetupReqPakHD->wValue == 0x01) {
                            if (MyCfgDescrHD[7] & 0x20) {
                                USBFS_Dev_SleepStatus = 0x01;
                            } else {
                                errflag = 0xFF;
                            }
                        } else {
                            errflag = 0xFF;
                        }
                    } else if (( pMySetupReqPakHD->bRequestType & 0x1F)
                            == 0x02) {
                        if ( pMySetupReqPakHD->wValue == 0x00) {
                            switch (( pMySetupReqPakHD->wIndex) & 0xff) {
                            case 0x82:
                                /* 设置端点2 IN STALL */
                                USBFSD->UEP2_TX_CTRL =
                                        ( USBFSD->UEP2_TX_CTRL &=
                                                ~USBFS_UEP_T_RES_MASK)
                                                | USBFS_UEP_T_RES_STALL;
                                break;

                            case 0x02:
                                /* 设置端点2 OUT Stall */
                                USBFSD->UEP2_RX_CTRL =
                                        ( USBFSD->UEP2_RX_CTRL &=
                                                ~USBFS_UEP_R_RES_MASK)
                                                | USBFS_UEP_R_RES_STALL;
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

                case USB_GET_INTERFACE:
                    pEP0_DataBuf[0] = 0x00;
                    if (SetupReqLen > 1)
                        SetupReqLen = 1;
                    break;

                case USB_SET_INTERFACE:
                    EP0_DatabufHD[0] = 0x00;
                    if (USBFS_Dev_SetupReqLen > 1) {
                        USBFS_Dev_SetupReqLen = 1;
                    }
                    break;

                case USB_GET_STATUS:
                    EP0_DatabufHD[0] = 0x00;
                    EP0_DatabufHD[1] = 0x00;
                    if ( pMySetupReqPakHD->wIndex == 0x81) {
                        if (( USBFSD->UEP1_TX_CTRL & USBFS_UEP_T_RES_MASK)
                                == USBFS_UEP_T_RES_STALL) {
                            EP0_DatabufHD[0] = 0x01;
                        }
                    } else if ( pMySetupReqPakHD->wIndex == 0x01) {
                        if (( USBFSD->UEP1_RX_CTRL & USBFS_UEP_R_RES_MASK)
                                == USBFS_UEP_R_RES_STALL) {
                            EP0_DatabufHD[0] = 0x01;
                        }
                    } else if ( pMySetupReqPakHD->wIndex == 0x82) {
                        if (( USBFSD->UEP2_TX_CTRL & USBFS_UEP_T_RES_MASK)
                                == USBFS_UEP_T_RES_STALL) {
                            EP0_DatabufHD[0] = 0x01;
                        }
                    } else if ( pMySetupReqPakHD->wIndex == 0x02) {
                        if (( USBFSD->UEP2_RX_CTRL & USBFS_UEP_R_RES_MASK)
                                == USBFS_UEP_R_RES_STALL) {
                            EP0_DatabufHD[0] = 0x01;
                        }
                    }
                    if (USBFS_Dev_SetupReqLen > 2) {
                        USBFS_Dev_SetupReqLen = 2;
                    }
                    break;

                default:
                    errflag = 0xff;
                    break;
                }
            }
            if (errflag == 0xff) {
                USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG
                        | USBFS_UEP_T_RES_STALL;
                USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG
                        | USBFS_UEP_R_RES_STALL;
            } else {
                if (chtype & 0x80) {
                    len = (SetupReqLen > DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                    SetupReqLen -= len;
                } else
                    len = 0;

                USBFSD->UEP0_TX_LEN = len;
                USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK;
                USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_TOG | USBFS_UEP_R_RES_ACK;
            }
            break;

        case USBFS_UIS_TOKEN_IN:
            switch ( USBFSD->INT_ST
                    & ( USBFS_UIS_TOKEN_MASK | USBFS_UIS_ENDP_MASK)) {
            case USBFS_UIS_TOKEN_IN:
                switch (SetupReqCode) {
                case USB_GET_DESCRIPTOR:
                    len = SetupReqLen >= DevEP0SIZE ? DevEP0SIZE : SetupReqLen;
                    memcpy( pEP0_DataBuf, pDescr, len);
                    SetupReqLen -= len;
                    pDescr += len;
                    USBFSD->UEP0_TX_LEN = len;
                    USBFSD->UEP0_TX_CTRL ^= USBFS_UEP_T_TOG;
                    break;

                case USB_SET_ADDRESS:
                    USBFSD->DEV_ADDR = (USBFSD->DEV_ADDR
                            & USBFS_UDA_GP_BIT) | SetupReqLen;
                    USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_RES_NAK;
                    USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_RES_ACK;
                    break;

                default:
                    USBFSD->UEP0_TX_LEN = 0;
                    USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_RES_NAK;
                    USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_RES_ACK;
                    break;

                }
                break;

            case USBFS_UIS_TOKEN_IN | 2:

                USBFSD->UEP2_TX_CTRL ^= USBFS_UEP_T_TOG;
                USBFSD->UEP2_TX_CTRL = (USBFSD->UEP2_TX_CTRL
                        & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_NAK;
                break;

            default:
                break;

            }
            break;

        case USBFS_UIS_TOKEN_OUT:
            switch ( USBFSD->INT_ST
                    & ( USBFS_UIS_TOKEN_MASK | USBFS_UIS_ENDP_MASK)) {
            case USBFS_UIS_TOKEN_OUT:
                len = USBFSD->RX_LEN;
                break;

            case USBFS_UIS_TOKEN_OUT | 2:
                if ( USBFSD->INT_ST & USBFS_UIS_TOG_OK) {
                    USBFSD->UEP2_RX_CTRL ^= USBFS_UEP_R_TOG;
                    len = USBFSD->RX_LEN;
                    DevEP2_OUT_Deal(len);
                }
                break;
            }
            break;

        case USBFS_UIS_TOKEN_SOF:

            break;

        default:
            break;

        }

        USBFSD->INT_FG = USBFS_UIF_TRANSFER;
    } else if (intflag & USBFS_UIF_BUS_RST) {
        USBFSD->DEV_ADDR = 0;

        USBFSD->UEP0_RX_CTRL = USBFS_UEP_R_RES_ACK;
        USBFSD->UEP2_RX_CTRL = USBFS_UEP_R_RES_ACK;

        USBFSD->UEP0_TX_CTRL = USBFS_UEP_T_RES_NAK;
        USBFSD->UEP2_TX_CTRL = USBFS_UEP_T_RES_NAK;

        USBFSD->INT_FG |= USBFS_UIF_BUS_RST;
    } else if (intflag & USBFS_UIF_SUSPEND) {
        if ( USBFSD->MIS_ST & USBFS_UMS_SUSPEND) {
            ;
        } else {
            ;
        }
        USBFSD->INT_FG = USBFS_UIF_SUSPEND;
    } else {
        USBFSD->INT_FG = intflag;
    }
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
void DevEP2_IN_Deal(UINT8 l) {
    USBFSD->UEP2_TX_LEN = l;
    USBFSD->UEP2_TX_CTRL = (USBFSD->UEP2_TX_CTRL & ~USBFS_UEP_T_RES_MASK)
            | USBFS_UEP_T_RES_ACK;
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
void DevEP2_OUT_Deal(UINT8 l) {
    UINT8 s ,EP2_Tx_Cnt;
    memcpy(EP2_Rx_Buffer, pEP2_OUT_DataBuf, l);
    s = RecData_Deal();
    if (s != ERR_End) {
        EP2_Tx_Buffer[0] = 0x00;
        if (s == ERR_ERROR)
            pEP2_IN_DataBuf[1] = 0x01;
        else
            pEP2_IN_DataBuf[1] = 0x00;
        EP2_Tx_Cnt = 2;
        DevEP2_IN_Deal(EP2_Tx_Cnt);
    }
}
