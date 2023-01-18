/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v30x_usbotg_device.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : This file provides all the USBOTG firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x_usbotg_device.h"
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
volatile UINT16 USBHD_Dev_SetupReqLen = 0x00;
volatile UINT8 USBHD_Dev_Config = 0x00;
volatile UINT8 USBHD_Dev_Address = 0x00;
volatile UINT8 USBHD_Dev_SleepStatus = 0x00;
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

void OTG_FS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      USBOTG_FS_DeviceInit
 *
 * @brief   Initializes USB device.
 *
 * @return  none
 */
void USBDeviceInit(void) {
    USBOTG_FS->BASE_CTRL = 0x00;

    USBOTG_FS->UEP2_3_MOD = USBHD_UEP2_RX_EN | USBHD_UEP2_TX_EN;

    USBOTG_FS->UEP0_DMA = (UINT32) pEP0_RAM_Addr;
    USBOTG_FS->UEP2_DMA = (UINT32) pEP2_RAM_Addr;

    USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_RES_ACK;
    USBOTG_FS->UEP1_RX_CTRL = USBHD_UEP_R_RES_NAK;
    USBOTG_FS->UEP2_RX_CTRL = USBHD_UEP_R_RES_ACK;
    USBOTG_FS->UEP3_RX_CTRL = USBHD_UEP_R_RES_NAK;
    USBOTG_FS->UEP4_RX_CTRL = USBHD_UEP_R_RES_NAK;
    USBOTG_FS->UEP5_RX_CTRL = USBHD_UEP_R_RES_NAK;
    USBOTG_FS->UEP6_RX_CTRL = USBHD_UEP_R_RES_NAK;
    USBOTG_FS->UEP7_RX_CTRL = USBHD_UEP_R_RES_NAK;


    USBOTG_FS->UEP2_TX_LEN = 8;
    USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_RES_NAK;
    USBOTG_FS->UEP2_TX_CTRL = USBHD_UEP_T_RES_ACK | USBHD_UEP_AUTO_TOG;

    USBOTG_FS->INT_FG = 0xFF;
    USBOTG_FS->INT_EN = USBHD_UIE_SUSPEND | USBHD_UIE_BUS_RST
            | USBHD_UIE_TRANSFER;
    USBOTG_FS->DEV_ADDR = 0x00;

    USBOTG_FS->BASE_CTRL = USBHD_UC_DEV_PU_EN | USBHD_UC_INT_BUSY
            | USBHD_UC_DMA_EN;
    USBOTG_FS->UDEV_CTRL = USBHD_UD_PD_DIS | USBHD_UD_PORT_EN;
}

/*********************************************************************
 * @fn      USBOTG_RCC_Init
 *
 * @brief   Initializes the usbotg clock configuration.
 *
 * @return  none
 */
void USBOTG_RCC_Init(void) {
#ifdef CH32V30x_D8C
    RCC_USBCLK48MConfig( RCC_USBCLK48MCLKSource_USBPHY);
    RCC_USBHSPLLCLKConfig( RCC_HSBHSPLLCLKSource_HSE);
    RCC_USBHSConfig( RCC_USBPLL_Div2);
    RCC_USBHSPLLCKREFCLKConfig( RCC_USBHSPLLCKREFCLK_4M);
    RCC_USBHSPHYPLLALIVEcmd(ENABLE);
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_USBHS, ENABLE);

#else
    RCC_OTGFSCLKConfig(RCC_OTGFSCLKSource_PLLCLK_Div1);;

#endif

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_OTG_FS, ENABLE);
}

/*********************************************************************
 * @fn      USBOTG_Init
 *
 * @brief   Initializes the USBOTG full speed device.
 *
 * @return  none
 */
void USBOTG_Init(void) {
    pEP0_RAM_Addr = EP0_DatabufHD;
    pEP2_RAM_Addr = EP2_DatabufHD;

    USBOTG_RCC_Init();
    Delay_Us(100);
    USBDeviceInit();
    NVIC_EnableIRQ(OTG_FS_IRQn);
}

/*********************************************************************
 * @fn      OTG_FS_IRQHandler
 *
 * @brief   This function handles OTG_FS exception.
 *
 * @return  none
 */
