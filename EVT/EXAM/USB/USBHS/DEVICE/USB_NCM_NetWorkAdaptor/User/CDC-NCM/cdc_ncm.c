/********************************** (C) COPYRIGHT *******************************
* File Name          : cdc_ncm.c
* Author             : WCH
* Version            : V1.3.0
* Date               : 2023/07/20
* Description        : cdc-ncm program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "cdc_ncm.h"

__attribute__ ((aligned(4))) uint8_t ECM_NetWork_Speed_Change[ 16 ] = { 0xA1, 0X2A, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
__attribute__ ((aligned(4))) uint8_t ECM_NetWork_Connection[ 8 ] = { 0xA1, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00 };
__attribute__ ((aligned(4))) uint8_t NTB_Parameter_Structure[ 28 ] = {  0x1C, 0x00, 0x01, 0x00,
                                                                        DEF_ETH_TX_USBPACK&0xff, DEF_ETH_TX_USBPACK>>8, 0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00,
                                                                        DEF_ETH_RX_USBPACK&0xff, DEF_ETH_RX_USBPACK>>8, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00 };
/* Ring Buffer */
__attribute__((__aligned__(4))) RING_BUFF_COMM U2E_Trance_Manage;/* usb to eth 缓冲区管理 */
__attribute__((__aligned__(4))) uint32_t U2E_PackAdr[DEF_U2E_MAXBLOCKS];/* usb to eth 包地址记录 */
__attribute__((__aligned__(4))) uint32_t U2E_PackLen[DEF_U2E_MAXBLOCKS];/* usb to eth 包长度记录 */

__attribute__((__aligned__(4))) RING_BUFF_COMM E2U_Trance_Manage;/* eth to usb 缓冲区管理 */
__attribute__((__aligned__(4))) uint32_t E2U_PackAdr[DEF_E2U_MAXBLOCKS];/* eth to usb 包地址记录 */
__attribute__((__aligned__(4))) uint32_t E2U_PackLen[DEF_E2U_MAXBLOCKS];/* eth to usb 包长度记录 */

__attribute__ ((aligned(4))) uint8_t USB_TxBuff[ DEF_ETH_TX_USBPACK ] = { 0 };
__attribute__ ((aligned(4))) uint8_t USB_RxBuff[ DEF_ETH_RX_USBPACK ] = { 0 };

__attribute__((__aligned__(4))) ETH_DMADESCTypeDef *DMARxDealTabs[ETH_RXBUFNB];/* MAC recv descriptor, 4-byte aligned， eth to usb 接收描述符包状态归属 */

NCM_BUFF_COMM USB_TxManage;
NCM_BUFF_COMM USB_RxManage;

/* Phy Status */
volatile uint8_t  ETH_LastStatus = 0;
volatile uint8_t  ECM_Pack_Filter = 0;
volatile uint8_t  ETH_NETWork_Status = 0;
volatile uint16_t LastPhyStatus = 0;

/* Transe Status */
volatile uint32_t U2E_PackCnounter;
volatile uint32_t E2U_PackCnounter;
uint8_t  PhyInit_Flag;

/* macaddr */
uint8_t MACAddr[ 6 ];

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
    ETH->MACFFR |= ETH_PromiscuousMode_Enable;
    ETH->MACFFR |= ETH_ReceiveAll_Enable;
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
    for( i=0; i<ETH_TXBUFNB; i++ )
    {
        U2E_PackLen[ i ] = 0;
        U2E_PackAdr[ i ] = (uint32_t)(uint8_t *)(&MACTxBuf[ i*ETH_MAX_PACKET_SIZE ]);
    }

    E2U_Trance_Manage.LoadPtr = 0;
    E2U_Trance_Manage.DealPtr = 0;
    E2U_Trance_Manage.RemainPack = 0;
    for( i=0; i<ETH_RXBUFNB; i++ )
    {
        E2U_PackLen[ i ] = 0;
        E2U_PackAdr[ i ] = (uint32_t)(uint8_t *)(&MACRxBuf[ i*ETH_MAX_PACKET_SIZE ]);
    }

    memset( &USB_TxManage, 0, sizeof( USB_TxManage ) );
    memset( &USB_RxManage, 0, sizeof( USB_RxManage ) );

    ( ( pNCM_NTH )USB_TxBuff )->dwSignature = 0x484D434E;
    ( ( pNCM_NTH )USB_TxBuff )->wHeaderLength = 0x000C;
    ( ( pNCM_NTH )USB_TxBuff )->wSequence = 0;

    ECM_SPD_Change->dlspeed = 0;
    ECM_SPD_Change->ulspeed = 0;
    ECM_Link_Status->wvalue = DEF_ECM_NETWORK_DISCONN;
}

/*********************************************************************
 * @fn      _PackNTB
 *
 * @brief   pack NTB
 *
 * @return  none
 */
