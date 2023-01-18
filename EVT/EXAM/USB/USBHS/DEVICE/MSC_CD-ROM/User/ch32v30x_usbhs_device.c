/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_usbhs_device.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/08/20
* Description        : This file provides all the USBHS firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x_usbhs_device.h"
#include "SW_CDROM.h"
/******************************************************************************/
/* Variable Definition */
const uint8_t    *pUSBHS_Descr;

/* Setup Request */
volatile uint8_t  USBHS_SetupReqCode;
volatile uint8_t  USBHS_SetupReqType;
volatile uint16_t USBHS_SetupReqValue;
volatile uint16_t USBHS_SetupReqIndex;
volatile uint16_t USBHS_SetupReqLen;

/* USB Device Status */
volatile uint8_t  USBHS_DevConfig;
volatile uint8_t  USBHS_DevAddr;
volatile uint16_t USBHS_DevMaxPackLen;
volatile uint8_t  USBHS_DevSpeed;
volatile uint8_t  USBHS_DevSleepStatus;
volatile uint8_t  USBHS_DevEnumStatus;

/* Endpoint Buffer */
__attribute__ ((aligned(4))) uint8_t USBHS_EP0_Buf[ DEF_USBD_UEP0_SIZE ];
__attribute__ ((aligned(4))) uint8_t CDROM_In_Buf[ DEF_CDROM_PACK_512 ];
__attribute__ ((aligned(4))) uint8_t CDROM_Out_Buf[ DEF_CDROM_PACK_512 ];


/* Endpoint tx busy flag */
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
 * @brief   Initializes USB device endpoints.
 *
 * @return  none
 */
void USBHS_Device_Endp_Init ( void )
{
    uint8_t i;

	USBHSD->ENDP_CONFIG = USBHS_UEP0_T_EN | USBHS_UEP0_R_EN | USBHS_UEP2_T_EN | USBHS_UEP3_R_EN;
	
    USBHSD->UEP0_MAX_LEN  = DEF_USBD_UEP0_SIZE;
	USBHSD->UEP2_MAX_LEN  = DEF_USB_EP2_HS_SIZE;
    USBHSD->UEP3_MAX_LEN  = DEF_USB_EP3_HS_SIZE;

    USBHSD->UEP0_DMA    = (uint32_t)(uint8_t *)USBHS_EP0_Buf;
	USBHSD->UEP2_TX_DMA = (uint32_t)(uint8_t *)CDROM_In_Buf;
    USBHSD->UEP3_RX_DMA = (uint32_t)(uint8_t *)CDROM_Out_Buf;

    USBHSD->UEP0_TX_LEN  = 0;
    USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_RES_NAK;
    USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_RES_ACK;
	
    USBHSD->UEP2_TX_LEN  = 0;
    USBHSD->UEP2_TX_CTRL = USBHS_UEP_T_RES_NAK;
	
    USBHSD->UEP3_RX_CTRL = USBHS_UEP_R_RES_ACK;



    /* Clear End-points Busy Status */
    for( i=0; i<DEF_UEP_NUM; i++ )
    {
        USBHS_Endp_Busy[ i ] = 0;
    }
}

/*********************************************************************
 * @fn      USBHS_Device_Init
 *
 * @brief   Initializes USB high-speed device.
 *
 * @return  none
 */
void USBHS_Device_Init ( FunctionalState sta )
{
    if( sta )
    {
        USBHSD->CONTROL = USBHS_UC_CLR_ALL | USBHS_UC_RESET_SIE;
        Delay_Us(10);
        USBHSD->CONTROL &= ~USBHS_UC_RESET_SIE;
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
        Delay_Us(10);
        USBHSD->CONTROL &= ~USBHS_UC_RESET_SIE;
        NVIC_DisableIRQ( USBHS_IRQn );
    }
}

/*********************************************************************
 * @fn      USBHS_Endp_DataUp
 *
 * @brief   usbhd-hs device data upload
 *          input: endp  - end-point numbers
 *                 *pubf - data buffer
 *                 len   - load data length
 *                 mod   - 0: DEF_UEP_DMA_LOAD 1: DEF_UEP_CPY_LOAD
 *
 * @return  none
 */
