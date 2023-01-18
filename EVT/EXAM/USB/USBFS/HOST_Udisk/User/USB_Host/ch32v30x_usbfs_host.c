/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32f20x_usbfs_host.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/09/01
* Description        : USB full-speed port host operation functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


/*******************************************************************************/
/* Header File */
#include "usb_host_config.h"

/*******************************************************************************/
/* Variable Definition */
__attribute__((aligned(4))) uint8_t  RxBuffer[ MAX_PACKET_SIZE ];           // IN, must even address
__attribute__((aligned(4))) uint8_t  TxBuffer[ MAX_PACKET_SIZE ];           // OUT, must even address

/*********************************************************************
 * @fn      USBFS_RCC_Init
 *
 * @brief   Set USB port clock.
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
 * @fn      USBFS_Host_Init
 *
 * @brief   Initialize USB port host configuration.
 *
 * @param   sta - ENABLE or DISABLE
 *
 * @return  none
 */
void USBFS_Host_Init( FunctionalState sta )
{
    if( sta == ENABLE )
    {
        USBOTG_H_FS->BASE_CTRL = USBFS_UC_HOST_MODE;
        USBOTG_H_FS->HOST_CTRL = 0;
        USBOTG_H_FS->DEV_ADDR = 0;
        USBOTG_H_FS->HOST_EP_MOD = USBFS_UH_EP_TX_EN | USBFS_UH_EP_RX_EN;

        USBOTG_H_FS->HOST_RX_DMA = (uint32_t)USBFS_RX_Buf;
        USBOTG_H_FS->HOST_TX_DMA = (uint32_t)USBFS_TX_Buf;

        USBOTG_H_FS->HOST_RX_CTRL = 0;
        USBOTG_H_FS->HOST_TX_CTRL = 0;
        USBOTG_H_FS->BASE_CTRL = USBFS_UC_HOST_MODE | USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN;

        USBOTG_H_FS->INT_FG = 0xFF;

        USBOTG_H_FS->INT_EN = USBFS_UIE_TRANSFER | USBFS_UIE_DETECT;
    }
    else
    {
        USBOTG_H_FS->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;
    }
}

/*********************************************************************
 * @fn      USBFSH_CheckRootHubPortStatus
 *
 * @brief   Check status of USB port.
 *
 * @para    dev_sta: The status of the root device connected to this port.
 *
 * @return  The current status of the port.
 */
uint8_t USBFSH_CheckRootHubPortStatus( uint8_t dev_sta )
{
    /* Check USB device connection or disconnection */
    if( ( USBOTG_H_FS->INT_FG ) & USBFS_UIF_DETECT )
    {
        USBOTG_H_FS->INT_FG = USBFS_UIF_DETECT; // Clear flag
        if( USBOTG_H_FS->MIS_ST & USBFS_UMS_DEV_ATTACH ) // Check that the USB device has been connected to the port
        {
            if( ( dev_sta == ROOT_DEV_DISCONNECT ) || ( ( dev_sta != ROOT_DEV_FAILED ) && ( USBFSH_CheckRootHubPortEnable( ) == 0x00 ) ) )
            {
                return ROOT_DEV_CONNECTED;
            }
            else
            {
                return ROOT_DEV_FAILED;
            }
        }
        else
        {
            return ROOT_DEV_DISCONNECT;
        }
    }
    else
    {
        return ROOT_DEV_FAILED;
    }
}

/*********************************************************************
 * @fn      USBFSH_CheckRootHubPortEnable
 *
 * @brief   Check the enable status of the USB port.
 *
 * @return  The current enable status of the port.
 */
uint8_t USBFSH_CheckRootHubPortEnable( void )
{
    return ( USBOTG_H_FS->HOST_CTRL & USBFS_UH_PORT_EN );
}

/*********************************************************************
 * @fn      USBFSH_CheckRootHubPortSpeed
 *
 * @brief   Check the speed of the USB port.
 *
 * @return  The current speed of the port.
 */
