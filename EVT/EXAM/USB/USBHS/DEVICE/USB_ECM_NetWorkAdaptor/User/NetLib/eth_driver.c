/********************************** (C) COPYRIGHT *******************************
* File Name          : eth_driver.c
* Author             : WCH
* Version            : V1.3.0
* Date               : 2022/06/02
* Description        : eth program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "eth_driver.h"

 __attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];      /* MAC receive descriptor, 4-byte aligned*/
 __attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];      /* MAC send descriptor, 4-byte aligned */

#if DEF_USB_TRANCE
 __attribute__((__aligned__(4))) uint8_t  MACRxBuf[ETH_RXBUFNB*DEF_ETH_USBPACK];     /* MAC receive buffer, 4-byte aligned */
 __attribute__((__aligned__(4))) uint8_t  MACTxBuf[ETH_TXBUFNB*DEF_ETH_USBPACK];     /* MAC send buffer, 4-byte aligned */
#else
 __attribute__((__aligned__(4))) uint8_t  MACRxBuf[ETH_RXBUFNB*ETH_RX_BUF_SZE];      /* MAC receive buffer, 4-byte aligned */
 __attribute__((__aligned__(4))) uint8_t  MACTxBuf[ETH_TXBUFNB*ETH_TX_BUF_SZE];      /* MAC send buffer, 4-byte aligned */
#endif

uint16_t gPHYAddress;
uint32_t volatile LocalTime;

uint8_t  ETH_AutoNego_Flag = 0; /* 检测到线拔出、以太网复位、重新初始化，置1 */
uint8_t  ETH_NETWork_Status;
uint16_t LastPhyStatus = 0;
uint16_t LastPhyControl = 0;
uint32_t LastQueryPhyTime = 0;

uint32_t volatile U2E_PackCnounter;
uint32_t volatile E2U_PackCnounter;

/* Ring Buffer */
__attribute__((__aligned__(4))) RING_BUFF_COMM U2E_Trance_Manage;/* usb to eth 缓冲区管理 */
__attribute__((__aligned__(4))) uint32_t U2E_PackAdr[DEF_U2E_MAXBLOCKS];/* usb to eth 包地址记录 */
__attribute__((__aligned__(4))) uint32_t U2E_PackLen[DEF_U2E_MAXBLOCKS];/* usb to eth 包长度记录 */

__attribute__((__aligned__(4))) RING_BUFF_COMM E2U_Trance_Manage;/* eth to usb 缓冲区管理 */
__attribute__((__aligned__(4))) uint32_t E2U_PackAdr[DEF_E2U_MAXBLOCKS];/* eth to usb 包地址记录 */
__attribute__((__aligned__(4))) uint32_t E2U_PackLen[DEF_E2U_MAXBLOCKS];/* eth to usb 包长度记录 */

__attribute__((__aligned__(4))) ETH_DMADESCTypeDef *DMARxDealTabs[ETH_RXBUFNB];/* MAC recv descriptor, 4-byte aligned， eth to usb 接收描述符包状态归属 */

volatile uint8_t loadlock;

/*********************************************************************
 * @fn      PHY_GetMacAddr
 *
 * @brief   Get MAC address
 *
 * @return  none.
 */
void PHY_GetMacAddr( uint8_t *p )
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
 * @fn      ETH_PHYLink
 *
 * @brief
 *
 * @return  none
 */
