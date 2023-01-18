/********************************** (C) COPYRIGHT  *******************************
 * File Name          : app_km.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/09/01
 * Description        : The USB host operates the keyboard and mouse.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/


/********************************************************************************/
/* Header File */
#include "usb_host_config.h"

/*******************************************************************************/
/* Variable Definition */
uint8_t  DevDesc_Buf[ 18 ];                                                     // Device Descriptor Buffer
uint8_t  Com_Buf[ DEF_COM_BUF_LEN ];                                            // General Buffer
struct   _ROOT_HUB_DEVICE RootHubDev[ DEF_TOTAL_ROOT_HUB ];
struct   __HOST_CTL HostCtl[ DEF_TOTAL_ROOT_HUB * DEF_ONE_USB_SUP_DEV_TOTAL ];

/*******************************************************************************/
/* Interrupt Function Declaration */
void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      TIM3_Init
 *
 * @brief   Initialize timer3 for getting keyboard and mouse data.
 *
 * @param   arr - The specific period value.
 *          psc - The specifies prescaler value.
 *
 * @return  none
 */
void TIM3_Init( uint16_t arr, uint16_t psc )
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };
    NVIC_InitTypeDef NVIC_InitStructure = { 0 };

    /* Enable timer3 clock */
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );

    /* Initialize timer3 */
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure );

    /* Enable updating timer3 interrupt */
    TIM_ITConfig( TIM3, TIM_IT_Update, ENABLE );

    /* Configure timer3 interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );

    /* Enable timer3 */
    TIM_Cmd( TIM3, ENABLE );

    /* Enable timer3 interrupt */
    NVIC_EnableIRQ( TIM3_IRQn );
}

/*********************************************************************
 * @fn      TIM3_IRQHandler
 *
 * @brief   This function handles TIM3 global interrupt request.
 *
 * @return  none
 */
void TIM3_IRQHandler( void )
{
    uint8_t usb_port;
    uint8_t hub_port;
    uint8_t index;
    uint8_t intf_num, in_num;

    if( TIM_GetITStatus( TIM3, TIM_IT_Update ) != RESET )
    {
        /* Clear interrupt flag */
        TIM_ClearITPendingBit( TIM3, TIM_IT_Update );

        /* USB HID Device Input Endpoint Timing */
        for( usb_port = 0; usb_port < DEF_TOTAL_ROOT_HUB; usb_port++ )
        {
            if( RootHubDev[ usb_port ].bStatus >= ROOT_DEV_SUCCESS )
            {
                index = RootHubDev[ usb_port ].DeviceIndex;
                if( RootHubDev[ usb_port ].bType == USB_DEV_CLASS_HID )
                {
                    for( intf_num = 0; intf_num < HostCtl[ index ].InterfaceNum; intf_num++ )
                    {
                        for( in_num = 0; in_num < HostCtl[ index ].Interface[ intf_num ].InEndpNum; in_num++ )
                        {
                            HostCtl[ index ].Interface[ intf_num ].InEndpTimeCount[ in_num ]++;
                        }
                    }
                }
                else if( RootHubDev[ usb_port ].bType == USB_DEV_CLASS_HUB )
                {
                    HostCtl[ index ].Interface[ 0 ].InEndpTimeCount[ 0 ]++;
                    for( hub_port = 0; hub_port < RootHubDev[ usb_port ].bPortNum; hub_port++ )
                    {
                        if( RootHubDev[ usb_port ].Device[ hub_port ].bStatus >= ROOT_DEV_SUCCESS )
                        {
                            index = RootHubDev[ usb_port ].Device[ hub_port ].DeviceIndex;

                            if( RootHubDev[ usb_port ].Device[ hub_port ].bType == USB_DEV_CLASS_HID )
                            {
                                for( intf_num = 0; intf_num < HostCtl[ index ].InterfaceNum; intf_num++ )
                                {
                                    for( in_num = 0; in_num < HostCtl[ index ].Interface[ intf_num ].InEndpNum; in_num++ )
                                    {
                                        HostCtl[ index ].Interface[ intf_num ].InEndpTimeCount[ in_num ]++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/*********************************************************************
 * @fn      USBH_CheckRootHubPortStatus
 *
 * @brief   Check status of USB port.
 *
 * @para    index: USB host port
 *
 * @return  The current status of the port.
 */
uint8_t USBH_CheckRootHubPortStatus( uint8_t usb_port )
{
    uint8_t s = ERR_USB_UNSUPPORT;
    
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        s = USBFSH_CheckRootHubPortStatus( RootHubDev[ usb_port ].bStatus );
#endif            
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN     
        s = USBHSH_CheckRootHubPortStatus( RootHubDev[ usb_port ].bStatus );
#endif      
    }
    
    return s;
}

/*********************************************************************
 * @fn      USBH_ResetRootHubPort
 *
 * @brief   Reset USB port.
 *
 * @para    index: USB host port
 *          mod: Reset host port operating mode.
 *               0 -> reset and wait end
 *               1 -> begin reset
 *               2 -> end reset
 *
 * @return  none
 */
void USBH_ResetRootHubPort( uint8_t usb_port, uint8_t mode )
{
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        USBFSH_ResetRootHubPort( mode );
#endif
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX ) 
    {
#if DEF_USBHS_PORT_EN
        USBHSH_ResetRootHubPort( mode );
#endif
    }
}

/*********************************************************************
 * @fn      USBH_EnableRootHubPort
 *
 * @brief   Enable USB host port.
 *
 * @para    index: USB host port
 *
 * @return  none
 */
uint8_t USBH_EnableRootHubPort( uint8_t usb_port )
{
    uint8_t s = ERR_USB_UNSUPPORT;
    
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        s = USBFSH_EnableRootHubPort( &RootHubDev[ usb_port ].bSpeed );
#endif            
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN     
        s = USBHSH_EnableRootHubPort( &RootHubDev[ usb_port ].bSpeed );
#endif      
    }
   
    return s;
}

/*********************************************************************
 * @fn      USBH_GetDeviceDescr
 *
 * @brief   Get the device descriptor of the USB device.
 *
 * @para    index: USB host port
 *
 * @return  none
 */
uint8_t USBH_GetDeviceDescr( uint8_t usb_port )
{
    uint8_t s = ERR_USB_UNSUPPORT;
    
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        s = USBFSH_GetDeviceDescr( &RootHubDev[ usb_port ].bEp0MaxPks, DevDesc_Buf );
#endif            
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN     
        s = USBHSH_GetDeviceDescr( &RootHubDev[ usb_port ].bEp0MaxPks, DevDesc_Buf );
#endif      
    }
    
    return s;
}

/*********************************************************************
 * @fn      USBH_SetUsbAddress
 *
 * @brief   Set USB device address.
 *
 * @para    index: USB host port
 *
 * @return  none
 */
uint8_t USBH_SetUsbAddress( uint8_t usb_port )
{
    uint8_t s = ERR_USB_UNSUPPORT;
    
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        RootHubDev[ usb_port ].bAddress = (uint8_t)( DEF_USBFS_PORT_INDEX + USB_DEVICE_ADDR );
        s = USBFSH_SetUsbAddress( RootHubDev[ usb_port ].bEp0MaxPks, RootHubDev[ usb_port ].bAddress );
#endif            
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN     
        RootHubDev[ usb_port ].bAddress = (uint8_t)( DEF_USBHS_PORT_INDEX + USB_DEVICE_ADDR );
        s = USBHSH_SetUsbAddress( RootHubDev[ usb_port ].bEp0MaxPks, RootHubDev[ usb_port ].bAddress );
#endif      
    }
    
    return s;
}

/*********************************************************************
 * @fn      USBH_GetConfigDescr
 *
 * @brief   Get the configuration descriptor of the USB device. 
 *
 * @para    index: USB host port
 *
 * @return  none
 */
uint8_t USBH_GetConfigDescr( uint8_t usb_port, uint16_t *pcfg_len )
{
    uint8_t s = ERR_USB_UNSUPPORT;

    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        s = USBFSH_GetConfigDescr( RootHubDev[ usb_port ].bEp0MaxPks, Com_Buf, DEF_COM_BUF_LEN, pcfg_len );
#endif            
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN     
        s = USBHSH_GetConfigDescr( RootHubDev[ usb_port ].bEp0MaxPks, Com_Buf, DEF_COM_BUF_LEN, pcfg_len );
#endif      
    }
    
    return s;
}

/*********************************************************************
 * @fn      USBH_AnalyseType
 *
 * @brief   Simply analyze USB device type.
 *
* @para     pdev_buf: Device descriptor buffer
 *          pcfg_buf: Configuration descriptor buffer
 *          ptype: Device type.
 *
 * @return  none
 */
void USBH_AnalyseType( uint8_t *pdev_buf, uint8_t *pcfg_buf, uint8_t *ptype )
{
    uint8_t  dv_cls, if_cls;

    dv_cls = ( (PUSB_DEV_DESCR)pdev_buf )->bDeviceClass;
    if_cls = ( (PUSB_CFG_DESCR_LONG)pcfg_buf )->itf_descr.bInterfaceClass;
    if( ( dv_cls == USB_DEV_CLASS_STORAGE ) || ( if_cls == USB_DEV_CLASS_STORAGE ) )
    {
        *ptype = USB_DEV_CLASS_STORAGE;
    }
    else if( ( dv_cls == USB_DEV_CLASS_PRINTER ) || ( if_cls == USB_DEV_CLASS_PRINTER ) )
    {
        *ptype = USB_DEV_CLASS_PRINTER;
    }
    else if( ( dv_cls == USB_DEV_CLASS_HID ) || ( if_cls == USB_DEV_CLASS_HID ) )
    {
        *ptype = USB_DEV_CLASS_HID;
    }
    else if( ( dv_cls == USB_DEV_CLASS_HUB ) || ( if_cls == USB_DEV_CLASS_HUB ) )
    {
        *ptype = USB_DEV_CLASS_HUB;
    }
    else
    {
        *ptype = DEF_DEV_TYPE_UNKNOWN;
    }
}

/*********************************************************************
 * @fn      USBFSH_SetUsbConfig
 *
 * @brief   Set USB configuration.
 *
 * @para    index: USB host port
 *
 * @return  none
 */
uint8_t USBH_SetUsbConfig( uint8_t usb_port, uint8_t cfg_val )
{
    uint8_t s = ERR_USB_UNSUPPORT;
    
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        s = USBFSH_SetUsbConfig( RootHubDev[ usb_port ].bEp0MaxPks, cfg_val );
#endif            
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN     
        s = USBHSH_SetUsbConfig( RootHubDev[ usb_port ].bEp0MaxPks, cfg_val );
#endif      
    }
    
    return s;
}

/*********************************************************************
 * @fn      USBH_GetStrDescr
 *
 * @brief   Get the string descriptor of the USB device.
 *
 * @para    index: USB host port
 *
 * @return  The result of getting the string descriptor.
 */
uint8_t USBH_GetStrDescr( uint8_t usb_port, uint8_t ep0_size, uint8_t str_num )
{
    uint8_t s = ERR_USB_UNSUPPORT;

    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        s = USBFSH_GetStrDescr( ep0_size, str_num, Com_Buf );
#endif
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN
        s = USBHSH_GetStrDescr( ep0_size, str_num, Com_Buf );
#endif
    }

    return s;
}

/*********************************************************************
 * @fn      USBH_GetHidData
 *
 * @brief
 *
 * @para    index - Corresponding host port.
 *
 * @return  none
 */
uint8_t USBH_GetHidData( uint8_t usb_port, uint8_t index, uint8_t intf_num, uint8_t endp_num, uint8_t *pbuf, uint16_t *plen )
{
    uint8_t s = ERR_USB_UNSUPPORT;

    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        s = USBFSH_GetEndpData( HostCtl[ index ].Interface[ intf_num ].InEndpAddr[ endp_num ],
                                &HostCtl[ index ].Interface[ intf_num ].InEndpTog[ endp_num ], pbuf, plen );
#endif
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN
        s = USBHSH_GetEndpData( HostCtl[ index ].Interface[ intf_num ].InEndpAddr[ endp_num ],
                                &HostCtl[ index ].Interface[ intf_num ].InEndpTog[ endp_num ], pbuf, plen );
#endif
    }

    return s;
}

/*********************************************************************
 * @fn      USBH_SendHidData
 *
 * @brief   Send data to the USB device output endpoint.
 *
 * @para    index: USB host port
 *
 * @return  The result of sending data.
 */
uint8_t USBH_SendHidData( uint8_t usb_port, uint8_t index, uint8_t intf_num, uint8_t endp_num, uint8_t *pbuf, uint16_t len )
{
    uint8_t s = ERR_USB_UNSUPPORT;

    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        s = USBFSH_SendEndpData( HostCtl[ index ].Interface[ intf_num ].OutEndpAddr[ endp_num ],
                                 &HostCtl[ index ].Interface[ intf_num ].OutEndpTog[ endp_num ], pbuf, len );
#endif
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN
        s = USBHSH_SendEndpData( HostCtl[ index ].Interface[ intf_num ].OutEndpAddr[ endp_num ],
                                 &HostCtl[ index ].Interface[ intf_num ].OutEndpTog[ endp_num ], pbuf, len );
#endif
    }

    return s;
}