uint8_t USBFSH_CheckRootHubPortSpeed( void )
{
    return ( USBOTG_H_FS->MIS_ST & USBFS_UMS_DM_LEVEL? USB_LOW_SPEED: USB_FULL_SPEED );
}

/*********************************************************************
 * @fn      USBFSH_SetSelfAddr
 *
 * @brief   Set the USB device address.
 *
 * @para    addr: USB device address.
 *
 * @return  none
 */
void USBFSH_SetSelfAddr( uint8_t addr )
{
    USBOTG_H_FS->DEV_ADDR = ( USBOTG_H_FS->DEV_ADDR & USBFS_UDA_GP_BIT ) | ( addr & USBFS_USB_ADDR_MASK );
}

/*********************************************************************
 * @fn      USBFSH_SetSelfSpeed
 *
 * @brief   Set USB speed.
 *
 * @para    speed: USB speed.
 *
 * @return  none
 */
void USBFSH_SetSelfSpeed( uint8_t speed )
{
    if( speed )
    {
        USBOTG_H_FS->BASE_CTRL &= ~USBFS_UC_LOW_SPEED;
        USBOTG_H_FS->HOST_SETUP &= ~USBFS_UH_PRE_PID_EN;
    }
    else
    {
        USBOTG_H_FS->BASE_CTRL |= USBFS_UC_LOW_SPEED;
    }
}

/*********************************************************************
 * @fn      USBFSH_ResetRootHubPort
 *
 * @brief   Reset USB port.
 *
 * @para    mod: Reset host port operating mode.
 *               0 -> reset and wait end
 *               1 -> begin reset
 *               2 -> end reset
 *
 * @return  none
 */
void USBFSH_ResetRootHubPort( uint8_t mode )
{
    USBFSH_SetSelfAddr( 0x00 );
    USBFSH_SetSelfSpeed( 1 );
    if( mode <= 1 )
    {
        USBOTG_H_FS->HOST_CTRL = ( USBOTG_H_FS->HOST_CTRL & ~USBFS_UH_LOW_SPEED ) | USBFS_UH_BUS_RESET;
    }
    if( mode == 0 )
    {
        Delay_Ms( DEF_BUS_RESET_TIME ); // Reset time from 10mS to 20mS
    }
    if( mode != 1 )
    {
        USBOTG_H_FS->HOST_CTRL = USBOTG_H_FS->HOST_CTRL & ~USBFS_UH_BUS_RESET; // End Reset
    }
    Delay_Ms( 2 );
    USBOTG_H_FS->INT_FG = USBFS_UIF_DETECT;
}

/*********************************************************************
 * @fn      USBFSH_EnableRootHubPort
 *
 * @brief   Enable USB host port.
 *
 * @para    *pspeed: USB speed.
 *
 * @return  Operation result of the enabled port.
 */
uint8_t USBFSH_EnableRootHubPort( uint8_t *pspeed )
{
    if( USBOTG_H_FS->MIS_ST & USBFS_UMS_DEV_ATTACH )
    {
        if( USBFSH_CheckRootHubPortEnable( ) == 0x00 )
        { 
            *pspeed = USBFSH_CheckRootHubPortSpeed( );
            if( *pspeed == 0 )
            {
                USBOTG_H_FS->HOST_CTRL |= USBFS_UH_LOW_SPEED;
            }
        }
        USBOTG_H_FS->HOST_CTRL |= USBFS_UH_PORT_EN;
        USBOTG_H_FS->HOST_SETUP |= USBFS_UH_SOF_EN;

        return ERR_SUCCESS;
    }

    return ERR_USB_DISCON;
}

/*********************************************************************
 * @fn      USBFSH_Transact
 *
 * @brief   Perform USB transaction.
 *
 * @para    endp_pid: Token PID.
 *          endp_tog: Toggle
 *          timeout: Timeout time.
 *
 * @return  USB transfer result.
 */
