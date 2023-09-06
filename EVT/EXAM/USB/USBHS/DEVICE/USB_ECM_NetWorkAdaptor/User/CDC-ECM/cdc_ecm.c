/********************************** (C) COPYRIGHT *******************************
* File Name          : cdc_ecm.c
* Author             : WCH
* Version            : V1.3.0
* Date               : 2022/06/02
* Description        : cdc-ecm program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "cdc_ecm.h"

/* ECM/NCM Network Status to Load */
__attribute__ ((aligned(4))) uint8_t ECM_NetWork_Speed_Change[ 16 ] = { 0xA1, 0x2A, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  /* ECM速度变化状态上传 */
__attribute__ ((aligned(4))) uint8_t ECM_NetWork_Connection[ 8 ]    = { 0xA1, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00 };

/* Ring Buffer */
__attribute__((__aligned__(4))) RING_BUFF_COMM U2E_Trance_Manage;/* usb to eth 缓冲区管理 */
__attribute__((__aligned__(4))) uint32_t U2E_PackAdr[DEF_U2E_MAXBLOCKS];/* usb to eth 包地址记录 */
__attribute__((__aligned__(4))) uint32_t U2E_PackLen[DEF_U2E_MAXBLOCKS];/* usb to eth 包长度记录 */

__attribute__((__aligned__(4))) RING_BUFF_COMM E2U_Trance_Manage;/* eth to usb 缓冲区管理 */
__attribute__((__aligned__(4))) uint32_t E2U_PackAdr[DEF_E2U_MAXBLOCKS];/* eth to usb 包地址记录 */
__attribute__((__aligned__(4))) uint32_t E2U_PackLen[DEF_E2U_MAXBLOCKS];/* eth to usb 包长度记录 */

__attribute__((__aligned__(4))) ETH_DMADESCTypeDef *DMARxDealTabs[ETH_RXBUFNB];/* MAC recv descriptor, 4-byte aligned， eth to usb 接收描述符包状态归属 */

/* Phy Status */
volatile uint8_t  ETH_LastStatus = 0;
volatile uint8_t  ECM_Pack_Filter = 0;
volatile uint8_t  ETH_NETWork_Status = 0;
volatile uint16_t LastPhyStatus = 0;

/* Transe Status */
volatile uint32_t U2E_PackCnounter;
volatile uint32_t E2U_PackCnounter;
uint8_t  PhyInit_Flag;

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @param   iError - error constants.
 *
 * @return  none
 */
void mStopIfError( uint8_t iError )
{
    if (iError == ETH_SUCCESS)
        return;
    printf( "Error: %02X\r\n", iError );
}

/*********************************************************************
 * @fn      ECM_Change_MAC_Filter
 *
 * @brief   Change MacFliter by USB Request
 *
 * @return  none
 */
void ECM_Change_MAC_Filter( uint8_t pac_filter )
{
    if( pac_filter != ECM_Pack_Filter )
    {
        ECM_Pack_Filter = pac_filter;
        /* Promiscuous Mode */
        if( ECM_Pack_Filter & DEF_ECM_PACK_PROMISCUOUS )
        {
            ETH->MACFFR |= ETH_PromiscuousMode_Enable;
            ETH->MACFFR |= ETH_ReceiveAll_Enable;
        }
        else
        {
            ETH->MACFFR &= ~ETH_PromiscuousMode_Enable;
            ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
        }
        /* All Multicast Mode */
        if( ECM_Pack_Filter & DEF_ECM_PACK_ALL_MULTICAST )
        {
            ETH->MACFFR |= ETH_MulticastFramesFilter_None;
        }
        else
        {
            ETH->MACFFR &= ~ETH_MulticastFramesFilter_None;
        }
        /* Direct mode(Unicast) */
        if( ECM_Pack_Filter & DEF_ECM_PACK_DIRECTED )
        {
            ETH->MACFFR &= ~ETH_UnicastFramesFilter_HashTable;
            ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
        }
        /* Multicast Mode */
        if( ECM_Pack_Filter & DEF_ECM_PACK_MULTICAST )
        {
            ETH->MACFFR &= ~ETH_MulticastFramesFilter_HashTable;
            ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
        }
        /* BroadCast Mode */
        if( (ECM_Pack_Filter & DEF_ECM_PACK_BROADCASRT) == RESET )
        {
            ETH->MACFFR |= ETH_BroadcastFramesReception_Disable;
            ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
        }
        else
        {
            ETH->MACFFR &= ~ETH_BroadcastFramesReception_Disable;
            ETH->MACFFR &= ~ETH_ReceiveAll_Enable;
        }
    }
}

/*********************************************************************
 * @fn      MACAddr_Change_To_SNDesc
 *
 * @brief   Change Mac address to USB SN String Descriptor
 *
 * @return  none
 */
void MACAddr_Change_To_SNDesc( uint8_t *pmacbuf )
{
    uint8_t  temp8;
    uint8_t  *p;
    uint8_t  i;

    p = pmacbuf;
    for( i = 0; i < 6; i++ )
    {
        /* 取高4位处理 */
        temp8 = ( *p & 0xF0 ) >> 4;
        temp8 = temp8 & 0x0F;
        if( temp8 >= 0x0A )
        {
            temp8 += 0x37;
        }
        else
        {
            temp8 += 0x30;
        }
        ECM_StrDesc_Sn[ ( ( i * 2 ) + 1 ) * 2 ] = temp8;

        /* 取低4位处理 */
        temp8 = *p & 0x0F;
        if( temp8 >= 0x0A )
        {
            temp8 += 0x37;
        }
        else
        {
            temp8 += 0x30;
        }
        ECM_StrDesc_Sn[ ( ( i * 2 ) + 2 ) * 2 ] = temp8;

        p++;
    }
}

/*********************************************************************
 * @fn      RB_Init
 *
 * @brief   RingBuffer Initialization
 *
 * @return  none
 */
void RB_Init( void )
{
    uint8_t i;
    U2E_Trance_Manage.LoadPtr = 0;
    U2E_Trance_Manage.DealPtr = 0;
    U2E_Trance_Manage.RemainPack = 0;
    U2E_Trance_Manage.StopFlag = 0;
    for( i=0; i<DEF_U2E_MAXBLOCKS; i++ )
    {
        U2E_PackLen[ i ] = 0;
        U2E_PackAdr[ i ] = (uint32_t)(uint8_t *)(&MACTxBuf[ i*DEF_ETH_USBPACK ]);
    }

    E2U_Trance_Manage.LoadPtr = 0;
    E2U_Trance_Manage.DealPtr = 0;
    E2U_Trance_Manage.RemainPack = 0;
    E2U_Trance_Manage.StopFlag = 0;
    for( i=0; i<DEF_E2U_MAXBLOCKS; i++ )
    {
        E2U_PackLen[ i ] = 0;
        E2U_PackAdr[ i ] = (uint32_t)(uint8_t *)(&MACRxBuf[ i*DEF_ETH_USBPACK ]);
    }
    U2E_PackCnounter = 0;
    E2U_PackCnounter = 0;

    ECM_SPD_Change->dlspeed = 0;
    ECM_SPD_Change->ulspeed = 0;
    ECM_Link_Status->wvalue = DEF_ECM_NETWORK_DISCONN;
}

/*********************************************************************
 * @fn      USB2ETH_Trance
 *
 * @brief   usb to eth trance
 *
 * @return  none
 */
void USB2ETH_Trance( void )
{
    uint8_t ret;
    uint32_t u2e_deal_ptr;
    if( U2E_Trance_Manage.RemainPack > 20 )
    {
        GPIOD->BSHR = GPIO_Pin_1;
    }
    else
    {
        GPIOD->BCR = GPIO_Pin_1;
    }

    /* usb to eth */
    if( U2E_Trance_Manage.RemainPack )
    {
        u2e_deal_ptr = U2E_Trance_Manage.DealPtr;

        ret = ETH_TxPktChainMode( (uint16_t)U2E_PackLen[ u2e_deal_ptr ], (uint32_t *)(U2E_PackAdr[ u2e_deal_ptr ] + DEF_U2E_PACKHEADOFFSET) );
        if( ret == ETH_SUCCESS )
        {
            __disable_irq( );
            __NOP( );__NOP( );
            U2E_Trance_Manage.DealPtr++;
            if( U2E_Trance_Manage.DealPtr >= DEF_U2E_MAXBLOCKS )
            {
                U2E_Trance_Manage.DealPtr = 0;
            }
            U2E_Trance_Manage.RemainPack--;
            __enable_irq( );
            __NOP( );__NOP( );
        }
    }

    if( U2E_Trance_Manage.RemainPack < (DEF_U2E_MAXBLOCKS - DEF_U2E_RESTART) )
    {
        /* USB Out Restart, Set Out Ack */
        if(U2E_Trance_Manage.StopFlag)
        {
            U2E_Trance_Manage.StopFlag = 0;
            USBHSD->UEP3_RX_CTRL = (USBHSD->UEP3_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
        }
    }
}

/*********************************************************************
 * @fn      ETH2USB_Trance
 *
 * @brief   eth to usb trance
 *
 * @return  none
 */
void ETH2USB_Trance( void )
{
    uint8_t  ret;
    uint32_t e2u_deal_ptr;
    static uint8_t upload_lock = 0;
    static uint8_t upload_check = 0;
    /* eth to usb */
    if( E2U_Trance_Manage.RemainPack )
    {
        e2u_deal_ptr = E2U_Trance_Manage.DealPtr;
        if( upload_lock )
        {
            if( e2u_deal_ptr == 0 ) upload_check = DEF_E2U_MAXBLOCKS-1;
            else upload_check = e2u_deal_ptr-1;
            /* Check If Upload End */
            if( USBHS_Endp_Busy[ DEF_UEP2 ] == DEF_UEP_FREE )
            {
                DMARxDealTabs[ upload_check ]->Status |= ETH_DMARxDesc_OWN;
                upload_lock = 0;
            }
            else
            {
                return;
            }
        }

        if( E2U_PackLen[ e2u_deal_ptr ] >= USBHS_DevMaxPackLen )
        {
            /* pack size >= USBHS_DevMaxPackLen */
            ret = USBHS_EP2_UpLoad( USBHS_DevMaxPackLen, E2U_PackAdr[ e2u_deal_ptr ] );
            if( ret == 0 )
            {
                /* this pack up load success, goto usb irq-hander for next deals */
                E2U_PackLen[ e2u_deal_ptr ] -= USBHS_DevMaxPackLen;
                E2U_PackAdr[ e2u_deal_ptr ] += USBHS_DevMaxPackLen;
                upload_lock = 0;
            }
        }
        else
        {
            /* Pack size < USBHS_DevMaxPackLen */
            ret = USBHS_EP2_UpLoad( E2U_PackLen[ e2u_deal_ptr ], E2U_PackAdr[ e2u_deal_ptr ] );
            if( ret == 0 )
            {
                /* only need to upload once */
                E2U_PackLen[ e2u_deal_ptr ] = 0;
                E2U_PackAdr[ e2u_deal_ptr ] = 0;
                __disable_irq( );
                 __NOP( );__NOP( );
                E2U_Trance_Manage.RemainPack--;
                __enable_irq( );
                __NOP( );__NOP( );
                E2U_Trance_Manage.DealPtr++;
                if( E2U_Trance_Manage.DealPtr >= DEF_E2U_MAXBLOCKS )
                {
                    E2U_Trance_Manage.DealPtr = 0;
                }
                upload_lock = 1;
            }
        }
    }
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 10000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = (2000 - 1);    /* 100ms */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef  NVIC_InitTypeStructure = { 0 };
    NVIC_InitTypeStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitTypeStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitTypeStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitTypeStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitTypeStructure );

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_Cmd(TIM2, ENABLE);
    NVIC_EnableIRQ(TIM2_IRQn);

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      ECM_Load_Status
 *
 * @brief   Load ECM NetWork Status
 *
 * @return  none
 */
void ECM_Load_Status( void )
{
    uint8_t  usbret;
    uint16_t phy_stat;
    static uint16_t Loacl_Timer = 0;
    static uint8_t  loadflag = 0;

    /* Phy status update */
    Loacl_Timer++;
    if( Loacl_Timer == DEF_PHY_QUERY_TIMEOUT )
    {
        Loacl_Timer = 0;
#if PHY_MODE == USE_10M_BASE
        phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
#elif PHY_MODE == USE_MAC_RGMII
        ETH_WritePHYRegister( PHY_ADDRESS, 0x1F, 0x0a43 );
        /*In some cases the status is not updated in time,
         * so read this register twice to get the correct status value.*/
        ETH_ReadPHYRegister( PHY_ADDRESS, 0x1A);
        phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, 0x1A );
#else
        phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
#endif
        if( phy_stat != LastPhyStatus )
        {
            LastPhyStatus = phy_stat;
            ETH_PHYLink( );

            /* ETH Status Update */
            if( ETH_NETWork_Status & DEF_NETSTAT_LINK_RDY )
            {
                ETH_NETWork_Status &= ~(DEF_NETSTAT_100MBITS | DEF_NETSTAT_1000MBITS);
                if( ETH->MACCR & ETH_Speed_100M )
                {
                    /* 100M Speed */
                    ETH_NETWork_Status |= DEF_NETSTAT_100MBITS;
                }
                else if( ETH->MACCR & ETH_Speed_1000M )
                {
                    /* 1000M Speed */
                    ETH_NETWork_Status |= DEF_NETSTAT_1000MBITS;
                }
                else
                {
                    /* 10M Speed */
                }
                ETH_NETWork_Status &= ~DEF_NETSTAT_FULLDUPLEX;
                if( ETH->MACCR & ETH_Mode_FullDuplex )
                {
                    /* Full Duplex */
                    ETH_NETWork_Status |= DEF_NETSTAT_FULLDUPLEX;
                }
                else
                {
                    /* half Duplex */
                }
                if( ETH_LastStatus != ETH_NETWork_Status )
                {
                    ETH_LastStatus = ETH_NETWork_Status;
                }
            }
            else
            {
                ETH_NETWork_Status &= ~(DEF_NETSTAT_100MBITS | DEF_NETSTAT_1000MBITS);
                ETH_NETWork_Status &= ~DEF_NETSTAT_FULLDUPLEX;
            }

            if( ETH_NETWork_Status & DEF_NETSTAT_LINK_RDY )
            {
                ECM_Link_Status->wvalue = DEF_ECM_NETWORK_CONNECT;
                if( ETH_LastStatus & DEF_NETSTAT_1000MBITS )
                {
                    ECM_SPD_Change->dlspeed = DEF_ECM_CONN_SPD_1000M;
                    ECM_SPD_Change->ulspeed = DEF_ECM_CONN_SPD_1000M;
                }
                else if( ETH_LastStatus & DEF_NETSTAT_100MBITS )
                {
                    ECM_SPD_Change->dlspeed = DEF_ECM_CONN_SPD_100M;
                    ECM_SPD_Change->ulspeed = DEF_ECM_CONN_SPD_100M;
                }
                else
                {
                    ECM_SPD_Change->dlspeed = DEF_ECM_CONN_SPD_10M;
                    ECM_SPD_Change->ulspeed = DEF_ECM_CONN_SPD_10M;
                }
            }
            else
            {
                ECM_SPD_Change->dlspeed = 0;
                ECM_SPD_Change->ulspeed = 0;
                ECM_Link_Status->wvalue = DEF_ECM_NETWORK_DISCONN;
            }
        }
        if( loadflag == 0 )
        {
            /* Load Link Status */
            ECM_NetWork_Connection[ 0 ] = 0xA1;
            ECM_NetWork_Connection[ 4 ] = 0x02;
            usbret = USBHS_EP1_UpLoad( 8, (uint32_t)(uint8_t *)ECM_NetWork_Connection );
            if( usbret == 0 )
            {
                loadflag = 1;
            }
        }
        else
        {
            /* Load speed Status */
            ECM_NetWork_Speed_Change[ 0 ] = 0xA1;
            ECM_NetWork_Speed_Change[ 1 ] = 0x2A;
            ECM_NetWork_Speed_Change[ 4 ] = 0x01;
            ECM_NetWork_Speed_Change[ 6 ] = 0x08;
            usbret = USBHS_EP1_UpLoad( 16, (uint32_t)(uint8_t *)ECM_NetWork_Speed_Change );
            if( usbret == 0 )
            {
                loadflag = 0;
            }
        }
    }
}

/*********************************************************************
 * @fn      ETH_GetMacAddr
 *
 * @brief   Get MAC address
 *
 * @return  none.
 */
void ETH_GetMacAddr( uint8_t *p )
{
    uint8_t i;
    uint8_t *macaddr=(uint8_t *)(ROM_CFG_USERADR_ID+5);

    for(i=0;i<6;i++)
    {
        *p = *macaddr;
        p++;
        macaddr--;
    }
}

/*********************************************************************
 * @fn      ETH_DriverInit
 *
 * @brief   Basic Ethernet initialization
 *
 * @return  none.
 */
void ETH_DriverInit( uint8_t *addr )
{
    uint8_t i;

    /* change mac-addr to ecm/ncm format */
    MACAddr_Change_To_SNDesc( addr );
    /* Ring buffer init */
    RB_Init( );
    /* Used for Time Base */
    TIM2_Init( );
    ETH_Init( addr );
    /* Enable flowConlrol */
    /* PT = 240(10 full eth pack); PLT = 01(28*PT) */
    ETH->MACFCR = 0;
    ETH->MACFCR |= (0xF0<<16);/* 240(64bits time) 10 full eth pack time */
    ETH->MACFCR |= (0x01<<4);
    ETH->MACFCR |= ETH_MACFCR_TFCE;
    NVIC_DisableIRQ( ETH_IRQn );
    /* flush U2E_PackAdr/E2U_PackAdr */
    printf( "TxDesc:\r\n" );
    for( i=0; i<ETH_TXBUFNB; i++ )
    {
        printf( "%08x ", DMATxDscrTab[i].Buffer1Addr );
        U2E_PackAdr[i] = DMATxDscrTab[i].Buffer1Addr;
    }
    printf( "\r\n" );
    printf( "RxDesc:\r\n" );
    for( i=0; i<ETH_RXBUFNB; i++ )
    {
        printf( "%08x ", DMARxDscrTab[i].Buffer1Addr );
        E2U_PackAdr[ i ] = DMARxDscrTab[i].Buffer1Addr;
    }
    printf( "\r\n" );
    NVIC_EnableIRQ( ETH_IRQn );
}

/*********************************************************************
 * @fn      ETH_PhyAbility_Set
 *
 * @brief   Set phy ability if necessary
 *
 * @return  none.
 */
void ETH_PhyAbility_Set( void )
{
#if( PHY_MODE != USE_10M_BASE )
    uint16_t RegValue;
    /* Limit Phy Speed If Necessary */
    if( USBHS_DevSpeed == USBHS_SPEED_FULL )
    {
        /* Read ADAR Register */
        RegValue = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_ANAR );
        RegValue |= (ANAR_10BASE_T | ANAR_10BASE_T_FD);
        RegValue &= ~(ANAR_100BASE_TX_FD | ANAR_100BASE_TX);
        /* Disable ANAR_100BASE_TX_FD&ANAR_100BASE_TX */
        ETH_WritePHYRegister( PHY_ADDRESS, PHY_ANAR, RegValue );
#if( PHY_MODE == USE_MAC_RGMII )
        /* RTL8211FS 0x09 : GBCR (1000Base-T Control Register) */
        RegValue = ETH_ReadPHYRegister( PHY_ADDRESS, 0x09 );
        RegValue &= ~(1<<9);/* Bit9 : 1000Base-T Full duplex not advertised */
        ETH_WritePHYRegister( PHY_ADDRESS, 0x09, RegValue );
#endif
        printf( "USB Full-Speed\r\n" );
        printf( "Local Phy Ability: 10M, Full Duplex\r\n" );
    }
    else
    {
        /* Read ADAR Register */
        RegValue = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_ANAR );
        RegValue |= (ANAR_10BASE_T | ANAR_10BASE_T_FD);
        RegValue |= (ANAR_100BASE_TX | ANAR_100BASE_TX_FD);
        /* Enable ANAR_100BASE_TX_FD&ANAR_100BASE_TX */
        ETH_WritePHYRegister( PHY_ADDRESS, PHY_ANAR, RegValue );
        printf( "USB High-Speed\r\n" );
        printf( "LocalPhy Ability: 100M, Full Duplex\r\n" );
#if( PHY_MODE == USE_MAC_RGMII )
        printf( "LocalPhy Ability: 10/100/1000M, Full Duplex\r\n" );
#else
        printf( "LocalPhy Ability: 10/100M, Full Duplex\r\n" );
#endif
    }
#else
    printf( "LocalPhy Ability: 10M(Fixed), Full Duplex\r\n" );
#endif
}
