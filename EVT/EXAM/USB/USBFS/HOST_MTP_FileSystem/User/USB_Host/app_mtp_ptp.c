/********************************** (C) COPYRIGHT  *******************************
 * File Name          : app_mtp_ptp.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/09/01
 * Description        :
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
__attribute__ ((aligned(4))) uint8_t  Com_Buf[ DEF_COM_BUF_LEN ];               // General Buffer
ROOT_HUB_DEVICE RootHubDev[ DEF_TOTAL_ROOT_HUB ];
HOST_CTL HostCtl[ DEF_TOTAL_ROOT_HUB * DEF_ONE_USB_SUP_DEV_TOTAL ];

uint8_t  *pMTP_Data_Buf;
PTP_ContainerTypeDef PTP_Container[ DEF_TOTAL_ROOT_HUB ];
PTP_OpContainerTypeDef PTP_OpContainer[ DEF_TOTAL_ROOT_HUB ];
PTP_RespContainerTypeDef PTP_RespContainer[ DEF_TOTAL_ROOT_HUB ];
PTP_DataContainerTypeDef PTP_DataContainer[ DEF_TOTAL_ROOT_HUB ];
PTP_HandleTypeDef PTP_Handle[ DEF_TOTAL_ROOT_HUB ];

PTP_DeviceInfoTypeDef PTP_DeviceInfo[ DEF_TOTAL_ROOT_HUB ];
PTP_StorageIDsTypeDef PTP_StorageIDs[ DEF_TOTAL_ROOT_HUB ];
PTP_StorageInfoTypeDef PTP_StorageInfo[ DEF_TOTAL_ROOT_HUB ];
PTP_ObjectInfoTypeDef PTP_ObjectInfo[ DEF_TOTAL_ROOT_HUB ];
MTP_ParaTypeDef MTP_Para[ DEF_TOTAL_ROOT_HUB ];
PTP_PropertyValueTypeDef PTP_PropertyValue[ DEF_TOTAL_ROOT_HUB ];

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
    if( ( dv_cls == USB_DEV_CLASS_HID ) || ( if_cls == USB_DEV_CLASS_HID ) )
    {
        *ptype = USB_DEV_CLASS_HID;
    }
    else if( ( dv_cls == USB_DEV_CLASS_IMAGE ) || ( if_cls == USB_DEV_CLASS_IMAGE ) )
    {
        *ptype = USB_DEV_CLASS_IMAGE;
    }
    else if( ( dv_cls == USB_DEV_CLASS_PRINTER ) || ( if_cls == USB_DEV_CLASS_PRINTER ) )
    {
        *ptype = USB_DEV_CLASS_PRINTER;
    }
    else if( ( dv_cls == USB_DEV_CLASS_STORAGE ) || ( if_cls == USB_DEV_CLASS_STORAGE ) )
    {
        *ptype = USB_DEV_CLASS_STORAGE;
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
    Delay_Ms( 10 );

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
 * @fn      USBH_GetStrDescr
 *
 * @brief   Get the string descriptor of the USB device.
 *
 * @para    index: USB host port
 *
 * @return  The result of getting the string descriptor.
 */
uint8_t USBH_GetStrDescr( uint8_t usb_port, uint8_t str_num )
{
    uint8_t s = ERR_USB_UNSUPPORT;

    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
#if DEF_USBFS_PORT_EN
        s = USBFSH_GetStrDescr( RootHubDev[ usb_port ].bEp0MaxPks, str_num, Com_Buf );
#endif
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
#if DEF_USBHS_PORT_EN
        s = USBHSH_GetStrDescr( RootHubDev[ usb_port ].bEp0MaxPks, str_num, Com_Buf );
#endif
    }

    return s;
}

/*********************************************************************
 * @fn      USBH_AnalyzeCfgDesc
 *
 * @brief   Generally analyze USB device configuration descriptor.
 *
 * @para    index: USB device number.
 *
 * @return  Result
 */
