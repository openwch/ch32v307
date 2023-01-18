/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v30x_usbhs_device.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/18
 * Description        : ch32v30x series usb interrupt processing.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


/*******************************************************************************/
/* Header File */
#include "ch32v30x_usbhs_device.h"
#include "usbd_composite_km.h"
#include "ch32v30x_usb.h"

/*******************************************************************************/
/* Variable Definition */

/* Global */
const    uint8_t  *pUSBHS_Descr;

/* Setup Request */
volatile uint8_t  USBHS_SetupReqCode;
volatile uint8_t  USBHS_SetupReqType;
volatile uint16_t USBHS_SetupReqValue;
volatile uint16_t USBHS_SetupReqIndex;
volatile uint16_t USBHS_SetupReqLen;

/* USB Device Status */
volatile uint8_t  USBHS_DevConfig;
volatile uint8_t  USBHS_DevAddr;
volatile uint8_t  USBHS_DevSleepStatus;
volatile uint8_t  USBHS_DevEnumStatus;

/* HID Class Command */
volatile uint8_t  USBHS_HidIdle[ 2 ];
volatile uint8_t  USBHS_HidProtocol[ 2 ];

/* Endpoint Buffer */
__attribute__ ((aligned(4))) uint8_t USBHS_EP0_Buf[ DEF_USBD_UEP0_SIZE ];        //ep0(64)
__attribute__ ((aligned(4))) uint8_t USBHS_EP1_TX_Buf[ DEF_USB_EP1_HS_SIZE ];    //ep1_in(64)
__attribute__ ((aligned(4))) uint8_t USBHS_EP2_TX_Buf[ DEF_USB_EP2_HS_SIZE ];    //ep2_in(64)

/* USB IN Endpoint Busy Flag */
volatile uint8_t  USBHS_Endp_Busy[ DEF_UEP_NUM ];

/******************************************************************************/
/* Interrupt Service Routine Declaration*/
void USBHS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      USBHS_RCC_Init
 *
 * @brief   Initializes the clock for USB2.0 High speed device.
 *
 * @return  none
 */
void USBHS_RCC_Init( void )
{
    RCC_USBCLK48MConfig( RCC_USBCLK48MCLKSource_USBPHY );
    RCC_USBHSPLLCLKConfig( RCC_HSBHSPLLCLKSource_HSE );
    RCC_USBHSConfig( RCC_USBPLL_Div2 );
    RCC_USBHSPLLCKREFCLKConfig( RCC_USBHSPLLCKREFCLK_4M );
    RCC_USBHSPHYPLLALIVEcmd( ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_USBHS, ENABLE );
}

/*********************************************************************
 * @fn      USBHS_Device_Endp_Init
 *
 * @brief   Initialize USB high-speed device endpoint.
 *
 * @return  none
 */
void USBHS_Device_Endp_Init( void )
{
    USBHSD->ENDP_CONFIG = USBHS_UEP0_T_EN | USBHS_UEP0_R_EN | USBHS_UEP1_T_EN | USBHS_UEP2_T_EN;

    USBHSD->UEP0_MAX_LEN = DEF_USBD_UEP0_SIZE;
    USBHSD->UEP1_MAX_LEN = DEF_USB_EP1_HS_SIZE;
    USBHSD->UEP2_MAX_LEN = DEF_USB_EP2_HS_SIZE;

    USBHSD->UEP0_DMA    = (uint32_t)(uint8_t *)USBHS_EP0_Buf;
    USBHSD->UEP1_TX_DMA = (uint32_t)(uint8_t *)USBHS_EP1_TX_Buf;
    USBHSD->UEP2_TX_DMA = (uint32_t)(uint8_t *)USBHS_EP2_TX_Buf;

    USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_RES_NAK;
    USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_RES_ACK;
    USBHSD->UEP1_TX_CTRL = USBHS_UEP_T_RES_NAK;
    USBHSD->UEP2_TX_CTRL = USBHS_UEP_T_RES_NAK;
}

