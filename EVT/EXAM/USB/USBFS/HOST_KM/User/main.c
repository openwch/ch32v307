/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/10/26
* Description        : Main program body.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
/*
 *@Note
  USBFS的简易枚举过程例程 (仅适用于CH32V305,CH32V307)：
  OTG_FS_DM(PA11)、OTG_FS_DP(PA12)。
  FS的OTG和H/D基地址一致，H/D不支持OTG功能。
*/
#include "stdio.h"
#include "string.h"
#include <ch32vf30x_usbfs_host.h>
#include "Desc_Analysis.h"
#include "debug.h"


/*********************************************************************
 * @fn      Timer3_Init
 *
 * @brief
 *
 * @return  none
 */
void Timer3_Init( )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = { 0 };
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE ); /* 打开TIM3时钟  */

    TIM_TimeBaseStructure.TIM_Period = 99; /* 计数值 */
    TIM_TimeBaseStructure.TIM_Prescaler = 1440; /* 预分频值 */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* 时钟分割  */
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* 计数方向  */
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ARRPreloadConfig(TIM3, DISABLE);                               //禁止ARR预装载缓冲器
    TIM_ITConfig(TIM3, TIM_IT_Update ,ENABLE );                        /* 使能更新中断 */
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    TIM_Cmd(TIM3, ENABLE);
}

/*********************************************************************
 * @fn      Test_IO_INIT
 *
 * @brief   Configuring GPIO For Test
 *
 * @return  none
 */