void IMAGE_AnalyzeCfgDesc( uint8_t index )
{
    uint8_t  intfacenum, innum, outnum;
    uint16_t i, j;
    uint16_t total_len;

    /* Save the number of interface of the USB device, only up to 4 */
    if( ( (PUSB_CFG_DESCR)Com_Buf )->bNumInterfaces > DEF_INTERFACE_NUM_MAX )
    {
        HostCtl[ index ].InterfaceNum = DEF_INTERFACE_NUM_MAX;
    }
    else
    {
        HostCtl[ index ].InterfaceNum = ( (PUSB_CFG_DESCR)Com_Buf )->bNumInterfaces;
    }

    /* Save device endpoint number */
    total_len = Com_Buf[ 2 ] | ((uint16_t)Com_Buf[ 3 ] << 8 );
    intfacenum = 0;
    for( i = 0; i < total_len; i++ )
    {
        if( ( Com_Buf[ i + 0 ] == 0x09 ) &&
            ( Com_Buf[ i + 1 ] == 0x04 ) &&
            ( Com_Buf[ i + 2 ] == intfacenum ) )
        {
            /* Analyze the current interface */
            innum = 0;
            outnum = 0;
            i += 9;
            for( j = 0; j < total_len - i; j++ )
            {
                if( ( Com_Buf[ i + j + 0 ] == 0x07 ) && ( Com_Buf[ i + j + 1 ] == 0x05 ) )
                {
                    if( ( Com_Buf[ i + j + 2 ] & 0x80 ) == 0x00 )
                    {
                        /* OUT */
                        if( outnum < 4 )
                        {
                            HostCtl[ index ].Interface[ intfacenum ].OutEndpAddr[ outnum ] = Com_Buf[ i + j + 2 ];
                            HostCtl[ index ].Interface[ intfacenum ].OutEndpType[ outnum ] = Com_Buf[ i + j + 3 ];
                            HostCtl[ index ].Interface[ intfacenum ].OutEndpSize[ outnum ] = Com_Buf[ i + j + 4 ] | ( (uint16_t)Com_Buf[ i + j + 5 ] << 8 );
                            HostCtl[ index ].Interface[ intfacenum ].OutEndpTog[ outnum ] = 0x00;
                            outnum++;
                            HostCtl[ index ].Interface[ intfacenum ].OutEndpNum = outnum;
                        }
                    }
                    else
                    {
                        /* IN */
                        if( Com_Buf[ i + j + 3 ] == 0x02 )
                        {
                            if( innum < 4 )
                            {
                                HostCtl[ index ].Interface[ intfacenum ].InEndpAddr[ innum ] = Com_Buf[ i + j + 2 ];
                                HostCtl[ index ].Interface[ intfacenum ].InEndpType[ innum ] = Com_Buf[ i + j + 3 ];
                                HostCtl[ index ].Interface[ intfacenum ].InEndpSize[ innum ] = Com_Buf[ i + j + 4 ] | ( (uint16_t)Com_Buf[ i + j + 5 ] << 8 );
                                HostCtl[ index ].Interface[ intfacenum ].InEndpInterval[ innum ] = Com_Buf[ i + j + 6 ];
                                HostCtl[ index ].Interface[ intfacenum ].InEndpTog[ innum ] = 0x00;
                                HostCtl[ index ].Interface[ intfacenum ].InEndpTimeCount[ innum ] = 0x00;
                                innum++;
                                HostCtl[ index ].Interface[ intfacenum ].InEndpNum = innum;
                            }
                        }
                    }
                }
                else if( ( Com_Buf[ i + j + 0 ] == 0x09 ) &&
                         ( Com_Buf[ i + j + 1 ] == 0x04 ) )
                {
                    intfacenum++;
                    break;
                }
            }
            if( intfacenum >= DEF_INTERFACE_NUM_MAX )
            {
                break;
            }
        }
    }
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
uint8_t USBH_EnumImageDevice( uint8_t usb_port, uint8_t index )
{
    uint8_t  s;
#if DEF_DEBUG_PRINTF
    uint8_t  i;
#endif

    DUG_PRINTF( "Enum Image:\r\n" );

    /* Analyze image class device configuration descriptor and save relevant parameters */
    DUG_PRINTF("Analyze CfgDesc: ");
    IMAGE_AnalyzeCfgDesc( index );

    /* Get the string descriptor contained in the configuration descriptor if it exists */
    if( Com_Buf[ 6 ] )
    {
        DUG_PRINTF("Get StringDesc4: ");
        s = USBH_GetStrDescr( usb_port, Com_Buf[ 6 ] );
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
        s = USBH_GetStrDescr( usb_port, DevDesc_Buf[ 14 ] );
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
        s = USBH_GetStrDescr( usb_port, DevDesc_Buf[ 15 ] );
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
        s = USBH_GetStrDescr( usb_port, DevDesc_Buf[ 16 ] );
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

    return ERR_SUCCESS;
}

/*********************************************************************
 * @fn      MTP_SendData
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t MTP_SendData( uint8_t usb_port, uint8_t *pbuf, uint32_t len )
{
    uint8_t  s;
    uint8_t  index;

    index = RootHubDev[ usb_port ].DeviceIndex;
    while( len )
    {
        if( usb_port == DEF_USBFS_PORT_INDEX )
        {
            if( len > HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ] )
            {
                USBOTG_H_FS->HOST_TX_LEN = HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ];
                memcpy( USBFS_TX_Buf, pbuf, HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ] );
            }
            else
            {
                USBOTG_H_FS->HOST_TX_LEN = len;
                memcpy( USBFS_TX_Buf, pbuf, len );
            }

            s = USBFSH_Transact( ( USB_PID_OUT << 4 ) | HostCtl[ index ].Interface[ 0 ].OutEndpAddr[ 0 ],
                                 HostCtl[ index ].Interface[ 0 ].OutEndpTog[ 0 ], 20000 );
            if( s == ERR_SUCCESS )
            {
                HostCtl[ index ].Interface[ 0 ].OutEndpTog[ 0 ] ^= USBFS_UH_T_TOG;

                if( len > HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ] )
                {
                    pbuf += HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ];
                    len -= HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ];
                }
                else
                {
                    break;
                }
            }
            else
            {
                return s;
            }
        }
        else if( usb_port == DEF_USBHS_PORT_INDEX )
        {
            if( len > HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ] )
            {
                USBHSH->HOST_TX_LEN = HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ];
                memcpy( USBHS_TX_Buf, pbuf, HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ] );
            }
            else
            {
                USBHSH->HOST_TX_LEN = len;
                memcpy( USBHS_TX_Buf, pbuf, len );
            }

            s = USBHSH_Transact( ( USB_PID_OUT << 4 ) | HostCtl[ index ].Interface[ 0 ].OutEndpAddr[ 0 ],
                                 HostCtl[ index ].Interface[ 0 ].OutEndpTog[ 0 ], 20000 );
            if( s == ERR_SUCCESS )
            {
                HostCtl[ index ].Interface[ 0 ].OutEndpTog[ 0 ] ^= USBHS_UH_T_TOG_DATA1;

                if( len > HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ] )
                {
                    pbuf += HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ];
                    len -= HostCtl[ index ].Interface[ 0 ].OutEndpSize[ 0 ];
                }
                else
                {
                    break;
                }
            }
            else
            {
                return s;
            }
        }
    }

    return ERR_SUCCESS;
}

/*********************************************************************
 * @fn      MTP_RecvData
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t MTP_RecvData( uint8_t usb_port, uint8_t *pbuf, uint32_t *plen )
{
    uint8_t  s = ERR_SUCCESS;
    uint8_t  index;

    index = RootHubDev[ usb_port ].DeviceIndex;
    if( usb_port == DEF_USBFS_PORT_INDEX )
    {
        s = USBFSH_Transact( ( USB_PID_IN << 4 ) | HostCtl[ index ].Interface[ 0 ].InEndpAddr[ 0 ],
                             HostCtl[ index ].Interface[ 0 ].InEndpTog[ 0 ], 20000 );
        if( s == ERR_SUCCESS )
        {
            HostCtl[ index ].Interface[ 0 ].InEndpTog[ 0 ] ^= USBFS_UH_R_TOG;

            *plen = USBOTG_H_FS->RX_LEN;
            memcpy( pbuf, USBFS_RX_Buf, *plen );
        }
    }
    else if( usb_port == DEF_USBHS_PORT_INDEX )
    {
        s = USBHSH_Transact( ( USB_PID_IN << 4 ) | HostCtl[ index ].Interface[ 0 ].InEndpAddr[ 0 ],
                             HostCtl[ index ].Interface[ 0 ].InEndpTog[ 0 ], 20000 );
        if( s == ERR_SUCCESS )
        {
            HostCtl[ index ].Interface[ 0 ].InEndpTog[ 0 ] ^= USBHS_UH_R_TOG_DATA1;

            *plen = USBHSH->RX_LEN;
            memcpy( pbuf, USBHS_RX_Buf, *plen );
        }
    }

    return s;
}

/*********************************************************************
 * @fn      MTP_PTP_ParaInit
 *
 * @brief
 *
 * @return  None
 */
void MTP_PTP_ParaInit( uint8_t usb_port )
{
    pMTP_Data_Buf = Com_Buf;
    PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf;
    PTP_Handle[ usb_port ].data_length = 0;
    PTP_Handle[ usb_port ].data_packet = HostCtl[ RootHubDev[ usb_port ].DeviceIndex ].Interface[ 0 ].InEndpSize[ 0 ];
}

/*********************************************************************
 * @fn      USBH_PTP_SendRequest
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_PTP_SendRequest( uint8_t usb_port )
{
    PTP_OpContainer[ usb_port ].length = PTP_USB_BULK_REQ_LEN - ( ( sizeof( uint32_t ) * ( 5 - (uint32_t)PTP_Container[ usb_port ].Nparam ) ) );
    if( PTP_Container[ usb_port ].Transaction_ID == 0XFFFFFFFF )
    {
        PTP_Container[ usb_port ].Transaction_ID = 0x00000001;
    }
    PTP_OpContainer[ usb_port ].type = PTP_USB_CONTAINER_COMMAND;
    PTP_OpContainer[ usb_port ].code = PTP_Container[ usb_port ].Code;
    PTP_OpContainer[ usb_port ].trans_id = PTP_Container[ usb_port ].Transaction_ID;
    PTP_OpContainer[ usb_port ].param1 = PTP_Container[ usb_port ].Param1;
    PTP_OpContainer[ usb_port ].param2 = PTP_Container[ usb_port ].Param2;
    PTP_OpContainer[ usb_port ].param3 = PTP_Container[ usb_port ].Param3;
    PTP_OpContainer[ usb_port ].param4 = PTP_Container[ usb_port ].Param4;
    PTP_OpContainer[ usb_port ].param5 = PTP_Container[ usb_port ].Param5;
    return MTP_SendData( usb_port, (uint8_t *)&PTP_OpContainer[ usb_port ], PTP_OpContainer[ usb_port ].length );
}

/*********************************************************************
 * @fn      USBH_PTP_GetResponse
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_PTP_GetResponse( uint8_t usb_port )
{
    uint8_t  s;
    uint32_t len;

    s = MTP_RecvData( usb_port, (uint8_t *)&PTP_RespContainer[ usb_port ], &len );
    if( s == ERR_SUCCESS )
    {
        if( PTP_RespContainer[ usb_port ].code == PTP_RC_OK )
        {
            s = PTP_OK;
        }
        else
        {
            s = PTP_FAIL;
        }
    }

    return s;
}

/*********************************************************************
 * @fn      USBH_PTP_OpenSession
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_PTP_OpenSession( uint8_t usb_port, uint32_t session )
{
    uint8_t  s;

    PTP_Container[ usb_port ].Transaction_ID = 0; // This command is the first command, so the transfer ID is 0.
    PTP_Container[ usb_port ].Param1 = session;
    PTP_Container[ usb_port ].Code = PTP_OC_OpenSession;
    PTP_Container[ usb_port ].Nparam = 1;

    s = USBH_PTP_SendRequest( usb_port );
    if( s != PTP_OK )
    {
        return s;
    }
    PTP_Container[ usb_port ].Transaction_ID++; // The transfer ID is incremented after the transfer is complete.
    return USBH_PTP_GetResponse( usb_port );
}

/*********************************************************************
 * @fn      USBH_PTP_GetDeviceInfo
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_PTP_GetDeviceInfo( uint8_t usb_port )
{
    uint8_t  s, flag;
    uint32_t len, temp;

    /* Command Phase */
    PTP_Container[ usb_port ].Code = PTP_OC_GetDeviceInfo;
    PTP_Container[ usb_port ].Nparam = 0;
    s = USBH_PTP_SendRequest( usb_port );
    if( s != PTP_OK )
    {
        return s;
    }
    PTP_Container[ usb_port ].Transaction_ID++;
    PTP_Handle[ usb_port ].data_length = 0;
    PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf;

    /* Data Phase */
    flag = 0;
    while( 1 )
    {
        s = MTP_RecvData( usb_port, PTP_Handle[ usb_port ].data_ptr, &len );
        if( s != PTP_OK )
        {
            return s;
        }

        if( flag == 0 ) // First Packet of Data
        {
            temp = LE32( PTP_Handle[ usb_port ].data_ptr );
            flag = 1;

            DUG_PRINTF( "temp: %04x\r\n", temp );
        }
        if( ( len < PTP_Handle[ usb_port ].data_packet ) || ( temp == len ) ) // End of File
        {
            PTP_Handle[ usb_port ].data_length += len;
            break;
        }
        PTP_Handle[ usb_port ].data_ptr += len;
        PTP_Handle[ usb_port ].data_length += len;
        temp -= len;
    }
    PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf + 12;

    /* Data Processing  */
    PTP_DeviceInfo[ usb_port ].StandardVersion = LE16( PTP_Handle[ usb_port ].data_ptr + PTP_di_StandardVersion );
    PTP_DeviceInfo[ usb_port ].VendorExtensionID = LE32( PTP_Handle[ usb_port ].data_ptr + PTP_di_VendorExtensionID );
    DUG_PRINTF( "StandardVersion: %02x, VendorExtensionID: %2x\r\n", PTP_DeviceInfo[ usb_port ].StandardVersion, PTP_DeviceInfo[ usb_port ].VendorExtensionID );

    /* Status Phase */
    return USBH_PTP_GetResponse( usb_port );
}

/*********************************************************************
 * @fn      USB_PTP_GetStorageIds
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_PTP_GetStorageIds( uint8_t usb_port )
{
    uint8_t  s, i, flag;
    uint32_t len, temp;

    /* Command Phase */
    PTP_Container[ usb_port ].Code = PTP_OC_GetStorageIDs;
    PTP_Container[ usb_port ].Nparam = 0;
    s = USBH_PTP_SendRequest( usb_port );
    if( s != PTP_OK )
    {
        return s;
    }
    PTP_Container[ usb_port ].Transaction_ID++;
    PTP_Handle[ usb_port ].data_length = 0;
    PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf;

    /* Data Phase */
    flag = 0;
    while( 1 )
    {
        s = MTP_RecvData( usb_port, PTP_Handle[ usb_port ].data_ptr, &len );
        if( s != PTP_OK )
        {
            return s;
        }

        if( flag == 0 ) // First packet of data
        {
            temp = LE32( PTP_Handle[ usb_port ].data_ptr );
            flag = 1;

            DUG_PRINTF( "temp: %02x\r\n", temp );
        }
        if( ( len < PTP_Handle[ usb_port ].data_packet ) || ( temp == len ) )
        {
            PTP_Handle[ usb_port ].data_length += len;
            break;
        }
        PTP_Handle[ usb_port ].data_ptr += len;
        PTP_Handle[ usb_port ].data_length += len;
        temp -= len;
    }

    PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf + 12;
    PTP_StorageIDs[ usb_port ].n = LE32( PTP_Handle[ usb_port ].data_ptr );
    PTP_Handle[ usb_port ].data_ptr += 4;
    DUG_PRINTF( "PTP_StorageIDs[ usb_port ].n: %02x\r\n", PTP_StorageIDs[ usb_port ].n );

    for( i = 0; i != PTP_StorageIDs[ usb_port ].n; i++ )
    {
        PTP_StorageIDs[ usb_port ].Storage[ i ] = LE32( PTP_Handle[ usb_port ].data_ptr + i * 4 );
    }

    /* Status Phase */
    return USBH_PTP_GetResponse( usb_port );
}

