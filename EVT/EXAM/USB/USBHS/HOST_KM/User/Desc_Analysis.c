/********************************** (C) COPYRIGHT *******************************
* File Name          : Desc_Analysis.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2022/04/21
* Description        : USB Descriptor Analysis functions
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "Desc_Analysis.h"
#include "ch32v30x_usbhs_host.h"

TPF_HOSTCTL HostCtl[ DEF_HOSTCTR_STRUCT_MAX ];

/*******************************************************************************
* Function Name  : USBH_Analyse_CfgDesc
* Description    : USB主机下的设备配置描述符分析
* Input          : index: 设备索引号
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 USBH_Analyse_CfgDesc( UINT8 index )
{
    UINT8  intfacenum, innum, outnum;
    UINT16 i, j;
    UINT16 total_len;

    /* 保存当前USB设备的接口数 */
    if( ( (PUSB_CFG_DESCR)( &ConfigDescriptor[ 0 ] ) )->bNumInterfaces > DEF_INTERFACE_NUM_MAX )
    {
        HostCtl[ index ].InterfaceNum = DEF_INTERFACE_NUM_MAX;
    }
    else
    {
        HostCtl[ index ].InterfaceNum = ( (PUSB_CFG_DESCR)( &ConfigDescriptor[ 0 ] ) )->bNumInterfaces;
    }

    /* 保存设备端点号 */
    total_len = ConfigDescriptor[ 2 ] + ((UINT16)ConfigDescriptor[ 3 ] << 8 );
    intfacenum = 0;
    for( i = 0; i < total_len; i++ )
    {
        if( ( ConfigDescriptor[ i + 0 ] == 0x09 ) &&
            ( ConfigDescriptor[ i + 1 ] == 0x04 ) &&
            ( ConfigDescriptor[ i + 2 ] == intfacenum ) )
        {
            /* 分析当前接口 */
            innum = 0;
            outnum = 0;
            i += 9;
            for( j = 0; j < total_len - i; j++ )
            {
                if( ( ConfigDescriptor[ i + j + 0 ] == 0x09 ) &&
                    ( ConfigDescriptor[ i + j + 1 ] == 0x21 ) &&
                    ( ConfigDescriptor[ i + j + 6 ] == 0x22 ) )
                {
                    HostCtl[ index ].Interface[ intfacenum ].HidDescLen = ConfigDescriptor[ i + j + 7 ] + ((UINT16)ConfigDescriptor[ i + j + 8 ] << 8 );
                }
                else if( ( ConfigDescriptor[ i + j + 0 ] == 0x07 ) &&
                         ( ConfigDescriptor[ i + j + 1 ] == 0x05 ) )
                {
                    if( ( ConfigDescriptor[ i + j + 2 ] & 0x80 ) == 0x00 )
                    {
                        /* OUT端点 */
                        if( outnum < DEF_ENDP_NUM_MAX )
                        {
                            HostCtl[ index ].Interface[ intfacenum ].OutEndpAddr[ outnum ] = ConfigDescriptor[ i + j + 2 ];
                            HostCtl[ index ].Interface[ intfacenum ].OutEndpType[ outnum ] = ConfigDescriptor[ i + j + 3 ];
                            HostCtl[ index ].Interface[ intfacenum ].OutEndpSize[ outnum ] = ConfigDescriptor[ i + j + 4 ];
                            HostCtl[ index ].Interface[ intfacenum ].OutEndpTog[ outnum ] = 0x00;
                            outnum++;
                            HostCtl[ index ].Interface[ intfacenum ].OutEndpNum = outnum;
                        }
                    }
                    else
                    {
                        /* IN端点 */
                        if( innum < DEF_ENDP_NUM_MAX )
                        {
                            HostCtl[ index ].Interface[ intfacenum ].InEndpAddr[ innum ] = ConfigDescriptor[ i + j + 2 ];
                            HostCtl[ index ].Interface[ intfacenum ].InEndpType[ innum ] = ConfigDescriptor[ i + j + 3 ];
                            HostCtl[ index ].Interface[ intfacenum ].InEndpSize[ innum ] = ConfigDescriptor[ i + j + 4 ];
                            HostCtl[ index ].Interface[ intfacenum ].InEndpInterval[ innum ] = ConfigDescriptor[ i + j + 6 ];
                            if(  HostCtl[ index ].Interface[ intfacenum ].InEndpInterval[ innum ] == 0 )
                            {
                                 HostCtl[ index ].Interface[ intfacenum ].InEndpInterval[ innum ] = 0;
                            }
                            HostCtl[ index ].Interface[ intfacenum ].InEndpTog[ innum ] = 0x00;
                            HostCtl[ index ].Interface[ intfacenum ].InEndpTimeCount[ innum ] = 0x00;
                            innum++;
                            HostCtl[ index ].Interface[ intfacenum ].InEndpNum = innum;
                        }
                    }
                }
                else if( ( ConfigDescriptor[ i + j + 0 ] == 0x09 ) &&
                         ( ConfigDescriptor[ i + j + 1 ] == 0x04 ) )
                {
                    intfacenum++;
                    break;
                }
            }
            if( intfacenum >= 4 )
            {
                break;
            }
        }
    }
    return( 0x00 );
}