/*********************************************************************
 * @fn      USBH_ClearEndpStall
 *
 * @brief
 *
 * @para    index - Corresponding host port.
 *
 * @return  none
 */
uint8_t USBH_ClearEndpStall( uint8_t usb_port, uint8_t endp_num )
{
    uint8_t s = ERR_USB_UNSUPPORT;

    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        s = USBFSH_ClearEndpStall( RootHubDev[ usb_port ].bEp0MaxPks, endp_num );
#endif
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN
        s = USBHSH_ClearEndpStall( RootHubDev[ usb_port ].bEp0MaxPks, endp_num );
#endif
    }

    return s;
}

/*********************************************************************
 * @fn      USBH_EnumRootDevice
 *
 * @brief   Generally enumerate a device connected to host port.
 *
 * @para    index: USB host port
 *
 * @return  Enumeration result
 */
uint8_t USBH_EnumRootDevice( uint8_t usb_port )
{
    uint8_t  s;
    uint8_t  enum_cnt;
    uint8_t  cfg_val;
    uint16_t i;
    uint16_t len;

    DUG_PRINTF( "Enum:\r\n" );

    enum_cnt = 0;
ENUM_START:
    /* Delay and wait for the device to stabilize */
    Delay_Ms( 100 );
    enum_cnt++;
    Delay_Ms( 8 << enum_cnt );

    /* Reset the USB device and wait for the USB device to reconnect */
    USBH_ResetRootHubPort( usb_port, 0 );
    for( i = 0, s = 0; i < DEF_RE_ATTACH_TIMEOUT; i++ )
    {
        if( USBH_EnableRootHubPort( usb_port ) == ERR_SUCCESS )
        {
            i = 0;
            s++;
            if( s > 6 )
            {
                break;
            }
        }
        Delay_Ms( 1 );
    }
    if( i )
    {
        /* Determine whether the maximum number of retries has been reached, and retry if not reached */
        if( enum_cnt <= 5 )
        {
            goto ENUM_START;
        }
        return ERR_USB_DISCON;
    }

    /* Get USB device device descriptor */
    DUG_PRINTF("Get DevDesc: ");
    s = USBH_GetDeviceDescr( usb_port );
    if( s == ERR_SUCCESS )
    {
        /* Print USB device device descriptor */
#if DEF_DEBUG_PRINTF
        for( i = 0; i < 18; i++ )
        {
            DUG_PRINTF( "%02x ", DevDesc_Buf[ i ] );
        }
        DUG_PRINTF("\r\n"); 
#endif
    }
    else
    {
        /* Determine whether the maximum number of retries has been reached, and retry if not reached */
        DUG_PRINTF( "Err(%02x)\r\n", s );
        if( enum_cnt <= 5 )
        {
            goto ENUM_START;
        }
        return DEF_DEV_DESCR_GETFAIL;
    }

    /* Set the USB device address */
    DUG_PRINTF("Set DevAddr: ");
    s = USBH_SetUsbAddress( usb_port );
    if( s == ERR_SUCCESS )
    {
        DUG_PRINTF( "OK\r\n" );    
    }
    else
    {
        /* Determine whether the maximum number of retries has been reached, and retry if not reached */
        DUG_PRINTF( "Err(%02x)\r\n", s );
        if( enum_cnt <= 5 )
        {
            goto ENUM_START;
        }
        return DEF_DEV_ADDR_SETFAIL;
    }
    Delay_Ms( 5 );

    /* Get the USB device configuration descriptor */
    DUG_PRINTF("Get CfgDesc: ");
    s = USBH_GetConfigDescr( usb_port, &len );
    if( s == ERR_SUCCESS )
    {
        cfg_val = ( (PUSB_CFG_DESCR)Com_Buf )->bConfigurationValue;
        
        /* Print USB device configuration descriptor  */
#if DEF_DEBUG_PRINTF
        for( i = 0; i < len; i++ )
        {
            DUG_PRINTF( "%02x ", Com_Buf[ i ] );
        }
        DUG_PRINTF("\r\n");
#endif

        /* Simply analyze USB device type  */
        USBH_AnalyseType( DevDesc_Buf, Com_Buf, &RootHubDev[ usb_port ].bType );
        DUG_PRINTF( "DevType: %02x\r\n", RootHubDev[ usb_port ].bType );
    }
    else
    {
        /* Determine whether the maximum number of retries has been reached, and retry if not reached */
        DUG_PRINTF( "Err(%02x)\r\n", s );
        if( enum_cnt <= 5 )
        {
            goto ENUM_START;
        }
        return DEF_CFG_DESCR_GETFAIL;
    }

    /* Set USB device configuration value */
    DUG_PRINTF("Set Cfg: ");
    s = USBH_SetUsbConfig( usb_port, cfg_val );
    if( s == ERR_SUCCESS )
    {
        DUG_PRINTF( "OK\r\n" );
    }
    else
    {
        /* Determine whether the maximum number of retries has been reached, and retry if not reached */
        DUG_PRINTF( "Err(%02x)\r\n", s );
        if( enum_cnt <= 5 )
        {
            goto ENUM_START;
        }
        return ERR_USB_UNSUPPORT;
    }

    return ERR_SUCCESS;
}