static uint8_t _PackNTB( void )
{
    uint16_t i = 0;
    uint16_t count;
    uint16_t ndp_offset = 16, ndp_length = 16;
    uint16_t curr_offset = 16, temp_offset;
    uint16_t ndp_offset_record[ ETH_RXBUFNB ];
    uint16_t ndp_length_record[ ETH_RXBUFNB ];
    uint32_t e2u_deal_ptr;
    pNCM_NDP ndp = NULL;

    count = E2U_Trance_Manage.RemainPack;

    if( USB_TxManage.Flag == 1 ) return 1;
    if( !count ) return 2;
    
    for( i = 0; i < count; i++ )
    {
        e2u_deal_ptr = E2U_Trance_Manage.DealPtr;

        /* Calculate NDP offset (4-byte alignment) */
        if( E2U_PackLen[ e2u_deal_ptr ] % 4 ) temp_offset = E2U_PackLen[ e2u_deal_ptr ] + ( 4 - ( E2U_PackLen[ e2u_deal_ptr ] % 4 ) );
        else temp_offset = E2U_PackLen[ e2u_deal_ptr ];

        if( i > 1 ) ndp_length += 4;

        /* Calculate NTB size overflow */
        if( ndp_offset + temp_offset + ndp_length <= DEF_ETH_TX_USBPACK )
        {
            ndp_offset += temp_offset;
            /* Data loading into NTB */
            memcpy( USB_TxBuff + curr_offset, ( uint8_t* )(E2U_PackAdr[ e2u_deal_ptr ]), E2U_PackLen[ e2u_deal_ptr ] );
            ndp_offset_record[ i ] = curr_offset;
            ndp_length_record[ i ] = E2U_PackLen[ e2u_deal_ptr ];
            curr_offset = ndp_offset;
            __disable_irq( );
            __NOP( );__NOP( );
            DMARxDealTabs[ e2u_deal_ptr ]->Status |= ETH_DMARxDesc_OWN;
            E2U_PackLen[ e2u_deal_ptr ] = 0;
            E2U_PackAdr[ e2u_deal_ptr ] = 0;
            E2U_Trance_Manage.RemainPack--;
            E2U_Trance_Manage.DealPtr++;
            if( E2U_Trance_Manage.DealPtr >= ETH_RXBUFNB )
            {
                E2U_Trance_Manage.DealPtr = 0;
            }
            __enable_irq( );
            __NOP( );__NOP( );
        }
        else
        {
            break;
        }
    }
    count = i;
    /* Build NDP */
    ndp = ( pNCM_NDP )( USB_TxBuff + ndp_offset );
    ndp->dwSignature = 0x304D434E;
    ndp->wLength = ndp_length;
    ndp->wNextNdpIndex = 0x0000;
    for( i = 0; i < count; i++ )
    {
        ndp->wDatagramInd_Len[ i * 2 ] = ndp_offset_record[ i ];
        ndp->wDatagramInd_Len[ i * 2 + 1 ] = ndp_length_record[ i ];
    }
    if( count == 1)
    {
        ndp->wDatagramInd_Len[ 2 ] = 0;
        ndp->wDatagramInd_Len[ 3 ] = 0;
    }
    /* Build NTH */
    ( ( pNCM_NTH )USB_TxBuff )->wSequence += 1;
    ( ( pNCM_NTH )USB_TxBuff )->wBlockLength = ndp_offset + ndp_length;
    ( ( pNCM_NTH )USB_TxBuff )->wNdpIndex = ndp_offset;

    /* if wBlockLength is alined to 64bytes, add 4bytes at the end of USB_TxBuff, and set that 4bytes to 0 */
    if( ( ( ( pNCM_NTH )USB_TxBuff )->wBlockLength % USBHS_DevMaxPackLen ) == 0 )
    {
        count = ( ( pNCM_NTH )USB_TxBuff )->wBlockLength;
        ( ( pNCM_NTH )USB_TxBuff )->wBlockLength += 4;
        *(uint32_t *)&USB_TxBuff[ count ] = 0;
    }

    USB_TxManage.Length = ( ( pNCM_NTH )USB_TxBuff )->wBlockLength;
    USB_TxManage.Address = ( uint32_t )USB_TxBuff;
    USB_TxManage.Flag = 1;
    return 0;
}

/*********************************************************************
 * @fn      _UnpackNTB
 *
 * @brief   Unpack NTB
 *
 * @return  none
 */