/*******************************************************************************
* Function Name  : KM_Analyse_ConfigDesc
* Description    : USB键盘鼠标配置描述符分析
* Input          : index: 设备索引号
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 KM_Analyse_ConfigDesc( UINT8 index )
{
    UINT8  s = 0;
    UINT16 i;
    UINT8  num;
    UINT8  count = 0;

    num = 0;
    for( i = 0; i < ConfigDescriptor[ 2 ]; )
    {
        if( ConfigDescriptor[ i + 1 ] == DEF_DECR_CONFIG )
        {
            /* 保存当前USB设备的接口数 */
            if( ( (PUSB_CFG_DESCR)( &ConfigDescriptor[ i ] ) )->bNumInterfaces > DEF_INTERFACE_NUM_MAX )
            {
                HostCtl[index].InterfaceNum = DEF_INTERFACE_NUM_MAX;
            }
            else
            {
                HostCtl[index].InterfaceNum = ( (PUSB_CFG_DESCR)( &ConfigDescriptor[ i ] ) )->bNumInterfaces;
            }
            i += ConfigDescriptor[ i ];
        }
        else if( ConfigDescriptor[ i + 1 ] == DEF_DECR_INTERFACE )
        {
            if( num == DEF_INTERFACE_NUM_MAX )
            {
                return( s );
            }
            if( ( (PUSB_ITF_DESCR)( &ConfigDescriptor[ i ] ) )->bInterfaceClass == USB_DEV_CLASS_HID )
            {
                HostCtl[index].DeviceType = USB_DEV_CLASS_HID;
                printf( "Device TYP %02x\n", HostCtl[index].DeviceType );
                /* HID类设备(如USB键盘鼠标) */
                if( ( (PUSB_ITF_DESCR)( &ConfigDescriptor[ i ] ) )->bInterfaceSubClass <= 0x01 &&
                    ( (PUSB_ITF_DESCR)( &ConfigDescriptor[ i ] ) )->bInterfaceProtocol <= 2 )
                {
                    if( ( (PUSB_ITF_DESCR)( &ConfigDescriptor[ i ] ) )->bInterfaceProtocol == 0x01 )
                    {
                        /* 设置当前接口为USB键盘 */
                        HostCtl[ index ].Interface[ num ].Type = DEC_KEY;
                        HostCtl[ index ].DeviceType = 1;
                    }
                    else if( ( (PUSB_ITF_DESCR)( &ConfigDescriptor[ i ] ) )->bInterfaceProtocol == 0x02 )
                    {
                        /* 设置当前接口为USB鼠标 */
                        HostCtl[ index ].Interface[ num ].Type = DEC_MOUSE;
                    }
                    else if( ( (PUSB_ITF_DESCR)( &ConfigDescriptor[ i ] ) )->bInterfaceProtocol == 0x00 )
                    {
                        /* 设置当前接口为USB键盘多媒体 */
                        HostCtl[ index ].Interface[ num ].Type = DEC_MEDIA;
                    }

                    s = DEC_SUCCESS;
                    i += ConfigDescriptor[ i ];
                    count = 0;
                    while( 1 )
                    {
                        if( ( ConfigDescriptor[ i + 1 ] == DEF_DECR_INTERFACE ) || ( i >= ConfigDescriptor[ 2 ] ) )
                        {
                            break;
                        }
                        else
                        {
                            /* 分析当前接口下的各个端点 */
                            if( ConfigDescriptor[ i + 1 ] == DEF_DECR_ENDPOINT )
                            {
                                /* 保存端点相关信息(端点地址、轮询间隔时间) */
                                if( ( (PUSB_ENDP_DESCR)( &ConfigDescriptor[ i ] ) )->bEndpointAddress & 0x80 )
                                {
                                    /* 输入端点 */
                                    HostCtl[ index ].Interface[ num ].InEndpAddr[ count ] = ( (PUSB_ENDP_DESCR)( &ConfigDescriptor[ i ] ) )->bEndpointAddress & 0x0f;
                                    HostCtl[ index ].Interface[ num ].InEndpType[ count ] = ( (PUSB_ENDP_DESCR)( &ConfigDescriptor[ i ] ) )->bmAttributes;
                                    HostCtl[ index ].Interface[ num ].InEndpSize[ count ] = (uint8_t)( ( (PUSB_ENDP_DESCR)( &ConfigDescriptor[ i ] ) )->wMaxPacketSize );
                                    HostCtl[ index ].Interface[ num ].InEndpNum++;
                                }
                                else
                                {
                                    /* 输出端点 */
                                    HostCtl[ index ].Interface[ num ].OutEndpAddr[ count ] = ( (PUSB_ENDP_DESCR)( &ConfigDescriptor[ i ] ) )->bEndpointAddress & 0x0f;
                                    HostCtl[ index ].Interface[ num ].OutEndpType[ count ] = ( (PUSB_ENDP_DESCR)( &ConfigDescriptor[ i ] ) )->bmAttributes;
                                    HostCtl[ index ].Interface[ num ].OutEndpSize[ count ] = (uint8_t)( ( (PUSB_ENDP_DESCR)( &ConfigDescriptor[ i ] ) )->wMaxPacketSize );
                                    HostCtl[ index ].Interface[ num ].OutEndpNum++;
                                }

                                HostCtl[ index ].Interface[ num ].InEndpInterval[ count ] = ( (PUSB_ENDP_DESCR)( &ConfigDescriptor[ i ] ) )->bInterval;
#if 0
                                /* 限制端点传输时间 */
                                if( HostCtl[ index ].Interface[ num ].InEndpInterval[ count] < 4 )
                                {
                                    HostCtl[ index ].Interface[ num ].InEndpInterval[ count ] = 2;
                                }
                                else if( HostCtl[ index ].Interface[ num ].InEndpInterval[ count ] < 8 )
                                {
                                    HostCtl[ index ].Interface[ num ].InEndpInterval[ count ] = 4;
                                }
                                else if( HostCtl[ index ].Interface[ num ].InEndpInterval[ count ] < 16 )
                                {
                                    HostCtl[index].Interface[ num ].InEndpInterval[ count ] = 8;
                                }
                                else if( HostCtl[ index ].Interface[ num ].InEndpInterval[ count ] == 0 )
                                {
                                    HostCtl[ index ].Interface[ num ].InEndpInterval[ count ] = 0;
                                }
#endif
                                count++;
                                i += ConfigDescriptor[ i ];
                            }
                            else if( ConfigDescriptor[ i + 1 ] == DEF_DECR_HID )
                            {
                                /* 保存当前接口HID报表描述符长度 */
                                HostCtl[ index ].Interface[ num ].HidDescLen = ( (PUSB_HID_DESCR)( &ConfigDescriptor[ i ] ) )->wDescriptorLengthL | \
                                                                               ((UINT16)( ( (PUSB_HID_DESCR)( &ConfigDescriptor[ i ] ) )->wDescriptorLengthH ) << 8 );   /* hid len */
                                i += ConfigDescriptor[ i ];
                            }
                            else
                            {
                                i += ConfigDescriptor[ i ];
                            }
                        }
                    }
                }
                else
                {
                    HostCtl[ index ].Interface[ num ].Type = DEC_UNKNOW;
                    i += ConfigDescriptor[ i ];
                }
            }
            else if( ( (PUSB_ITF_DESCR)( &ConfigDescriptor[ i ] ) )->bInterfaceClass == USB_DEV_CLASS_MASSSTORE )
            {
                HostCtl[index].DeviceType = USB_DEV_CLASS_MASSSTORE;
                /* 大容量存储类设备(如U盘) */
                s = DEC_SUCCESS;
                /* 按照标准USB设备分析 */
                USBH_Analyse_CfgDesc( index );
                break;
            }
            else if( ( (PUSB_ITF_DESCR)( &ConfigDescriptor[ i ] ) )->bInterfaceClass == USB_DEV_CLASS_HUB )
            {
                HostCtl[index].DeviceType = USB_DEV_CLASS_HUB;
                /* HUB类设备 */
                HostCtl[ index ].DeviceType |= 0x80;
                i += ConfigDescriptor[ i ];
                while( 1 )
                {
                    if( i >= ConfigDescriptor[ 2 ] )
                    {
                        return( 0x01 );
                    }
                    if( ConfigDescriptor[ i + 1 ] == DEF_DECR_ENDPOINT )
                    {
                        /* 分析端点号 */
                        if( ( (PUSB_ENDP_DESCR)( &ConfigDescriptor[ i ] ) )->bEndpointAddress & 0x80 )
                        {
                            /* 当前为上传端点 */
                            HostCtl[ index ].HubInAddr = ( ( (PUSB_ENDP_DESCR)( &ConfigDescriptor[ i ] ) )->bEndpointAddress) & 0x0f;
                            break;
                        }
                    }
                    else
                    {
                        i += ConfigDescriptor[ i ];
                    }
                }
                return( DEC_SUCCESS );
            }
            else
            {
                /* USB设备类型未知 */
                HostCtl[ index ].Interface[ num ].Type = DEC_UNKNOW;
                i += ConfigDescriptor[ i ];

                /* 按照标准USB设备分析 */
                USBH_Analyse_CfgDesc( index );
                break;
            }
            num++;
        }
        else
        {
            i += ConfigDescriptor[ i ];
        }
    }
    return( s );
}