uint8_t USBFSH_Transact( uint8_t endp_pid, uint8_t endp_tog, uint16_t timeout )
{
    uint8_t  r, trans_rerty;
    uint16_t i;

    USBOTG_H_FS->HOST_TX_CTRL = USBOTG_H_FS->HOST_RX_CTRL = endp_tog;
    trans_rerty = 0;
    do
    {
        USBOTG_H_FS->HOST_EP_PID = endp_pid;       // Specify token PID and endpoint number
        USBOTG_H_FS->INT_FG = USBFS_UIF_TRANSFER;  // Allow transmission
        for( i = DEF_WAIT_USB_TOUT_200US; ( i != 0 ) && ( ( USBOTG_H_FS->INT_FG & USBFS_UIF_TRANSFER ) == 0 ); i-- )
        {
            Delay_Us( 1 );
        }
        USBOTG_H_FS->HOST_EP_PID = 0x00;  // Stop USB transfer
        if( ( USBOTG_H_FS->INT_FG & USBFS_UIF_TRANSFER ) == 0 )
        {
            return ERR_USB_UNKNOWN;
        }
        else
        {
            /* Complete transfer */
            if( USBOTG_H_FS->INT_ST & USBFS_UIS_TOG_OK )
            {
                return ERR_SUCCESS;
            }
            r = USBOTG_H_FS->INT_ST & USBFS_UIS_H_RES_MASK;  // USB device answer status
            if( r == USB_PID_STALL )
            {
                return ( r | ERR_USB_TRANSFER );
            }
            if( r == USB_PID_NAK )
            {
                if( timeout == 0 )
                {
                    return ( r | ERR_USB_TRANSFER );
                }
                if( timeout < 0xFFFF )
                {
                    timeout--;
                }
                --trans_rerty;
            }
            else switch ( endp_pid >> 4 )
            {
                case USB_PID_SETUP:
                case USB_PID_OUT:
                    if( r ) 
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                case USB_PID_IN:
                    if( ( r == USB_PID_DATA0 ) && ( r == USB_PID_DATA1 ) )
                    {
                        ;
                    }
                    else if( r )
                    {
                        return ( r | ERR_USB_TRANSFER );
                    }
                    break;
                default:
                    return ERR_USB_UNKNOWN;
            }
        } 
        Delay_Us( 15 );
        if( USBOTG_H_FS->INT_FG & USBFS_UIF_DETECT )
        {
            Delay_Us( 200 );
            if( USBFSH_CheckRootHubPortEnable( ) == 0 )
            {
                return ERR_USB_DISCON;  // USB device disconnect event
            }
        }
    }while( ++trans_rerty < 10 );

    return ERR_USB_TRANSFER; // Reply timeout
}

/*********************************************************************
 * @fn      USBFSH_CtrlTransfer
 *
 * @brief   USB host control transfer.
 *
 * @para    ep0_size: Device endpoint 0 size
 *          pbuf: Data buffer
 *          plen: Data length
 *
 * @return  USB control transfer result.
 */