/*********************************************************************
 * @fn      KM_AnalyzeConfigDesc
 *
 * @brief   Analyze keyboard and mouse configuration descriptor.
 *
 * @para    index: USB host port
 *
 * @return  The result of the analysis.
 */
uint8_t KM_AnalyzeConfigDesc( uint8_t usb_port, uint8_t index )
{
    uint8_t  s = 0;
    uint16_t i;
    uint8_t  num, innum, outnum;

    num = 0;
    for( i = 0; i < ( Com_Buf[ 2 ] + ( (uint16_t)Com_Buf[ 3 ] << 8 ) ); )
    {
        if( Com_Buf[ i + 1 ] == DEF_DECR_CONFIG )
        {
            /* Save the number of interface of the USB device, only up to 4 */
            if( ( (PUSB_CFG_DESCR)( &Com_Buf[ i ] ) )->bNumInterfaces > DEF_INTERFACE_NUM_MAX )
            {
                HostCtl[ index ].InterfaceNum = DEF_INTERFACE_NUM_MAX;
            }
            else
            {
                HostCtl[ index ].InterfaceNum = ( (PUSB_CFG_DESCR)( &Com_Buf[ i ] ) )->bNumInterfaces;
            }
            i += Com_Buf[ i ];
        }
        else if( Com_Buf[ i + 1 ] == DEF_DECR_INTERFACE )
        {
            if( num == DEF_INTERFACE_NUM_MAX )
            {
                return s;
            }
            if( ( (PUSB_ITF_DESCR)( &Com_Buf[ i ] ) )->bInterfaceClass == 0x03 )
            {
                /* HID devices (such as USB keyboard and mouse) */
                if( ( (PUSB_ITF_DESCR)( &Com_Buf[ i ] ) )->bInterfaceSubClass <= 0x01 &&
                    ( (PUSB_ITF_DESCR)( &Com_Buf[ i ] ) )->bInterfaceProtocol <= 2 )
                {
                    if( ( (PUSB_ITF_DESCR)( &Com_Buf[ i ] ) )->bInterfaceProtocol == 0x01 ) // Keyboard
                    {
                        HostCtl[ index ].Interface[ num ].Type = DEC_KEY;
                        HID_SetIdle( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, num, 0, 0 );
                    }
                    else if( ( (PUSB_ITF_DESCR)( &Com_Buf[ i ] ) )->bInterfaceProtocol == 0x02 ) // Mouse
                    {
                        HostCtl[ index ].Interface[ num ].Type = DEC_MOUSE;
                        HID_SetIdle( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, num, 0, 0 );
                    }
                    s = ERR_SUCCESS;
                    i += Com_Buf[ i ];
                    innum = 0;
                    outnum = 0;
                    while( 1 )
                    {
                        if( ( Com_Buf[ i + 1 ] == DEF_DECR_INTERFACE ) || ( i >= Com_Buf[ 2 ] ) )
                        {
                            break;
                        }
                        else
                        {
                            /* Analyze each endpoint of the current interface */
                            if( Com_Buf[ i + 1 ] == DEF_DECR_ENDPOINT )
                            {
                                /* Save endpoint related information (endpoint address, attribute, max packet size, polling interval) */
                                if( ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->bEndpointAddress & 0x80 )
                                {
                                    /* IN */
                                    HostCtl[ index ].Interface[ num ].InEndpAddr[ innum ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->bEndpointAddress & 0x0F;
                                    HostCtl[ index ].Interface[ num ].InEndpType[ innum ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->bmAttributes;
                                    HostCtl[ index ].Interface[ num ].InEndpSize[ innum ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->wMaxPacketSizeL +
                                                                              (uint16_t)( ( ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->wMaxPacketSizeH ) << 8 );
                                    HostCtl[ index ].Interface[ num ].InEndpInterval[ innum ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->bInterval;
                                    HostCtl[ index ].Interface[ num ].InEndpNum++;
                                    
                                    innum++;
                                }
                                else
                                {
                                    /* OUT */
                                    HostCtl[ index ].Interface[ num ].OutEndpAddr[ outnum ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->bEndpointAddress & 0x0f;
                                    HostCtl[ index ].Interface[ num ].OutEndpType[ outnum ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->bmAttributes;
                                    HostCtl[ index ].Interface[ num ].OutEndpSize[ outnum ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->wMaxPacketSizeL +
                                                                                (uint16_t)( ( ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->wMaxPacketSizeH ) << 8 );
                                    HostCtl[ index ].Interface[ num ].OutEndpNum++;

                                    outnum++;
                                }

                                i += Com_Buf[ i ];
                            }
                            else if( Com_Buf[ i + 1 ] == DEF_DECR_HID )
                            {
                                /* Save the current interface HID report descriptor length */
                                HostCtl[ index ].Interface[ num ].HidDescLen = ( (PUSB_HID_DESCR)( &Com_Buf[ i ] ) )->wDescriptorLengthL | \
                                                                               ( (uint16_t)( ( (PUSB_HID_DESCR)( &Com_Buf[ i ] ) )->wDescriptorLengthH ) << 8 );
                                i += Com_Buf[ i ];
                            }
                            else
                            {
                                i += Com_Buf[ i ];
                            }
                        }
                    }

                    if( ( outnum == 1 ) && ( HostCtl[ index ].Interface[ num ].Type == DEC_KEY ) )
                    {
                        HostCtl[ index ].Interface[ num ].SetReport_Swi = 0xFF;
                    }
                }
                else
                {
                    HostCtl[ index ].Interface[ num ].Type = DEC_UNKNOW;
                    i += Com_Buf[ i ];
                }
            }
            else
            {
                /* USB device type unknown */
                HostCtl[ index ].Interface[ num ].Type = DEC_UNKNOW;
                i += Com_Buf[ i ];

                break;
            }
                  
            num++;
        }
        else
        {
            i += Com_Buf[ i ];
        }
    }
    
    return s;
}

/*********************************************************************
 * @fn      KM_AnalyzeHidReportDesc
 *
 * @brief   Analyze keyboard and mouse report descriptor.
 *
 * @para    index: USB host port
 *
 * @return  The result of the analysis.
 */
void KM_AnalyzeHidReportDesc( uint8_t index, uint8_t intf_num )
{
    uint8_t  id = 0x00;
    uint8_t  led = 0x00;
    uint8_t  size, type, tag;
    uint8_t  report_size;
    uint8_t  report_cnt;
    uint16_t report_bits;

    uint16_t i = 0;

    /* Usage Page(Generic Desktop), Usage(Kyeboard) */
    if( ( Com_Buf[ i + 0 ] == 0x05 ) && ( Com_Buf[ i + 1 ] == 0x01 ) &&
        ( Com_Buf[ i + 2 ] == 0x09 ) && ( Com_Buf[ i + 3 ] == 0x06 ) )
    {
        i += 4;
        report_size = 0;
        report_cnt = 0;
        report_bits = 0;

        while( i < HostCtl[ index ].Interface[ intf_num ].HidDescLen )
        {
            /* Item Size, Item Type, Item Tag */
            size = Com_Buf[ i ] & 0x03;
            type = Com_Buf[ i ] & 0x0C;
            tag = Com_Buf[ i ] & 0xF0;

            switch( type )
            {
                /* MAIN */
                case 0x00:
                    switch( tag )
                    {
                        /* Output */
                        case 0x90:
                            if( led )
                            {
                                report_bits += report_cnt * report_size;

                                /* Save report ID for output */
                                if( ( id != 0 ) && ( HostCtl[ index ].Interface[ intf_num ].IDFlag == 0 ) )
                                {
                                    HostCtl[ index ].Interface[ intf_num ].IDFlag = 1;
                                    HostCtl[ index ].Interface[ intf_num ].ReportID = id;
                                }
                            }
                            i++;
                            break;

                        default:
                            i++;
                            break;
                    }
                    break;

                /* Global */
                case 0x04:
                    switch( tag )
                    {
                        /* Report ID */
                        case 0x80:
                            i++;
                            id = Com_Buf[ i ];
                            break;

                        /* Report Count */
                        case 0x90:
                            i++;
                            report_cnt = Com_Buf[ i ];
                            break;

                        /* Report Size */
                        case 0x70:
                            i++;
                            report_size = Com_Buf[ i ];
                            break;

                        /* Usage Page */
                        case 0x00:
                            i++;
                            if( Com_Buf[ i ] == 0x08 )      // LED
                            {
                                led = 1;
                            }
                            else
                            {
                                led = 0;
                            }
                            break;

                        default:
                            i++;
                            break;
                    }
                    break;

                /* Local */
                case 0x08:
                    switch( tag )
                    {
                        /* Usage Minimum */
                        case 0x10:
                            i++;
                            if( led )
                            {
                                HostCtl[ index ].Interface[ intf_num ].LED_Usage_Min = Com_Buf[ i ];
                            }
                            break;

                        /* Usage Maximum */
                        case 0x20:
                            i++;
                            if( led )
                            {
                                HostCtl[ index ].Interface[ intf_num ].LED_Usage_Max = Com_Buf[ i ];
                            }
                            break;

                        default:
                            i++;
                            break;
                    }
                    break;

                default:
                    i++;
                    break;
            }
            i += size;
        }

        if( report_bits == 8 )
        {
            if( HostCtl[ index ].Interface[ intf_num ].SetReport_Swi == 0 )
            {
                HostCtl[ index ].Interface[ intf_num ].SetReport_Swi = 1;
            }
        }
        else
        {
            HostCtl[ index ].Interface[ intf_num ].SetReport_Swi = 0;
        }
    }
}

/*********************************************************************
 * @fn      KM_DealHidReportDesc
 *
 * @brief   Get and analyze keyboard and mouse report descriptor.
 *
 * @para    index: USB host port
 *
 * @return  The result of the acquisition and analysis.
 */
uint8_t KM_DealHidReportDesc( uint8_t usb_port, uint8_t index, uint8_t ep0_size )
{
    uint8_t  s;
    uint8_t  num, num_tmp;
    uint8_t  getrep_cnt;
#if DEF_DEBUG_PRINTF
    uint16_t i;
#endif

    getrep_cnt = 0;
    num_tmp = HostCtl[ index ].InterfaceNum;
    while( num_tmp )
    {
        num = HostCtl[ index ].InterfaceNum - num_tmp;
        if( HostCtl[ index ].Interface[ num ].HidDescLen )
        {
GETREP_START:
            getrep_cnt++;
            
            /* Get HID report descriptor */
            DUG_PRINTF("Get Interface%x RepDesc: ", num );
            s = HID_GetHidDesr( usb_port, ep0_size, num, Com_Buf, &HostCtl[ index ].Interface[ num ].HidDescLen );
            if( s == ERR_SUCCESS )
            {
                /* Print HID report descriptor */
#if DEF_DEBUG_PRINTF
                for( i = 0; i < HostCtl[ index ].Interface[ num ].HidDescLen; i++ )
                {
                    DUG_PRINTF( "%02x " , Com_Buf[ i ]);
                }
                DUG_PRINTF("\r\n");
#endif

                /* Analyze Report Descriptor */
                KM_AnalyzeHidReportDesc( index, num );

                num_tmp--;
            }
            else
            {
                DUG_PRINTF( "Err(%02x)\r\n", s );
                if( getrep_cnt <= 5 )
                {
                    goto GETREP_START;
                }

                return DEF_REP_DESCR_GETFAIL;
            }
        }
        else
        {
            num_tmp--;
        }
    }

    return s;
}

/*********************************************************************
 * @fn      USBH_EnumHidDevice
 *
 * @brief   Enumerate HID device.
 *
 * @para    index: USB host port
 *
 * @return  The result of the enumeration.
 */
uint8_t USBH_EnumHidDevice( uint8_t usb_port, uint8_t index, uint8_t ep0_size )
{
    uint8_t  s;
    uint8_t  intf_num;
#if DEF_DEBUG_PRINTF
    uint8_t  i;
#endif

    DUG_PRINTF( "Enum Hid:\r\n" );
    
    /* Analyze HID class device configuration descriptor and save relevant parameters */
    DUG_PRINTF("Analyze CfgDesc: ");
    s = KM_AnalyzeConfigDesc( usb_port, index );
    if( s == ERR_SUCCESS )
    {
        DUG_PRINTF( "OK\r\n" );
    }
    else
    {
        DUG_PRINTF( "Err(%02x)\r\n", s );
        return s;
    }

    /* Get the string descriptor contained in the configuration descriptor if it exists */
    if( Com_Buf[ 6 ] )
    {
        DUG_PRINTF("Get StringDesc4: ");
        s = USBH_GetStrDescr( usb_port, ep0_size, Com_Buf[ 6 ] );
        if( s == ERR_SUCCESS )
        {
            /* Print the string descriptor contained in the configuration descriptor */
#if DEF_DEBUG_PRINTF
            for( i = 0; i < Com_Buf[ 0 ]; i++ )
            {
                DUG_PRINTF( "%02x ", Com_Buf[ i ] );
            }
            DUG_PRINTF("\r\n");
#endif
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
        }
    }

    /* Get HID report descriptor */
    s = KM_DealHidReportDesc( usb_port, index, ep0_size );
    
    /* Get USB vendor string descriptor  */
    if( DevDesc_Buf[ 14 ] )
    {
        DUG_PRINTF("Get StringDesc1: ");
        s = USBH_GetStrDescr( usb_port, ep0_size, DevDesc_Buf[ 14 ] );
        if( s == ERR_SUCCESS )
        {
            /* Print USB vendor string descriptor */
#if DEF_DEBUG_PRINTF
            for( i = 0; i < Com_Buf[ 0 ]; i++ )
            {
                DUG_PRINTF( "%02x ", Com_Buf[ i ]);
            }
            DUG_PRINTF("\r\n");
#endif
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
        }
    }

    /* Get USB product string descriptor */
    if( DevDesc_Buf[ 15 ] )
    {
        DUG_PRINTF("Get StringDesc2: ");
        s = USBH_GetStrDescr( usb_port, ep0_size, DevDesc_Buf[ 15 ] );
        if( s == ERR_SUCCESS )
        {
            /* Print USB product string descriptor */
#if DEF_DEBUG_PRINTF
            for( i = 0; i < Com_Buf[ 0 ]; i++ )
            {
                DUG_PRINTF( "%02x ", Com_Buf[ i ] );
            }
            DUG_PRINTF("\r\n");
#endif
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
        }
    }

    /* Get USB serial number string descriptor */
    if( DevDesc_Buf[ 16 ] )
    {
        DUG_PRINTF("Get StringDesc3: ");
        s = USBH_GetStrDescr( usb_port, ep0_size, DevDesc_Buf[ 16 ] );
        if( s == ERR_SUCCESS )
        {
            /* Print USB serial number string descriptor */
#if DEF_DEBUG_PRINTF
            for( i = 0; i < Com_Buf[ 0 ]; i++ )
            {
                DUG_PRINTF( "%02x ", Com_Buf[ i ] );
            }
            DUG_PRINTF("\r\n");
#endif
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
        }
    }

    /* Get USB serial number string descriptor */
    for( intf_num = 0; intf_num < HostCtl[ index ].InterfaceNum; intf_num++ )
    {
        if( HostCtl[ index ].Interface[ intf_num ].Type == DEC_KEY )
        {
            HostCtl[ index ].Interface[ intf_num ].SetReport_Value = 0x00;
            KB_SetReport( usb_port, index, ep0_size, intf_num );
        }
    }

    return ERR_SUCCESS;
}

/*********************************************************************
 * @fn      HUB_Analyse_ConfigDesc
 *
 * @brief   Analyze HUB configuration descriptor.
 *
 * @para
 *
 * @return  none
 */
uint8_t HUB_AnalyzeConfigDesc( uint8_t index )
{
    uint8_t  s = ERR_SUCCESS;
    uint16_t i;

    for( i = 0; i < ( Com_Buf[ 2 ] + ( (uint16_t)Com_Buf[ 3 ] << 8 ) ); )
    {
        if( Com_Buf[ i + 1 ] == DEF_DECR_CONFIG )
        {
            /* Save the number of interface of the USB device, only up to 4 */
            if( ( (PUSB_CFG_DESCR)( &Com_Buf[ i ] ) )->bNumInterfaces > 1 )
            {
                HostCtl[ index ].InterfaceNum = 1;
            }
            else
            {
                HostCtl[ index ].InterfaceNum = ( (PUSB_CFG_DESCR)( &Com_Buf[ i ] ) )->bNumInterfaces;
            }
            i += Com_Buf[ i ];
        }
        else if( Com_Buf[ i + 1 ] == DEF_DECR_INTERFACE )
        {
            if( ( (PUSB_ITF_DESCR)( &Com_Buf[ i ] ) )->bInterfaceClass == 0x09 )
            {
                i += Com_Buf[ i ];
                while( 1 )
                {
                    if( ( Com_Buf[ i + 1 ] == DEF_DECR_INTERFACE ) || ( i >= ( Com_Buf[ 2 ] + ( (uint16_t)Com_Buf[ 3 ] << 8 ) ) ) )
                    {
                        break;
                    }
                    else
                    {
                        /* Analyze each endpoint of the current interface */
                        if( Com_Buf[ i + 1 ] == DEF_DECR_ENDPOINT )
                        {
                            /* Save endpoint related information (endpoint address, attribute, max packet size, polling interval) */
                            if( ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->bEndpointAddress & 0x80 )
                            {
                                /* IN */
                                HostCtl[ index ].Interface[ 0 ].InEndpAddr[ 0 ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->bEndpointAddress & 0x0F;
                                HostCtl[ index ].Interface[ 0 ].InEndpType[ 0 ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->bmAttributes;
                                HostCtl[ index ].Interface[ 0 ].InEndpSize[ 0 ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->wMaxPacketSizeL + \
                                                                              (uint16_t)( ( ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->wMaxPacketSizeH ) << 8 );
                                HostCtl[ index ].Interface[ 0 ].InEndpInterval[ 0 ] = ( (PUSB_ENDP_DESCR)( &Com_Buf[ i ] ) )->bInterval;
                                HostCtl[ index ].Interface[ 0 ].InEndpNum++;
                            }

                            i += Com_Buf[ i ];
                        }
                        else
                        {
                            i += Com_Buf[ i ];
                        }
                    }
                }
            }
            else
            {
                /* USB device type unknown */
                i += Com_Buf[ i ];
            }
        }
        else
        {
            i += Com_Buf[ i ];
        }
    }
    return s;
}

/*********************************************************************
 * @fn      USBH_EnumHubDevice
 *
 * @brief   Enumerate HUB device.
 *
 * @para    index: USB host port
 *
 * @return  The result of the enumeration.
 */
uint8_t USBH_EnumHubDevice( uint8_t usb_port, uint8_t ep0_size )
{
    uint8_t  s, retry;
    uint16_t len;
    uint16_t  i;

    DUG_PRINTF( "Enum Hub:\r\n" );

    /* Analyze HID class device configuration descriptor and save relevant parameters */
    DUG_PRINTF("Analyze CfgDesc: ");
    s = HUB_AnalyzeConfigDesc( RootHubDev[ usb_port ].DeviceIndex );
    if( s == ERR_SUCCESS )
    {
        DUG_PRINTF( "OK\r\n" );
    }
    else
    {
        DUG_PRINTF( "Err(%02x)\r\n", s );
        return s;
    }

    /* Get the string descriptor contained in the configuration descriptor if it exists */
    if( Com_Buf[ 6 ] )
    {
        DUG_PRINTF("Get StringDesc4: ");
        s = USBH_GetStrDescr( usb_port, ep0_size, Com_Buf[ 6 ] );
        if( s == ERR_SUCCESS )
        {
            /* Print the string descriptor contained in the configuration descriptor */
#if DEF_DEBUG_PRINTF
            for( i = 0; i < Com_Buf[ 0 ]; i++ )
            {
                DUG_PRINTF( "%02x ", Com_Buf[ i ] );
            }
            DUG_PRINTF("\r\n");
#endif
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
        }
    }

    /* Get USB vendor string descriptor  */
    if( DevDesc_Buf[ 14 ] )
    {
        DUG_PRINTF("Get StringDesc1: ");
        s = USBH_GetStrDescr( usb_port, ep0_size, DevDesc_Buf[ 14 ] );
        if( s == ERR_SUCCESS )
        {
            /* Print USB vendor string descriptor */
#if DEF_DEBUG_PRINTF
            for( i = 0; i < Com_Buf[ 0 ]; i++ )
            {
                DUG_PRINTF( "%02x ", Com_Buf[ i ]);
            }
            DUG_PRINTF("\r\n");
#endif
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
        }
    }

    /* Get USB product string descriptor */
    if( DevDesc_Buf[ 15 ] )
    {
        DUG_PRINTF("Get StringDesc2: ");
        s = USBH_GetStrDescr( usb_port, ep0_size, DevDesc_Buf[ 15 ] );
        if( s == ERR_SUCCESS )
        {
            /* Print USB product string descriptor */
#if DEF_DEBUG_PRINTF
            for( i = 0; i < Com_Buf[ 0 ]; i++ )
            {
                DUG_PRINTF( "%02x ", Com_Buf[ i ] );
            }
            DUG_PRINTF("\r\n");
#endif
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
        }
    }

    /* Get USB serial number string descriptor */
    if( DevDesc_Buf[ 16 ] )
    {
        DUG_PRINTF("Get StringDesc3: ");
        s = USBH_GetStrDescr( usb_port, ep0_size, DevDesc_Buf[ 16 ] );
        if( s == ERR_SUCCESS )
        {
            /* Print USB serial number string descriptor */
#if DEF_DEBUG_PRINTF
            for( i = 0; i < Com_Buf[ 0 ]; i++ )
            {
                DUG_PRINTF( "%02x ", Com_Buf[ i ] );
            }
            DUG_PRINTF("\r\n");
#endif
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
        }
    }

    /* Get hub descriptor */
    DUG_PRINTF("Get Hub Desc: ");
    for( retry = 0; retry < 5; retry++ )
    {
        s = HUB_GetClassDevDescr( usb_port, ep0_size, Com_Buf, &len );
        if( s == ERR_SUCCESS )
        {
            /* Print USB device device descriptor */
#if DEF_DEBUG_PRINTF
            for( i = 0; i < len; i++ )
            {
                DUG_PRINTF( "%02x ", Com_Buf[ i ] );
            }
            DUG_PRINTF("\r\n");
#endif

            RootHubDev[ usb_port ].bPortNum = ( (PUSB_HUB_DESCR)Com_Buf)->bNbrPorts;
            if( RootHubDev[ usb_port ].bPortNum > DEF_NEXT_HUB_PORT_NUM_MAX )
            {
                RootHubDev[ usb_port ].bPortNum = DEF_NEXT_HUB_PORT_NUM_MAX;
            }
            DUG_PRINTF( "RootHubDev[ %02x ].bPortNum: %02x\r\n", usb_port, RootHubDev[ usb_port ].bPortNum );
            break;
        }
        else
        {
            /* Determine whether the maximum number of retries has been reached, and retry if not reached */
            DUG_PRINTF( "Err(%02x)\r\n", s );

            if( retry == 4 )
            {
                return ERR_USB_UNKNOWN;
            }
        }
    }

    /* Set the HUB port to power on */
    for( retry = 0, i = 1; i <= RootHubDev[ usb_port ].bPortNum; i++ )
    {
        s = HUB_SetPortFeature( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, i, HUB_PORT_POWER );
        if( s == ERR_SUCCESS )
        {
            continue;
        }
        else
        {
            Delay_Ms( 5 );

            i--;
            retry++;
            if( retry >= 5 )
            {
                return ERR_USB_UNKNOWN;
            }
        }
    }
    
    return ERR_SUCCESS;
}

/*********************************************************************
 * @fn      HUB_Port_PreEnum1
 *
 * @brief
 *
 * @para
 *
 * @return  none
 */
uint8_t HUB_Port_PreEnum1( uint8_t usb_port, uint8_t hub_port, uint8_t *pbuf )
{
    uint8_t  s;
    uint8_t  buf[ 4 ];
    uint8_t  retry;

    if( ( *pbuf ) & ( 1 << hub_port ) )
    {
        s = HUB_GetPortStatus( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, hub_port, &buf[ 0 ] );
        if( s != ERR_SUCCESS )
        {
            DUG_PRINTF( "HUB_PE1_ERR1:%x\r\n", s );
            return s;
        }
        else
        {
            if( buf[ 2 ] & 0x01 )
            {
                s = HUB_ClearPortFeature( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, hub_port, HUB_C_PORT_CONNECTION );
                if( s != ERR_SUCCESS )
                {
                    DUG_PRINTF( "HUB_PE1_ERR2:%x\r\n", s );
                    return s;
                }

                retry = 0;
                do
                {
                    s = HUB_GetPortStatus( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, hub_port, &buf[ 0 ] );
                    if( s != ERR_SUCCESS )
                    {
                        DUG_PRINTF( "HUB_PE1_ERR3:%x\r\n", s );
                        return s;
                    }
                    retry++;
                }while( ( buf[ 2 ] & 0x01 ) && ( retry < 10 ) );

                if( retry != 10 )
                {
                    if( !( buf[ 0 ] & 0x01 ) )
                    {
                        DUG_PRINTF( "Hub Port%x Out\r\n", hub_port );
                        return ERR_USB_DISCON;
                    }
                }
            }
        }
    }

    return ERR_USB_UNKNOWN;
}

/*********************************************************************
 * @fn      HUB_Port_PreEnum2
 *
 * @brief
 *
 * @para
 *
 * @return  none
 */
uint8_t HUB_Port_PreEnum2( uint8_t usb_port, uint8_t hub_port, uint8_t *pbuf )
{
    uint8_t  s;
    uint8_t  buf[ 4 ];
    uint8_t  retry = 0;

    if( ( *pbuf ) & ( 1 << hub_port ) )
    {
        s = HUB_SetPortFeature( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, hub_port, HUB_PORT_RESET );
        if( s != ERR_SUCCESS )
        {
            DUG_PRINTF( "HUB_PE2_ERR1:%x\r\n", s );
            return s;
        }

        Delay_Ms( 10 );
        do
        {
            s = HUB_GetPortStatus( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, hub_port, &buf[ 0 ] );
            if( s != ERR_SUCCESS )
            {
                DUG_PRINTF( "HUB_PE2_ERR2:%x\r\n", s );
                return s;
            }
            retry++;
        }while( ( !( buf[ 2 ] & 0x10 ) ) && ( retry <= 10 ) );

        if( retry != 10 )
        {
            retry = 0;
            s = HUB_ClearPortFeature( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, hub_port, HUB_C_PORT_RESET  );

            do
            {
                s = HUB_GetPortStatus( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, hub_port, &buf[ 0 ] );
                if( s != ERR_SUCCESS )
                {
                    DUG_PRINTF( "HUB_PE2_ERR3:%x\r\n", s );
                    return s;
                }
                retry++;
            }while( ( buf[ 2 ] & 0x10 ) && ( retry <= 10 ) ); // ����˿ڸ�λ���

            if( retry != 10 )
            {
                if( buf[ 0 ] & 0x01 )
                {
                    DUG_PRINTF( "Hub Port%x In\r\n", hub_port );
                    return ERR_USB_CONNECT;
                }
            }
        }
    }

    return ERR_USB_UNKNOWN;
}

/*********************************************************************
 * @fn      HUB_CheckPortSpeed
 *
 * @brief
 *
 * @para
 *
 * @return  none
 */
uint8_t HUB_CheckPortSpeed( uint8_t usb_port, uint8_t hub_port, uint8_t *pbuf )
{
    uint8_t  s;

    s = HUB_GetPortStatus( usb_port, RootHubDev[ usb_port ].bEp0MaxPks, hub_port, pbuf );
    if( s )
    {
        return s;
    }

    if( pbuf[ 1 ] & 0x02 )
    {
        return USB_LOW_SPEED;
    }
    else
    {
        if( pbuf[ 1 ] & 0x04 )
        {
            return USB_HIGH_SPEED;
        }
        else
        {
            return USB_FULL_SPEED;
        }
    }
}

/*********************************************************************
 * @fn      USBH_EnumHubPortDevice
 *
 * @brief
 *
 * @para
 *
 * @return  none
 */
uint8_t USBH_EnumHubPortDevice( uint8_t usb_port, uint8_t hub_port, uint8_t *paddr, uint8_t *ptype )
{
    uint8_t  s;
    uint8_t  enum_cnt;
    uint16_t len;
    uint8_t  cfg_val;
#if DEF_DEBUG_PRINTF
    uint16_t i;
#endif

    /* Get USB device descriptor */
    DUG_PRINTF("(S1)Get DevDesc: \r\n");
    enum_cnt = 0;
    do
    {
        enum_cnt++;
        s = USBFSH_GetDeviceDescr( &RootHubDev[ usb_port ].Device[ hub_port ].bEp0MaxPks, DevDesc_Buf );
        if( s == ERR_SUCCESS )
        {
#if DEF_DEBUG_PRINTF
            for( i = 0; i < 18; i++ )
            {
                DUG_PRINTF( "%02x ", DevDesc_Buf[ i ] );
            }
            DUG_PRINTF( "\r\n" );
#endif
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
            if( enum_cnt >= 10 )
            {
                return DEF_DEV_DESCR_GETFAIL;
            }
        }
    }while( ( s != ERR_SUCCESS ) && ( enum_cnt < 10 ) );

    /* Set the USB device address */
    DUG_PRINTF( "Set DevAddr: \r\n" );
    enum_cnt = 0;
    do
    {
        enum_cnt++;
        s = USBFSH_SetUsbAddress( RootHubDev[ usb_port ].Device[ hub_port ].bEp0MaxPks, \
                                  RootHubDev[ usb_port ].Device[ hub_port ].DeviceIndex + USB_DEVICE_ADDR );
        if( s == ERR_SUCCESS )
        {
            /* Save address */
            *paddr = RootHubDev[ usb_port ].Device[ hub_port ].DeviceIndex + USB_DEVICE_ADDR;
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
            if( enum_cnt >= 10 )
            {
                return DEF_DEV_ADDR_SETFAIL;
            }
        }
    }while( ( s != ERR_SUCCESS ) && ( enum_cnt < 10 ) );
    Delay_Ms( 5 );

    /* Get USB configuration descriptor */
    DUG_PRINTF( "Get DevCfgDesc: \r\n" );
    enum_cnt = 0;
    do
    {
        enum_cnt++;
        s = USBFSH_GetConfigDescr( RootHubDev[ usb_port ].Device[ hub_port ].bEp0MaxPks, Com_Buf, DEF_COM_BUF_LEN, &len );
        if( s == ERR_SUCCESS )
        {
#if DEF_DEBUG_PRINTF
            for( i = 0; i < len; i++ )
            {
                DUG_PRINTF( "%02x ", Com_Buf[ i ] );
            }
            DUG_PRINTF( "\r\n" );
#endif

            /* Save configuration value */
            cfg_val = ( (PUSB_CFG_DESCR)Com_Buf )->bConfigurationValue;

            /* Analyze USB device type */
            USBH_AnalyseType( DevDesc_Buf, Com_Buf, ptype );
            DUG_PRINTF( "DevType: %02x\r\n", *ptype );
        }
        else
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
            if( enum_cnt >= 10 )
            {
                return DEF_DEV_DESCR_GETFAIL;
            }
        }
    }while( ( s != ERR_SUCCESS ) && ( enum_cnt < 10 ) );

    /* Set USB device configuration value */
    DUG_PRINTF( "Set CfgValue: \r\n" );
    enum_cnt = 0;
    do
    {
        enum_cnt++;
        s = USBFSH_SetUsbConfig( RootHubDev[ usb_port ].Device[ hub_port ].bEp0MaxPks, cfg_val );
        if( s != ERR_SUCCESS )
        {
            DUG_PRINTF( "Err(%02x)\r\n", s );
            if( enum_cnt >= 10 )
            {
                return DEF_CFG_VALUE_SETFAIL;
            }
        }
    }while( ( s != ERR_SUCCESS ) && ( enum_cnt < 10 ) );

    return( ERR_SUCCESS );
}

/*********************************************************************
 * @fn      KB_AnalyzeKeyValue
 *
 * @brief   Handle keyboard lighting.
 *
 * @para    index: USB host port
 *          intfnum: Interface number.
 *          pbuf: Data buffer.
 *          len: Data length.
 *
 * @return  The result of the analysis.
 */
void KB_AnalyzeKeyValue( uint8_t index, uint8_t intf_num, uint8_t *pbuf, uint16_t len )
{
    uint8_t  i;
    uint8_t  value;
    uint8_t  bit_pos = 0x00;

    value = HostCtl[ index ].Interface[ intf_num ].SetReport_Value;

    for( i = HostCtl[ index ].Interface[ intf_num ].LED_Usage_Min; i <= HostCtl[ index ].Interface[ intf_num ].LED_Usage_Max; i++ )
    {
        if( i == 0x01 )
        {
            if( memchr( pbuf, DEF_KEY_NUM, len ) )
            {
                HostCtl[ index ].Interface[ intf_num ].SetReport_Value ^= ( 1 << bit_pos );
            }
        }
        else if( i == 0x02 )
        {
            if( memchr( pbuf, DEF_KEY_CAPS, len ) )
            {
                HostCtl[ index ].Interface[ intf_num ].SetReport_Value ^= ( 1 << bit_pos );
            }
        }
        else if( i == 0x03 )
        {
            if( memchr( pbuf, DEF_KEY_SCROLL, len ) )
            {
                HostCtl[ index ].Interface[ intf_num ].SetReport_Value ^= ( 1 << bit_pos );
            }
        }

        bit_pos++;
    }

    if( value != HostCtl[ index ].Interface[ intf_num ].SetReport_Value )
    {
        HostCtl[ index ].Interface[ intf_num ].SetReport_Flag = 1;
    }
    else
    {
        HostCtl[ index ].Interface[ intf_num ].SetReport_Flag = 0;
    }
}

/*********************************************************************
 * @fn      KB_SetReport
 *
 * @brief   Handle keyboard lighting.
 *
 * @para    index: USB device number.
 *          intf_num: Interface number.
 *
 * @return  The result of the handling keyboard lighting.
 */
uint8_t KB_SetReport( uint8_t usb_port, uint8_t index, uint8_t ep0_size, uint8_t intf_num )
{
    uint8_t  dat[ 2 ];
    uint16_t len;

    if( HostCtl[ index ].Interface[ intf_num ].IDFlag )
    {
        dat[ 0 ] = HostCtl[ index ].Interface[ intf_num ].ReportID;
        dat[ 1 ] = HostCtl[ index ].Interface[ intf_num ].SetReport_Value;
        len = 2;
    }
    else
    {
        dat[ 0 ] = HostCtl[ index ].Interface[ intf_num ].SetReport_Value;
        len = 1;
    }

    if( HostCtl[ index ].Interface[ intf_num ].SetReport_Swi == 1 ) // Perform lighting operation through endpoint0
    {
        /* Send set report command */
        return HID_SetReport( usb_port, ep0_size, intf_num, dat, &len );
    }
    else if( HostCtl[ index ].Interface[ intf_num ].SetReport_Swi == 0xFF )  // Perform lighting operation through other endpoint
    {
        return USBH_SendHidData( usb_port, index, intf_num, 0, dat, len );
    }

    return ERR_SUCCESS;
}

/*********************************************************************
 * @fn      USBH_MainDeal
 *
 * @brief   Provide a simple enumeration process for USB devices and
 *          obtain keyboard and mouse data at regular intervals.
 *
 * @return  none
 */
void USBH_MainDeal( void )
{
    uint8_t  s;
    uint8_t  usb_port;
#if DEF_USBFS_PORT_EN
    uint8_t  hub_port;
    uint8_t  hub_dat;
#endif
    uint8_t  index;
    uint8_t  intf_num, in_num;
    uint16_t len;
#if DEF_DEBUG_PRINTF
    uint16_t i;
#endif
    
    for( usb_port = 0; usb_port < DEF_TOTAL_ROOT_HUB; usb_port++ )
    {
        s = USBH_CheckRootHubPortStatus( usb_port ); // Check USB device connection or disconnection
        if( s == ROOT_DEV_CONNECTED )
        {
            DUG_PRINTF( "USB Port%x Dev In.\r\n", usb_port );
            
            /* Set root device state parameters */
            RootHubDev[ usb_port ].bStatus = ROOT_DEV_CONNECTED;
            RootHubDev[ usb_port ].DeviceIndex = usb_port * DEF_ONE_USB_SUP_DEV_TOTAL;

            s = USBH_EnumRootDevice( usb_port ); // Simply enumerate root device
            if( s == ERR_SUCCESS )
            {
                if( RootHubDev[ usb_port ].bType == USB_DEV_CLASS_HID ) // Further enumerate it if this device is a HID device
                {
                    DUG_PRINTF("Root Device Is HID. ");

                    s = USBH_EnumHidDevice( usb_port, RootHubDev[ usb_port ].DeviceIndex, RootHubDev[ usb_port ].bEp0MaxPks );
                    DUG_PRINTF( "Further Enum Result: " );
                    if( s == ERR_SUCCESS )
                    {
                        DUG_PRINTF( "OK\r\n" );
                        
                        /* Set the connection status of the device  */
                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_SUCCESS;
                    }
                    else if( s != ERR_USB_DISCON )
                    {
                        DUG_PRINTF( "Err(%02x)\r\n", s );
                        
                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED;
                    }
                }
                else if( RootHubDev[ usb_port ].bType == USB_DEV_CLASS_HUB )
                {
                    DUG_PRINTF("Root Device Is HUB. ");

                    s = USBH_EnumHubDevice( usb_port, RootHubDev[ usb_port ].bEp0MaxPks );
                    DUG_PRINTF( "Further Enum Result: " );
                    if( s == ERR_SUCCESS )
                    {
                        DUG_PRINTF( "OK\r\n" );

                        /* Set the connection status of the device  */
                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_SUCCESS;
                    }
                    else if( s != ERR_USB_DISCON )
                    {
                        DUG_PRINTF( "Err(%02x)\r\n", s );

                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED;
                    }
                }
                else // Detect that this device is a Non-HID device
                {
                    DUG_PRINTF( "Root Device Is " );
                    switch( RootHubDev[ usb_port ].bType )
                    {
                        case USB_DEV_CLASS_STORAGE:
                            DUG_PRINTF("Storage. ");
                            break;
                        case USB_DEV_CLASS_PRINTER:
                            DUG_PRINTF("Printer. ");
                            break;
                        case DEF_DEV_TYPE_UNKNOWN:
                            DUG_PRINTF("Unknown. ");
                            break;
                    }
                    DUG_PRINTF( "End Enum.\r\n" );
                    
                    RootHubDev[ usb_port ].bStatus = ROOT_DEV_SUCCESS;
                }
            }
            else if( s != ERR_USB_DISCON )
            {
                /* Enumeration failed */
                DUG_PRINTF( "Enum Fail with Error Code:%x\r\n",s );
                RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED;
            }
        }
        else if( s == ROOT_DEV_DISCONNECT )
        {
            DUG_PRINTF( "USB Port%x Dev Out.\r\n", usb_port );
            
            /* Clear parameters */
            index = RootHubDev[ usb_port ].DeviceIndex;
            memset( &RootHubDev[ usb_port ].bStatus, 0, sizeof( ROOT_HUB_DEVICE ) );
            memset( &HostCtl[ index ].InterfaceNum, 0, sizeof( HOST_CTL ) );
        }
    }

    /* Get the data of the HID device connected to the USB host port */
    for( usb_port = 0; usb_port < DEF_TOTAL_ROOT_HUB; usb_port++ )
    {
        if( RootHubDev[ usb_port ].bStatus >= ROOT_DEV_SUCCESS )
        {
            index = RootHubDev[ usb_port ].DeviceIndex;
            if( RootHubDev[ usb_port ].bType == USB_DEV_CLASS_HID )
            {
                for( intf_num = 0; intf_num < HostCtl[ index ].InterfaceNum; intf_num++ )
                {
                    for( in_num = 0; in_num < HostCtl[ index ].Interface[ intf_num ].InEndpNum; in_num++ )
                    {                   
                        /* Get endpoint data based on the interval time of the device */
                        if( HostCtl[ index ].Interface[ intf_num ].InEndpTimeCount[ in_num ] >= HostCtl[ index ].Interface[ intf_num ].InEndpInterval[ in_num ] )
                        {
                            HostCtl[ index ].Interface[ intf_num ].InEndpTimeCount[ in_num ] %= HostCtl[ index ].Interface[ intf_num ].InEndpInterval[ in_num ];
               
                            /* Get endpoint data */
                            s = USBH_GetHidData( usb_port, index, intf_num, in_num, Com_Buf, &len );
                            if( s == ERR_SUCCESS )
                            {
#if DEF_DEBUG_PRINTF
                                for( i = 0; i < len; i++ )
                                {
                                    DUG_PRINTF( "%02x ", Com_Buf[ i ] );
                                }
                                DUG_PRINTF( "\r\n" );
#endif
                                
                                /* Handle keyboard lighting */
                                if( HostCtl[ index ].Interface[ intf_num ].Type == DEC_KEY )
                                {
                                    KB_AnalyzeKeyValue( index, intf_num, Com_Buf, len );

                                    if( HostCtl[ index ].Interface[ intf_num ].SetReport_Flag )
                                    {
                                        KB_SetReport( usb_port, index, RootHubDev[ usb_port ].bEp0MaxPks, intf_num );
                                    }
                                }
                            }
                            else if( s == ERR_USB_DISCON )
                            {
                                break;
                            }
                            else if( s == ( USB_PID_STALL | ERR_USB_TRANSFER ) )
                            {
                                /* USB device abnormal event */
                                DUG_PRINTF("Abnormal\r\n");
                                
                                /* Clear endpoint */
                                USBH_ClearEndpStall( usb_port, HostCtl[ index ].Interface[ intf_num ].InEndpAddr[ in_num ] | 0x80 );
                                HostCtl[ index ].Interface[ intf_num ].InEndpTog[ in_num ] = 0x00;
                                
                                /* Judge the number of error */
                                HostCtl[ index ].ErrorCount++;
                                if( HostCtl[ index ].ErrorCount >= 10 )
                                {
                                    /* Re-enumerate the device and clear the endpoint again */
                                    memset( &RootHubDev[ usb_port ].bStatus, 0, sizeof( ROOT_HUB_DEVICE ) );
                                    s = USBH_EnumRootDevice( usb_port );
                                    if( s == ERR_SUCCESS )
                                    {
                                        USBH_ClearEndpStall( usb_port, HostCtl[ index ].Interface[ intf_num ].InEndpAddr[ in_num ] | 0x80 );
                                        HostCtl[ index ].ErrorCount = 0x00;
                                        
                                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_CONNECTED; 
                                        RootHubDev[ usb_port ].DeviceIndex = usb_port * DEF_ONE_USB_SUP_DEV_TOTAL;
                                        
                                        memset( &HostCtl[ index ].InterfaceNum, 0, sizeof( HOST_CTL ) );
                                        s = USBH_EnumHidDevice( usb_port, index, RootHubDev[ usb_port ].bEp0MaxPks );
                                        if( s == ERR_SUCCESS )
                                        {
                                            RootHubDev[ usb_port ].bStatus = ROOT_DEV_SUCCESS; 
                                        }
                                        else if( s != ERR_USB_DISCON )
                                        {
                                            RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED; 
                                        }
                                    }
                                    else if( s != ERR_USB_DISCON )
                                    {
                                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED;
                                    }
                                }
                            }
                        }
                    }

                    if( s == ERR_USB_DISCON )
                    {
                        break;
                    }
                }
            }
#if DEF_USBFS_PORT_EN
            else if( ( usb_port == DEF_USBFS_PORT_INDEX ) && ( RootHubDev[ usb_port ].bType == USB_DEV_CLASS_HUB ) )
            {
                /* Query port status change */
                if( HostCtl[ index ].Interface[ 0 ].InEndpTimeCount[ 0 ] >= HostCtl[ index ].Interface[ 0 ].InEndpInterval[ 0 ] )
                {
                    HostCtl[ index ].Interface[ 0 ].InEndpTimeCount[ 0 ] %= HostCtl[ index ].Interface[ 0 ].InEndpInterval[ 0 ];

                    /* Select HUB port */
                    USBFSH_SetSelfAddr( RootHubDev[ usb_port ].bAddress );
                    USBFSH_SetSelfSpeed( RootHubDev[ usb_port ].bSpeed );

                    /* Get HUB interrupt endpoint data */
                    s = USBFSH_GetEndpData( HostCtl[ index ].Interface[ 0 ].InEndpAddr[ 0 ], &HostCtl[ index ].Interface[ 0 ].InEndpTog[ 0 ], Com_Buf, &len );
                    if( s == ERR_SUCCESS )
                    {
                        hub_dat = Com_Buf[ 0 ];
                        DUG_PRINTF( "Hub Int Data:%02x\r\n", hub_dat );

                        for( hub_port = 0; hub_port < RootHubDev[ usb_port ].bPortNum; hub_port++ )
                        {
                            /* HUB Port PreEnumate Step 1: C_PORT_CONNECTION */
                            s = HUB_Port_PreEnum1( usb_port, ( hub_port + 1 ), &hub_dat );
                            if( s == ERR_USB_DISCON )
                            {
                                hub_dat &= ~( 1 << ( hub_port + 1 ) );

                                /* Clear parameters */
                                memset( &HostCtl[ RootHubDev[ usb_port ].Device[ hub_port ].DeviceIndex ], 0, sizeof( HOST_CTL ) );
                                memset( &RootHubDev[ usb_port ].Device[ hub_port ].bStatus, 0, sizeof( HUB_DEVICE ) );
                                continue;
                            }

                            /* HUB Port PreEnumate Step 2: Set/Clear PORT_RESET */
                            Delay_Ms( 100 );
                            s = HUB_Port_PreEnum2( usb_port, ( hub_port + 1 ), &hub_dat );
                            if( s == ERR_USB_CONNECT )
                            {
                                /* Set parameters */
                                RootHubDev[ usb_port ].Device[ hub_port ].bStatus = ROOT_DEV_CONNECTED;
                                RootHubDev[ usb_port ].Device[ hub_port ].bEp0MaxPks = DEFAULT_ENDP0_SIZE;
                                RootHubDev[ usb_port ].Device[ hub_port ].DeviceIndex = usb_port * DEF_ONE_USB_SUP_DEV_TOTAL + hub_port + 1;
                            }
                            else
                            {
                                hub_dat &= ~( 1 << ( hub_port + 1 ) );
                            }

                            /* Enumerate HUB Device */
                            if( RootHubDev[ usb_port ].Device[ hub_port ].bStatus == ROOT_DEV_CONNECTED )
                            {
                                /* Check device speed */
                                RootHubDev[ usb_port ].Device[ hub_port ].bSpeed = HUB_CheckPortSpeed( usb_port, ( hub_port + 1 ), Com_Buf );
                                DUG_PRINTF( "Dev Speed:%x\r\n", RootHubDev[ usb_port ].Device[ hub_port ].bSpeed );

                                /* Select the specified port */
                                USBFSH_SetSelfAddr( RootHubDev[ usb_port ].Device[ hub_port ].bAddress );
                                USBFSH_SetSelfSpeed( RootHubDev[ usb_port ].Device[ hub_port ].bSpeed );
                                if( RootHubDev[ usb_port ].bSpeed != USB_LOW_SPEED )
                                {
                                    USBOTG_H_FS->HOST_CTRL &= ~USBFS_UH_LOW_SPEED;
                                }

                                /* Enumerate the USB device of the current HUB port */
                                DUG_PRINTF("Enum_HubDevice\r\n");
                                s = USBH_EnumHubPortDevice( usb_port, hub_port, &RootHubDev[ usb_port ].Device[ hub_port ].bAddress, \
                                                            &RootHubDev[ usb_port ].Device[ hub_port ].bType );
                                if( s == ERR_SUCCESS )
                                {
                                    if( RootHubDev[ usb_port ].Device[ hub_port ].bType == USB_DEV_CLASS_HID )
                                    {
                                        DUG_PRINTF( "HUB port%x device is HID! Further Enum:\r\n", hub_port );

                                        /* Perform HID class enumeration on the current device */
                                        s = USBH_EnumHidDevice( usb_port, RootHubDev[ usb_port ].Device[ hub_port ].DeviceIndex, \
                                                                RootHubDev[ usb_port ].Device[ hub_port ].bEp0MaxPks );
                                        if( s == ERR_SUCCESS )
                                        {
                                            RootHubDev[ usb_port ].Device[ hub_port ].bStatus = ROOT_DEV_SUCCESS;
                                            DUG_PRINTF( "OK!\r\n" );
                                        }
                                    }
                                    else // Detect that this device is a Non-HID device
                                    {
                                        DUG_PRINTF( "HUB port%x device is ", hub_port );
                                        switch( RootHubDev[ usb_port ].Device[ hub_port ].bType )
                                        {
                                            case USB_DEV_CLASS_STORAGE:
                                                DUG_PRINTF("storage!\r\n");
                                                break;
                                            case USB_DEV_CLASS_PRINTER:
                                                DUG_PRINTF("printer!\r\n");
                                                break;
                                            case USB_DEV_CLASS_HUB:
                                                DUG_PRINTF("printer!\r\n");
                                                break;
                                            case DEF_DEV_TYPE_UNKNOWN:
                                                DUG_PRINTF("unknown!\r\n");
                                                break;
                                        }
                                        RootHubDev[ usb_port ].Device[ hub_port ].bStatus = ROOT_DEV_SUCCESS;
                                    }
                                }
                                else
                                {
                                    RootHubDev[ usb_port ].Device[ hub_port ].bStatus = ROOT_DEV_FAILED;
                                    DUG_PRINTF( "HUB Port%x Enum Err!\r\n", hub_port );
                                }
                            }
                        }
                    }
                }

                /* Get HUB port HID device data */
                for( hub_port = 0; hub_port < RootHubDev[ usb_port ].bPortNum; hub_port++ )
                {
                    if( RootHubDev[ usb_port ].Device[ hub_port ].bStatus == ROOT_DEV_SUCCESS )
                    {
                        index = RootHubDev[ usb_port ].Device[ hub_port ].DeviceIndex;

                        if( RootHubDev[ usb_port ].Device[ hub_port ].bType == USB_DEV_CLASS_HID )
                        {
                            for( intf_num = 0; intf_num < HostCtl[ index ].InterfaceNum; intf_num++ )
                            {
                                for( in_num = 0; in_num < HostCtl[ index ].Interface[ intf_num ].InEndpNum; in_num++ )
                                {
                                    /* Get endpoint data based on the interval time of the device */
                                    if( HostCtl[ index ].Interface[ intf_num ].InEndpTimeCount[ in_num ] >= HostCtl[ index ].Interface[ intf_num ].InEndpInterval[ in_num ] )
                                    {
                                        HostCtl[ index ].Interface[ intf_num ].InEndpTimeCount[ in_num ] %= HostCtl[ index ].Interface[ intf_num ].InEndpInterval[ in_num ];

                                        /* Select HUB device port */
                                        USBFSH_SetSelfAddr( RootHubDev[ usb_port ].Device[ hub_port ].bAddress );
                                        USBFSH_SetSelfSpeed( RootHubDev[ usb_port ].Device[ hub_port ].bSpeed );
                                        if( RootHubDev[ usb_port ].bSpeed != USB_LOW_SPEED )
                                        {
                                            USBOTG_H_FS->HOST_CTRL &= ~USBFS_UH_LOW_SPEED;
                                        }

                                        /* Get endpoint data */
                                        s = USBH_GetHidData( usb_port, index, intf_num, in_num, Com_Buf, &len );
                                        if( s == ERR_SUCCESS )
                                        {
#if DEF_DEBUG_PRINTF
                                            for( i = 0; i < len; i++ )
                                            {
                                                DUG_PRINTF( "%02x ", Com_Buf[ i ] );
                                            }
                                            DUG_PRINTF( "\r\n" );
#endif

                                            if( HostCtl[ index ].Interface[ intf_num ].Type == DEC_KEY )
                                            {
                                                KB_AnalyzeKeyValue( index, intf_num, Com_Buf, len );

                                                if( HostCtl[ index ].Interface[ intf_num ].SetReport_Flag )
                                                {
                                                    KB_SetReport( usb_port, index, RootHubDev[ usb_port ].Device[ hub_port ].bEp0MaxPks, intf_num );
                                                }
                                            }
                                        }
                                        else if( s == ERR_USB_DISCON )
                                        {
                                            break;
                                        }
                                    }
                                }

                                if( s == ERR_USB_DISCON )
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
#endif
        }
    }
}