/*********************************************************************
 * @fn      PTP_GetString
 *
 * @brief
 *
 * @para
 *
 * @return
 */
void PTP_GetString( uint8_t *str, uint8_t *data, uint32_t *len )
{
    uint32_t i, strlength;

    *len = data[ 0 ];
    strlength =  2 * ( *len );
    data++; /* Adjust the offset ignoring the String Len */

    for( i = 0; i < strlength; i++ )
    {
        /* Copy Only the string and ignore the UNICODE ID, hence add the src */
        *str = data[ i ];
        str++;
    }
    *str = 0; /* mark end of string */
}

/*********************************************************************
 * @fn      USBH_PTP_GetDeviceInfo
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_PTP_GetStorageInfo( uint8_t usb_port, uint32_t storage_id )
{
    uint8_t  s, flag;
    uint32_t len, temp;

    /* Command Phase */
    PTP_Container[ usb_port ].Code = PTP_OC_GetStorageInfo;
    PTP_Container[ usb_port ].Param1 = storage_id; // dev_id.Storage[0];
    PTP_Container[ usb_port ].Nparam = 0x01;
    s = USBH_PTP_SendRequest( usb_port );
    if( s != PTP_OK )
    {
        return s;
    }
    PTP_Container[ usb_port ].Transaction_ID++;
    PTP_Handle[ usb_port ].data_length = 0;
    PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf;

    /* Data Phase */
    flag = 0;
    while( 1 )
    {
        s = MTP_RecvData( usb_port, PTP_Handle[ usb_port ].data_ptr, &len );
        if( s != PTP_OK )
        {
            return s;
        }

        if( flag == 0 ) // First packet of data
        {
            temp = LE32( PTP_Handle[ usb_port ].data_ptr );
            flag = 1;
        }
        if( ( len < PTP_Handle[ usb_port ].data_packet ) || ( temp == len ) )
        {
            PTP_Handle[ usb_port ].data_length += len;
            break;
        }
        PTP_Handle[ usb_port ].data_ptr += len;
        PTP_Handle[ usb_port ].data_length += len;
        temp -= len;
    }
    PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf + 12;
    PTP_StorageInfo[ usb_port ].StorageType = LE16( PTP_Handle[ usb_port ].data_ptr + PTP_si_StorageType );
    PTP_StorageInfo[ usb_port ].FilesystemType = LE16( PTP_Handle[ usb_port ].data_ptr + PTP_si_FilesystemType );
    PTP_StorageInfo[ usb_port ].AccessCapability = LE16( PTP_Handle[ usb_port ].data_ptr + PTP_si_AccessCapability );
    PTP_StorageInfo[ usb_port ].MaxCapability = LE64( PTP_Handle[ usb_port ].data_ptr + PTP_si_MaxCapability );
    PTP_StorageInfo[ usb_port ].FreeSpaceInBytes = LE64( PTP_Handle[ usb_port ].data_ptr + PTP_si_FreeSpaceInBytes );
    PTP_StorageInfo[ usb_port ].FreeSpaceInImages = LE32( PTP_Handle[ usb_port ].data_ptr + PTP_si_FreeSpaceInImages );
    PTP_GetString( PTP_StorageInfo[ usb_port ].StorageDescription, (uint8_t *)( &PTP_Handle[ usb_port ].data_ptr + PTP_si_StorageDescription ), &len );
    PTP_GetString( PTP_StorageInfo[ usb_port ].VolumeLabel, (uint8_t *)( &PTP_Handle[ usb_port ].data_ptr + ( PTP_si_StorageDescription + ( len * 2 ) + 1 ) ), &len );

    /* Status Phase */
    return USBH_PTP_GetResponse( usb_port );
}