void OTG_FS_IRQHandler(void) {
    UINT8 len, chtype;
    UINT8 intflag, errflag = 0;

    intflag = USBOTG_FS->INT_FG;

    if (intflag & USBHD_UIF_TRANSFER) {
        switch ( USBOTG_FS->INT_ST & USBHD_UIS_TOKEN_MASK) {
        /* SETUP */
        case USBHD_UIS_TOKEN_SETUP:
            USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_TOG | USBHD_UEP_T_RES_NAK;
            USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_TOG | USBHD_UEP_R_RES_ACK;
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
                    len = (USBHD_Dev_SetupReqLen >= DevEP0SIZE) ?
                            DevEP0SIZE : USBHD_Dev_SetupReqLen;
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
 ;
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
                            USBOTG_FS->UEP2_TX_CTRL =
                                    (USBOTG_FS->UEP2_TX_CTRL
                                            & ~( USBHD_UEP_T_TOG
                                                    | USBHD_UEP_T_RES_MASK))
                                            | USBHD_UEP_T_RES_NAK;
                            break;

                        case 0x02:
                            USBOTG_FS->UEP2_RX_CTRL =
                                    (USBOTG_FS->UEP2_RX_CTRL
                                            & ~( USBHD_UEP_R_TOG
                                                    | USBHD_UEP_R_RES_MASK))
                                            | USBHD_UEP_R_RES_ACK;
                            break;

                        case 0x81:
                            USBOTG_FS->UEP1_TX_CTRL =
                                    (USBOTG_FS->UEP1_TX_CTRL
                                            & ~( USBHD_UEP_T_TOG
                                                    | USBHD_UEP_T_RES_MASK))
                                            | USBHD_UEP_T_RES_NAK;
                            break;

                        case 0x01:
                            USBOTG_FS->UEP1_RX_CTRL =
                                    (USBOTG_FS->UEP1_RX_CTRL
                                            & ~( USBHD_UEP_R_TOG
                                                    | USBHD_UEP_R_RES_MASK))
                                            | USBHD_UEP_R_RES_ACK;
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
                                USBHD_Dev_SleepStatus = 0x01;
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
                                USBOTG_FS->UEP2_TX_CTRL =
                                        ( USBOTG_FS->UEP2_TX_CTRL &=
                                                ~USBHD_UEP_T_RES_MASK)
                                                | USBHD_UEP_T_RES_STALL;
                                break;

                            case 0x02:
                                /* 设置端点2 OUT Stall */
                                USBOTG_FS->UEP2_RX_CTRL =
                                        ( USBOTG_FS->UEP2_RX_CTRL &=
                                                ~USBHD_UEP_R_RES_MASK)
                                                | USBHD_UEP_R_RES_STALL;
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
                    if (USBHD_Dev_SetupReqLen > 1) {
                        USBHD_Dev_SetupReqLen = 1;
                    }
                    break;

                case USB_GET_STATUS:
                    EP0_DatabufHD[0] = 0x00;
                    EP0_DatabufHD[1] = 0x00;
                    if ( pMySetupReqPakHD->wIndex == 0x81) {
                        if (( USBOTG_FS->UEP1_TX_CTRL & USBHD_UEP_T_RES_MASK)
                                == USBHD_UEP_T_RES_STALL) {
                            EP0_DatabufHD[0] = 0x01;
                        }
                    } else if ( pMySetupReqPakHD->wIndex == 0x01) {
                        if (( USBOTG_FS->UEP1_RX_CTRL & USBHD_UEP_R_RES_MASK)
                                == USBHD_UEP_R_RES_STALL) {
                            EP0_DatabufHD[0] = 0x01;
                        }
                    } else if ( pMySetupReqPakHD->wIndex == 0x82) {
                        if (( USBOTG_FS->UEP2_TX_CTRL & USBHD_UEP_T_RES_MASK)
                                == USBHD_UEP_T_RES_STALL) {
                            EP0_DatabufHD[0] = 0x01;
                        }
                    } else if ( pMySetupReqPakHD->wIndex == 0x02) {
                        if (( USBOTG_FS->UEP2_RX_CTRL & USBHD_UEP_R_RES_MASK)
                                == USBHD_UEP_R_RES_STALL) {
                            EP0_DatabufHD[0] = 0x01;
                        }
                    }
                    if (USBHD_Dev_SetupReqLen > 2) {
                        USBHD_Dev_SetupReqLen = 2;
                    }
                    break;

                default:
                    errflag = 0xff;
                    break;
                }
            }
            if (errflag == 0xff) {
                USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_TOG
                        | USBHD_UEP_T_RES_STALL;
                USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_TOG
                        | USBHD_UEP_R_RES_STALL;
            } else {
                if (chtype & 0x80) {
                    len = (SetupReqLen > DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                    SetupReqLen -= len;
                } else
                    len = 0;

                USBOTG_FS->UEP0_TX_LEN = len;
                USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_TOG | USBHD_UEP_T_RES_ACK;
                USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_TOG | USBHD_UEP_R_RES_ACK;
            }
            break;

        case USBHD_UIS_TOKEN_IN:
            switch ( USBOTG_FS->INT_ST
                    & ( USBHD_UIS_TOKEN_MASK | USBHD_UIS_ENDP_MASK)) {
            case USBHD_UIS_TOKEN_IN:
                switch (SetupReqCode) {
                case USB_GET_DESCRIPTOR:
                    len = SetupReqLen >= DevEP0SIZE ? DevEP0SIZE : SetupReqLen;
                    memcpy( pEP0_DataBuf, pDescr, len);
                    SetupReqLen -= len;
                    pDescr += len;
                    USBOTG_FS->UEP0_TX_LEN = len;
                    USBOTG_FS->UEP0_TX_CTRL ^= USBHD_UEP_T_TOG;
                    break;

                case USB_SET_ADDRESS:
                    USBOTG_FS->DEV_ADDR = (USBOTG_FS->DEV_ADDR
                            & USBHD_UDA_GP_BIT) | SetupReqLen;
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

            case USBHD_UIS_TOKEN_IN | 2:

                USBOTG_FS->UEP2_TX_CTRL ^= USBHD_UEP_T_TOG;
                USBOTG_FS->UEP2_TX_CTRL = (USBOTG_FS->UEP2_TX_CTRL
                        & ~USBHD_UEP_T_RES_MASK) | USBHD_UEP_T_RES_NAK;
                break;

            default:
                break;

            }
            break;

        case USBHD_UIS_TOKEN_OUT:
            switch ( USBOTG_FS->INT_ST
                    & ( USBHD_UIS_TOKEN_MASK | USBHD_UIS_ENDP_MASK)) {
            case USBHD_UIS_TOKEN_OUT:
                len = USBOTG_FS->RX_LEN;
                break;

            case USBHD_UIS_TOKEN_OUT | 2:
                if ( USBOTG_FS->INT_ST & USBHD_UIS_TOG_OK) {
                    USBOTG_FS->UEP2_RX_CTRL ^= USBHD_UEP_R_TOG;
                    len = USBOTG_FS->RX_LEN;
                    DevEP2_OUT_Deal(len);
                }
                break;
            }
            break;

        case USBHD_UIS_TOKEN_SOF:

            break;

        default:
            break;

        }

        USBOTG_FS->INT_FG = USBHD_UIF_TRANSFER;
    } else if (intflag & USBHD_UIF_BUS_RST) {
        USBOTG_FS->DEV_ADDR = 0;

        USBOTG_FS->UEP0_RX_CTRL = USBHD_UEP_R_RES_ACK;
        USBOTG_FS->UEP2_RX_CTRL = USBHD_UEP_R_RES_ACK;

        USBOTG_FS->UEP0_TX_CTRL = USBHD_UEP_T_RES_NAK;
        USBOTG_FS->UEP2_TX_CTRL = USBHD_UEP_T_RES_NAK;

        USBOTG_FS->INT_FG |= USBHD_UIF_BUS_RST;
    } else if (intflag & USBHD_UIF_SUSPEND) {
        if ( USBOTG_FS->MIS_ST & USBHD_UMS_SUSPEND) {
            ;
        } else {
            ;
        }
        USBOTG_FS->INT_FG = USBHD_UIF_SUSPEND;
    } else {
        USBOTG_FS->INT_FG = intflag;
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
    USBOTG_FS->UEP2_TX_LEN = l;
    USBOTG_FS->UEP2_TX_CTRL = (USBOTG_FS->UEP2_TX_CTRL & ~USBHD_UEP_T_RES_MASK)
            | USBHD_UEP_T_RES_ACK;
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