void Clear_HOST_CTL_Struct( UINT8 index )
{
    UINT8 i, j;

    HostCtl[index].DeviceAddr = 0xFF;
    HostCtl[index].DeviceState = 0xFF;
    HostCtl[index].DeviceSpeed = 0xFF;
    HostCtl[index].DeviceType = 0xFF;

    HostCtl[index].InterfaceNum = 0x00;
    HostCtl[index].HubPortNum = 0x00;
    HostCtl[index].HubInAddr = 0x00;
    HostCtl[index].ErrorCount = 0x00;

    for( i=0; i<DEF_INTERFACE_NUM_MAX; i++ )
    {
        HostCtl[index].Interface[i].Type = 0x00;
        HostCtl[index].Interface[i].HidReportID = 0x00;
        HostCtl[index].Interface[i].Full_KB_Flag = 0x00;
        HostCtl[index].Interface[i].HidDescLen = 0x00;

        HostCtl[index].Interface[i].InEndpNum = 0x00;
        HostCtl[index].Interface[i].OutEndpNum = 0x00;
        for( j=0; j<DEF_ENDP_NUM_MAX; j++ )
        {
            HostCtl[index].Interface[i].InEndpAddr[j] = 0x00;
            HostCtl[index].Interface[i].InEndpSize[j] = 0x00;
            HostCtl[index].Interface[i].InEndpType[j] = 0x00;
            HostCtl[index].Interface[i].InEndpTog[j] = 0x00;
            HostCtl[index].Interface[i].InEndpInterval[j] = 0x00;
            HostCtl[index].Interface[i].InEndpTimeCount[j] = 0x00;

            HostCtl[index].Interface[i].OutEndpAddr[j] = 0x00;
            HostCtl[index].Interface[i].OutEndpType[j] = 0x00;
            HostCtl[index].Interface[i].OutEndpSize[j] = 0x00;
            HostCtl[index].Interface[i].OutEndpTog[j] = 0x00;
        }

        HostCtl[index].Interface[i].IDFlag = 0x00;
        HostCtl[index].Interface[i].Media_Sp_Flag = 0x00;

        HostCtl[index].Interface[i].Disbility = 0x00;
        HostCtl[index].Interface[i].Button = 0x00;
        HostCtl[index].Interface[i].Desktop_X = 0x00;
        HostCtl[index].Interface[i].Desktop_Y = 0x00;
        HostCtl[index].Interface[i].Wheel = 0x00;
        HostCtl[index].Interface[i].Asolute = 0x00;
        HostCtl[index].Interface[i].Mouse_ReportID = 0x00;

        HostCtl[index].Interface[i].NoSetReport_Flag = 0x00;
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    UINT8  s;
    UINT16 i, j, k;
    UINT16 desclen;
    UINT8  lockflag = 0;
    UINT8  len;
    UINT8  endp0outdata[16];
    UINT8  keybufdata[16];
    USART_Printf_Init(115200);
    Delay_Init();
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf("USBFS HOST KM Test\r\n");
    Timer3_Init( );

    USBOTG_HostInit(ENABLE);
    while(1)
    {
        s = ERR_SUCCESS;
        if ( USBOTG_H_FS->INT_FG & USBHD_UIF_DETECT )
        {
            USBOTG_H_FS->INT_FG = USBHD_UIF_DETECT ;
            s = AnalyzeRootHub( );
            HostCtl[0].DeviceState = s;
            if ( s == ERR_USB_CONNECT )
            {
                printf( "New Device In\r\n" );
                /* 准备设备操作相关的存储空间 */
                Clear_HOST_CTL_Struct( 0 );
                lockflag = 0;
                FoundNewDev = 1;
                for( i=0; i<16; i++ )
                {
                    keybufdata[i] = 0;
                    endp0outdata[i] = 0;
                }
                endp0outdata[ 0 ] = 0x01;
            }
            if( s == ERR_USB_DISCON )
            {
                printf( "Device Out\r\n" );
                HostCtl[0].DeviceState = 0xFF;
                HostCtl[0].DeviceType  = 0xFF;
                USBOTG_H_FS->HOST_RX_DMA = (UINT32)&endpRXbuff[0];                 //host rx DMA address
                USBOTG_H_FS->HOST_TX_DMA = (UINT32)&endpTXbuff[0];                 //host tx DMA address
            }
        }

        if ( FoundNewDev || s == ERR_USB_CONNECT )
        {
            FoundNewDev = 0;
            Delay_Ms( 200 );
            s = USBOTG_HostEnum( endpRXbuff );
            HostCtl[0].DeviceState = s;
            if ( s == ERR_SUCCESS )
            {
                TIM_Cmd(TIM3, ENABLE);
                printf( "Enum Succeed\r\n" );

                printf( "Device Desc\n" );
                desclen = DeviceDescriptor[0];
                for( i=0; i<desclen ; i++ )
                {
                    printf( "%02x ", DeviceDescriptor[i] );
                }
                printf( "\n" );
                /* Analysis Configuration Descriptor */
                printf( "Configuration Desc\n" );
                desclen = (UINT16)ConfigDescriptor[2] + (UINT16)(ConfigDescriptor[3]<<8);
                for( i=0; i<desclen ; i++ )
                {
                    printf( "%02x ", ConfigDescriptor[i] );
                }
                printf( "\n" );
                /* Analysis Device Descriptor */
                KM_Analyse_ConfigDesc( 0 );
                printf( "Device Status %02x\n", HostCtl[0].DeviceState );
                printf( "Device Speed  %02x\n", HostCtl[0].DeviceSpeed );
                printf( "Device Type   %02x\n", HostCtl[0].DeviceType );
                printf( "Device Addr   %02x\n", HostCtl[0].DeviceAddr );
                for( i=0; i<HostCtl[0].InterfaceNum; i++ )
                {
                    printf( "InterFace Type %02x\n", HostCtl[0].Interface[i].Type );
                    for( j=0; j<HostCtl[0].Interface[i].InEndpNum; j++ )
                    {
                        printf( "EndpIn   %02x\n", j );
                        printf( "Addr     %02x\n", HostCtl[0].Interface[i].InEndpAddr[j] );
                        printf( "Type     %02x\n", HostCtl[0].Interface[i].InEndpType[j] );
                        printf( "Size     %02x\n", HostCtl[0].Interface[i].InEndpSize[j] );
                        printf( "InterVal %02x\n", HostCtl[0].Interface[i].InEndpInterval[j] );
                    }

                    for( j=0; j<HostCtl[0].Interface[i].OutEndpNum; j++ )
                    {
                        printf( "EndpOut  %02x\n", j );
                        printf( "Addr     %02x\n", HostCtl[0].Interface[i].OutEndpAddr[j] );
                        printf( "Type     %02x\n", HostCtl[0].Interface[i].OutEndpType[j] );
                        printf( "Size     %02x\n", HostCtl[0].Interface[i].OutEndpSize[j] );
                    }
                }
            }
            else printf( "Enum Failed:%02x\r\n", s );
        }

        if( (HostCtl[0].DeviceState == 0x00) && (HostCtl[0].DeviceType = USB_DEV_CLASS_HID) )
        {
            /* Enter HID */
            if( TIM_GetITStatus(TIM3, TIM_IT_Update)  )
            {
                TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
                /* HID Data  Trans */
                for( i=0; i<HostCtl[0].InterfaceNum; i++ )
                {
                    for( j=0; j<HostCtl[0].Interface[i].InEndpNum; j++ )
                    {
                        HostCtl[0].Interface[i].InEndpTimeCount[j] ++;
                        if( HostCtl[0].Interface[i].InEndpTimeCount[j] >= HostCtl[0].Interface[i].InEndpInterval[j] )
                        {
                            HostCtl[0].Interface[i].InEndpTimeCount[j] = 0;
                            /* 使用单独的缓冲区地址 */
                            USBOTG_H_FS->HOST_RX_DMA = (UINT32)keybufdata;                                //设置接收DMA地址
                            s = USBHostTransact( USB_PID_IN << 4 | HostCtl[0].Interface[i].InEndpAddr[j], HostCtl[0].Interface[i].InEndpTog[j], 0 );
                            if ( s == ERR_SUCCESS )
                            {
                                HostCtl[0].Interface[i].InEndpTog[j] ^= USBHD_UH_R_TOG;
                                if( USBOTG_H_FS->RX_LEN > 0 )
                                {
                                    printf( "Recv " );
                                    for( k=0; k<USBOTG_H_FS->RX_LEN; k++ )
                                    {
                                        printf( "%02x ", keybufdata[k] );
                                    }
                                    printf( "\n" );
                                    /* CapsLock */
//                                  NUMLOCK 0X01
//                                  CAPLOCK 0X02
//                                  SCROLLLOCK 0X04
                                    if( keybufdata[0x02] == 0x39 )
                                    {
                                       endp0outdata[ 0 ] ^= 0x02;
                                       lockflag = 1;
                                    }
                                    /* ScrollLock */
                                    else if( keybufdata[0x02] == 0x47 )
                                    {
                                       endp0outdata[ 0 ] ^= 0x04;
                                       lockflag = 1;
                                    }
                                    /* NumLock */
                                    else if( keybufdata[0x02] == 0x53 )
                                    {
                                       endp0outdata[ 0 ] ^= 0x01;
                                       lockflag = 1;
                                    }
                                    if( lockflag )
                                    {
                                        lockflag = 0;
                                        /* SetReport */
                                        CopySetupReqPkg( HIDSetReport );
                                        USBOTG_H_FS->HOST_TX_DMA = (UINT32)endpTXbuff;
                                        USBOTG_H_FS->HOST_TX_LEN = 8;
                                        s = HostCtrlTransfer( endp0outdata, &len );
                                        if ( s != ERR_SUCCESS )
                                        {
                                           printf( "err %02x\n", s );
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
}