/*********************************************************************
 * @fn      USBHS_Device_Init
 *
 * @brief   Initializes USB high-speed device.
 *
 * @return  none
 */
void USBHS_Device_Init( FunctionalState sta )
{
    if( sta )
    {
        /* Reset USB module */
        USBHSD->CONTROL = USBHS_UC_CLR_ALL | USBHS_UC_RESET_SIE;
        Delay_Us( 10 );
        USBHSD->CONTROL = 0;

        /* Initialize USB device configuration */
        USBHSD->HOST_CTRL = USBHS_UH_PHY_SUSPENDM;
        USBHSD->CONTROL = USBHS_UC_DMA_EN | USBHS_UC_INT_BUSY | USBHS_UC_SPEED_HIGH;
        USBHSD->INT_EN = USBHS_UIE_SETUP_ACT | USBHS_UIE_TRANSFER | USBHS_UIE_DETECT | USBHS_UIE_SUSPEND;
        USBHS_Device_Endp_Init( );
        USBHSD->CONTROL |= USBHS_UC_DEV_PU_EN;
        NVIC_EnableIRQ( USBHS_IRQn );
    }
    else
    {
        USBHSD->CONTROL = USBHS_UC_CLR_ALL | USBHS_UC_RESET_SIE;
        Delay_Us( 10 );
        USBHSD->CONTROL = 0;
        NVIC_DisableIRQ( USBHS_IRQn );
    }
}

/*********************************************************************
 * @fn      USBHS_Endp_DataUp
 *
 * @brief   usbhd-hs device data upload
 *
 * @para    endp: end-point numbers
 *          pubf: data buffer
 *          len: load data length
 *          mod: 0: DEF_UEP_DMA_LOAD 1: DEF_UEP_CPY_LOAD
 *
 * @return  none
 */