static uint8_t _UnpackNTB( void )
{
    pNCM_NTH nth = NULL;
    pNCM_NDP ndp = NULL;
    static uint16_t i, count;
    uint16_t offset, length;

    nth = ( pNCM_NTH )USB_RxBuff;

    if( USB_RxManage.Flag == 1 || USB_RxManage.Flag == 2 )
    {
        if( USB_RxManage.Flag == 1 )
        {
            /* Check NTH16 Header and Length */
            if( nth->dwSignature != 0x484D434E || nth->wHeaderLength != 0x000C ) return 1;

            if( nth->wNdpIndex < 0x000C ) return 2;

            ndp = ( pNCM_NDP )( USB_RxBuff + nth->wNdpIndex );

            if( !( ndp->dwSignature == 0x304D434E || ndp->dwSignature == 0x314D434E ) || ndp->wLength < 16) return 3;

            i = 0;
            count = ( ndp->wLength - 8 ) / 4;
            if( count > ( ETH_TXBUFNB - U2E_Trance_Manage.RemainPack ) ) USB_RxManage.Flag = 2;
        }
        for(; i < count; i++ )
        {
            offset = ndp->wDatagramInd_Len[ i * 2 ];
            length = ndp->wDatagramInd_Len[ i * 2 + 1 ];

            if( offset == 0 || length == 0 ) continue;
            if( offset < 0x000C || length < 14 ) return 4;
            if( U2E_Trance_Manage.RemainPack >= ETH_TXBUFNB ) break;

            memcpy( ( uint8_t* )( U2E_PackAdr[ U2E_Trance_Manage.LoadPtr ] ), USB_RxBuff + offset, length );
            U2E_PackLen[ U2E_Trance_Manage.LoadPtr ] = length;

            U2E_Trance_Manage.LoadPtr++;
            if( U2E_Trance_Manage.LoadPtr >= ETH_TXBUFNB )
            {
                U2E_Trance_Manage.LoadPtr = 0;
            }
            U2E_Trance_Manage.RemainPack++;

        }
        if( i == count ) USB_RxManage.Flag = 3;
    }
    return 0;
}


/*********************************************************************
 * @fn      ETH2USB_DataSend(based on hardware)
 *
 * @brief   Send ETH Data
 *
 * @return  none
 */
