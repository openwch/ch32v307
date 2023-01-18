/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_usbfs_device.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/08/20
* Description        : This file provides all the USBOTG firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "ch32v30x_usbfs_device.h"

/*******************************************************************************/
/* Variable Definition */
/* Global */
const    uint8_t  *pUSBFS_Descr;

/* Setup Request */
volatile uint8_t  USBFS_SetupReqCode;
volatile uint8_t  USBFS_SetupReqType;
volatile uint16_t USBFS_SetupReqValue;
volatile uint16_t USBFS_SetupReqIndex;
volatile uint16_t USBFS_SetupReqLen;

/* USB Device Status */
volatile uint8_t  USBFS_DevConfig;
volatile uint8_t  USBFS_DevAddr;
volatile uint8_t  USBFS_DevSleepStatus;
volatile uint8_t  USBFS_DevEnumStatus;

/* HID Class Command */
volatile uint8_t  USBFS_HidIdle;
volatile uint8_t  USBFS_HidProtocol;
volatile uint16_t Hid_Report_Ptr;

/* HID Report Buffer */
__attribute__ ((aligned(4))) uint8_t  HID_Report_Buffer[DEF_USBD_FS_PACK_SIZE];

/* Endpoint Buffer */
__attribute__ ((aligned(4))) uint8_t USBFS_EP0_Buf[ DEF_USBD_UEP0_SIZE ];
__attribute__ ((aligned(4))) uint8_t USBFS_EP1_Buf[ DEF_USBD_ENDP1_SIZE ];
__attribute__ ((aligned(4))) uint8_t USBFS_EP2_Buf[ DEF_USBD_ENDP2_SIZE ];
__attribute__ ((aligned(4))) uint8_t USBFS_EP3_Buf[ DEF_USBD_ENDP3_SIZE ];
__attribute__ ((aligned(4))) uint8_t USBFS_EP4_Buf[ DEF_USBD_ENDP4_SIZE*2 ];

/* USB IN Endpoint Busy Flag */
volatile uint8_t  USBFS_Endp_Busy[ DEF_UEP_NUM ];

/******************************************************************************/
/* Interrupt Service Routine Declaration*/
void OTG_FS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      USBFS_RCC_Init
 *
 * @brief   Initializes the usbotg clock configuration.
 *
 * @return  none
 */
void USBFS_RCC_Init(void)
{
#ifdef CH32V30x_D8C
    RCC_USBCLK48MConfig( RCC_USBCLK48MCLKSource_USBPHY );
    RCC_USBHSPLLCLKConfig( RCC_HSBHSPLLCLKSource_HSE );
    RCC_USBHSConfig( RCC_USBPLL_Div2 );
    RCC_USBHSPLLCKREFCLKConfig( RCC_USBHSPLLCKREFCLK_4M );
    RCC_USBHSPHYPLLALIVEcmd( ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_USBHS, ENABLE );
#else
    if( SystemCoreClock == 144000000 )
    {
        RCC_OTGFSCLKConfig( RCC_OTGFSCLKSource_PLLCLK_Div3 );
    }
    else if( SystemCoreClock == 96000000 ) 
    {
        RCC_OTGFSCLKConfig( RCC_OTGFSCLKSource_PLLCLK_Div2 );
    }
    else if( SystemCoreClock == 48000000 ) 
    {
        RCC_OTGFSCLKConfig( RCC_OTGFSCLKSource_PLLCLK_Div1 );
    }
#endif
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_OTG_FS, ENABLE );
}

/*********************************************************************
 * @fn      USBFS_Device_Endp_Init
 *
 * @brief   Initializes USB device endpoints.
 *
 * @return  none
 */
