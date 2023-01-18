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

__attribute__ ((aligned(4))) uint8_t ECM_NetWork_Speed_Change[ 16 ] = { 0xA1, 0X2A, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  /* ECM速度变化状态上传 */
__attribute__ ((aligned(4))) uint8_t ECM_NetWork_Connection[ 8 ]    = { 0xA1, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00 };

volatile uint8_t ETH_LastStatus;
volatile uint8_t ECM_Pack_Filter;

uint32_t u2e_adr_store[DEF_U2E_MAXBLOCKS];
uint32_t e2u_adr_store[DEF_E2U_MAXBLOCKS];

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
    ETH->MACCR |= ETH_PromiscuousMode_Enable;
    ETH->MACCR |= ETH_ReceiveAll_Enable;
}

/*********************************************************************
 * @fn      ECM_Load_Status
 *
 * @brief   Load ECM NetWork Status
 *
 * @return  none
 */
void ECM_Load_Status( uint8_t eth_sta )
{
    static uint8_t loadflag = 0;
    uint8_t usbret;
    uint8_t eth_status = eth_sta;

    if( loadflag == 0 )
    {
        if( ETH_LastStatus != eth_status )
        {
            ETH_LastStatus = eth_status;
            if( eth_status & DEF_NETSTAT_LINK_RDY )
            {
                ECM_Link_Status->wvalue = DEF_ECM_NETWORK_CONNECT;
                if( eth_status & DEF_NETSTAT_100MBITS )
                {
                    ECM_SPD_Change->dlspeed = DEF_ECM_CONN_SPD_100M;
                    ECM_SPD_Change->ulspeed = DEF_ECM_CONN_SPD_100M;
                }
                else
                {
                    ECM_SPD_Change->dlspeed = DEF_ECM_CONN_SPD_10M;
                    ECM_SPD_Change->ulspeed = DEF_ECM_CONN_SPD_10M;
                }
                loadflag = 0x01 | 0x02;
            }
            else
            {
                ECM_Link_Status->wvalue = DEF_ECM_NETWORK_DISCONN;
                loadflag = 0x01;
            }
        }
        else
        {
            loadflag = 0x01;
        }
    }

    if( loadflag & 0x01 )
    {
        /* Load Link Status */
        usbret = ECM_Status_USB_UpLoad( 8, (uint32_t)(uint8_t *)ECM_NetWork_Connection );
        if( usbret == 0 )
        {
            loadflag &= ~(0x01);
        }
    }
    else if( loadflag & 0x02 )
    {
        /* Load speed Status */
        usbret = ECM_Status_USB_UpLoad( 16, (uint32_t)(uint8_t *)ECM_NetWork_Speed_Change );
        if( usbret == 0 )
        {
            loadflag &= ~(0x02);
        }
    }
    else
    {
        loadflag = 0;
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
        u2e_adr_store[ i ] = (uint32_t)(uint8_t *)(&MACTxBuf[ i*DEF_ETH_USBPACK ]);
    }

    E2U_Trance_Manage.LoadPtr = 0;
    E2U_Trance_Manage.DealPtr = 0;
    E2U_Trance_Manage.RemainPack = 0;
    E2U_Trance_Manage.StopFlag = 0;
    for( i=0; i<DEF_E2U_MAXBLOCKS; i++ )
    {
        E2U_PackLen[ i ] = 0;
        E2U_PackAdr[ i ] = (uint32_t)(uint8_t *)(&MACRxBuf[ i*DEF_ETH_USBPACK ]);
        e2u_adr_store[ i ] = (uint32_t)(uint8_t *)(&MACRxBuf[ i*DEF_ETH_USBPACK ]);
    }
    loadlock = 0x00;
    U2E_PackCnounter = 0;
    E2U_PackCnounter = 0;
}

/*********************************************************************
 * @fn      Check_Adr
 *
 * @brief   Check Buffer address if miss alined
 *
 * @return  none
 */
uint8_t Check_Adr( uint8_t if_u2e, uint32_t addr, uint8_t ptr )
{
    if( if_u2e )
    {
        if( u2e_adr_store[ ptr ] == addr )
        {
            return 0;
        }
        else
        {
            if( addr & 4 )
            {
                printf( "miss alined %08x, should be %08x\r\n", addr, u2e_adr_store[ ptr ] );
            }
            return 1;
        }
    }
    else
    {
        if( e2u_adr_store[ ptr ] == addr )
        {
            return 0;
        }
        else
        {
            if( addr & 4 )
            {
                printf( "miss alined %08x, should be %08x\r\n", addr, e2u_adr_store[ ptr ] );
            }
            return 1;
        }
    }
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
        ret = ETH_TxPktChainMode( (uint16_t)U2E_PackLen[ u2e_deal_ptr ], U2E_PackAdr[ u2e_deal_ptr ] + DEF_U2E_PACKHEADOFFSET );
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
    /* eth to usb */
    if( E2U_Trance_Manage.RemainPack )
    {
        e2u_deal_ptr = E2U_Trance_Manage.DealPtr;
        if( E2U_PackLen[ e2u_deal_ptr ] >= DEF_USB_EP3_HS_SIZE )
        {
            /* pack size > 512 */
            ret = ETH2USB_USB_UpLoad( DEF_USB_EP3_HS_SIZE, E2U_PackAdr[ e2u_deal_ptr ] );
            if( ret == 0 )
            {
                /* this pack up load success, goto usb irq-hander for next deals */
                E2U_PackLen[ e2u_deal_ptr ] -= DEF_USB_EP3_HS_SIZE;
                E2U_PackAdr[ e2u_deal_ptr ] += DEF_USB_EP3_HS_SIZE;
                E2U_Trance_Manage.StopFlag = 1;
            }
        }
        else
        {
            /* Pack size <= 512 */
            ret = ETH2USB_USB_UpLoad( E2U_PackLen[ e2u_deal_ptr ], E2U_PackAdr[ e2u_deal_ptr ] );
            if( ret == 0 )
            {
                __disable_irq( );
                __NOP( );__NOP( );
                /* only need to upload once */
                DMARxDealTabs[ e2u_deal_ptr ]->Status |= ETH_DMARxDesc_OWN;
                E2U_PackLen[ e2u_deal_ptr ] = 0;
                E2U_PackAdr[ e2u_deal_ptr ] = 0;
                E2U_Trance_Manage.RemainPack--;
                E2U_Trance_Manage.DealPtr++;
                if( E2U_Trance_Manage.DealPtr >= DEF_E2U_MAXBLOCKS )
                {
                    E2U_Trance_Manage.DealPtr = 0;
                }
                __enable_irq( );
                __NOP( );__NOP( );
                E2U_Trance_Manage.StopFlag = 0;
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