/*********************************************************************
 * @fn      USBH_PTP_GetNumObjects
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_PTP_GetNumObjects( uint8_t usb_port, uint32_t storage_id, uint32_t objectformatcode, uint32_t objecthandle, uint32_t *numobs )
{
    uint8_t  s;

    PTP_Container[ usb_port ].Code = PTP_OC_GetNumObjects;
    PTP_Container[ usb_port ].Param1 = storage_id;
    PTP_Container[ usb_port ].Param2 = objectformatcode;
    PTP_Container[ usb_port ].Param3 = objecthandle;
    PTP_Container[ usb_port ].Nparam = 3;
    s = USBH_PTP_SendRequest( usb_port );
    if( s != PTP_OK )
    {
        return s;
    }

    PTP_Container[ usb_port ].Transaction_ID++; // Increment the transfer ID after the transfer is complete
    s = USBH_PTP_GetResponse( usb_port );
    if( s == PTP_OK )
    {
        *numobs = PTP_RespContainer[ usb_port ].param1; // The number of this file type
    }

    return s;
}

/*********************************************************************
 * @fn      USBH_PTP_GetNumObjects
 *
 * @brief
 *
 * @para
 *
 * @return
 */
void ObjectHandles_Process( uint8_t usb_port, uint8_t *pdata, uint32_t len )
{
    uint32_t i;

    if( MTP_Para[ usb_port ].mtp_handle.flag )
    {
        return;
    }

    for( i = 0; i != len / 4; i++ )
    {
        if( MTP_Para[ usb_port ].mtp_handle.Handle_cur_num == MTP_Para[ usb_port ].mtp_handle.Handle_locate_num )
        {
            MTP_Para[ usb_port ].mtp_handle.Handle_num = LE32( pdata );
            MTP_Para[ usb_port ].mtp_handle.flag = 1;
        }
        MTP_Para[ usb_port ].mtp_handle.Handle_cur_num++;
        pdata += 4;
    }
}