void ETH_PHYLink( void )
{
    u32 phy_stat;

    phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
    LastPhyStatus = phy_stat;
    if( LastPhyStatus&PHY_Linked_Status )
    {
        ETH_NETWork_Status |= DEF_NETSTAT_LINK_RDY;
        if( phy_stat&PHY_AutoNego_Complete )
        {
            ETH_NETWork_Status |= DEF_NETSTAT_ANC_RDY;
            phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BCR );
            printf( "phy sat %04x\r\n", phy_stat );
            if( LastPhyControl !=  phy_stat)
            {
                LastPhyControl = phy_stat;

                /* PHY negotiation result */
                if(LastPhyControl&(1<<13))
                {
                    /* Link speed 100Mbits */
                    ETH->MACCR &= ~(ETH_Speed_100M|ETH_Speed_1000M);
                    ETH->MACCR |= ETH_Speed_100M;
                    ETH_NETWork_Status |= DEF_NETSTAT_100MBITS;
                }
                else
                {
                    /* Link speed 10Mbits */
                    ETH->MACCR &= ~(ETH_Speed_100M|ETH_Speed_1000M);
                    ETH_NETWork_Status &= ~DEF_NETSTAT_100MBITS;
                }
                if(LastPhyControl&(1<<8))
                {
                    /* Link Duplex full-duplex */
                    ETH->MACCR |= ETH_Mode_FullDuplex;
                    ETH_NETWork_Status |= DEF_NETSTAT_FULLDUPLEX;
                }
                else
                {
                    /* Link Duplex full-duplex */
                    ETH->MACCR &= ~ETH_Mode_FullDuplex;
                    ETH_NETWork_Status &= ~DEF_NETSTAT_FULLDUPLEX;
                }

            }
        }
    }
    else
    {
        if( (LastPhyStatus & PHY_AutoNego_Complete) == 0 )
        {
            ETH_NETWork_Status &= ~DEF_NETSTAT_ANC_RDY;
        }
        ETH_NETWork_Status &= ~DEF_NETSTAT_LINK_RDY;
        ETH_AutoNego_Flag = 1;
    }
}

/*********************************************************************
 * @fn      PHY_QueryPhySta
 *
 * @brief   Query external PHY status
 *
 * @return  none.
 */
void PHY_QueryPhySta( void )
{
    uint16_t phy_stat;
    static uint16_t Loacl_Timer = 0;
    Loacl_Timer++;
    if( Loacl_Timer == DEF_PHY_QUERY_TIMEOUT )
    {
        Loacl_Timer = 0;
        phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
        if(phy_stat != LastPhyStatus)
        {
            LastPhyStatus = phy_stat;
            ETH_PHYLink( );
        }
    }
}

/*********************************************************************
 * @fn      ETH_MIIPinInit
 *
 * @brief   PHY MII interface GPIO initialization.
 *
 * @return  none
 */
void ETH_MIIPinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);

    define_O(GPIOA,GPIO_Pin_2);                                                         /* MDC */
    define_O(GPIOC,GPIO_Pin_1);                                                         /* MDIO */

    define_I(GPIOC,GPIO_Pin_3);                                                         //txclk
    define_O(GPIOB,GPIO_Pin_11);                                                        //txen
    define_O(GPIOB,GPIO_Pin_12);                                                        //txd0
    define_O(GPIOB,GPIO_Pin_13);                                                        //txd1
    define_O(GPIOC,GPIO_Pin_2);                                                         //txd2
    define_O(GPIOB,GPIO_Pin_8);                                                         //txd3
    /* RX */
    define_I(GPIOA,GPIO_Pin_1);                                                         /* PA1 RXC */
    define_I(GPIOA,GPIO_Pin_7);                                                         /* PA7 RXDV */
    define_I(GPIOC,GPIO_Pin_4);                                                         /* RXD0 */
    define_I(GPIOC,GPIO_Pin_5);                                                         /* RXD1 */
    define_I(GPIOB,GPIO_Pin_0);                                                         /* RXD2 */
    define_I(GPIOB,GPIO_Pin_1);                                                         /* RXD3 */
    define_IPD(GPIOB,GPIO_Pin_10);                                                      /* RXER */

//    define_O(GPIOA,GPIO_Pin_0);                                                         /* PA0 */
//    define_O(GPIOA,GPIO_Pin_3);                                                         /* PA3 */
}

/*********************************************************************
 * @fn      ETH_RegInit
 *
 * @brief   ETH register initialization.
 *
 * @param   ETH_InitStruct:initialization struct.
 *          PHYAddress:PHY address.
 *
 * @return  Initialization status.
 */