uint8_t USBHS_Endp_DataUp( uint8_t endp, uint8_t *pbuf, uint16_t len, uint8_t mod )
{
    uint8_t endp_buf_mode, endp_en, endp_tx_ctrl;

    /* DMA config, endp_ctrl config, endp_len config */
    if( ( endp >= DEF_UEP1 ) && ( endp <= DEF_UEP15 ) )
    {
        endp_en =  USBHSD->ENDP_CONFIG;
        if( endp_en & USBHSD_UEP_TX_EN( endp ) )
        {
            if( ( USBHS_Endp_Busy[ endp ] & DEF_UEP_BUSY ) == 0x00 )
            {
                endp_buf_mode = USBHSD->BUF_MODE;
                /* if end-point buffer mode is double buffer */
                if( endp_buf_mode & USBHSD_UEP_DOUBLE_BUF( endp ) )
                {
                    /* end-point buffer mode is double buffer */
                    /* only end-point tx enable  */
                    if( ( endp_en & USBHSD_UEP_RX_EN( endp ) ) == 0x00 )
                    {
                        endp_tx_ctrl = USBHSD_UEP_TXCTRL( endp );
                        if( mod == DEF_UEP_DMA_LOAD )
                        {
                            if( endp_tx_ctrl & USBHS_UEP_T_TOG_DATA1 )
                            {
                                /* use UEPn_TX_DMA */
                                USBHSD_UEP_TXDMA( endp ) = (uint32_t)pbuf;
                            }
                            else
                            {
                                /* use UEPn_RX_DMA */
                                USBHSD_UEP_RXDMA( endp ) = (uint32_t)pbuf;
                            }
                        }
                        else if( mod == DEF_UEP_CPY_LOAD )
                        {
                            if( endp_tx_ctrl & USBHS_UEP_T_TOG_DATA1 )
                            {
                                /* use UEPn_TX_DMA */
                                memcpy( USBHSD_UEP_TXBUF( endp ), pbuf, len );
                            }
                            else
                            {
                                /* use UEPn_RX_DMA */
                                memcpy( USBHSD_UEP_RXBUF(endp), pbuf, len );
                            }
                        }
                        else
                        {
                            return NoREADY;
                        }
                    }
                    else
                    {
                        return NoREADY;
                    }
                }
                else
                {
                    /* end-point buffer mode is single buffer */
                    if( mod == DEF_UEP_DMA_LOAD )
                    {

                        USBHSD_UEP_TXDMA( endp ) = (uint32_t)pbuf;
                    }
                    else if( mod == DEF_UEP_CPY_LOAD )
                    {
                        /* if end-point buffer mode is double buffer */
                        memcpy( USBHSD_UEP_TXBUF(endp), pbuf, len );
                    }
                    else
                    {
                        return NoREADY;
                    }
                }

                /* endpoint n response tx ack */
                USBHSD_UEP_TLEN( endp ) = len;
                USBHSD_UEP_TXCTRL( endp ) = ( USBHSD_UEP_TXCTRL( endp ) &= ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_ACK;

                /* Set endpoint busy */
                USBHS_Endp_Busy[ endp ] |= DEF_UEP_BUSY;
            }
            else
            {
                return NoREADY;
            }
        }
        else
        {
            return NoREADY;
        }
    }
    else
    {
        return NoREADY;
    }

    return READY;
}

/*********************************************************************
 * @fn      USBHS_IRQHandler
 *
 * @brief   USB device transfer process.
 *
 * @return  none
 */
void USBHS_IRQHandler( void )
{
    uint8_t  intflag, intst, errflag;
    uint16_t len;

    intflag = USBHSD->INT_FG;
    intst = USBHSD->INT_ST;

    if( intflag & USBHS_UIF_TRANSFER )
    {
        switch( intst & USBHS_UIS_TOKEN_MASK )
        {
            /* data-in stage processing */
            case USBHS_UIS_TOKEN_IN:
                switch( intst & ( USBHS_UIS_TOKEN_MASK | USBHS_UIS_ENDP_MASK ) )
                {
                    /* end-point 0 data in interrupt */
                    case USBHS_UIS_TOKEN_IN | DEF_UEP0:
                        if( USBHS_SetupReqLen == 0 )
                        {
                            USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_ACK;
                        }

                        if( ( USBHS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                        {
                            /* Non-standard request endpoint 0 Data upload */
                        }
                        else
                        {
                            switch( USBHS_SetupReqCode )
                            {
                                case USB_GET_DESCRIPTOR:
                                    len = ( USBHS_SetupReqLen >= DEF_USBD_UEP0_SIZE )? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                                    memcpy( USBHS_EP0_Buf, pUSBHS_Descr, len );
                                    USBHS_SetupReqLen -= len;
                                    pUSBHS_Descr += len;
                                    USBHSD->UEP0_TX_LEN = len;
                                    USBHSD->UEP0_TX_CTRL ^= USBHS_UEP_T_TOG_DATA1;
                                    break;

                                case USB_SET_ADDRESS:
                                    USBHSD->DEV_AD = USBHS_DevAddr;
                                    break;

                                default:
                                    break;
                            }
                        }
                        break;

                    /* end-point 1 data in interrupt */
                    case USBHS_UIS_TOKEN_IN | DEF_UEP1:
                        USBHSD->UEP1_TX_CTRL = ( USBHSD->UEP1_TX_CTRL & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_NAK;
                        USBHSD->UEP1_TX_CTRL ^= USBHS_UEP_T_TOG_DATA1;
                        USBHS_Endp_Busy[ DEF_UEP1 ] &= ~DEF_UEP_BUSY;
                        break;

                    /* end-point 2 data in interrupt */
                    case USBHS_UIS_TOKEN_IN | DEF_UEP2:
                        USBHSD->UEP2_TX_CTRL = ( USBHSD->UEP2_TX_CTRL & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_NAK;
                        USBHSD->UEP2_TX_CTRL ^= USBHS_UEP_T_TOG_DATA1;
                        USBHS_Endp_Busy[ DEF_UEP2 ] &= ~DEF_UEP_BUSY;
                        break;

                    default :
                        break;
                }
                break;

            /* data-out stage processing */
            case USBHS_UIS_TOKEN_OUT:
                switch( intst & ( USBHS_UIS_TOKEN_MASK | USBHS_UIS_ENDP_MASK ) )
                {
                    /* end-point 0 data out interrupt */
                    case USBHS_UIS_TOKEN_OUT | DEF_UEP0:
                        if( intst & USBHS_UIS_TOG_OK )
                        {
                            if( ( USBHS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                            {
                                if( ( USBHS_SetupReqType & USB_REQ_TYP_MASK ) == USB_REQ_TYP_CLASS )
                                {
                                    switch( USBHS_SetupReqCode )
                                    {
                                        case HID_SET_REPORT:
                                            KB_LED_Cur_Status = USBHS_EP0_Buf[ 0 ];
                                            USBHS_SetupReqLen = 0;
                                            break;

                                        default:
                                            break;
                                    }
                                }
                            }
                            else
                            {
                                /* Standard request end-point 0 Data download */
                                /* Add your code here */
                            }
                        }
                        if( USBHS_SetupReqLen == 0 )
                        {
                            USBHSD->UEP0_TX_LEN = 0;
                            USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
                        }
                        break;

                    default:
                        break;
                }
                break;

            /* Sof pack processing */
            case USBHS_UIS_TOKEN_SOF:
                break;

            default:
                break;
        }
        USBHSD->INT_FG = USBHS_UIF_TRANSFER;
    }
    else if( intflag & USBHS_UIF_SETUP_ACT )
    {
        USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_NAK;
        USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_NAK;

        /* Store All Setup Values */
        USBHS_SetupReqType  = pUSBHS_SetupReqPak->bRequestType;
        USBHS_SetupReqCode  = pUSBHS_SetupReqPak->bRequest;
        USBHS_SetupReqLen   = pUSBHS_SetupReqPak->wLength;
        USBHS_SetupReqValue = pUSBHS_SetupReqPak->wValue;
        USBHS_SetupReqIndex = pUSBHS_SetupReqPak->wIndex;
        len = 0;
        errflag = 0;

        if( ( USBHS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
        {
            if( ( USBHS_SetupReqType & USB_REQ_TYP_MASK ) == USB_REQ_TYP_CLASS )
            {
                /* Class Request */
                switch( USBHS_SetupReqCode )
                {
                    case HID_SET_REPORT:
                        break;

                    case HID_SET_IDLE:
                        if( USBHS_SetupReqIndex == 0x00 )
                        {
                            USBHS_HidIdle[ 0 ] = (uint8_t)( USBHS_SetupReqValue >> 8 );
                        }
                        else if( USBHS_SetupReqIndex == 0x01 )
                        {
                            USBHS_HidIdle[ 1 ] = (uint8_t)( USBHS_SetupReqValue >> 8 );
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                        break;

                    case HID_SET_PROTOCOL:
                        if( USBHS_SetupReqIndex == 0x00 )
                        {
                            USBHS_HidProtocol[ 0 ] = (uint8_t)USBHS_SetupReqValue;
                        }
                        else if( USBHS_SetupReqIndex == 0x01 )
                        {
                            USBHS_HidProtocol[ 1 ] = (uint8_t)USBHS_SetupReqValue;
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                        break;

                    case HID_GET_IDLE:
                        if( USBHS_SetupReqIndex == 0x00 )
                        {
                            USBHS_EP0_Buf[ 0 ] = USBHS_HidIdle[ 0 ];
                            len = 1;
                        }
                        else if( USBHS_SetupReqIndex == 0x01 )
                        {
                            USBHS_EP0_Buf[ 0 ] = USBHS_HidIdle[ 1 ];
                            len = 1;
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                        break;

                    case HID_GET_PROTOCOL:
                        if( USBHS_SetupReqIndex == 0x00 )
                        {
                            USBHS_EP0_Buf[ 0 ] = USBHS_HidProtocol[ 0 ];
                            len = 1;
                        }
                        else if( USBHS_SetupReqIndex == 0x01 )
                        {
                            USBHS_EP0_Buf[ 0 ] = USBHS_HidProtocol[ 1 ];
                            len = 1;
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                        break;

                    default:
                        errflag = 0xFF;
                        break;
                }
            }
        }
        else
        {
            /* usb standard request processing */
            switch( USBHS_SetupReqCode )
            {
                /* get device/configuration/string/report/... descriptors */
                case USB_GET_DESCRIPTOR:
                    switch( (uint8_t)( USBHS_SetupReqValue >> 8 ) )
                    {
                        /* get usb device descriptor */
                        case USB_DESCR_TYP_DEVICE:
                            pUSBHS_Descr = MyDevDescr;
                            len = DEF_USBD_DEVICE_DESC_LEN;
                            break;

                        /* get usb configuration descriptor */
                        case USB_DESCR_TYP_CONFIG:
                            pUSBHS_Descr = MyCfgDescr;
                            len = DEF_USBD_CONFIG_DESC_LEN;
                            break;

                        /* get usb hid descriptor */
                        case USB_DESCR_TYP_HID:
                            if( USBHS_SetupReqIndex == 0x00 )
                            {
                                pUSBHS_Descr = &MyCfgDescr[ 18 ];
                                len = 9;
                            }
                            else if( USBHS_SetupReqIndex == 0x01 )
                            {
                                pUSBHS_Descr = &MyCfgDescr[ 43 ];
                                len = 9;
                            }
                            else
                            {
                                errflag = 0xFF;
                            }
                            break;

                        /* get usb report descriptor */
                        case USB_DESCR_TYP_REPORT:
                            if( USBHS_SetupReqIndex == 0x00 )
                            {
                                pUSBHS_Descr = KeyRepDesc;
                                len = DEF_USBD_REPORT_DESC_LEN_KB;
                            }
                            else if( USBHS_SetupReqIndex == 0x01 )
                            {
                                pUSBHS_Descr = MouseRepDesc;
                                len = DEF_USBD_REPORT_DESC_LEN_MS;
                            }
                            else
                            {
                                errflag = 0xFF;
                            }
                            break;

                        /* get usb string descriptor */
                        case USB_DESCR_TYP_STRING:
                            switch( (uint8_t)( USBHS_SetupReqValue & 0xFF ) )
                            {
                                /* Descriptor 0, Language descriptor */
                                case DEF_STRING_DESC_LANG:
                                    pUSBHS_Descr = MyLangDescr;
                                    len = DEF_USBD_LANG_DESC_LEN;
                                    break;

                                /* Descriptor 1, Manufacturers String descriptor */
                                case DEF_STRING_DESC_MANU:
                                    pUSBHS_Descr = MyManuInfo;
                                    len = DEF_USBD_MANU_DESC_LEN;
                                    break;

                                /* Descriptor 2, Product String descriptor */
                                case DEF_STRING_DESC_PROD:
                                    pUSBHS_Descr = MyProdInfo;
                                    len = DEF_USBD_PROD_DESC_LEN;
                                    break;

                                /* Descriptor 3, Serial-number String descriptor */
                                case DEF_STRING_DESC_SERN:
                                    pUSBHS_Descr = MySerNumInfo;
                                    len = DEF_USBD_SN_DESC_LEN;
                                    break;

                                default:
                                    errflag = 0xFF;
                                    break;
                            }
                            break;

                        /* get usb qualifier descriptor */
                        case USB_DESCR_TYP_QUALIF:
                            pUSBHS_Descr = MyQuaDesc;
                            len =  DEF_USBD_QUALFY_DESC_LEN;
                            break;

                        default :
                            errflag = 0xFF;
                            break;
                    }

                    /* Copy Descriptors to Endp0 DMA buffer */
                    if( USBHS_SetupReqLen > len )
                    {
                        USBHS_SetupReqLen = len;
                    }
                    len = ( USBHS_SetupReqLen >= DEF_USBD_UEP0_SIZE ) ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                    memcpy( USBHS_EP0_Buf, pUSBHS_Descr, len );
                    pUSBHS_Descr += len;
                    break;

                /* Set usb address */
                case USB_SET_ADDRESS:
                    USBHS_DevAddr = (uint16_t)( USBHS_SetupReqValue & 0xFF );
                    break;

                /* Get usb configuration now set */
                case USB_GET_CONFIGURATION:
                    USBHS_EP0_Buf[ 0 ] = USBHS_DevConfig;
                    if( USBHS_SetupReqLen > 1 )
                    {
                        USBHS_SetupReqLen = 1;
                    }
                    break;

                /* Set usb configuration to use */
                case USB_SET_CONFIGURATION:
                    USBHS_DevConfig = (uint16_t)( USBHS_SetupReqValue & 0xFF );
                    USBHS_DevEnumStatus = 0x01;
                    break;

                /* Clear or disable one usb feature */
                case USB_CLEAR_FEATURE:
                    if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                    {
                        /* Clear End-point Feature */
                        switch( (uint8_t)( USBHS_SetupReqIndex & 0xFF ) )
                        {
                            case ( DEF_UEP_IN | DEF_UEP1 ):
                                /* Set End-point 1 OUT ACK */
                                USBHSD->UEP1_TX_CTRL = USBHS_UEP_T_TOG_DATA0 | USBHS_UEP_T_RES_NAK;
                                break;

                            case ( DEF_UEP_IN | DEF_UEP2 ):
                                /* Set End-point 2 IN NAK */
                                USBHSD->UEP2_TX_CTRL = USBHS_UEP_T_TOG_DATA0 | USBHS_UEP_T_RES_NAK;
                                break;

                            default:
                                errflag = 0xFF;
                                break;
                        }
                    }
                    else if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                    {
                        /* clear one device feature */
                        if( (uint8_t)( USBHS_SetupReqValue & 0xFF ) == 0x01 )
                        {
                            /* clear usb sleep status, device not prepare to sleep */
                            USBHS_DevSleepStatus &= ~0x01;
                        }
                    }
                    else
                    {
                        errflag = 0xFF;
                    }
                    break;

                /* set or enable one usb feature */
                case USB_SET_FEATURE:
                    if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                    {
                        /* Set Device Feature */
                        if( (uint8_t)( USBHS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
                        {
                            if( MyCfgDescr[ 7 ] & 0x20 )
                            {
                                /* Set Wake-up flag, device prepare to sleep */
                                USBHS_DevSleepStatus |= 0x01;
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
                    else if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                    {
                        /* Set End-point Feature */
                        if( (uint8_t)( USBHS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
                        {
                            switch( (uint8_t)( USBHS_SetupReqIndex & 0xFF ) )
                            {
                                case ( DEF_UEP_IN | DEF_UEP1 ):
                                    USBHSD->UEP1_TX_CTRL = ( USBHSD->UEP1_TX_CTRL & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_STALL;
                                    break;

                                case ( DEF_UEP_IN | DEF_UEP2 ):
                                    USBHSD->UEP2_TX_CTRL = ( USBHSD->UEP2_TX_CTRL & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_STALL;
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
                    USBHS_EP0_Buf[ 0 ] = 0x00;
                    if( USBHS_SetupReqLen > 1 )
                    {
                        USBHS_SetupReqLen = 1;
                    }
                    break;

                case USB_SET_INTERFACE:
                    break;

                /* host get status of specified device/interface/end-points */
                case USB_GET_STATUS:
                    USBHS_EP0_Buf[ 0 ] = 0x00;
                    USBHS_EP0_Buf[ 1 ] = 0x00;
                    if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                    {
                        if( USBHS_DevSleepStatus & 0x01 )
                        {
                            USBHS_EP0_Buf[ 0 ] = 0x02;
                        }
                        else
                        {
                            USBHS_EP0_Buf[ 0 ] = 0x00;
                        }
                    }
                    else if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                    {
                        if( (uint8_t)( USBHS_SetupReqIndex & 0xFF ) == ( DEF_UEP_IN | DEF_UEP1 ) )
                        {
                            if( ( USBHSD->UEP1_TX_CTRL & USBHS_UEP_T_RES_MASK ) == USBHS_UEP_T_RES_STALL )
                            {
                                USBHS_EP0_Buf[ 0 ] = 0x01;
                            }
                        }
                        else if( (uint8_t)( USBHS_SetupReqIndex & 0xFF ) == ( DEF_UEP_IN | DEF_UEP2 ) )
                        {
                            if( ( USBHSD->UEP2_TX_CTRL & USBHS_UEP_T_RES_MASK ) == USBHS_UEP_T_RES_STALL )
                            {
                                USBHS_EP0_Buf[ 0 ] = 0x01;
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

                    if( USBHS_SetupReqLen > 2 )
                    {
                        USBHS_SetupReqLen = 2;
                    }
                    break;

                default:
                    errflag = 0xFF;
                    break;
            }
        }

        /* errflag = 0xFF means a request not support or some errors occurred, else correct */
        if( errflag == 0xFF )
        {
            /* if one request not support, return stall */
            USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_STALL;
            USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_STALL;
        }
        else
        {
            /* end-point 0 data Tx/Rx */
            if( USBHS_SetupReqType & DEF_UEP_IN )
            {
                len = ( USBHS_SetupReqLen > DEF_USBD_UEP0_SIZE ) ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                USBHS_SetupReqLen -= len;
                USBHSD->UEP0_TX_LEN = len;
                USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
            }
            else
            {
                if( USBHS_SetupReqLen == 0 )
                {
                    USBHSD->UEP0_TX_LEN = 0;
                    USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
                }
                else
                {
                    USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_ACK;
                }
            }
        }
        USBHSD->INT_FG = USBHS_UIF_SETUP_ACT;
    }
    else if( intflag & USBHS_UIF_BUS_RST )
    {
        /* usb reset interrupt processing */
        USBHS_DevConfig = 0;
        USBHS_DevAddr = 0;
        USBHS_DevSleepStatus = 0;
        USBHS_DevEnumStatus = 0;

        USBHSD->DEV_AD = 0;
        USBHS_Device_Endp_Init( );
        USBHSD->INT_FG = USBHS_UIF_BUS_RST;
    }
    else if( intflag & USBHS_UIF_SUSPEND )
    {
        USBHSD->INT_FG = USBHS_UIF_SUSPEND;

        /* usb suspend interrupt processing */
        if( USBHSD->MIS_ST & USBHS_UMS_SUSPEND )
        {
            USBHS_DevSleepStatus |= 0x02;
            if( USBHS_DevSleepStatus == 0x03 )
            {
                /* Handling usb sleep here */
                MCU_Sleep_Wakeup_Operate( );
            }
        }
        else
        {
            USBHS_DevSleepStatus &= ~0x02;
        }
    }
    else
    {
        /* other interrupts */
        USBHSD->INT_FG = intflag;
    }
}

/*********************************************************************
 * @fn      USBHS_Send_Resume
 *
 * @brief   Send usb k signal, Wake up usb host
 *
 * @return  none
 */
void USBHS_Send_Resume( void )
{
    USBHSH->HOST_CTRL |= USBHS_UH_REMOTE_WKUP;
    Delay_Ms( 5 );
    USBHSH->HOST_CTRL &= ~USBHS_UH_REMOTE_WKUP;
    Delay_Ms( 1 );
}