/*********************************************************************
 * @fn      USBH_MTP_GetObjectHandles
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_MTP_GetObjectHandles( uint8_t usb_port, uint32_t storage_id, uint32_t objectformatcode, uint32_t objecthandle )
{
    uint8_t  s, flag;
    uint32_t len, temp;

    /* Command Phase */
    PTP_Container[ usb_port ].Code = PTP_OC_GetObjectHandles;
    PTP_Container[ usb_port ].Param1 = storage_id;
    PTP_Container[ usb_port ].Param2 = objectformatcode;
    PTP_Container[ usb_port ].Param3 = objecthandle;
    PTP_Container[ usb_port ].Nparam = 3;
    s = USBH_PTP_SendRequest( usb_port );
    if( s != PTP_OK )
    {
        return s;
    }

    /* Data Phase */
    PTP_Container[ usb_port ].Transaction_ID++; // Increment the transfer ID after the transfer is complete
    PTP_Handle[ usb_port ].data_length = 0;
    PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf;
    flag = 0;
    while( 1 )
    {
        s = MTP_RecvData( usb_port, PTP_Handle[ usb_port ].data_ptr, &len );
        if( s != PTP_OK )
        {
            return s;
        }

        if( flag == 0 )
        {
            temp = LE32( PTP_Handle[ usb_port ].data_ptr );
            flag = 1;
            MTP_Para[ usb_port ].mtp_handle.Handle_total_num = LE32( PTP_Handle[ usb_port ].data_ptr + 12 ); // Total
            ObjectHandles_Process( usb_port, PTP_Handle[ usb_port ].data_ptr + 16, len - 16 );
        }
        else
        {
            ObjectHandles_Process( usb_port, PTP_Handle[ usb_port ].data_ptr, len );
        }
        if( ( len < PTP_Handle[ usb_port ].data_packet ) || ( temp == len ) )
        {
            PTP_Handle[ usb_port ].data_length += len;
            break;
        }
        PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf;
        PTP_Handle[ usb_port ].data_length += len;
        temp -= len;
    }
    return USBH_PTP_GetResponse( usb_port );
}