uint32_t ETH_RegInit( ETH_InitTypeDef* ETH_InitStruct, uint16_t PHYAddress )
{
    uint32_t tmpreg = 0;

    /*---------------------- Physical layer configuration -------------------*/
    /* Set the SMI interface clock, set as the main frequency divided by 42  */
    tmpreg = ETH->MACMIIAR;
    tmpreg &= MACMIIAR_CR_MASK;
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div42;
    ETH->MACMIIAR = (uint32_t)tmpreg;

    /*------------------------ MAC register configuration  ----------------------- --------------------*/
    tmpreg = ETH->MACCR;
    tmpreg &= MACCR_CLEAR_MASK;
    tmpreg |= (uint32_t)(ETH_InitStruct->ETH_AutoNegotiation |
                  ETH_InitStruct->ETH_Watchdog |
                  ETH_InitStruct->ETH_Jabber |
                  ETH_InitStruct->ETH_InterFrameGap |
                  ETH_InitStruct->ETH_CarrierSense |
                  ETH_InitStruct->ETH_Speed |
                  ETH_InitStruct->ETH_ReceiveOwn |
                  ETH_InitStruct->ETH_LoopbackMode |
                  ETH_InitStruct->ETH_Mode |
                  ETH_InitStruct->ETH_ChecksumOffload |
                  ETH_InitStruct->ETH_RetryTransmission |
                  ETH_InitStruct->ETH_AutomaticPadCRCStrip |
                  ETH_InitStruct->ETH_BackOffLimit |
                  ETH_InitStruct->ETH_DeferralCheck);
    /* Write MAC Control Register */
    ETH->MACCR = (uint32_t)tmpreg;

    ETH->MACFFR = (uint32_t)(ETH_InitStruct->ETH_ReceiveAll |
                          ETH_InitStruct->ETH_SourceAddrFilter |
                          ETH_InitStruct->ETH_PassControlFrames |
                          ETH_InitStruct->ETH_BroadcastFramesReception |
                          ETH_InitStruct->ETH_DestinationAddrFilter |
                          ETH_InitStruct->ETH_PromiscuousMode |
                          ETH_InitStruct->ETH_MulticastFramesFilter |
                          ETH_InitStruct->ETH_UnicastFramesFilter);
    /*--------------- ETHERNET MACHTHR and MACHTLR Configuration ---------------*/
    /* Write to ETHERNET MACHTHR */
    ETH->MACHTHR = (uint32_t)ETH_InitStruct->ETH_HashTableHigh;
    /* Write to ETHERNET MACHTLR */
    ETH->MACHTLR = (uint32_t)ETH_InitStruct->ETH_HashTableLow;
    /*----------------------- ETHERNET MACFCR Configuration --------------------*/
    /* Get the ETHERNET MACFCR value */
    tmpreg = ETH->MACFCR;
    /* Clear xx bits */
    tmpreg &= MACFCR_CLEAR_MASK;
    tmpreg |= (uint32_t)((ETH_InitStruct->ETH_PauseTime << 16) |
                     ETH_InitStruct->ETH_ZeroQuantaPause |
                     ETH_InitStruct->ETH_PauseLowThreshold |
                     ETH_InitStruct->ETH_UnicastPauseFrameDetect |
                     ETH_InitStruct->ETH_ReceiveFlowControl |
                     ETH_InitStruct->ETH_TransmitFlowControl);
    ETH->MACFCR = (uint32_t)tmpreg;

    ETH->MACVLANTR = (uint32_t)(ETH_InitStruct->ETH_VLANTagComparison |
                               ETH_InitStruct->ETH_VLANTagIdentifier);

    tmpreg = ETH->DMAOMR;
    tmpreg &= DMAOMR_CLEAR_MASK;
    tmpreg |= (uint32_t)(ETH_InitStruct->ETH_DropTCPIPChecksumErrorFrame |
                    ETH_InitStruct->ETH_ReceiveStoreForward |
                    ETH_InitStruct->ETH_FlushReceivedFrame |
                    ETH_InitStruct->ETH_TransmitStoreForward |
                    ETH_InitStruct->ETH_TransmitThresholdControl |
                    ETH_InitStruct->ETH_ForwardErrorFrames |
                    ETH_InitStruct->ETH_ForwardUndersizedGoodFrames |
                    ETH_InitStruct->ETH_ReceiveThresholdControl |
                    ETH_InitStruct->ETH_SecondFrameOperate);
    ETH->DMAOMR = (uint32_t)tmpreg;

    ETH->DMABMR = (uint32_t)(ETH_InitStruct->ETH_AddressAlignedBeats |
                            ETH_InitStruct->ETH_FixedBurst |
                            ETH_InitStruct->ETH_RxDMABurstLength | /* !! if 4xPBL is selected for Tx or Rx it is applied for the other */
                            ETH_InitStruct->ETH_TxDMABurstLength |
                           (ETH_InitStruct->ETH_DescriptorSkipLength << 2) |
                            ETH_InitStruct->ETH_DMAArbitration |
                            ETH_DMABMR_USP);
    return ETH_SUCCESS;
}