uint8_t ETH2USB_DataSend(uint16_t len, uint32_t *pBuff ) 
{
    /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
    if((pDMATxSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
    {
        /* Return ERROR: OWN bit set */
        return ETH_ERROR;
    }
    /* Setting the Frame Length: bits[12:0] */
    pDMATxSet->ControlBufferSize = (len & ETH_DMATxDesc_TBS1);
    pDMATxSet->Buffer1Addr = (uint32_t)pBuff;

    /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
    pDMATxSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    pDMATxSet->Status |= ETH_DMATxDesc_OWN;

    /* Clear TBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    ETH->DMATPDR = 0;

    /* Update the ETHERNET DMA global Tx descriptor with next Tx descriptor */
    /* Chained Mode */
    /* Selects the next DMA Tx descriptor list for next buffer to send */
    pDMATxSet = (ETH_DMADESCTypeDef*) (pDMATxSet->Buffer2NextDescAddr);
    /* Return SUCCESS */
    return ETH_SUCCESS;
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

    if( _UnpackNTB( ) )
    {
        USB_RxManage.Flag = 0;
        USBHSD->UEP3_RX_CTRL = (USBHSD->UEP3_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
    }

    /* usb to eth */
    if( U2E_Trance_Manage.RemainPack )
    {
        u2e_deal_ptr = U2E_Trance_Manage.DealPtr;
        ret = ETH2USB_DataSend( (uint16_t)U2E_PackLen[ u2e_deal_ptr ], (uint32_t *)U2E_PackAdr[ u2e_deal_ptr ] );
        if( ret == ETH_SUCCESS )
        {
            __disable_irq( );
            __NOP( );__NOP( );
            U2E_Trance_Manage.DealPtr++;
            if( U2E_Trance_Manage.DealPtr >= ETH_TXBUFNB )
            {
                U2E_Trance_Manage.DealPtr = 0;
            }
            U2E_Trance_Manage.RemainPack--;
            __enable_irq( );
            __NOP( );__NOP( );
        }
    }

    if( USB_RxManage.Flag == 3 )
    {
        /* USB Out Restart, Set Out Ack */
        USB_RxManage.Flag = 0;
        USBHSD->UEP3_RX_CTRL = (USBHSD->UEP3_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
    }
}

/*********************************************************************
 * @fn      ETH2USB_Trance(based on hardware)
 *
 * @brief   Receive Eth data  
 *
 * @return  none
 */
void ETH2USB_DataRecv( void )
{
    uint8_t e2u_load_ptr;
    /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
    if((pDMARxSet->Status & ETH_DMARxDesc_OWN) == (u32)RESET)
    {
        /* Update the ETHERNET DMA global Rx descriptor with next Rx descriptor */
        /* Chained Mode */
        /* Selects the next DMA Rx descriptor list for next buffer to read */
        if(
        ((pDMARxSet->Status & ETH_DMARxDesc_ES) == (u32)RESET) &&
        ((pDMARxSet->Status & ETH_DMARxDesc_LS) != (u32)RESET) &&
        ((pDMARxSet->Status & ETH_DMARxDesc_FS) != (u32)RESET))
        {
            e2u_load_ptr = E2U_Trance_Manage.LoadPtr;
            E2U_PackAdr[ e2u_load_ptr ]   = pDMARxSet->Buffer1Addr;
            E2U_PackLen[ e2u_load_ptr ]   = ((pDMARxSet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;
            DMARxDealTabs[ e2u_load_ptr ] = pDMARxSet;
            E2U_Trance_Manage.LoadPtr++;
            if( E2U_Trance_Manage.LoadPtr >= DEF_E2U_MAXBLOCKS )
            {
                E2U_Trance_Manage.LoadPtr = 0;
            }
            E2U_Trance_Manage.RemainPack++;
            E2U_PackCnounter ++;
            if( E2U_Trance_Manage.RemainPack >= (DEF_E2U_MAXBLOCKS - DEF_E2U_REMINE) )
            {
                /* Throw away the oldest packages */
                E2U_Trance_Manage.RemainPack = DEF_E2U_MAXBLOCKS - DEF_E2U_REMINE;
                /* Send a Flow-Control Frame */
                ETH_InitiatePauseControlFrame( );
            }
        }
        else
        {
            /* Return ERROR */
            printf("Error:recv error frame,status 0x%08x.\n",pDMARxSet->Status);
            pDMARxSet->Status |= ETH_DMARxDesc_OWN;
        }
        pDMARxSet = (ETH_DMADESCTypeDef*) (pDMARxSet->Buffer2NextDescAddr);
    }
    else
    {
        /* unusual status, unexpected error */
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
    
    _PackNTB( );
    
    /* eth to usb */
    if( USB_TxManage.Flag == 1 )
    {
        if( USB_TxManage.Length >= USBHS_DevMaxPackLen )
        {
            /* pack size >= USBHS_DevMaxPackLen */
            ret = USBHS_EP2_UpLoad( USBHS_DevMaxPackLen, USB_TxManage.Address );
            if( ret == 0 )
            {
                /* this pack up load success, goto usb irq-hander for next deals */
                USB_TxManage.Length -= USBHS_DevMaxPackLen;
                USB_TxManage.Address += USBHS_DevMaxPackLen;
            }
        }
        else
        {
            /* Pack size < USBHS_DevMaxPackLen */
            ret = USBHS_EP2_UpLoad( USB_TxManage.Length, USB_TxManage.Address );
            if( ret == 0 )
            {
                USB_TxManage.Flag = 0;
            }
        }
    }
}

/*********************************************************************
 * @fn      Timer_Init
 *
 * @brief   Initializes Timer, we use tim2 in this example.
 *
 * @return  none
 */
void Timer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
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
    static uint8_t last_sat = 0;
    static uint16_t Loacl_Timer = 0;
    static uint8_t  loadflag = 0;

    /* Phy status update */
    Loacl_Timer++;
    if( Loacl_Timer == DEF_PHY_QUERY_TIMEOUT )
    {
        Loacl_Timer = 0;

        if( last_sat != LinkSta )
        {
            last_sat = LinkSta;
            /* ETH Status Update */
            if( LinkSta )
            {
                ETH_NETWork_Status |= DEF_NETSTAT_LINK_RDY;
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
                ETH_NETWork_Status = 0;
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
            usbret = USBHS_EP1_UpLoad( 8, (uint32_t)(uint8_t *)ECM_NetWork_Connection );
            if( usbret == 0 )
            {
                loadflag = 1;
            }
        }
        else
        {
            /* Load speed Status */
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

    /* Ring buffer init */
    RB_Init( );
    /* Used for Time Base */
    Timer_Init( );
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
    if( USBHS_DevSpeed == USBHS_SPEED_FULL )
    {
        USBETH_Limit_Spd( 1 );
    }
    else
    {
        USBETH_Limit_Spd( 0 );
    }
}

/*********************************************************************
 * @fn      ETH_PhyAbility_Set
 *
 * @brief   Set phy ability if necessary
 *
 * @return  none.
 */
 void USBETH_Main( void )
 {
    if( USBHS_DevEnumStatus && ( PhyInit_Flag == 0 ) )
    {
        printf( "Reset\r\n" );
        /* MAC&Phy Initialize  */
        PhyInit_Flag = 1;
        ETH_NETWork_Status = 0;
        ETH_DriverInit( MACAddr );
        ETH_PhyAbility_Set( );
    }
    if( PhyInit_Flag )
    {
        USBETH_MainTask( );
    }
    if( LinkSta )
    {
        ETH2USB_DataRecv( );
        ETH2USB_Trance( );
        USB2ETH_Trance( );
    }
 }