/*********************************************************************
 * @fn      USBH_MTP_GetObjectInfo
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_MTP_GetObjectInfo( uint8_t usb_port, uint32_t objecthandle )
{
    uint8_t  s, flag;
    uint32_t len,temp;

    PTP_Container[ usb_port ].Code = PTP_OC_GetObjectInfo;
    PTP_Container[ usb_port ].Param1 = objecthandle;
    PTP_Container[ usb_port ].Nparam = 1;
    s = USBH_PTP_SendRequest( usb_port );
    if( s != PTP_OK )
    {
        return s;
    }

    PTP_Container[ usb_port ].Transaction_ID++;
    PTP_Handle[ usb_port ].data_length = 0;
    PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf;
    flag = 0;
    while( 1 )
    {
        s = MTP_RecvData( usb_port, PTP_Handle[ usb_port ].data_ptr, &len );
        if( s != PTP_OK )
        {
            return s;
        }

        if( flag == 0 )
        {
            temp = LE32( PTP_Handle[ usb_port ].data_ptr );
            flag = 1;
        }
        if( ( len < PTP_Handle[ usb_port ].data_packet) || ( temp == len ) )
        {
            PTP_Handle[ usb_port ].data_length += len;
            PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf + 12;
            PTP_ObjectInfo[ usb_port ].ObjectFormat = LE16( PTP_Handle[ usb_port ].data_ptr + PTP_oi_ObjectFormat );

            /* For Samsung Galaxy */
            if( ( *( PTP_Handle[ usb_port ].data_ptr + PTP_oi_filenamelen ) == 0 ) && ( *( PTP_Handle[ usb_port ].data_ptr + PTP_oi_filenamelen + 4 ) != 0 ) )
            {
                PTP_Handle[ usb_port ].data_ptr += 4;
            }
            PTP_GetString( PTP_ObjectInfo[ usb_port ].Filename, PTP_Handle[ usb_port ].data_ptr + PTP_oi_filenamelen, &len );
            PTP_ObjectInfo[ usb_port ].FileName_len = len << 1;
            temp = len + PTP_oi_Filename;
            PTP_GetString( PTP_ObjectInfo[ usb_port ].Data_Creat_Time, PTP_Handle[ usb_port ].data_ptr + temp, &len );
            PTP_ObjectInfo[ usb_port ].Data_Creat_len = len << 1;
            temp += len + 1;
            PTP_GetString( PTP_ObjectInfo[ usb_port ].Data_Modified_Time, PTP_Handle[ usb_port ].data_ptr + temp, &len );
            PTP_ObjectInfo[ usb_port ].Data_Modified_len = len << 1;
            break;
        }
        PTP_Handle[ usb_port ].data_ptr += len;
        PTP_Handle[ usb_port ].data_length += len;
        temp -= len;
    }

    return USBH_PTP_GetResponse( usb_port );
}

/*********************************************************************
 * @fn      PTP_GetDevicePropValue
 *
 * @brief
 *
 * @para
 *
 * @return
 */
void PTP_GetDevicePropValue( uint8_t usb_port, uint8_t *data, uint32_t *offset, uint16_t datatype )
{
    uint32_t len;

    switch( datatype )
    {
        case PTP_DTC_INT8:
            PTP_PropertyValue[ usb_port ].i8 = *(int8_t *)(void *) & ( data[ *offset ] );
            *offset += 1;
            break;
        case PTP_DTC_UINT8:
            PTP_PropertyValue[ usb_port ].u8 = *(uint8_t *) & ( data[ *offset ] );
            *offset += 1;
            break;
        case PTP_DTC_INT16:
            PTP_PropertyValue[ usb_port ].i16 = *(int16_t *)(void *) & ( data[ *offset ] );
            *offset += 2;
            break;
        case PTP_DTC_UINT16:
            PTP_PropertyValue[ usb_port ].u16 = LE16( &( data[ *offset ] ) );
            *offset += 2;
            break;
        case PTP_DTC_INT32:
            PTP_PropertyValue[ usb_port ].i32 = *(int32_t *)(void *)( &( data[ *offset ] ) );
            *offset += 4;
            break;
        case PTP_DTC_UINT32:
            PTP_PropertyValue[ usb_port ].u32 = LE32( &( data[ *offset ] ) );
            *offset += 4;
            break;
        case PTP_DTC_INT64:
            PTP_PropertyValue[ usb_port ].i64 = *(int64_t *)(void *)( &( data[ *offset ] ) );
            *offset += 8;
            break;
        case PTP_DTC_UINT64:
            PTP_PropertyValue[ usb_port ].u64 = LE64( &( data[ *offset ] ) );
            *offset += 8;
            break;
        case PTP_DTC_UINT128:
            *offset += 16;
            break;
        case PTP_DTC_INT128:
            *offset += 16;
            break;
        case PTP_DTC_STR:
            PTP_GetString( (uint8_t *)(void *)PTP_PropertyValue[ usb_port ].str, (uint8_t *) & ( data[ *offset ] ), &len );
            *offset += (uint32_t)( len * 2 ) + 1;
            break;
        default:
            break;
  }
}