/*********************************************************************
 * @fn      ETH_Configuration
 *
 * @brief   Ethernet configure.
 *
 * @return  none
 */
void ETH_Configuration( uint8_t *macAddr )
{
    NVIC_InitTypeDef  NVIC_InitTypeStructure = { 0 };
    ETH_InitTypeDef ETH_InitStructure;
    uint16_t timeout = 10000;
    LastPhyStatus = 0;
    LastPhyControl = 0;
    LastQueryPhyTime = 0;

    /* Enable Ethernet MAC clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC|RCC_AHBPeriph_ETH_MAC_Tx|RCC_AHBPeriph_ETH_MAC_Rx,ENABLE);
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_ETH_MAC, ENABLE);
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_ETH_MAC, DISABLE);
    gPHYAddress = PHY_ADDRESS;

    /* Software reset */
    ETH_SoftwareReset( );
    /* Wait for software reset */
    do{
        Delay_Us(10);
        if( !--timeout )  break;
    }while(ETH->DMABMR & ETH_DMABMR_SR);

    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();

    /* ETHERNET Configuration */
    /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
    ETH_StructInit(&ETH_InitStructure);
    /* Fill ETH_InitStructure parameters */
    /*------------------------   MAC   -----------------------------------*/
    ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
    ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Enable;
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
    /*------------------------   DMA   -----------------------------------*/
    /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
    the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum,
    if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
    ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Enable;
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Enable;
    ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Disable;
    ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
    ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
    ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
    ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
    /* Configure Ethernet */
    ETH_RegInit( &ETH_InitStructure, gPHYAddress );

    /* Configure MacAddr */
    ETH_MACAddressConfig( ETH_MAC_Address0, macAddr );

    /* NVIC proity config */
    NVIC_InitTypeStructure.NVIC_IRQChannel = ETH_IRQn;
    NVIC_InitTypeStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitTypeStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitTypeStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitTypeStructure );
    /* Enable the Ethernet Rx/Tx/NIS Interrupt */
    ETH_DMAITConfig( ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE );

    /*  Enable MII GPIO */
    ETH_MIIPinInit( );
    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);

    while (PHY_Reset == (PHY_Reset & ETH_ReadPHYRegister(PHY_ADDRESS, PHY_BCR)))
    {
    }
    ETH_AutoNego_Flag = 1;
}

/*********************************************************************
 * @fn      ETH_Query_AtuoNego
 *
 * @brief   Ethernet Query AtuoNego.
 *
 * @return  none
 */
void ETH_Query_AtuoNego( void )
{
    if( ETH_AutoNego_Flag )
    {
        if( PHY_AutoNego_Complete == ( PHY_AutoNego_Complete & ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BMSR ) ) )
        {
            /* Auto Nego Complete */
            ETH_AutoNego_Flag = 0;
            ETH_PHYLink( );
            ETH_Start( );
        }
    }
}

/*********************************************************************
 * @fn      ETH_TxPktChainMode
 *
 * @brief   process net send a Ethernet frame in chain mode.
 *
 * @param   Send length
 *
 * @return  Send status.
 */
uint32_t ETH_TxPktChainMode(uint16_t len, uint32_t pBuff )
{
    /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
    if( DMATxDescToSet->Status & ETH_DMATxDesc_OWN )
    {
        /* Return ERROR: OWN bit set */
        return ETH_ERROR;
    }
    /* Setting the Frame Length: bits[12:0] */
    DMATxDescToSet->ControlBufferSize = (len & ETH_DMATxDesc_TBS1);
    DMATxDescToSet->Buffer1Addr = (uint32_t)pBuff;
    /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

    /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
    if ((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)
    {
        /* Clear TBUS ETHERNET DMA flag */
        ETH->DMASR = ETH_DMASR_TBUS;
        /* Resume DMA transmission*/
        ETH->DMATPDR = 0;
    }
    /* Update the ETHERNET DMA global Tx descriptor with next Tx descriptor */
    /* Chained Mode */
    /* Selects the next DMA Tx descriptor list for next buffer to send */
    DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);
    /* Return SUCCESS */
    return ETH_SUCCESS;
}