void USBFS_Device_Endp_Init( void )
{
    uint8_t i;

    USBOTG_FS->UEP4_1_MOD = USBFS_UEP1_TX_EN|USBFS_UEP4_TX_EN|USBFS_UEP4_RX_EN;
    USBOTG_FS->UEP2_3_MOD = USBFS_UEP2_RX_EN|USBFS_UEP3_TX_EN;

    USBOTG_FS->UEP0_DMA = (uint32_t)USBFS_EP0_Buf;
    USBOTG_FS->UEP1_DMA = (uint32_t)USBFS_EP1_Buf;
    USBOTG_FS->UEP2_DMA = (uint32_t)(uint8_t *)&UART2_Tx_Buf[ 0 ];
    USBOTG_FS->UEP3_DMA = (uint32_t)(uint8_t *)&UART2_Rx_Buf[ 0 ];
    USBOTG_FS->UEP4_DMA = (uint32_t)USBFS_EP4_Buf;

    USBOTG_FS->UEP0_RX_CTRL = USBFS_UEP_R_RES_ACK;
    USBOTG_FS->UEP2_RX_CTRL = USBFS_UEP_R_RES_ACK;
    USBOTG_FS->UEP4_RX_CTRL = USBFS_UEP_R_RES_ACK;

    USBOTG_FS->UEP1_TX_LEN = 0;
    USBOTG_FS->UEP3_TX_LEN = 0;
    USBOTG_FS->UEP4_TX_LEN = 0;

    USBOTG_FS->UEP0_TX_CTRL = USBFS_UEP_T_RES_NAK;
    USBOTG_FS->UEP1_TX_CTRL = USBFS_UEP_T_RES_NAK;
    USBOTG_FS->UEP3_TX_CTRL = USBFS_UEP_T_RES_NAK;
    USBOTG_FS->UEP4_TX_CTRL = USBFS_UEP_T_RES_NAK;

    /* Clear End-points Busy Status */
    for( i=0; i<DEF_UEP_NUM; i++ )
    {
        USBFS_Endp_Busy[ i ] = 0;
    }
}

/*********************************************************************
 * @fn      USBFS_Device_Init
 *
 * @brief   Initializes USB device.
 *
 * @return  none
 */
void USBFS_Device_Init( FunctionalState sta )
{
    if( sta )
    {
        USBOTG_H_FS->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;
        Delay_Us( 10 );
        USBOTG_H_FS->BASE_CTRL = 0x00;
        USBOTG_FS->INT_EN = USBFS_UIE_SUSPEND | USBFS_UIE_BUS_RST | USBFS_UIE_TRANSFER;
        USBOTG_FS->BASE_CTRL = USBFS_UC_DEV_PU_EN | USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN;
        USBFS_Device_Endp_Init( );
        USBOTG_FS->UDEV_CTRL = USBFS_UD_PD_DIS | USBFS_UD_PORT_EN;
        NVIC_EnableIRQ(OTG_FS_IRQn);
    }
    else
    {
        USBOTG_H_FS->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;
        Delay_Us( 10 );
        USBOTG_H_FS->BASE_CTRL = 0x00;
        NVIC_DisableIRQ(OTG_FS_IRQn);
    }
}

/*********************************************************************
 * @fn      USBFS_Endp_DataUp
 *
 * @brief   USBFS device data upload
 *
 * @return  none
 */