/*********************************************************************
 * @fn      PTP_GetObjectPropList
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint32_t PTP_GetObjectPropList( uint8_t usb_port, uint8_t *data, uint32_t len, uint16_t propval )
{
    uint32_t prop_count;
    uint32_t offset = 0, i;
    uint16_t datatype, propval1;

    prop_count = LE32( data );
    if( prop_count == 0 )
    {
        return 0;
    }
    data += sizeof( uint32_t );
    len -= sizeof( uint32_t );

    for( i = 0; i < prop_count; i++ )
    {
        if( len <= 0 )
        {
          return 0;
        }

        data += sizeof( uint32_t );
        len -= sizeof( uint32_t );

        propval1 = LE16( data );
        data += sizeof( uint16_t );
        len -= sizeof( uint16_t );

        datatype = LE16( data );

        data += sizeof( uint16_t );
        len -= sizeof( uint16_t );

        offset = 0;

        PTP_GetDevicePropValue( usb_port, data, &offset, datatype );

        data += offset;
        len -= offset;
        if( propval1 == propval )
        {
            break;
        }
    }

    return prop_count;
}

/*********************************************************************
 * @fn      USBH_MTP_GetObjectSize
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_MTP_GetObjectSize( uint8_t usb_port, uint32_t objecthandle )
{
    uint8_t  s, flag;
    uint32_t len, temp;

    PTP_Container[ usb_port ].Code = PTP_OC_GetObjPropList;
    PTP_Container[ usb_port ].Param1 = objecthandle;
    PTP_Container[ usb_port ].Param2 = 0x00000000;
    PTP_Container[ usb_port ].Param3 = PTP_OPC_ObjectSize;
    PTP_Container[ usb_port ].Param4 = 0x00000000;
    PTP_Container[ usb_port ].Param5 = 0x00000000;
    PTP_Container[ usb_port ].Nparam = 5;
    s = USBH_PTP_SendRequest( usb_port );
    if( s != PTP_OK )
    {
        return s;
    }

    PTP_Container[ usb_port ].Transaction_ID++;
    PTP_Handle[ usb_port ].data_length = 0;
    PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf;
    flag = 0;
    while( 1 )
    {
        s = MTP_RecvData( usb_port, PTP_Handle[ usb_port ].data_ptr, &len );
        if( s != PTP_OK )
        {
            return s;
        }

        if( flag == 0 )
        {
            temp = LE32( PTP_Handle[ usb_port ].data_ptr );
            flag = 1;
        }
        if( ( len < PTP_Handle[ usb_port ].data_packet) || ( temp == len ) )
        {
            PTP_Handle[ usb_port ].data_length += len;
            PTP_Handle[ usb_port ].data_ptr = pMTP_Data_Buf + 12;
            PTP_GetObjectPropList( usb_port, PTP_Handle[ usb_port ].data_ptr, PTP_Handle[ usb_port ].data_length - 12, PTP_OPC_ObjectSize );
            PTP_ObjectInfo[ usb_port ].File_Size = PTP_PropertyValue[ usb_port ].u64;
            break;
        }
        PTP_Handle[ usb_port ].data_ptr += len;
        PTP_Handle[ usb_port ].data_length += len;
        temp -= len;
    }
    return USBH_PTP_GetResponse( usb_port );
}

/*********************************************************************
 * @fn      USBH_PTP_GetPartialObject
 *
 * @brief
 *
 * @para
 *
 * @return
 */
uint8_t USBH_PTP_GetPartialObject( uint8_t usb_port )
{
    uint8_t  s, flag;
    uint32_t len, temp;

    PTP_Container[ usb_port ].Code = PTP_OC_GetPartialObject;
    PTP_Container[ usb_port ].Param1 = MTP_Para[ usb_port ].get_data.Handle_num;
    PTP_Container[ usb_port ].Param2 = MTP_Para[ usb_port ].get_data.Handle_locate;
    PTP_Container[ usb_port ].Param3 = MTP_Para[ usb_port ].get_data.Handle_maxlen;
    PTP_Container[ usb_port ].Nparam = 3;
    s = USBH_PTP_SendRequest( usb_port );
    if( s != PTP_OK )
    {
        return s;
    }

    PTP_Container[ usb_port ].Transaction_ID++;
    PTP_Handle[ usb_port ].data_length = 0;
    flag = 0;
    while( 1 )
    {
        s = MTP_RecvData( usb_port, MTP_Para[ usb_port ].get_data.data_str.pdata, &len );
        if( s != PTP_OK )
        {
            return s;
        }

        if( flag == 0 )
        {
            temp = LE32( MTP_Para[ usb_port ].get_data.data_str.pdata );
            flag = 1;
        }
        if( ( len < PTP_Handle[ usb_port ].data_packet) || ( temp == len ) )
        {
            PTP_Handle[ usb_port ].data_length += len;
            MTP_Para[ usb_port ].get_data.Handle_maxlen = PTP_Handle[ usb_port ].data_length - 12; // Actual file size
            break;
        }
        MTP_Para[ usb_port ].get_data.data_str.pdata += len;
        PTP_Handle[ usb_port ].data_length += len;
        temp -= len;
    }
    return USBH_PTP_GetResponse( usb_port );
}

/*********************************************************************
 * @fn      MTP_Init_Process
 *
 * @brief
 *
 * @para
 *
 * @return
 */