uint8_t USBFSH_CtrlTransfer( uint8_t ep0_size, uint8_t *pbuf, uint16_t *plen )
{
    uint8_t  s;
    uint16_t rem_len, rx_len, rx_cnt, tx_cnt;

    if( plen )
    {
        *plen = 0;
    }
    USBOTG_H_FS->HOST_TX_LEN = sizeof( USB_SETUP_REQ );
    s = USBFSH_Transact( USB_PID_SETUP << 4 | 0x00, 0x00, DEF_CTRL_TRANS_TIMEOVER_CNT );  // SETUP stage, 200mS timeout
    if( s != ERR_SUCCESS )
    {
        return s;
    }
    USBOTG_H_FS->HOST_TX_CTRL = USBOTG_H_FS->HOST_RX_CTRL = USBFS_UH_T_TOG | USBFS_UH_R_TOG | USBFS_UH_T_AUTO_TOG | USBFS_UH_R_AUTO_TOG; // Default DATA1
    USBOTG_H_FS->HOST_TX_LEN = 0x01; // The default no-data state stage is IN
    rem_len = pUSBFS_SetupRequest->wLength;
    if( rem_len && pbuf )
    {
        if( pUSBFS_SetupRequest->bRequestType & USB_REQ_TYP_IN )
        {
            /* Receive data */
            while( rem_len )
            {
                Delay_Us( 100 );
                s = USBFSH_Transact( USB_PID_IN << 4 | 0x00, USBOTG_H_FS->HOST_RX_CTRL, DEF_CTRL_TRANS_TIMEOVER_CNT );  // IN
                if( s != ERR_SUCCESS )
                {
                    return s;
                }
                rx_len = ( USBOTG_H_FS->RX_LEN < rem_len )? USBOTG_H_FS->RX_LEN : rem_len;
                rem_len -= rx_len;
                if( plen )
                {
                    *plen += rx_len; // The total length of the actual successful transmission and reception
                }
                for( rx_cnt = 0; rx_cnt != rx_len; rx_cnt++ )
                {
                    *pbuf = USBFS_RX_Buf[ rx_cnt ];
                    pbuf++;
                }

                if( ( USBOTG_H_FS->RX_LEN == 0 ) || ( USBOTG_H_FS->RX_LEN & ( ep0_size - 1 ) ) )
                {
                    break; // Short package
                }
            }
            USBOTG_H_FS->HOST_TX_LEN = 0x00; // Status stage is OUT
        }
        else
        {
            /* Send data */
            while( rem_len )
            {
                Delay_Us( 100 );
                USBOTG_H_FS->HOST_TX_LEN = ( rem_len >= ep0_size )? ep0_size : rem_len;
                for( tx_cnt = 0; tx_cnt != USBOTG_H_FS->HOST_TX_LEN; tx_cnt++ )
                {
                    USBFS_TX_Buf[ tx_cnt ] = *pbuf;
                    pbuf++;
                }
                s = USBFSH_Transact( USB_PID_OUT << 4 | 0x00, USBOTG_H_FS->HOST_TX_CTRL, DEF_CTRL_TRANS_TIMEOVER_CNT ); // OUT
                if( s != ERR_SUCCESS )
                {
                    return s;
                }
                rem_len -= USBOTG_H_FS->HOST_TX_LEN;
                if( plen )
                {
                    *plen += USBOTG_H_FS->HOST_TX_LEN; // The total length of the actual successful transmission and reception
                }
            }
        }
    }
    Delay_Us( 100 );
    s = USBFSH_Transact( ( USBOTG_H_FS->HOST_TX_LEN )? ( USB_PID_IN << 4 | 0x00 ) : ( USB_PID_OUT << 4 | 0x00 ), USBFS_UH_R_TOG | USBFS_UH_T_TOG, DEF_CTRL_TRANS_TIMEOVER_CNT );  /* STATUS½×¶Î */
    if( s != ERR_SUCCESS )
    {
        return s;
    }
    if( USBOTG_H_FS->HOST_TX_LEN == 0 )
    {
        return ERR_SUCCESS;
    }
    if( USBOTG_H_FS->RX_LEN == 0 )
    {
        return ERR_SUCCESS;
    }
    return ERR_USB_BUF_OVER;
}

/*********************************************************************
 * @fn      USBFSH_GetDeviceDescr
 *
 * @brief   Get the device descriptor of the USB device.
 *
 * @para    pep0_size: Device endpoint 0 size
 *          pbuf: Data buffer
 *
 * @return  The result of getting the device descriptor.
 */
