/********************************** (C) COPYRIGHT  *******************************
 * File Name          : usb_host_hub.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/29
 * Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
 
 
/********************************************************************************/
/* Header File */
#include "usb_host_config.h"

/*********************************************************************
 * @fn      HUB_ClearPortFeature
 *
 * @brief
 *
 * @para
 *
 * @return  none
 */
uint8_t HUB_GetPortStatus( uint8_t usb_port, uint8_t hub_ep0_size, uint8_t hub_port, uint8_t *pbuf )
{
    uint8_t s = ERR_USB_UNSUPPORT;
    uint16_t len;
    
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        memcpy( pUSBFS_SetupRequest, GetPortStatus, sizeof( USB_SETUP_REQ ) );
        pUSBFS_SetupRequest->wIndex = (uint16_t)hub_port;
        s = USBFSH_CtrlTransfer( hub_ep0_size, pbuf, &len );
#endif            
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN     
        memcpy( pUSBHS_SetupRequest, GetPortStatus, sizeof( USB_SETUP_REQ ) );
        pUSBHS_SetupRequest->wIndex = (uint16_t)hub_port;
        s = USBHSH_CtrlTransfer( hub_ep0_size, pbuf, &len );
#endif      
    }
   
    return s;
}

/*********************************************************************
 * @fn      HUB_ClearPortFeature
 *
 * @brief
 *
 * @para
 *
 * @return  none
 */
uint8_t HUB_ClearPortFeature( uint8_t usb_port, uint8_t hub_ep0_size, uint8_t hub_port, uint8_t selector )
{
    uint8_t s = ERR_USB_UNSUPPORT;
    
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        memcpy( pUSBFS_SetupRequest, ClearPortFeature, sizeof( USB_SETUP_REQ ) );
        pUSBFS_SetupRequest->wValue = (uint16_t)selector;
        pUSBFS_SetupRequest->wIndex = (uint16_t)hub_port;
        s = USBFSH_CtrlTransfer( hub_ep0_size, NULL, NULL );
#endif            
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN     
        memcpy( pUSBHS_SetupRequest, ClearPortFeature, sizeof( USB_SETUP_REQ ) );
        pUSBHS_SetupRequest->wValue = (uint16_t)selector;
        pUSBHS_SetupRequest->wIndex = (uint16_t)hub_port;
        s = USBHSH_CtrlTransfer( hub_ep0_size, NULL, NULL );
#endif      
    }
   
    return s;
}

/*********************************************************************
 * @fn      HUB_SetPortFeature
 *
 * @brief
 *
 * @para
 *
 * @return  none
 */
uint8_t HUB_SetPortFeature( uint8_t usb_port, uint8_t hub_ep0_size, uint8_t hub_port, uint8_t selector )
{
    uint8_t s = ERR_USB_UNSUPPORT;
    
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        memcpy( pUSBFS_SetupRequest, SetPortFeature, sizeof( USB_SETUP_REQ ) );
        pUSBFS_SetupRequest->wValue = (uint16_t)selector;
        pUSBFS_SetupRequest->wIndex = (uint16_t)hub_port;
        s = USBFSH_CtrlTransfer( hub_ep0_size, NULL, NULL );
#endif            
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN     
        memcpy( pUSBHS_SetupRequest, SetPortFeature, sizeof( USB_SETUP_REQ ) );
        pUSBHS_SetupRequest->wValue = (uint16_t)selector;
        pUSBHS_SetupRequest->wIndex = (uint16_t)hub_port;
        s = USBHSH_CtrlTransfer( hub_ep0_size, NULL, NULL );
#endif      
    }
   
    return s;
}

/*********************************************************************
 * @fn      HUB_GetClassDevDescr
 *
 * @brief
 *
 * @para
 *
 * @return  none
 */
uint8_t HUB_GetClassDevDescr( uint8_t usb_port, uint8_t hub_ep0_size, uint8_t *pbuf, uint16_t *plen )
{
    uint8_t s = ERR_USB_UNSUPPORT;
    
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        memcpy( pUSBFS_SetupRequest, GetHubDescr, sizeof( USB_SETUP_REQ ) );
        s = USBFSH_CtrlTransfer( hub_ep0_size, pbuf, plen );
        if( s != ERR_SUCCESS )
        {
            return s;
        }
        else
        {
            pUSBFS_SetupRequest->wLength = *plen = (uint16_t)pbuf[ 0 ];
            s = USBFSH_CtrlTransfer( hub_ep0_size, pbuf, plen );
        }
        
#endif            
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN     
        memcpy( pUSBHS_SetupRequest, GetHubDescr, sizeof( USB_SETUP_REQ ) );
        s = USBHSH_CtrlTransfer( hub_ep0_size, pbuf, plen );
        if( s != ERR_SUCCESS )
        {
            return s;
        }
        else
        {
            pUSBHS_SetupRequest->wLength = *plen = (uint16_t)pbuf[ 0 ];
            s = USBHSH_CtrlTransfer( hub_ep0_size, pbuf, plen );
        }
#endif      
    }
   
    return s;
}