uint8_t USBFS_Endp_DataUp(uint8_t endp, uint8_t *pbuf, uint16_t len, uint8_t mod)
{
    uint8_t endp_mode;
    uint8_t buf_load_offset;

    /* DMA config, endp_ctrl config, endp_len config */
    if( (endp>=DEF_UEP1) && (endp<=DEF_UEP7) )
    {
        if( USBFS_Endp_Busy[ endp ] == 0 )
        {
            if( (endp == DEF_UEP1) || (endp == DEF_UEP4) )
            {
                /* endp1/endp4 */
                endp_mode = USBFSD_UEP_MOD(0);
                if( endp == DEF_UEP1 )
                {
                    endp_mode = (uint8_t)(endp_mode>>4);
                }
            }
            else if( (endp == DEF_UEP2) || (endp == DEF_UEP3) )
            {
                /* endp2/endp3 */
                endp_mode = USBFSD_UEP_MOD(1);
                if( endp == DEF_UEP3 )
                {
                    endp_mode = (uint8_t)(endp_mode>>4);
                }
            }
            else if( (endp == DEF_UEP5) || (endp == DEF_UEP6) )
            {
                /* endp5/endp6 */
                endp_mode = USBFSD_UEP_MOD(2);
                if( endp == DEF_UEP6 )
                {
                    endp_mode = (uint8_t)(endp_mode>>4);
                }
            }
            else
            {
                /* endp7 */
                endp_mode = USBFSD_UEP_MOD(3);
            }

            if( endp_mode & USBFSD_UEP_TX_EN )
            {
                if( endp_mode & USBFSD_UEP_RX_EN )
                {
                    buf_load_offset = 64;
                }
                else
                {
                    buf_load_offset = 0;
                }

                if( buf_load_offset == 0 )
                {
                    if( mod == DEF_UEP_DMA_LOAD )
                    {
                        /* DMA mode */
                        USBFSD_UEP_DMA(endp) = (uint16_t)(uint32_t)pbuf;
                    }
                    else
                    {
                        /* copy mode */
                        memcpy( USBFSD_UEP_BUF(endp), pbuf, len );
                    }
                }
                else
                {
                    memcpy( USBFSD_UEP_BUF(endp)+buf_load_offset, pbuf, len );
                }
                /* tx length */
                USBFSD_UEP_TLEN(endp) = len;
                /* response ack */
                USBFSD_UEP_TX_CTRL(endp) = (USBFSD_UEP_TX_CTRL(endp) & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;
                /* Set end-point busy */
                USBFS_Endp_Busy[ endp ] = 0x01;
            }
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 1;
    }
    return 0;
}


/*********************************************************************
 * @fn      USBHD_IRQHandler
 *
 * @brief   This function handles HD-FS exception.
 *
 * @return  none
 */
void OTG_FS_IRQHandler( void )
{
    uint8_t  intflag, intst, errflag;
    uint16_t len, i;
    uint32_t baudrate;

    intflag = USBOTG_FS->INT_FG;
    intst   = USBOTG_FS->INT_ST;

    if( intflag & USBFS_UIF_TRANSFER )
    {
        switch (intst & USBFS_UIS_TOKEN_MASK)
        {
            /* data-in stage processing */
            case USBFS_UIS_TOKEN_IN:
                switch ( intst & ( USBFS_UIS_TOKEN_MASK | USBFS_UIS_ENDP_MASK ) )
                {
                    /* end-point 0 data in interrupt */
                    case USBFS_UIS_TOKEN_IN | DEF_UEP0:
                        if( USBFS_SetupReqLen == 0 )
                        {
                            USBOTG_FS->UEP0_RX_CTRL = USBFS_UEP_R_TOG | USBFS_UEP_R_RES_ACK;
                        }
                        if ( ( USBFS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                        {
                            /* Non-standard request endpoint 0 Data upload */
                        }
                        else
                        {
                            /* Standard request endpoint 0 Data upload */
                            switch( USBFS_SetupReqCode )
                            {
                                case USB_GET_DESCRIPTOR:
                                        len = USBFS_SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
                                        memcpy( USBFS_EP0_Buf, pUSBFS_Descr, len );
                                        USBFS_SetupReqLen -= len;
                                        pUSBFS_Descr += len;
                                        USBOTG_FS->UEP0_TX_LEN   = len;
                                        USBOTG_FS->UEP0_TX_CTRL ^= USBFS_UEP_T_TOG;
                                        break;

                                case USB_SET_ADDRESS:
                                        USBOTG_FS->DEV_ADDR = (USBOTG_FS->DEV_ADDR & USBFS_UDA_GP_BIT) | USBFS_DevAddr;
                                        break;

                                default:
                                        break;
                            }
                        }
                        break;

                    /* end-point 1 data in interrupt */
                    case ( USBFS_UIS_TOKEN_IN | DEF_UEP1 ):
                        USBOTG_FS->UEP1_TX_CTRL ^= USBFS_UEP_T_TOG;
                        USBOTG_FS->UEP1_TX_CTRL = (USBOTG_FS->UEP1_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_NAK;
                        USBFS_Endp_Busy[ DEF_UEP1 ] = 0;
                        break;

                    /* end-point 3 data in interrupt */
                    case ( USBFS_UIS_TOKEN_IN | DEF_UEP3 ):
                        USBOTG_FS->UEP3_TX_CTRL ^= USBFS_UEP_T_TOG;
                        USBOTG_FS->UEP3_TX_CTRL = (USBOTG_FS->UEP3_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_NAK;
                        USBFS_Endp_Busy[ DEF_UEP3 ] = 0;
                        Uart.USB_Up_IngFlag = 0x00;
                        break;

                    /* end-point 4 data in interrupt */
                    case ( USBFS_UIS_TOKEN_IN | DEF_UEP4 ):
                        USBOTG_FS->UEP4_TX_CTRL ^= USBFS_UEP_T_TOG;
                        USBOTG_FS->UEP4_TX_CTRL = (USBOTG_FS->UEP4_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_NAK;
                        USBFS_Endp_Busy[ DEF_UEP4 ] = 0;
                        break;

                    default :
                        break;
                }
                break;

            /* data-out stage processing */
            case USBFS_UIS_TOKEN_OUT:
                switch ( intst & ( USBFS_UIS_TOKEN_MASK | USBFS_UIS_ENDP_MASK ) )
                {
                    /* end-point 0 data out interrupt */
                    case USBFS_UIS_TOKEN_OUT | DEF_UEP0:
                        len = USBOTG_FS->RX_LEN;
                        if ( intst & USBFS_UIS_TOG_OK )
                        {
                            if ( ( USBFS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                            {
                                /* Non-standard request end-point 0 Data download */
                                USBFS_SetupReqLen = 0;
                                /* Non-standard request end-point 0 Data download */
                                if( USBFS_SetupReqCode == CDC_SET_LINE_CODING )
                                {
                                      /* Save relevant parameters such as serial port baud rate */
                                      /* The downlinked data is processed in the endpoint 0 OUT packet, the 7 bytes of the downlink are, in order
                                         4 bytes: baud rate value: lowest baud rate byte, next lowest baud rate byte, next highest baud rate byte, highest baud rate byte.
                                         1 byte: number of stop bits (0: 1 stop bit; 1: 1.5 stop bit; 2: 2 stop bits).
                                         1 byte: number of parity bits (0: None; 1: Odd; 2: Even; 3: Mark; 4: Space).
                                         1 byte: number of data bits (5,6,7,8,16); */
                                      Uart.Com_Cfg[ 0 ] = USBFS_EP0_Buf[ 0 ];
                                      Uart.Com_Cfg[ 1 ] = USBFS_EP0_Buf[ 1 ];
                                      Uart.Com_Cfg[ 2 ] = USBFS_EP0_Buf[ 2 ];
                                      Uart.Com_Cfg[ 3 ] = USBFS_EP0_Buf[ 3 ];
                                      Uart.Com_Cfg[ 4 ] = USBFS_EP0_Buf[ 4 ];
                                      Uart.Com_Cfg[ 5 ] = USBFS_EP0_Buf[ 5 ];
                                      Uart.Com_Cfg[ 6 ] = USBFS_EP0_Buf[ 6 ];
                                      Uart.Com_Cfg[ 7 ] = DEF_UARTx_RX_TIMEOUT;

                                      /* 保存当前串口的波特率 */
                                      baudrate = USBFS_EP0_Buf[ 0 ];
                                      baudrate += ((uint32_t)USBFS_EP0_Buf[ 1 ] << 8 );
                                      baudrate += ((uint32_t)USBFS_EP0_Buf[ 2 ] << 16 );
                                      baudrate += ((uint32_t)USBFS_EP0_Buf[ 3 ] << 24 );
                                      Uart.Com_Cfg[ 7 ] = Uart.Rx_TimeOutMax;

                                      /* 串口初始化操作 */
                                      UART2_USB_Init( );
                                 }
                                 else if( USBFS_SetupReqCode == HID_SET_REPORT )
                                 {
                                     memcpy(&HID_Report_Buffer[Hid_Report_Ptr],USBFS_EP0_Buf,len);
                                     USBFS_SetupReqLen -= len;
                                     Hid_Report_Ptr += len;
                                     USBOTG_FS->UEP0_RX_CTRL ^= USBFS_UEP_R_TOG;
                                     USBOTG_FS->UEP0_RX_CTRL = (USBOTG_FS->UEP0_RX_CTRL & USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_ACK;
                                 }
                            }
                            else
                            {
                                /* Standard request end-point 0 Data download */
                                /* Add your code here */
                            }
                            if( USBFS_SetupReqLen == 0 )
                            {
                                USBOTG_FS->UEP0_TX_LEN  = 0;
                                USBOTG_FS->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK;
                            }
                        }
                        break;

                    /* end-point 2 data out interrupt */
                    case USBFS_UIS_TOKEN_OUT | DEF_UEP2:
                        USBOTG_FS->UEP2_RX_CTRL ^= USBFS_UEP_R_TOG;

                        Uart.Tx_PackLen[ Uart.Tx_LoadNum ] = USBOTG_FS->RX_LEN;
                        Uart.Tx_LoadNum++;
                        USBOTG_FS->UEP2_DMA = (uint32_t)(uint8_t *)&UART2_Tx_Buf[ ( Uart.Tx_LoadNum * DEF_USB_FS_PACK_LEN ) ];
                        if( Uart.Tx_LoadNum >= DEF_UARTx_TX_BUF_NUM_MAX )
                        {
                            Uart.Tx_LoadNum = 0x00;
                            USBOTG_FS->UEP2_DMA = (uint32_t)(uint8_t *)&UART2_Tx_Buf[ 0 ];
                        }
                        Uart.Tx_RemainNum++;

                        if( Uart.Tx_RemainNum >= ( DEF_UARTx_TX_BUF_NUM_MAX - 2 ) )
                        {
                            USBOTG_FS->UEP2_RX_CTRL &= ~USBFS_UEP_R_RES_MASK;
                            USBOTG_FS->UEP2_RX_CTRL |= USBFS_UEP_R_RES_NAK;
                            Uart.USB_Down_StopFlag = 0x01;
                        }
                        break;

                        /* end-point 4 data out interrupt */
                    case USBFS_UIS_TOKEN_OUT | DEF_UEP4:
                          USBOTG_FS->UEP4_RX_CTRL ^= USBFS_UEP_R_TOG;
                          /* Reverse the data and re-upload */
                          len = USBOTG_FS->RX_LEN;
                          for( i = 0; i < len; i++ )
                          {
                              USBFS_EP4_Buf[ i + 64 ] = ~USBFS_EP4_Buf[ i ];
                          }
                          USBOTG_FS->UEP4_TX_LEN  = len;
                          USBOTG_FS->UEP4_TX_CTRL &= ~USBFS_UEP_R_RES_MASK;
                          USBOTG_FS->UEP4_TX_CTRL |= USBFS_UEP_R_RES_ACK;
                        break;

                    default:
                        break;
                }
                break;

            /* Setup stage processing */
            case USBFS_UIS_TOKEN_SETUP:
                USBOTG_FS->UEP0_TX_CTRL = USBFS_UEP_T_TOG|USBFS_UEP_T_RES_NAK;
                USBOTG_FS->UEP0_RX_CTRL = USBFS_UEP_R_TOG|USBFS_UEP_R_RES_NAK;
                /* Store All Setup Values */
                USBFS_SetupReqType  = pUSBFS_SetupReqPak->bRequestType;
                USBFS_SetupReqCode  = pUSBFS_SetupReqPak->bRequest;
                USBFS_SetupReqLen   = pUSBFS_SetupReqPak->wLength;
                USBFS_SetupReqValue = pUSBFS_SetupReqPak->wValue;
                USBFS_SetupReqIndex = pUSBFS_SetupReqPak->wIndex;
                len = 0;
                errflag = 0;
                if ( ( USBFS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                {
                    /* usb non-standard request processing */
                    if( USBFS_SetupReqType & USB_REQ_TYP_CLASS )
                    {
                        /* Class requests */
                        switch( USBFS_SetupReqCode )
                        {
                            case CDC_GET_LINE_CODING:
                                pUSBFS_Descr = (uint8_t *)&Uart.Com_Cfg[ 0 ];
                                len = 7;
                                break;

                            case CDC_SET_LINE_CODING:
                                break;

                            case CDC_SET_LINE_CTLSTE:
                                break;

                            case CDC_SEND_BREAK:
                                break;

                            case HID_SET_REPORT:                            /* 0x09: SET_REPORT */
                                 Hid_Report_Ptr = 0;
                                 break;

                            case HID_GET_REPORT:                            /* 0x01: GET_REPORT */
                                if( USBFS_SetupReqIndex == 0x00 )
                                {
                                    Hid_Report_Ptr = 0;
                                    len = (USBFS_SetupReqLen >= DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
                                    memcpy( USBFS_EP0_Buf, &HID_Report_Buffer[Hid_Report_Ptr], len );
                                    Hid_Report_Ptr += len;
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                                break;

                            case HID_SET_IDLE:                              /* 0x0A: SET_IDLE */
                                if( USBFS_SetupReqIndex == 0x00 )
                                {
                                    USBFS_HidIdle = (uint8_t)( USBFS_SetupReqValue >> 8 );
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                                break;

                            case HID_SET_PROTOCOL:                          /* 0x0B: SET_PROTOCOL */
                                if( USBFS_SetupReqIndex == 0x00 )
                                {
                                    USBFS_HidProtocol = (uint8_t)USBFS_SetupReqValue;
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                                break;

                            case HID_GET_IDLE:                              /* 0x02: GET_IDLE */
                                if( USBFS_SetupReqIndex == 0x00 )
                                {
                                    USBFS_EP0_Buf[ 0 ] = USBFS_HidIdle;
                                    len = 1;
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                                break;

                             case HID_GET_PROTOCOL:                          /* 0x03: GET_PROTOCOL */
                                 if( USBFS_SetupReqIndex == 0x00 )
                                 {
                                     USBFS_EP0_Buf[ 0 ] = USBFS_HidProtocol;
                                     len = 1;
                                 }
                                 else
                                 {
                                     errflag = 0xFF;
                                 }
                                 break;

                            default:
                                errflag = 0xff;
                                break;
                        }
                    }
                    else if( USBFS_SetupReqType & USB_REQ_TYP_VENDOR )
                    {
                        /* Manufacturer request */
                    }
                    else
                    {
                        errflag = 0xFF;
                    }

                    /* Copy Descriptors to Endp0 DMA buffer */
                    len = (USBFS_SetupReqLen >= DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
                    memcpy( USBFS_EP0_Buf, pUSBFS_Descr, len );
                    pUSBFS_Descr += len;
                }
                else
                {
                    /* usb standard request processing */
                    switch( USBFS_SetupReqCode )
                    {
                        /* get device/configuration/string/report/... descriptors */
                        case USB_GET_DESCRIPTOR:
                            switch( (uint8_t)( USBFS_SetupReqValue >> 8 ) )
                            {
                                /* get usb device descriptor */
                                case USB_DESCR_TYP_DEVICE:
                                    pUSBFS_Descr = MyDevDescr;
                                    len = DEF_USBD_DEVICE_DESC_LEN;
                                    break;

                                /* get usb configuration descriptor */
                                case USB_DESCR_TYP_CONFIG:
                                    pUSBFS_Descr = MyCfgDescr;
                                    len = DEF_USBD_CONFIG_DESC_LEN;
                                    break;

                                /* get hid report descriptor */
                                case USB_DESCR_TYP_REPORT:
                                    pUSBFS_Descr = MyHIDReportDesc;
                                    len = DEF_USBD_REPORT_DESC_LEN;
                                    break;

                                /* get hid descriptor */
                                case USB_DESCR_TYP_HID:
                                    if( USBFS_SetupReqIndex == 0x00 )
                                    {
                                        pUSBFS_Descr = &MyCfgDescr[ 18 ];
                                        len = 9;
                                    }
                                    else
                                    {
                                        errflag = 0xFF;
                                    }
                                    break;

                                /* get usb string descriptor */
                                case USB_DESCR_TYP_STRING:
                                    switch( (uint8_t)( USBFS_SetupReqValue & 0xFF ) )
                                    {
                                        /* Descriptor 0, Language descriptor */
                                        case DEF_STRING_DESC_LANG:
                                            pUSBFS_Descr = MyLangDescr;
                                            len = DEF_USBD_LANG_DESC_LEN;
                                            break;

                                        /* Descriptor 1, Manufacturers String descriptor */
                                        case DEF_STRING_DESC_MANU:
                                            pUSBFS_Descr = MyManuInfo;
                                            len = DEF_USBD_MANU_DESC_LEN;
                                            break;

                                        /* Descriptor 2, Product String descriptor */
                                        case DEF_STRING_DESC_PROD:
                                            pUSBFS_Descr = MyProdInfo;
                                            len = DEF_USBD_PROD_DESC_LEN;
                                            break;

                                        /* Descriptor 3, Serial-number String descriptor */
                                        case DEF_STRING_DESC_SERN:
                                            pUSBFS_Descr = MySerNumInfo;
                                            len = DEF_USBD_SN_DESC_LEN;
                                            break;

                                        default:
                                            errflag = 0xFF;
                                            break;
                                    }
                                    break;

                                default :
                                    errflag = 0xFF;
                                    break;
                            }

                            /* Copy Descriptors to Endp0 DMA buffer */
                            if( USBFS_SetupReqLen>len )
                            {
                                USBFS_SetupReqLen = len;
                            }
                            len = (USBFS_SetupReqLen >= DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
                            memcpy( USBFS_EP0_Buf, pUSBFS_Descr, len );
                            pUSBFS_Descr += len;
                            break;

                        /* Set usb address */
                        case USB_SET_ADDRESS:
                            USBFS_DevAddr = (uint8_t)( USBFS_SetupReqValue & 0xFF );
                            break;

                        /* Get usb configuration now set */
                        case USB_GET_CONFIGURATION:
                            USBFS_EP0_Buf[0] = USBFS_DevConfig;
                            if ( USBFS_SetupReqLen > 1 )
                            {
                                USBFS_SetupReqLen = 1;
                            }
                            break;

                        /* Set usb configuration to use */
                        case USB_SET_CONFIGURATION:
                            USBFS_DevConfig = (uint8_t)( USBFS_SetupReqValue & 0xFF );
                            USBFS_DevEnumStatus = 0x01;
                            break;

                        /* Clear or disable one usb feature */
                        case USB_CLEAR_FEATURE:
                            if ( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                            {
                                /* clear one device feature */
                                if( (uint8_t)( USBFS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
                                {
                                    /* clear usb sleep status, device not prepare to sleep */
                                    USBFS_DevSleepStatus &= ~0x01;
                                }
                            }
                            else if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                            {
                                /* Clear End-point Feature */
                                if( (uint8_t)( USBFS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
                                {
                                    switch( (uint8_t)( USBFS_SetupReqIndex & 0xFF ) )
                                    {
                                        case ( DEF_UEP_IN | DEF_UEP1 ):
                                            /* Set End-point 1 IN NAK */
                                            USBOTG_FS->UEP1_TX_CTRL = USBFS_UEP_T_RES_NAK;
                                            break;

                                        case ( DEF_UEP_OUT | DEF_UEP2 ):
                                            /* Set End-point 2 OUT ACK */
                                            USBOTG_FS->UEP2_RX_CTRL = USBFS_UEP_R_RES_ACK;
                                            break;

                                        case ( DEF_UEP_IN | DEF_UEP3 ):
                                            /* Set End-point 3 IN NAK */
                                            USBOTG_FS->UEP3_TX_CTRL = USBFS_UEP_T_RES_NAK;
                                            break;

                                        case ( DEF_UEP_IN | DEF_UEP4 ):
                                            /* Set End-point 4 IN NAK */
                                            USBOTG_FS->UEP4_TX_CTRL = USBFS_UEP_T_RES_NAK;
                                            break;

                                        case ( DEF_UEP_OUT | DEF_UEP4 ):
                                            /* Set End-point 4 OUT ACK */
                                            USBOTG_FS->UEP4_RX_CTRL = USBFS_UEP_R_RES_ACK;
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

                        /* set or enable one usb feature */
                        case USB_SET_FEATURE:
                            if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                            {
                                /* Set Device Feature */
                                if( (uint8_t)( USBFS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
                                {
                                    if( MyCfgDescr[ 7 ] & 0x20 )
                                    {
                                        /* Set Wake-up flag, device prepare to sleep */
                                        USBFS_DevSleepStatus |= 0x01;
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
                            else if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                            {
                                /* Set End-point Feature */
                                if( (uint8_t)( USBFS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
                                {
                                    /* Set end-points status stall */
                                    switch( (uint8_t)( USBFS_SetupReqIndex & 0xFF ) )
                                    {
                                        case ( DEF_UEP_IN | DEF_UEP1 ):
                                            /* Set End-point 1 IN STALL */
                                            USBOTG_FS->UEP1_TX_CTRL = ( USBOTG_FS->UEP1_TX_CTRL & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_STALL;
                                            break;

                                        case ( DEF_UEP_OUT | DEF_UEP2 ):
                                            /* Set End-point 2 OUT STALL */
                                            USBOTG_FS->UEP2_RX_CTRL = ( USBOTG_FS->UEP2_RX_CTRL & ~USBFS_UEP_R_RES_MASK ) | USBFS_UEP_R_RES_STALL;
                                            break;

                                        case ( DEF_UEP_IN | DEF_UEP3 ):
                                            /* Set End-point 3 IN STALL */
                                            USBOTG_FS->UEP3_TX_CTRL = ( USBOTG_FS->UEP3_TX_CTRL & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_STALL;
                                            break;

                                        case ( DEF_UEP_IN | DEF_UEP4 ):
                                            /* Set End-point 4 IN STALL */
                                            USBOTG_FS->UEP4_TX_CTRL = ( USBOTG_FS->UEP4_TX_CTRL & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_STALL;
                                            break;

                                        case ( DEF_UEP_OUT | DEF_UEP4 ):
                                            /* Set End-point 4 OUT STALL */
                                            USBOTG_FS->UEP4_RX_CTRL = ( USBOTG_FS->UEP4_RX_CTRL & ~USBFS_UEP_R_RES_MASK ) | USBFS_UEP_R_RES_STALL;
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

                        /* This request allows the host to select another setting for the specified interface  */
                        case USB_GET_INTERFACE:
                            USBFS_EP0_Buf[0] = 0x00;
                            if ( USBFS_SetupReqLen > 1 )
                            {
                                USBFS_SetupReqLen = 1;
                            }
                            break;

                        case USB_SET_INTERFACE:
                            break;

                        /* host get status of specified device/interface/end-points */
                        case USB_GET_STATUS:
                            USBFS_EP0_Buf[ 0 ] = 0x00;
                            USBFS_EP0_Buf[ 1 ] = 0x00;
                            if ( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                            {
                                if( USBFS_DevSleepStatus & 0x01 )
                                {
                                    USBFS_EP0_Buf[ 0 ] = 0x02;
                                }
                            }
                            else if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                            {
                                switch( (uint8_t)( USBFS_SetupReqIndex & 0xFF ) )
                                {
                                    case ( DEF_UEP_IN | DEF_UEP1 ):
                                        if( ( (USBOTG_FS->UEP1_TX_CTRL) & USBFS_UEP_T_RES_MASK ) == USBFS_UEP_T_RES_STALL )
                                        {
                                            USBFS_EP0_Buf[ 0 ] = 0x01;
                                        }
                                        break;

                                    case ( DEF_UEP_OUT | DEF_UEP2 ):
                                        if( ( (USBOTG_FS->UEP2_RX_CTRL) & USBFS_UEP_R_RES_MASK ) == USBFS_UEP_R_RES_STALL )
                                        {
                                            USBFS_EP0_Buf[ 0 ] = 0x01;
                                        }
                                        break;

                                    case ( DEF_UEP_IN | DEF_UEP3 ):
                                        if( ( (USBOTG_FS->UEP3_TX_CTRL) & USBFS_UEP_T_RES_MASK ) == USBFS_UEP_T_RES_STALL )
                                        {
                                            USBFS_EP0_Buf[ 0 ] = 0x01;
                                        }
                                        break;

                                    case ( DEF_UEP_IN | DEF_UEP4 ):
                                        if( ( (USBOTG_FS->UEP4_TX_CTRL) & USBFS_UEP_T_RES_MASK ) == USBFS_UEP_T_RES_STALL )
                                        {
                                            USBFS_EP0_Buf[ 0 ] = 0x01;
                                        }
                                        break;

                                    case ( DEF_UEP_OUT | DEF_UEP4 ):
                                        if( ( (USBOTG_FS->UEP4_RX_CTRL) & USBFS_UEP_R_RES_MASK ) == USBFS_UEP_R_RES_STALL )
                                        {
                                            USBFS_EP0_Buf[ 0 ] = 0x01;
                                        }
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

                            if( USBFS_SetupReqLen > 2 )
                            {
                                USBFS_SetupReqLen = 2;
                            }

                            break;

                        default:
                            errflag = 0xFF;
                            break;
                    }
                }
                /* errflag = 0xFF means a request not support or some errors occurred, else correct */
                if( errflag == 0xff)
                {
                    /* if one request not support, return stall */
                    USBOTG_FS->UEP0_TX_CTRL = USBFS_UEP_T_TOG|USBFS_UEP_T_RES_STALL;
                    USBOTG_FS->UEP0_RX_CTRL = USBFS_UEP_R_TOG|USBFS_UEP_R_RES_STALL;
                }
                else
                {
                    /* end-point 0 data Tx/Rx */
                    if( USBFS_SetupReqType & DEF_UEP_IN )
                    {
                        /* tx */
                        len = (USBFS_SetupReqLen>DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
                        USBFS_SetupReqLen -= len;
                        USBOTG_FS->UEP0_TX_LEN  = len;
                        USBOTG_FS->UEP0_TX_CTRL = USBFS_UEP_T_TOG|USBFS_UEP_T_RES_ACK;
                    }
                    else
                    {
                        /* rx */
                        if( USBFS_SetupReqLen == 0 )
                        {
                            USBOTG_FS->UEP0_TX_LEN  = 0;
                            USBOTG_FS->UEP0_TX_CTRL = USBFS_UEP_T_TOG|USBFS_UEP_T_RES_ACK;
                        }
                        else
                        {
                            USBOTG_FS->UEP0_RX_CTRL = USBFS_UEP_R_TOG|USBFS_UEP_R_RES_ACK;
                        }
                    }
                }
                break;

            /* Sof pack processing */
            case USBFS_UIS_TOKEN_SOF:
                break;

            default :
                break;
        }
        USBOTG_FS->INT_FG = USBFS_UIF_TRANSFER;
    }
    else if( intflag & USBFS_UIF_BUS_RST )
    {
        /* usb reset interrupt processing */
        USBOTG_FS->DEV_ADDR = 0;
        USBFS_Device_Endp_Init( );
        UART2_ParaInit( 1 );
        USBOTG_FS->INT_FG = USBFS_UIF_BUS_RST;
    }
    else if( intflag & USBFS_UIF_SUSPEND )
    {
        /* usb suspend interrupt processing */
        if ( USBOTG_FS->MIS_ST & USBFS_UMS_SUSPEND )
        {
            USBFS_DevSleepStatus |= 0x02;
            if( USBFS_DevSleepStatus == 0x03 )
            {
                /* Handling usb sleep here */
            }
        }
        else
        {
            USBFS_DevSleepStatus &= ~0x02;
        }
        USBOTG_FS->INT_FG = USBFS_UIF_SUSPEND;
    }
    else
    {
        /* other interrupts */
        USBOTG_FS->INT_FG = intflag;
    }
}

/*********************************************************************
 * @fn      USBFS_Send_Resume
 *
 * @brief   USBFS device sends wake-up signal to host
 *
 * @return  none
 */
void USBFS_Send_Resume(void)
{

}