uint8_t USBFSH_GetDeviceDescr( uint8_t *pep0_size, uint8_t *pbuf )
{
    uint8_t  s;
    uint16_t len;

    *pep0_size = DEFAULT_ENDP0_SIZE;
    memcpy( pUSBFS_SetupRequest, SetupGetDevDesc, sizeof( USB_SETUP_REQ ) );
    s = USBFSH_CtrlTransfer( *pep0_size, pbuf, &len );
    if( s != ERR_SUCCESS )
    {
        return s;
    }

    *pep0_size = ( (PUSB_DEV_DESCR)pbuf )->bMaxPacketSize0;
    if( len < ( (PUSB_SETUP_REQ)SetupGetDevDesc )->wLength )
    {
        return ERR_USB_BUF_OVER;
    }
    return ERR_SUCCESS;
}

/*********************************************************************
 * @fn      USBFSH_GetConfigDescr
 *
 * @brief   Get the configuration descriptor of the USB device. 
 *
 * @para    ep0_size: Device endpoint 0 size
 *          pbuf: Data buffer
 *          buf_len: Data buffer length
 *          pcfg_len: The length of the device configuration descriptor
 *
 * @return  The result of getting the configuration descriptor.
 */
uint8_t USBFSH_GetConfigDescr( uint8_t ep0_size, uint8_t *pbuf, uint16_t buf_len, uint16_t *pcfg_len )
{
    uint8_t  s;
    
    memcpy( pUSBFS_SetupRequest, SetupGetCfgDesc, sizeof( USB_SETUP_REQ ) );
    s = USBFSH_CtrlTransfer( ep0_size, pbuf, pcfg_len );
    if( s != ERR_SUCCESS )
    {
        return s;
    }
    if( *pcfg_len < ( (PUSB_SETUP_REQ)SetupGetCfgDesc )->wLength )
    {
        return ERR_USB_BUF_OVER;
    }

    *pcfg_len = ( (PUSB_CFG_DESCR)pbuf )->wTotalLength;
    if( *pcfg_len > buf_len  )
    {
        *pcfg_len = buf_len;
    }
    memcpy( pUSBFS_SetupRequest, SetupGetCfgDesc, sizeof( USB_SETUP_REQ ) );
    pUSBFS_SetupRequest->wLength = *pcfg_len;
    s = USBFSH_CtrlTransfer( ep0_size, pbuf, pcfg_len );
    return s;
}

/*********************************************************************
 * @fn      USBFSH_GetStrDescr
 *
 * @brief   Get the string descriptor of the USB device.
 *
 * @para    ep0_size: Device endpoint 0 size
 *          str_num: Index of string descriptor  
 *          pbuf: Data buffer
 *
 * @return  The result of getting the string descriptor.
 */
uint8_t USBFSH_GetStrDescr( uint8_t ep0_size, uint8_t str_num, uint8_t *pbuf )
{
    uint8_t  s;
    uint16_t len;

    /* Get the string descriptor of the first 4 bytes */
    memcpy( pUSBFS_SetupRequest, SetupGetStrDesc, sizeof( USB_SETUP_REQ ) );
    pUSBFS_SetupRequest->wValue = ( (uint16_t)USB_DESCR_TYP_STRING << 8 ) | str_num;
    s = USBFSH_CtrlTransfer( ep0_size, pbuf, &len );
    if( s != ERR_SUCCESS )
    {
        return s;
    }

    /* Get the complete string descriptor */
    len = pbuf[ 0 ];
    memcpy( pUSBFS_SetupRequest, SetupGetStrDesc, sizeof( USB_SETUP_REQ ) );
    pUSBFS_SetupRequest->wValue = ( (uint16_t)USB_DESCR_TYP_STRING << 8 ) | str_num;
    pUSBFS_SetupRequest->wLength = len;
    s = USBFSH_CtrlTransfer( ep0_size, pbuf, &len );
    if( s != ERR_SUCCESS )
    {
        return s;
    }
    return ERR_SUCCESS;
}

/*********************************************************************
 * @fn      USBFSH_SetUsbAddress
 *
 * @brief   Set USB device address.
 *
 * @para    ep0_size: Device endpoint 0 size
 *          addr: Device address
 *
 * @return  The result of setting device address.
 */