/*********************************************************************
 * @fn      ETH_Isr
 *
 * @brief   Ethernet Interrupt Service program
 *
 * @return  none
 */
void ETH_Isr(void)
{
    uint32_t int_sta;
    uint8_t  e2u_load_ptr;
    int_sta = ETH->DMASR;
    if( int_sta & ETH_DMA_IT_NIS )
    {
        if( int_sta & ETH_DMA_IT_R )
        {
            if ((int_sta & ETH_DMA_IT_RBU) != (u32)RESET)
            {
                /* Clear RBUS ETHERNET DMA flag */
                ETH->DMASR = ETH_DMA_IT_RBU;
                /* Throw away the oldest packages */
                ((ETH_DMADESCTypeDef *)(((ETH_DMADESCTypeDef *)(ETH->DMACHRDR))->Buffer2NextDescAddr))->Status = ETH_DMARxDesc_OWN;
                E2U_Trance_Manage.RemainPack = DEF_E2U_MAXBLOCKS - DEF_E2U_REMINE;
                /* Resume DMA reception */
                ETH->DMARPDR = 0;
                printf( "Rx OWN\r\n" );
            }

            /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
            if((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) == (u32)RESET)
            {
                 /* Update the ETHERNET DMA global Rx descriptor with next Rx descriptor */
                 /* Chained Mode */
                 /* Selects the next DMA Rx descriptor list for next buffer to read */
                 if(
                    ((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (u32)RESET) &&
                    ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (u32)RESET) &&
                    ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (u32)RESET))
                 {
                     e2u_load_ptr = E2U_Trance_Manage.LoadPtr;
                     E2U_PackAdr[ e2u_load_ptr ]   = DMARxDescToGet->Buffer1Addr;
                     E2U_PackLen[ e2u_load_ptr ]   = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;
                     DMARxDealTabs[ e2u_load_ptr ] = DMARxDescToGet;
                     E2U_Trance_Manage.LoadPtr++;
                     if( E2U_Trance_Manage.LoadPtr >= DEF_E2U_MAXBLOCKS )
                     {
                         E2U_Trance_Manage.LoadPtr = 0;
                     }
                     E2U_Trance_Manage.RemainPack++;
                     if( E2U_Trance_Manage.RemainPack >= (DEF_E2U_MAXBLOCKS - DEF_E2U_REMINE) )
                     {
                         /* Throw away the oldest packages */
                         E2U_Trance_Manage.RemainPack = DEF_E2U_MAXBLOCKS - DEF_E2U_REMINE;
                     }
                 }
                 else
                 {
                     /* Return ERROR */
                     printf("Error:recv error frame,status：0x%08x.\n",DMARxDescToGet->Status);
                 }
                 DMARxDescToGet = (ETH_DMADESCTypeDef*) (DMARxDescToGet->Buffer2NextDescAddr);

            }
            else
            {
                /* unusual status, unexpected error */
            }

            ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
        }
        ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
    }
}

/*********************************************************************
 * @fn      ETH_Init
 *
 * @brief   Ethernet initialization.
 *
 * @return  none
 */
void ETH_Init( uint8_t *macAddr )
{
    uint8_t i;
    Delay_Ms(100);
    ETH_Configuration( macAddr );
    ETH_DMATxDescChainInit( DMATxDscrTab, MACTxBuf, ETH_TXBUFNB );
    ETH_DMARxDescChainInit( DMARxDscrTab, MACRxBuf, ETH_RXBUFNB );

    printf( "TxDesc:\r\n" );
    for( i=0; i<ETH_TXBUFNB; i++ )
    {
        printf( "%08x\r\n", DMATxDscrTab[i].Buffer1Addr );
        U2E_PackAdr[i] = DMATxDscrTab[i].Buffer1Addr;
    }
    printf( "RxDesc:\r\n" );
    for( i=0; i<ETH_RXBUFNB; i++ )
    {
        printf( "%08x\r\n", DMARxDscrTab[i].Buffer1Addr );
        E2U_PackAdr[ i ] = DMARxDscrTab[i].Buffer1Addr;
    }

    NVIC_EnableIRQ(ETH_IRQn);
}

/******************************** endfile @ eth_driver ******************************/
