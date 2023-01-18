/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_usbotg_device.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/08/18
* Description        : This file provides all the USBOTG firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "ch32v30x_usbfs_device.h"
#include "ch32v30x_usb.h"
#include "usbd_compatibility_hid.h"
/*******************************************************************************/
/* Variable Definition */

/* Global */
const uint8_t    *pUSBFS_Descr;

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
volatile uint8_t USBFS_HidIdle;
volatile uint8_t USBFS_HidProtocol;

/* Endpoint Buffer */
__attribute__ ((aligned(4))) uint8_t USBFS_EP0_Buf[DEF_USBD_UEP0_SIZE];   
__attribute__ ((aligned(4))) uint8_t USBFS_EP2_Buf[DEF_USB_EP2_FS_SIZE]; 

/* USB IN Endpoint Busy Flag */
volatile uint8_t  USBFS_Endp_Busy[ DEF_UEP_NUM ];

/* Ring buffer */
RING_BUFF_COMM  RingBuffer_Comm;
__attribute__ ((aligned(4))) uint8_t Data_Buffer[DEF_RING_BUFFER_SIZE];

/******************************************************************************/
/* Interrupt Service Routine Declaration*/
void OTG_FS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      USBFS_RCC_Init
 *
 * @brief   Set USB clock.
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
    USBOTG_FS->UEP4_1_MOD = USBFS_UEP1_RX_EN;
    USBOTG_FS->UEP2_3_MOD = USBFS_UEP2_TX_EN;

    USBOTG_FS->UEP0_DMA = (uint32_t)USBFS_EP0_Buf;
    USBOTG_FS->UEP1_DMA = (uint32_t)Data_Buffer;
    USBOTG_FS->UEP2_DMA = (uint32_t)USBFS_EP2_Buf;

    USBOTG_FS->UEP0_TX_CTRL = USBFS_UEP_T_RES_NAK;
    USBOTG_FS->UEP0_RX_CTRL = USBFS_UEP_R_RES_ACK;
    USBOTG_FS->UEP1_RX_CTRL = USBFS_UEP_R_RES_ACK;
    USBOTG_FS->UEP2_TX_CTRL = USBFS_UEP_T_RES_NAK;
	USBFS_Endp_Busy[ 2 ] = 0;
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
        USBOTG_FS->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;
        Delay_Us( 10 );
        USBOTG_FS->BASE_CTRL = 0;
        USBOTG_FS->INT_EN = USBFS_UIE_SUSPEND | USBFS_UIE_BUS_RST | USBFS_UIE_TRANSFER;
        USBOTG_FS->BASE_CTRL = USBFS_UC_DEV_PU_EN | USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN;
		USBFS_Device_Endp_Init( );
        USBOTG_FS->UDEV_CTRL = USBFS_UD_PD_DIS | USBFS_UD_PORT_EN;
        NVIC_EnableIRQ(OTG_FS_IRQn);
    }
    else
    {
        USBOTG_FS->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;
        Delay_Us( 10 );
        USBOTG_FS->BASE_CTRL = 0x00;
        NVIC_DisableIRQ(OTG_FS_IRQn);
    }
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
    uint8_t  intflag, intst, errflag;
    uint16_t len;

    intflag = USBOTG_FS->INT_FG;
    intst   = USBOTG_FS->INT_ST;

    if( intflag & USBFS_UIF_TRANSFER )
    {
        switch( intst & USBFS_UIS_TOKEN_MASK )
        {
            /* data-in stage processing */
            case USBFS_UIS_TOKEN_IN:
                switch( intst & ( USBFS_UIS_TOKEN_MASK | USBFS_UIS_ENDP_MASK ) )
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
                            switch( USBFS_SetupReqCode )
                            {
                                case USB_GET_DESCRIPTOR:
                                    len = USBFS_SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
                                    memcpy( USBFS_EP0_Buf, pUSBFS_Descr, len );
                                    USBFS_SetupReqLen -= len;
                                    pUSBFS_Descr += len;
                                    USBOTG_FS->UEP0_TX_LEN = len;
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


                        /* end-point 2 data in interrupt */
                        case USBFS_UIS_TOKEN_IN | DEF_UEP2:
                            USBOTG_FS->UEP2_TX_CTRL = (USBOTG_FS->UEP2_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_NAK;
							USBOTG_FS->UEP2_TX_CTRL ^= USBFS_UEP_T_TOG;
                            USBFS_Endp_Busy[ DEF_UEP2 ] = 0;
                            break;

                    default :
                        break;
                }
                break;

            /* data-out stage processing */
            case USBFS_UIS_TOKEN_OUT:
                switch( intst & ( USBFS_UIS_TOKEN_MASK | USBFS_UIS_ENDP_MASK ) )
                {
                    /* end-point 0 data out interrupt */
                    case USBFS_UIS_TOKEN_OUT | DEF_UEP0:
                            if( intst & USBFS_UIS_TOG_OK )
                            {
                                if ( ( USBFS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                                {
                                    if (( USBFS_SetupReqType & USB_REQ_TYP_MASK ) == USB_REQ_TYP_CLASS)
                                    {
                                        switch( USBFS_SetupReqCode )
                                        {
                                            case HID_SET_REPORT:
                                                memcpy(&HID_Report_Buffer[0],USBFS_EP0_Buf,DEF_USBD_UEP0_SIZE);
                                                HID_Set_Report_Flag = SET_REPORT_WAIT_DEAL;
                                                USBOTG_FS->UEP0_TX_CTRL = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_NAK;
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
                            break;

                    /* end-point 1 data out interrupt */
                    case USBFS_UIS_TOKEN_OUT | DEF_UEP1:
                        if ( intst & USBFS_UIS_TOG_OK )
                        {
                            /* Write In Buffer */
                            USBOTG_FS->UEP1_RX_CTRL ^= USBFS_UEP_R_TOG;
                            RingBuffer_Comm.PackLen[RingBuffer_Comm.LoadPtr] = USBOTG_FS->RX_LEN;
                            RingBuffer_Comm.LoadPtr ++;
                            if(RingBuffer_Comm.LoadPtr == DEF_Ring_Buffer_Max_Blks)
                            {
                                RingBuffer_Comm.LoadPtr = 0;
                            }
                            USBOTG_FS->UEP1_DMA = (uint32_t)(&Data_Buffer[(RingBuffer_Comm.LoadPtr) * DEF_USBD_FS_PACK_SIZE]);
                            RingBuffer_Comm.RemainPack ++;
                            if(RingBuffer_Comm.RemainPack >= DEF_Ring_Buffer_Max_Blks-DEF_RING_BUFFER_REMINE)
                            {
                                USBOTG_FS->UEP1_RX_CTRL = (USBOTG_FS->UEP1_RX_CTRL & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_NAK;
                                RingBuffer_Comm.StopFlag = 1;
                            }
                        }
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
                    if (( USBFS_SetupReqType & USB_REQ_TYP_MASK ) == USB_REQ_TYP_CLASS)
                    {
                        switch( USBFS_SetupReqCode )
                        {
                            case HID_SET_REPORT:
                                break;

                            case HID_GET_REPORT:
                                if( USBFS_SetupReqIndex == 0x00 )
                                {
                                    len = DEF_USBD_UEP0_SIZE;
                                    memcpy(USBFS_EP0_Buf,&HID_Report_Buffer[0],DEF_USBD_UEP0_SIZE);
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                                break;

                            case HID_SET_IDLE:
                                if( USBFS_SetupReqIndex == 0x00 )
                                {
                                    USBFS_HidIdle = USBFS_EP0_Buf[ 3 ];
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                                break;

                            case HID_SET_PROTOCOL:
                                if( USBFS_SetupReqIndex == 0x00 )
                                {
                                    USBFS_HidProtocol = USBFS_EP0_Buf[ 2 ];
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                                break;

                            case HID_GET_IDLE:
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
                            case HID_GET_PROTOCOL:
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
                                errflag = 0xFF;
                                break;
                        }
                    }
                }
                else
                {
                    /* usb standard request processing */
                    switch( USBFS_SetupReqCode )
                    {
                        /* get device/configuration/string/report/... descriptors */
                        case USB_GET_DESCRIPTOR:
                            switch( (uint8_t)(USBFS_SetupReqValue>>8) )
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
                              /* get usb report descriptor */
                              case USB_DESCR_TYP_REPORT:
                                    if (USBFS_SetupReqIndex == 0)
                                    {
                                        pUSBFS_Descr = MyHIDReportDesc;
                                        len = DEF_USBD_REPORT_DESC_LEN;
                                    }
                                    else
                                    {
                                        errflag = 0xFF;
                                    }
                                    break;
                                /* get hid descriptor */
                                case USB_DESCR_TYP_HID:
                                    if (USBFS_SetupReqIndex == 0)
                                    {
                                        pUSBFS_Descr = &MyCfgDescr[18];
                                        len = 0x09;
                                    }
                                    else
                                    {
                                        errflag = 0xFF;
                                    }
                                    break;

                                /* get usb string descriptor */
                                case USB_DESCR_TYP_STRING:
                                    switch( (uint8_t)(USBFS_SetupReqValue&0xFF) )
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
                            USBFS_EP0_Buf[ 0 ] = USBFS_DevConfig;
                            if( USBFS_SetupReqLen > 1 )
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
                            if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
                            {
                                /* clear one device feature */
                                if( (uint8_t)( USBFS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
                                {
                                    /* clear usb sleep status, device not prepare to sleep */
                                    USBFS_DevSleepStatus &= ~0x01;
                                }
                            }
                            else if ( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
                            {
                                if( (uint8_t)( USBFS_SetupReqValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
                                {
                                    switch( (uint8_t)(USBFS_SetupReqIndex&0xFF) )
                                    {
                                        case ( DEF_UEP_OUT | DEF_UEP1 ):
                                            /* Set End-point 1 OUT ACK */
                                            USBOTG_FS->UEP1_RX_CTRL =  USBFS_UEP_R_RES_ACK;
                                            break;

                                        case ( DEF_UEP_IN | DEF_UEP2 ):
                                            /* Set End-point 2 IN NAK */
                                            USBOTG_FS->UEP2_TX_CTRL =  USBFS_UEP_T_RES_NAK;
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

                                    switch( (uint8_t)(USBFS_SetupReqIndex&0xFF) )
                                    {
                                        case ( DEF_UEP_OUT | DEF_UEP1 ):
                                            USBOTG_FS->UEP1_RX_CTRL = ( USBOTG_FS->UEP1_RX_CTRL & ~USBFS_UEP_R_RES_MASK ) | USBFS_UEP_R_RES_STALL;
                                            break;
	                                    case ( DEF_UEP_IN | DEF_UEP2 ):
                                            USBOTG_FS->UEP2_TX_CTRL = ( USBOTG_FS->UEP2_TX_CTRL & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_STALL;
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
                                if((uint8_t)(USBFS_SetupReqIndex&0xFF) == ( DEF_UEP_OUT |DEF_UEP1 ))
                                {
                                    if( ( USBOTG_FS->UEP1_RX_CTRL & USBFS_UEP_R_RES_MASK ) == USBFS_UEP_R_RES_STALL )
                                    {
                                        USBFS_EP0_Buf[ 0 ] = 0x01;
                                    }
                                }
                                else if((uint8_t)(USBFS_SetupReqIndex&0xFF) == ( DEF_UEP_IN | DEF_UEP2 ))
                                {
                                    if( ( USBOTG_FS->UEP2_TX_CTRL & USBFS_UEP_T_RES_MASK ) == USBFS_UEP_T_RES_STALL )
                                    {
                                        USBFS_EP0_Buf[ 0 ] = 0x01;
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

                            if ( USBFS_SetupReqLen > 2 )
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
                if( errflag == 0xFF)
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
                        len = ( USBFS_SetupReqLen > DEF_USBD_UEP0_SIZE )? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
                        USBFS_SetupReqLen -= len;
                        USBOTG_FS->UEP0_TX_LEN  = len;
                        USBOTG_FS->UEP0_TX_CTRL = USBFS_UEP_T_TOG|USBFS_UEP_T_RES_ACK;
                    }
                    else
                    {
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
        USBFS_DevConfig = 0;
        USBFS_DevAddr = 0;
        USBFS_DevSleepStatus = 0;
        USBFS_DevEnumStatus = 0;
        USBOTG_FS->DEV_ADDR = 0;
        USBFS_Device_Endp_Init( );
        USBOTG_FS->INT_FG = USBFS_UIF_BUS_RST;
    }
    else if( intflag & USBFS_UIF_SUSPEND )
    {
        USBOTG_FS->INT_FG = USBFS_UIF_SUSPEND;
        /* usb suspend interrupt processing */
        if( USBOTG_FS->MIS_ST & USBFS_UMS_SUSPEND )
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
 * @brief   USBHD device sends wake-up signal to host
 *
 * @return  none
 */
void USBFS_Send_Resume( void )
{
    USBOTG_FS->UDEV_CTRL ^= USBFS_UD_LOW_SPEED;
    Delay_Ms( 5 );
    USBOTG_FS->UDEV_CTRL ^= USBFS_UD_LOW_SPEED;
    Delay_Ms( 1 );
}