uint8_t USBHS_Endp_DataUp( uint8_t endp, uint8_t *pbuf, uint16_t len, uint8_t mod )
{
    uint8_t endp_buf_mode, endp_en, endp_tx_ctrl;

    /* DMA config, endp_ctrl config, endp_len config */
    if( (endp>=DEF_UEP1) && (endp<=DEF_UEP15) )
    {
        endp_en =  USBHSD->ENDP_CONFIG;
        if( endp_en & USBHSD_UEP_TX_EN( endp ) )
        {
            if( (USBHS_Endp_Busy[ endp ] & DEF_UEP_BUSY) == 0x00 )
            {
                endp_buf_mode = USBHSD->BUF_MODE;
                /* if end-point buffer mode is double buffer */
                if( endp_buf_mode & USBHSD_UEP_DOUBLE_BUF( endp ) )
                {
                    /* end-point buffer mode is double buffer */
                    /* only end-point tx enable  */
                    if( (endp_en & USBHSD_UEP_RX_EN( endp )) == 0x00 )
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
                                memcpy( USBHSD_UEP_TXBUF(endp), pbuf, len );
                            }
                            else
                            {
                                /* use UEPn_RX_DMA */
                                memcpy( USBHSD_UEP_RXBUF(endp), pbuf, len );
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
                        return 1;
                    }
                }

                /* end-point n response tx ack */
                USBHSD_UEP_TLEN( endp ) = len;
                USBHSD_UEP_TXCTRL( endp ) = (USBHSD_UEP_TXCTRL( endp ) &= ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_ACK;
                /* Set end-point busy */
                USBHS_Endp_Busy[ endp ] |= DEF_UEP_BUSY;
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
    }
    else
    {
        return 1;
    }

    return 0;
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
    uint8_t  intflag, intst, errflag;
    uint16_t len;

    intflag = USBHSD->INT_FG;
    intst = USBHSD->INT_ST;
    if( intflag & USBHS_UIF_TRANSFER )
    {
        switch ( intst & USBHS_UIS_TOKEN_MASK )
        {
            /* data-in stage processing */
            case USBHS_UIS_TOKEN_IN:
                switch ( intst & ( USBHS_UIS_TOKEN_MASK | USBHS_UIS_ENDP_MASK ) )
                {
                    /* end-point 0 data in interrupt */
                    case USBHS_UIS_TOKEN_IN | DEF_UEP0:
                        if( USBHS_SetupReqLen == 0 )
                        {
                            USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_ACK;
                        }
                        if ( ( USBHS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                        {
                            /* Non-standard request endpoint 0 Data upload */
                        }
                        else
                        {
                            /* Standard request endpoint 0 Data upload */
                            switch( USBHS_SetupReqCode )
                            {
                                case USB_GET_DESCRIPTOR:
                                    len = USBHS_SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                                    memcpy(USBHS_EP0_Buf, pUSBHS_Descr, len);
                                    USBHS_SetupReqLen -= len;
                                    pUSBHS_Descr += len;
                                    USBHSD->UEP0_TX_LEN = len;
                                    USBHSD->UEP0_TX_CTRL ^= USBHS_UEP_T_TOG_DATA1;
                                    break;

                                case USB_SET_ADDRESS:
                                    USBHSD->DEV_AD = USBHS_DevAddr;
                                    break;

                                default:
                                    USBHSD->UEP0_TX_LEN = 0;
                                    break;
                            }
                        }
                        break;

                    /* end-point 1 data in interrupt */
                    case USBHS_UIS_TOKEN_IN | DEF_UEP2:
                        USBHSD->UEP2_TX_CTRL = (USBHSD->UEP2_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_NAK;
                        USBHSD->UEP2_TX_CTRL ^= USBHS_UEP_T_TOG_DATA1;
                        USBHS_Endp_Busy[ DEF_UEP2 ] &= ~DEF_UEP_BUSY;
						CDROM_In_EP_Deal();
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
                         len = USBHSH->RX_LEN;
                         if ( intst & USBHS_UIS_TOG_OK )
                         {
                             /* if any processing about rx, set it here */
                             if ( ( USBHS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                             {
                                 /* Non-standard request end-point 0 Data download */
                             }
                             else
                             {
                                 /* Standard request end-point 0 Data download */
                             }
                             USBHS_SetupReqLen -= len;
                             if( USBHS_SetupReqLen == 0 )
                             {
                                 USBHSD->UEP0_TX_LEN  = 0;
                                 USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
                             }
                         }
                            break;

                    /* end-point 3 data out interrupt */
                    case USBHS_UIS_TOKEN_OUT | DEF_UEP3:
                        if ( intst & USBHS_UIS_TOG_OK )
                        {
                            len = (uint16_t)(USBHSD->RX_LEN);
                            USBHSD->UEP3_RX_CTRL ^= USBHS_UEP_R_TOG_DATA1;
                            USBHSD->UEP3_RX_CTRL = ((USBHSD->UEP3_RX_CTRL) & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_NAK;
							CDROM_Out_EP_Deal(CDROM_Out_Buf,len);
							USBHSD->UEP3_RX_CTRL = ((USBHSD->UEP3_RX_CTRL) & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
                        }
                        break;

                    default:
                        errflag = 0xFF;
                        break;
                }
                break;

            /* Sof pack processing */
            case USBHS_UIS_TOKEN_SOF:
                break;

            default :
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
        if ( ( USBHS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
        {
            /* usb non-standard request processing */
            if (( USBHS_SetupReqType & USB_REQ_TYP_MASK ) == USB_REQ_TYP_CLASS)
            {
               if (USBHS_SetupReqCode == CMD_CDROM_GET_MAX_LUN)
               {
                   USBHS_EP0_Buf[0] = 0;
                   pUSBHS_Descr = (uint8_t*)USBHS_EP0_Buf;
                   len = 1;
               }
               else if (USBHS_SetupReqCode == CMD_CDROM_RESET)
               {
                   /* CDROM Reset */
                   CDROM_Sense_Key = 0x00;
                   CDROM_Sense_ASC = 0x00;
                   CDROM_CSW_Status = 0x00;
                   CDROM_Transfer_Status = 0x00;
                   CDROM_Transfer_DataLen = 0x00;
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
        else
        {
            /* usb standard request processing */
            switch( USBHS_SetupReqCode )
            {
                /* get device/configuration/string/report/... descriptors */
                case USB_GET_DESCRIPTOR:
                    switch( (uint8_t)(USBHS_SetupReqValue>>8) )
                    {
                        /* get usb device descriptor */
                        case USB_DESCR_TYP_DEVICE:
                            pUSBHS_Descr = MyDevDescr;
                            len = DEF_USBD_DEVICE_DESC_LEN;
                            break;

                        /* get usb configuration descriptor */
                        case USB_DESCR_TYP_CONFIG:
                            /* Query current usb speed */
                            if( ( USBHSD->SPEED_TYPE & USBHS_SPEED_TYPE_MASK ) == USBHS_SPEED_HIGH )
                            {
                                /* High speed mode */
                                USBHS_DevSpeed = USBHS_SPEED_HIGH;
                                USBHS_DevMaxPackLen = DEF_USBD_HS_PACK_SIZE;
                                CDROM_Pack_Size = DEF_CDROM_PACK_512;
                            }
                            else
                            {
                                /* Full speed mode */
                                USBHS_DevSpeed = USBHS_SPEED_FULL;
                                USBHS_DevMaxPackLen = DEF_USBD_FS_PACK_SIZE;
                                CDROM_Pack_Size = DEF_CDROM_PACK_64;
                            }

                            /* Load usb configuration descriptor by speed */
                            if( USBHS_DevSpeed == USBHS_SPEED_HIGH )
                            {
                                /* High speed mode */
                                pUSBHS_Descr = MyCfgDescr_HS;
                                len = DEF_USBD_CONFIG_HS_DESC_LEN;
                            }
                            else
                            {
                                /* Full speed mode */
                                pUSBHS_Descr = MyCfgDescr_FS;
                                len = DEF_USBD_CONFIG_FS_DESC_LEN;
                            }
                            break;

                        /* get usb string descriptor */
                        case USB_DESCR_TYP_STRING:
                            switch( (uint8_t)(USBHS_SetupReqValue&0xFF) )
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

                        /* get usb device qualify descriptor */
                        case USB_DESCR_TYP_QUALIF:
                            pUSBHS_Descr = MyQuaDesc;
                            len = DEF_USBD_QUALFY_DESC_LEN;
                            break;

                        /* get usb BOS descriptor */
                        case USB_DESCR_TYP_BOS:
                            /* USB 2.00 DO NOT support BOS descriptor */
                            errflag = 0xFF;
                            break;

                        /* get usb other-speed descriptor */
                        case USB_DESCR_TYP_SPEED:
                            if( USBHS_DevSpeed == USBHS_SPEED_HIGH )
                            {
                                /* High speed mode */
                                memcpy( &TAB_USB_HS_OSC_DESC[ 2 ], &MyCfgDescr_FS[ 2 ], DEF_USBD_CONFIG_FS_DESC_LEN - 2 );
                                pUSBHS_Descr = ( uint8_t * )&TAB_USB_HS_OSC_DESC[ 0 ];
                                len = DEF_USBD_CONFIG_FS_DESC_LEN;
                            }
                            else if( USBHS_DevSpeed == USBHS_SPEED_FULL )
                            {
                                /* Full speed mode */
                                memcpy( &TAB_USB_FS_OSC_DESC[ 2 ], &MyCfgDescr_HS[ 2 ], DEF_USBD_CONFIG_HS_DESC_LEN - 2 );
                                pUSBHS_Descr = ( uint8_t * )&TAB_USB_FS_OSC_DESC[ 0 ];
                                len = DEF_USBD_CONFIG_HS_DESC_LEN;
                            }
                            else
                            {
                                errflag = 0xFF;
                            }
                            break;

                        default :
                            errflag = 0xFF;
                            break;
                    }

                    /* Copy Descriptors to Endp0 DMA buffer */
                    if( USBHS_SetupReqLen>len )
                    {
                        USBHS_SetupReqLen = len;
                    }
                    len = (USBHS_SetupReqLen >= DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                    memcpy( USBHS_EP0_Buf, pUSBHS_Descr, len );
                    pUSBHS_Descr += len;
                    break;

                /* Set usb address */
                case USB_SET_ADDRESS:
                    USBHS_DevAddr = (uint16_t)(USBHS_SetupReqValue&0xFF);
                    break;

                /* Get usb configuration now set */
                case USB_GET_CONFIGURATION:
                    USBHS_EP0_Buf[0] = USBHS_DevConfig;
                    if ( USBHS_SetupReqLen > 1 )
                    {
                        USBHS_SetupReqLen = 1;
                    }
                    break;

                /* Set usb configuration to use */
                case USB_SET_CONFIGURATION:
                    USBHS_DevConfig = (uint8_t)(USBHS_SetupReqValue&0xFF);
                    USBHS_DevEnumStatus = 0x01;
                    break;

                /* Clear or disable one usb feature */
                case USB_CLEAR_FEATURE:
                    if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                    {
                        /* clear one device feature */
                        if((uint8_t)(USBHS_SetupReqValue&0xFF) == 0x01)
                        {
                            /* clear usb sleep status, device not prepare to sleep */
                            USBHS_DevSleepStatus &= ~0x01;
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                    }
                    else if ( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                    {
                        /* Set End-point Feature */
                        if( (uint8_t)(USBHS_SetupReqValue&0xFF) == USB_REQ_FEAT_ENDP_HALT )
                        {
                            /* Clear End-point Feature */
                            switch( (uint8_t)(USBHS_SetupReqIndex&0xFF) )
                            {

                                case (DEF_UEP2 | DEF_UEP_IN):
                                    /* Set End-point 2 IN NAK */
                                    USBHSD->UEP2_TX_CTRL = USBHS_UEP_T_RES_NAK;
                                    /* upload CSW */
                                    if( CDROM_Transfer_Status & DEF_CDROM_CSW_UP_FLAG )
                                    {
                                        CDROM_Up_CSW( );
                                    }
                                    break;

                                case (DEF_UEP3 | DEF_UEP_OUT):
                                    /* Set End-point 3 OUT ACK */
                                    USBHSD->UEP3_RX_CTRL = USBHS_UEP_R_RES_NAK;
                                    /* upload CSW */
                                    if( CDROM_Transfer_Status & DEF_CDROM_CSW_UP_FLAG )
                                    {
                                        CDROM_Up_CSW( );
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
                        if( (uint8_t)(USBHS_SetupReqValue&0xFF) == USB_REQ_FEAT_REMOTE_WAKEUP )
                        {
                            if( MyCfgDescr_FS[ 7 ] & 0x20 )
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
                        if( (uint8_t)(USBHS_SetupReqValue&0xFF) == USB_REQ_FEAT_ENDP_HALT )
                        {
                            /* Set end-points status stall */
                            switch((uint8_t)(USBHS_SetupReqIndex&0xFF) )
                            {

                                case (DEF_UEP2 | DEF_UEP_IN):
                                    /* Set End-point 2 IN STALL */
                                    USBHSD->UEP2_TX_CTRL = ( USBHSD->UEP2_TX_CTRL & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_STALL;
                                    break;

                                case (DEF_UEP3 | DEF_UEP_OUT):
                                    /* Set End-point 3 OUT STALL */
                                    USBHSD->UEP3_RX_CTRL = ( USBHSD->UEP3_RX_CTRL & ~USBHS_UEP_R_RES_MASK ) | USBHS_UEP_R_RES_STALL;
                                    break;

                                default:
                                    errflag = 0xFF;
                                    break;
                            }
                        }
                    }
                    break;

                /* This request allows the host to select another setting for the specified interface  */
                case USB_GET_INTERFACE:
                    USBHS_EP0_Buf[0] = 0x00;
                    if ( USBHS_SetupReqLen > 1 )
                    {
                        USBHS_SetupReqLen = 1;
                    }
                    break;

                case USB_SET_INTERFACE:
                    break;

                /* host get status of specified device/interface/end-points */
                case USB_GET_STATUS:
                    USBHS_EP0_Buf[0] = 0x00;
                    USBHS_EP0_Buf[1] = 0x00;
                    if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                    {
                        switch( (uint8_t)( USBHS_SetupReqIndex & 0xFF ) )
                        {
                             case ( DEF_UEP_IN | DEF_UEP2 ):
                                 if( ( (USBHSD->UEP2_TX_CTRL) & USBHS_UEP_T_RES_MASK ) == USBHS_UEP_T_RES_STALL )
                                 {
                                     USBHS_EP0_Buf[ 0 ] = 0x01;
                                 }
                                 break;

                             case ( DEF_UEP_OUT | DEF_UEP3 ):
                                 if( ( (USBHSD->UEP3_RX_CTRL) & USBHS_UEP_R_RES_MASK ) == USBHS_UEP_R_RES_STALL )
                                 {
                                     USBHS_EP0_Buf[ 0 ] = 0x01;
                                 }
                                 break;

                             default:
                                 errflag = 0xFF;
                                 break;
                        }
                    }
                    else if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                    {
                          if( USBHS_DevSleepStatus & 0x01 )
                          {
                              USBHS_EP0_Buf[ 0 ] = 0x02;
                          }
                    }

                    if ( USBHS_SetupReqLen > 2 )
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
                /* tx */
                len = (USBHS_SetupReqLen>DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                USBHS_SetupReqLen -= len;
                USBHSD->UEP0_TX_LEN = len;
                USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
            }
            else
            {
                /* rx */
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
        if ( USBHSD->MIS_ST & USBHS_UMS_SUSPEND  )
        {
            USBHS_DevSleepStatus |= 0x02;
            if( USBHS_DevSleepStatus == 0x03 )
            {
                /* Handling usb sleep here */
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
 * @fn      USBHD_Send_Resume
 *
 * @brief   USBHD device sends wake-up signal to host
 *
 * @return  none
 */
void USBHD_Send_Resume(void)
{

}