void MTP_Init_Process( uint8_t usb_port )
{
    uint32_t i, total_num, s;

    MTP_PTP_ParaInit( usb_port );
    USBH_PTP_OpenSession( usb_port, 0x01 );
    USBH_PTP_GetDeviceInfo( usb_port );
    USBH_PTP_GetStorageIds( usb_port ); // Get how many MTP devices there are
    if( PTP_StorageIDs[ usb_port ].n ) // Indicates that the device is mounted
    {
        USBH_PTP_GetStorageInfo( usb_port, PTP_StorageIDs[ usb_port ].Storage[ 0 ] ); // Get disk capacity, including remaining capacity
        DUG_PRINTF( "MaxCapability = %08x, %08x\r\n", (uint32_t)PTP_StorageInfo[ usb_port ].MaxCapability,
                                                    (uint32_t)( PTP_StorageInfo[ usb_port ].MaxCapability >> 32 ) );
        DUG_PRINTF( "FreeSpaceInBytes = %08x, %08x\r\n", (uint32_t)PTP_StorageInfo[ usb_port ].FreeSpaceInBytes,
                                                       (uint32_t)( PTP_StorageInfo[ usb_port ].FreeSpaceInBytes >> 32 ) );

        /* Enumerate all files under the root directory */
        USBH_PTP_GetNumObjects( usb_port, PTP_StorageIDs[ usb_port ].Storage[ 0 ], 0x00000000, 0xFFFFFFFF, &total_num ); // Get the number of files under the root directory
        DUG_PRINTF( " total_num = %02x\r\n", total_num );
        for( i = 0; i != total_num; i++ )
        {
            MTP_Para[ usb_port ].mtp_handle.Handle_cur_num = 0;
            MTP_Para[ usb_port ].mtp_handle.Handle_locate_num = i;
            MTP_Para[ usb_port ].mtp_handle.flag = 0;
            USBH_MTP_GetObjectHandles( usb_port, PTP_StorageIDs[ usb_port ].Storage[ 0 ], 0x00000000, 0xFFFFFFFF );
            DUG_PRINTF( "handle = %08x\r\n", MTP_Para[ usb_port ].mtp_handle.Handle_num );
            USBH_MTP_GetObjectInfo( usb_port, MTP_Para[ usb_port ].mtp_handle.Handle_num );
            DUG_PRINTF( "obj_info.ObjectFormat = %02x\r\n", PTP_ObjectInfo[ usb_port ].ObjectFormat );
            DUG_PRINTF( "obj_info.FileName:" );
            for( s = 0; s != PTP_ObjectInfo[ usb_port ].FileName_len; s++ )
            {
                DUG_PRINTF( "%02x ", PTP_ObjectInfo[ usb_port ].Filename[ s ] );
            }
            DUG_PRINTF( "\r\n" );
            DUG_PRINTF( "obj_info.Data_Creat:" );
            for( s = 0; s != PTP_ObjectInfo[ usb_port ].FileName_len; s++ )
            {
                DUG_PRINTF( "%02x ", PTP_ObjectInfo[ usb_port ].Data_Creat_Time[ s ] );
            }
            DUG_PRINTF( "\r\n" );
            DUG_PRINTF( "obj_info.Data_Modified:" );
            for( s = 0; s != PTP_ObjectInfo[ usb_port ].FileName_len; s++ )
            {
                DUG_PRINTF( "%02x ", PTP_ObjectInfo[ usb_port ].Data_Modified_Time[ s ] );
            }
            DUG_PRINTF( "\r\n" );

            USBH_MTP_GetObjectSize( usb_port, MTP_Para[ usb_port ].mtp_handle.Handle_num );
            DUG_PRINTF( "obj_info.File_Size = %08lx\r\n", (uint32_t)PTP_ObjectInfo[ usb_port ].File_Size );
        }

        /* The following demonstrates how to read data */
        MTP_Para[ usb_port ].get_data.Handle_locate = 0;
        MTP_Para[ usb_port ].get_data.Handle_maxlen = 0;
        while( 1 )
        {
            MTP_Para[ usb_port ].get_data.Handle_num = 0x16; // Handle number
            MTP_Para[ usb_port ].get_data.Handle_locate += MTP_Para[ usb_port ].get_data.Handle_maxlen; // Locate
            MTP_Para[ usb_port ].get_data.Handle_maxlen = 8192; // File size to read
            MTP_Para[ usb_port ].get_data.data_str.pdata = Com_Buf; // Read data, the data starts from the Com_Buf[12] position, and the front is the packet header
            s = USBH_PTP_GetPartialObject( usb_port );
            DUG_PRINTF( "s = %02x\r\n", s );
            DUG_PRINTF( "MTP_Para[ %02x ].get_data.Handle_maxlen=%d\r\n", usb_port, MTP_Para[ usb_port ].get_data.Handle_maxlen );
            if( MTP_Para[ usb_port ].get_data.Handle_maxlen < 8192 )
            {
                break;
            }
       }
       DUG_PRINTF( "end\r\n" );
    }
}

/*********************************************************************
 * @fn      USBH_MainDeal
 *
 * @brief   Provide a simple enumeration process for USB devices and
 *          xxxxxxxxxxx.
 *
 * @return  none
 */
void USBH_MainDeal( void )
{
    uint8_t  s;
    uint8_t  usb_port;
    uint8_t  index;

    for( usb_port = 0; usb_port < DEF_TOTAL_ROOT_HUB; usb_port++ )
    {
        s = USBH_CheckRootHubPortStatus( usb_port ); // Check USB device connection or disconnection
        if( s == ROOT_DEV_CONNECTED )
        {
            DUG_PRINTF( "USB Port%x Dev In.\r\n", usb_port );

            /* Set root device state parameters */
            RootHubDev[ usb_port ].bStatus = ROOT_DEV_CONNECTED;
            RootHubDev[ usb_port ].DeviceIndex = usb_port * DEF_ONE_USB_SUP_DEV_TOTAL;

            /* Enumerate root device */
            s = USBH_EnumRootDevice( usb_port );
            if( s == ERR_SUCCESS )
            {
                if( RootHubDev[ usb_port ].bType == USB_DEV_CLASS_IMAGE ) // Further enumerate
                {
                    DUG_PRINTF("Root Device Is Image. ");

                    s = USBH_EnumImageDevice( usb_port, RootHubDev[ usb_port ].DeviceIndex );
                    DUG_PRINTF( "Further Enum Result: " );
                    if( s == ERR_SUCCESS )
                    {
                        DUG_PRINTF( "OK\r\n" );

                        /* Set the connection status of the device  */
                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_SUCCESS;

                        MTP_Init_Process( usb_port );
                    }
                    else if( s != ERR_USB_DISCON )
                    {
                        DUG_PRINTF( "Err(%02x)\r\n", s );

                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED;
                    }
                }
                else // Detect that this device is a non-image device
                {
                    DUG_PRINTF( "Root Device Is " );
                    switch( RootHubDev[ usb_port ].bType )
                    {
                        case USB_DEV_CLASS_HID:
                            DUG_PRINTF("Hid. ");
                            break;
                        case USB_DEV_CLASS_STORAGE:
                            DUG_PRINTF("Storage. ");
                            break;
                        case USB_DEV_CLASS_PRINTER:
                            DUG_PRINTF("Printer. ");
                            break;
                        case USB_DEV_CLASS_HUB:
                            DUG_PRINTF("Hub. ");
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
}