uint8_t USBFSH_SetUsbAddress( uint8_t ep0_size, uint8_t addr )
{
    uint8_t  s;

    memcpy( pUSBFS_SetupRequest, SetupSetAddr, sizeof( USB_SETUP_REQ ) );
    pUSBFS_SetupRequest->wValue = (uint16_t)addr;
    s = USBFSH_CtrlTransfer( ep0_size, NULL, NULL );
    if( s != ERR_SUCCESS )
    {
        return s;
    }
    USBFSH_SetSelfAddr( addr );
    Delay_Ms( DEF_BUS_RESET_TIME >> 1 ); // Wait for the USB device to complete its operation.
    return ERR_SUCCESS;
}

/*********************************************************************
 * @fn      USBFSH_SetUsbConfig
 *
 * @brief   Set USB configuration.
 *
 * @para    ep0_size: Device endpoint 0 size
 *          cfg: Device configuration value
 *
 * @return  The result of setting device configuration.
 */
uint8_t USBFSH_SetUsbConfig( uint8_t ep0_size, uint8_t cfg_val )
{
    memcpy( pUSBFS_SetupRequest, SetupSetConfig, sizeof( USB_SETUP_REQ ) );
    pUSBFS_SetupRequest->wValue = (uint16_t)cfg_val;
    return USBFSH_CtrlTransfer( ep0_size, NULL, NULL );
}

/*********************************************************************
 * @fn      USBFSH_ClearEndpStall
 *
 * @brief   Clear endpoint stall.
 *
 * @para    ep0_size: Device endpoint 0 size
 *          endp_num: Endpoint number.
 *
 * @return  The result of clearing endpoint stall.
 */
uint8_t USBFSH_ClearEndpStall( uint8_t ep0_size, uint8_t endp_num )
{
    memcpy( pUSBFS_SetupRequest, SetupClearEndpStall, sizeof( USB_SETUP_REQ ) );
    pUSBFS_SetupRequest->wIndex = (uint16_t)endp_num;
    return USBFSH_CtrlTransfer( ep0_size, NULL, NULL );
}

/*********************************************************************
 * @fn      USBFSH_GetEndpData
 *
 * @brief   Get data from USB device input endpoint.
 *
 * @para    endp_num: Endpoint number
 *          pendp_tog: Endpoint toggle
 *          pbuf: Data Buffer
 *          plen: Data length
 *
 * @return  The result of getting data.
 */
uint8_t USBFSH_GetEndpData( uint8_t endp_num, uint8_t *pendp_tog, uint8_t *pbuf, uint16_t *plen )
{
    uint8_t  s;
    
    s = USBFSH_Transact( ( USB_PID_IN << 4 ) | endp_num, *pendp_tog, 0 );
    if( s == ERR_SUCCESS )
    {
        *pendp_tog ^= USBFS_UH_T_TOG | USBFS_UH_R_TOG;
        *plen = USBOTG_H_FS->RX_LEN;
        memcpy( pbuf, USBFS_RX_Buf, *plen );
    }
    
    return s;
}

/*********************************************************************
 * @fn      USBFSH_SendEndpData
 *
 * @brief   Send data to the USB device output endpoint.
 *
 * @para    endp_num: Endpoint number
 *          pendp_tog: Endpoint toggle
 *          pbuf: Data Buffer
 *          plen: Data length
 *
 * @return  The result of sending data.
 */
uint8_t USBFSH_SendEndpData( uint8_t endp_num, uint8_t *pendp_tog, uint8_t *pbuf, uint16_t len )
{
    uint8_t  s;
    
    memcpy( USBFS_TX_Buf, pbuf, len );
    USBOTG_H_FS->HOST_TX_LEN = len;
    
    s = USBFSH_Transact( ( USB_PID_OUT << 4 ) | endp_num, *pendp_tog, 0 );
    if( s == ERR_SUCCESS )
    {
        *pendp_tog ^= USBFS_UH_T_TOG | USBFS_UH_R_TOG;
    }
  
    return s;
}
