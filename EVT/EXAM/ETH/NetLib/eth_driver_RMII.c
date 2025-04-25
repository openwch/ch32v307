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

#include "string.h"
#include "eth_driver.h"

__attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];       /* MAC receive descriptor, 4-byte aligned*/
__attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];       /* MAC send descriptor, 4-byte aligned */

__attribute__((__aligned__(4))) uint8_t  MACRxBuf[ETH_RXBUFNB*ETH_RX_BUF_SZE];      /* MAC receive buffer, 4-byte aligned */
__attribute__((__aligned__(4))) uint8_t  MACTxBuf[ETH_TXBUFNB*ETH_TX_BUF_SZE];      /* MAC send buffer, 4-byte aligned */

__attribute__((__aligned__(4))) SOCK_INF SocketInf[WCHNET_MAX_SOCKET_NUM];          /* Socket information table, 4-byte alignment */
const uint16_t MemNum[8] = {WCHNET_NUM_IPRAW,
                         WCHNET_NUM_UDP,
                         WCHNET_NUM_TCP,
                         WCHNET_NUM_TCP_LISTEN,
                         WCHNET_NUM_TCP_SEG,
                         WCHNET_NUM_IP_REASSDATA,
                         WCHNET_NUM_PBUF,
                         WCHNET_NUM_POOL_BUF
                         };
const uint16_t MemSize[8] = {WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IPRAW_PCB),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_UDP_PCB),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_PCB_LISTEN),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_TCP_SEG),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_IP_REASSDATA),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF),
                          WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_PBUF) + WCHNET_MEM_ALIGN_SIZE(WCHNET_SIZE_POOL_BUF)
                         };
__attribute__((__aligned__(4)))uint8_t Memp_Memory[WCHNET_MEMP_SIZE];
__attribute__((__aligned__(4)))uint8_t Mem_Heap_Memory[WCHNET_RAM_HEAP_SIZE];
__attribute__((__aligned__(4)))uint8_t Mem_ArpTable[WCHNET_RAM_ARP_TABLE_SIZE];

uint16_t gPHYAddress;
uint32_t ChipId = 0;
uint32_t volatile LocalTime;

ETH_DMADESCTypeDef *pDMARxSet;
ETH_DMADESCTypeDef *pDMATxSet;

extern u8 MACAddr[6];
volatile uint8_t LinkSta = 0;  //0:Link down 1:Link up
uint8_t LinkVaildFlag = 0;  //0:invalid 1:valid
uint8_t AccelerateLinkStep = 0;
uint8_t AccelerateLinkTime = 0;
uint8_t LinkProcessingStep = 0;
uint32_t LinkProcessingTime = 0;
uint32_t TaskExecutionTime = 0;

#if !LINK_STAT_ACQUISITION_METHOD
u16 LastPhyStat = 0;
u32 LastQueryPhyTime = 0;
#endif
uint8_t PhyWaitNegotiationSuc = 0;
void ETH_LinkDownCfg(void);
void PHY_FuncInit(void);
/*********************************************************************
 * @fn      WCHNET_GetMacAddr
 *
 * @brief   Get the MAC address
 *
 * @return  none.
 */
void WCHNET_GetMacAddr( uint8_t *p )
{
    uint8_t i;
    uint8_t *macaddr = (uint8_t *)(ROM_CFG_USERADR_ID+5);

    for(i=0;i<6;i++)
    {
        *p = *macaddr;
        p++;
        macaddr--;
    }
}

/*********************************************************************
 * @fn      WCHNET_TimeIsr
 *
 * @brief
 *
 * @return  none.
 */
void WCHNET_TimeIsr( uint16_t timperiod )
{
    LocalTime += timperiod;
}

/*********************************************************************
 * @fn      WCHNET_QueryPhySta
 *
 * @brief   Query external PHY status
 *
 * @return  none.
 */

void WCHNET_QueryPhySta(void)
{
#if LINK_STAT_ACQUISITION_METHOD
    if(PhyWaitNegotiationSuc)
    {
        ETH_PHYLink();
    }
#else
    u16 phy_stat;
    if(QUERY_STAT_FLAG){                                         /* Query the PHY link status every 1s */
        LastQueryPhyTime = LocalTime / 1000;
        phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
        if((phy_stat != LastPhyStat) && (phy_stat != 0xffff)){
            ETH_PHYLink();
        }
    }
#endif
}

/*********************************************************************
 * @fn      WCHNET_CheckPHYPN
 *
 * @brief   check PHY PN polarity
 *
 * @return  none.
 */
void WCHNET_CheckPHYPN(uint16_t time)
{
    u16 phy_stat;

    //check PHY PN
    if((LinkProcessingStep == 0)||(LocalTime >= LinkProcessingTime))
    {
        ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x0 );
        phy_stat = ETH_ReadPHYRegister( gPHYAddress, 0x10);
        if(phy_stat & (1<<12))
        {
            if(LinkProcessingStep == 0)
            {
                LinkProcessingStep = 1;
                LinkProcessingTime = LocalTime + time;
            }
            else {
                LinkProcessingStep = 0;
                LinkProcessingTime = 0;
                phy_stat = ETH_ReadPHYRegister( gPHYAddress, PHY_ANER);
                if((time == 200) || ((phy_stat & 1) == 0))
                {
#if LINK_STAT_ACQUISITION_METHOD
                    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x07 );
                    /* Configure interrupt function */
                    phy_stat = ETH_ReadPHYRegister(gPHYAddress, 0x13);
                    phy_stat &= ~(0x01 << 13);
                    ETH_WritePHYRegister(gPHYAddress, 0x13, phy_stat );
#endif
                    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x0 );
                    phy_stat = ETH_ReadPHYRegister( gPHYAddress, 0x16);
                    phy_stat |= 1<<5;
                    ETH_WritePHYRegister(gPHYAddress, 0x16, phy_stat );

                    phy_stat = ETH_ReadPHYRegister( gPHYAddress, 0x16);
                    phy_stat &= ~(1<<5);
                    ETH_WritePHYRegister(gPHYAddress, 0x16, phy_stat );

                    phy_stat = ETH_ReadPHYRegister( gPHYAddress, 0x1E);   /* Clear the Interrupt status */

#if LINK_STAT_ACQUISITION_METHOD
                    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x07 );
                    /* Configure interrupt function */
                    phy_stat = ETH_ReadPHYRegister(gPHYAddress, 0x13);
                    phy_stat |= 0x01 << 13;
                    ETH_WritePHYRegister(gPHYAddress, 0x13, phy_stat );
#endif
                }
            }
        }
        else {
            LinkProcessingStep = 0;
            LinkProcessingTime = 0;
        }
    }
}

/*********************************************************************
 * @fn      WCHNET_AccelerateLink
 *
 * @brief   accelerate Link processing
 *
 * @return  none.
 */
void WCHNET_AccelerateLink(void)
{
    uint16_t phy_stat;

    switch(AccelerateLinkStep)
    {
        case 0:
            AccelerateLinkStep++;
            AccelerateLinkTime = 0;

            ETH_WritePHYRegister(PHY_ADDRESS, PHY_PAG_SEL, 0x00 );
            phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, 0x18);
            phy_stat &= ~(1<<15);
            ETH_WritePHYRegister(PHY_ADDRESS, 0x18, phy_stat );

            //power down
            phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BCR);
            phy_stat |= (1<<11);
            ETH_WritePHYRegister(PHY_ADDRESS, PHY_BCR, phy_stat );

            //decrease Link Time
            ETH_WritePHYRegister(PHY_ADDRESS, PHY_PAG_SEL, 0x00 );
            ETH_WritePHYRegister(PHY_ADDRESS, 0x13, 0x4 );
            break;

        case 1:
            if(AccelerateLinkTime++ > 120) //unit:10ms,total time:1.2s~1.5s
            {
                AccelerateLinkStep++;
                ETH_WritePHYRegister(PHY_ADDRESS, PHY_PAG_SEL, 0x00 );
                phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, 0x18);
                phy_stat |= 1<<15;
                ETH_WritePHYRegister(PHY_ADDRESS, 0x18, phy_stat );
                // power up
                phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BCR);
                phy_stat &= ~(1<<11);
                ETH_WritePHYRegister(PHY_ADDRESS, PHY_BCR, phy_stat );

                Delay_Us(300);
                PHY_FuncInit();
            }
            break;

        case 2:
            ETH_WritePHYRegister(PHY_ADDRESS, PHY_PAG_SEL, 99 );
            phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, 0x19);
            if((phy_stat & 0xf) == 2)
            {
                AccelerateLinkStep++;
                ETH_WritePHYRegister(PHY_ADDRESS, PHY_PAG_SEL, 0x00 );
                ETH_WritePHYRegister(PHY_ADDRESS, 0x13, 0x0 );
            }
            break;

        default:
            /*do nothing*/
            break;
    }
}

/*********************************************************************
 * @fn      WCHNET_CheckLinkVaild
 *
 * @brief   check whether Link is valid
 *
 * @return  none.
 */
void WCHNET_CheckLinkVaild(void)
{
    uint16_t phy_stat, phy_bcr;

    if(LinkVaildFlag == 0)
    {
        phy_bcr = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BCR);
        if((phy_bcr & (1<<13)) == 0)   //Do nothing if Link mode is 10M.
        {
            LinkVaildFlag = 1;
            LinkProcessingTime = 0;
            return;
        }
        ETH_WritePHYRegister(gPHYAddress, 0x1F, 0 );
        phy_stat = ETH_ReadPHYRegister( gPHYAddress, 0x10);
        if((phy_stat & (1<<9)) == 0)
        {
            LinkProcessingTime++;
            if(LinkProcessingTime == 5)
            {
                LinkProcessingTime = 0;
                phy_stat = ETH_ReadPHYRegister(gPHYAddress, PHY_BCR);
                ETH_WritePHYRegister(gPHYAddress, PHY_BCR, PHY_Reset );
                Delay_Us(100);
                ETH_WritePHYRegister(gPHYAddress, PHY_BCR, phy_stat );
                ETH_LinkDownCfg();
            }
        }
        else {
            LinkVaildFlag = 1;
            LinkProcessingTime = 0;
        }
    }
}

/*********************************************************************
 * @fn      WCHNET_LinkProcessing
 *
 * @brief   process Link stage task
 *
 * @return  none.
 */
void WCHNET_LinkProcessing(void)
{
    u16 phy_bcr;

    if(LocalTime >= TaskExecutionTime)
    {
        TaskExecutionTime = LocalTime + 10;         //execution cycle:10ms
        if(LinkSta == 0)                            //Link down
        {
            phy_bcr = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BCR);
            if(phy_bcr & PHY_AutoNegotiation)       //auto-negotiation is enabled
            {
                WCHNET_CheckPHYPN(300);             //check PHY PN
                WCHNET_AccelerateLink();            //accelerate Link processing
            }
            else {                                  //auto-negotiation is disabled
                if((phy_bcr & (1<<13)) == 0)        // 10M
                {
                    WCHNET_CheckPHYPN(200);         //check PHY PN
                }
            }
        }
        else {                                      //Link up
            WCHNET_CheckLinkVaild();                //check whether Link is valid
        }
    }
}

/*********************************************************************
 * @fn      WCHNET_MainTask
 *
 * @brief   library main task function
 *
 * @param   none.
 *
 * @return  none.
 */
void WCHNET_MainTask(void)
{
    WCHNET_NetInput( );                     /* Ethernet data input */
    WCHNET_PeriodicHandle( );               /* Protocol stack time-related task processing */
    WCHNET_QueryPhySta();                   /* Query external PHY status */
    WCHNET_LinkProcessing();                /* process Link stage task */
}

/*********************************************************************
 * @fn      ETH_RMIIPinInit
 *
 * @brief   PHY RMII interface GPIO initialization.
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_RMIIPinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);
    GPIO_Output(GPIOA, GPIO_Pin_2);                                                         /* MDIO */
    GPIO_Output(GPIOC, GPIO_Pin_1);                                                         /* MDC */

    GPIO_Output(GPIOB, GPIO_Pin_11);                                                        /* TXEN */
    GPIO_Output(GPIOB, GPIO_Pin_12);                                                        /* TXD0 */
    GPIO_Output(GPIOB, GPIO_Pin_13);                                                        /* TXD1 */

    GPIO_Input(GPIOA, GPIO_Pin_1);                                                          /* REFCLK */
    GPIO_Input(GPIOA, GPIO_Pin_7);                                                          /* CRSDV */
    GPIO_Input(GPIOC, GPIO_Pin_4);                                                          /* RXD0 */
    GPIO_Input(GPIOC, GPIO_Pin_5);                                                          /* RXD1 */
}

#if LINK_STAT_ACQUISITION_METHOD
/*********************************************************************
 * @fn      EXTI_Line_Init
 *
 * @brief   Configure EXTI Line7.
 *
 * @param   none.
 *
 * @return  none.
 */
void EXTI_Line_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* GPIOC 7 ----> EXTI_Line7 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*********************************************************************
 * @fn      PHY_InterruptInit
 *
 * @brief   Configure PHY interrupt function,Supported chip is:RTL8211FS
 *
 * @param   none.
 *
 * @return  none.
 */
void PHY_InterruptInit(void)
{
    uint16_t RegValue;

    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x07 );
    /* Configure interrupt function */
    RegValue = ETH_ReadPHYRegister(gPHYAddress, 0x13);
    RegValue |= 0x01 << 13;
    ETH_WritePHYRegister(gPHYAddress, 0x13, RegValue );
    /* Clear the Interrupt status */
    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x00 );
    ETH_ReadPHYRegister( gPHYAddress, 0x1E);
}
#endif

/*********************************************************************
 * @fn      ETH_LinkUpCfg
 *
 * @brief   When the PHY is connected, configure the relevant functions.
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_LinkUpCfg(void)
{
    uint16_t phy_stat;

    phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BCR );
    /* PHY negotiation result */
    if(phy_stat & (1<<13))                                  /* 100M */
    {
        ETH->MACCR &= ~(ETH_Speed_100M|ETH_Speed_1000M);
        ETH->MACCR |= ETH_Speed_100M;
    }
    else                                                    /* 10M */
    {
        ETH->MACCR &= ~(ETH_Speed_100M|ETH_Speed_1000M);
    }
    if(phy_stat & (1<<8))                                   /* full duplex */
    {
        ETH->MACCR |= ETH_Mode_FullDuplex;
    }
    else                                                    /* half duplex */
    {
        ETH->MACCR &= ~ETH_Mode_FullDuplex;
    }

    LinkSta = 1;
    AccelerateLinkStep = 0;
    LinkProcessingStep = 0;
    LinkProcessingTime = 0;
    PhyWaitNegotiationSuc = 0;
    ETH_Start( );

    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x0 );
    phy_stat = 0x0;
    ETH_WritePHYRegister(gPHYAddress, 0x13, phy_stat );
}

/*********************************************************************
 * @fn      ETH_LinkDownCfg
 *
 * @brief   When the PHY is disconnected, configure the relevant functions.
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_LinkDownCfg(void)
{
    LinkSta = 0;
    LinkVaildFlag = 0;
    LinkProcessingTime = 0;
}

/*********************************************************************
 * @fn      ETH_PHYLink
 *
 * @brief   Configure MAC parameters after the PHY Link is successful.
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_PHYLink( void )
{
    u32 phy_stat, phy_anlpar, phy_bcr;
#if !LINK_STAT_ACQUISITION_METHOD
    uint8_t timeout = 0;
#endif
    phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
    phy_anlpar = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_ANLPAR);
    phy_bcr = ETH_ReadPHYRegister( gPHYAddress, PHY_BCR);

#if !LINK_STAT_ACQUISITION_METHOD
    if((ChipId & 0xf0) <= 0x20)
    {
        while(phy_stat == 0)
        {
            Delay_Us(100);
            phy_stat = ETH_ReadPHYRegister( gPHYAddress, PHY_BSR);
            if(timeout++ == 15)   break;
        }
        if(LastPhyStat == phy_stat) return;
    }
    LastPhyStat = phy_stat;
#endif

    WCHNET_PhyStatus( phy_stat );
    if(phy_stat & PHY_Linked_Status)   //LinkUp
    {
        if(phy_bcr & PHY_AutoNegotiation)
        {
            if(phy_anlpar == 0)
            {
                ETH_LinkUpCfg();
            }
            else {
                if(phy_stat & PHY_AutoNego_Complete)
                {
                    ETH_LinkUpCfg();
                }
                else{
                    PhyWaitNegotiationSuc = 1;
                }
            }
        }
        else {
            ETH_LinkUpCfg();
        }
    }
    else {                              //LinkDown
        /*Link down*/
        ETH_LinkDownCfg();
    }
    phy_stat = ETH_ReadPHYRegister( gPHYAddress, 0x1E);   /* Clear the Interrupt status */
}

/*********************************************************************
 * @fn      PHY_FuncInit
 *
 * @brief   PHY register initialization.
 *
 * @param   None
 *
 * @return  None.
 */
void PHY_FuncInit(void)
{
    uint16_t regval;
    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x00 );
    /* Configure Repeater mode */
    regval = ETH_ReadPHYRegister(gPHYAddress, 28);
    regval |= 1<<13;
    ETH_WritePHYRegister(gPHYAddress, 28, regval );

    /*rmii rx clock change */
    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x07 );
    regval = ETH_ReadPHYRegister( gPHYAddress, 16);
    regval &= ~(0x0f<<4);
    regval |= 0x04<<4;
    ETH_WritePHYRegister(gPHYAddress, 16, regval );

#if LINK_STAT_ACQUISITION_METHOD
    /* Configure the PHY interrupt function, the supported chip is: CH182H RMII */
    PHY_InterruptInit( );
    /* Configure EXTI Line7. */
    EXTI_Line_Init( );
#else
    /*Reads the default value of the PHY_BSR register*/
    LastPhyStat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
#endif
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
    uint16_t tmpreg = 0;
    /* Set the SMI interface clock, set as the main frequency divided by 42  */
    ETH->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div42;

    /*------------------------ MAC register configuration  -------------------------------------------*/
    ETH->MACCR = (uint32_t)(ETH_InitStruct->ETH_Watchdog |
                  ETH_InitStruct->ETH_Jabber |
                  ETH_InitStruct->ETH_InterFrameGap |
                  ETH_InitStruct->ETH_ChecksumOffload |
                  ETH_InitStruct->ETH_AutomaticPadCRCStrip |
                  ETH_InitStruct->ETH_LoopbackMode);

    ETH->MACFFR = (uint32_t)(ETH_InitStruct->ETH_ReceiveAll |
                          ETH_InitStruct->ETH_SourceAddrFilter |
                          ETH_InitStruct->ETH_PassControlFrames |
                          ETH_InitStruct->ETH_BroadcastFramesReception |
                          ETH_InitStruct->ETH_DestinationAddrFilter |
                          ETH_InitStruct->ETH_PromiscuousMode |
                          ETH_InitStruct->ETH_MulticastFramesFilter |
                          ETH_InitStruct->ETH_UnicastFramesFilter);

    ETH->MACHTHR = (uint32_t)ETH_InitStruct->ETH_HashTableHigh;
    ETH->MACHTLR = (uint32_t)ETH_InitStruct->ETH_HashTableLow;

    ETH->MACFCR = (uint32_t)((ETH_InitStruct->ETH_PauseTime << 16) |
                     ETH_InitStruct->ETH_UnicastPauseFrameDetect |
                     ETH_InitStruct->ETH_ReceiveFlowControl |
                     ETH_InitStruct->ETH_TransmitFlowControl);

    ETH->MACVLANTR = (uint32_t)(ETH_InitStruct->ETH_VLANTagComparison |
                               ETH_InitStruct->ETH_VLANTagIdentifier);

    ETH->DMAOMR = (uint32_t)(ETH_InitStruct->ETH_DropTCPIPChecksumErrorFrame |
                    ETH_InitStruct->ETH_TransmitStoreForward |
                    ETH_InitStruct->ETH_ForwardErrorFrames |
                    ETH_InitStruct->ETH_ForwardUndersizedGoodFrames);

    /* Reset the physical layer */
    ETH_WritePHYRegister(PHYAddress, PHY_BCR, PHY_Reset);
    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x00 );
    tmpreg = ETH_ReadPHYRegister(gPHYAddress, 24);
    if(tmpreg & (1<<1)) ETH_WritePHYRegister(PHYAddress, PHY_BCR, 0x3100);
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
    ETH_InitTypeDef ETH_InitStructure;
    uint16_t timeout = 10000;

    /* Enable Ethernet MAC clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | \
                          RCC_AHBPeriph_ETH_MAC_Tx | \
                          RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);

    gPHYAddress = PHY_ADDRESS;

    /* Enable RMII GPIO */
    ETH_RMIIPinInit();

    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();

    /* Software reset */
    ETH_SoftwareReset();

    /* Wait for software reset */
    do{
        Delay_Us(10);
        if( !--timeout )  break;
    }while(ETH->DMABMR & ETH_DMABMR_SR);

    /* ETHERNET Configuration */
    /*------------------------   MAC   -----------------------------------*/
    ETH_InitStructure.ETH_Watchdog = ETH_Watchdog_Enable;
    ETH_InitStructure.ETH_Jabber = ETH_Jabber_Enable;
    ETH_InitStructure.ETH_InterFrameGap = ETH_InterFrameGap_96Bit;
#if HARDWARE_CHECKSUM_CONFIG
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#else
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Disable;
#endif
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;

    /* Filter function configuration */
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
    ETH_InitStructure.ETH_PassControlFrames = ETH_PassControlFrames_BlockAll;
    ETH_InitStructure.ETH_DestinationAddrFilter = ETH_DestinationAddrFilter_Normal;
    ETH_InitStructure.ETH_SourceAddrFilter = ETH_SourceAddrFilter_Disable;

    ETH_InitStructure.ETH_HashTableHigh = 0x0;
    ETH_InitStructure.ETH_HashTableLow = 0x0;

    /* VLan function configuration */
    ETH_InitStructure.ETH_VLANTagComparison = ETH_VLANTagComparison_16Bit;
    ETH_InitStructure.ETH_VLANTagIdentifier = 0x0;

    /* Flow Control function configuration */
    ETH_InitStructure.ETH_PauseTime = 0x0;
    ETH_InitStructure.ETH_UnicastPauseFrameDetect = ETH_UnicastPauseFrameDetect_Disable;
    ETH_InitStructure.ETH_ReceiveFlowControl = ETH_ReceiveFlowControl_Disable;
    ETH_InitStructure.ETH_TransmitFlowControl = ETH_TransmitFlowControl_Disable;

    /*------------------------   DMA   -----------------------------------*/
    /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
    the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum,
    if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Enable;
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Enable;
    /* Configure Ethernet */
    ETH_RegInit( &ETH_InitStructure, gPHYAddress );

    /* Configure MAC address */
    ETH->MACA0HR = (uint32_t)((macAddr[5]<<8) | macAddr[4]);
    ETH->MACA0LR = (uint32_t)(macAddr[0] | (macAddr[1]<<8) | (macAddr[2]<<16) | (macAddr[3]<<24));

    /* Mask the interrupt that Tx good frame count counter reaches half the maximum value */
    ETH->MMCTIMR = ETH_MMCTIMR_TGFM;
    /* Mask the interrupt that Rx good unicast frames counter reaches half the maximum value */
    /* Mask the interrupt that Rx crc error counter reaches half the maximum value */
    ETH->MMCRIMR = ETH_MMCRIMR_RGUFM | ETH_MMCRIMR_RFCEM;

    ETH_DMAITConfig(ETH_DMA_IT_NIS |\
                ETH_DMA_IT_R |\
                ETH_DMA_IT_T |\
                ETH_DMA_IT_AIS |\
                ETH_DMA_IT_RBU,\
                ENABLE);

    PHY_FuncInit();
}

/*********************************************************************
 * @fn      ETH_TxPktChainMode
 *
 * @brief   Ethernet sends data frames in chain mode.
 *
 * @param   len     Send data length
 *          pBuff   send buffer pointer
 *
 * @return  Send status.
 */
uint32_t ETH_TxPktChainMode(uint16_t len, uint32_t *pBuff )
{
    /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
    if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
    {
        /* Return ERROR: OWN bit set */
        return ETH_ERROR;
    }
    /* Setting the Frame Length: bits[12:0] */
    DMATxDescToSet->ControlBufferSize = (len & ETH_DMATxDesc_TBS1);
    DMATxDescToSet->Buffer1Addr = (uint32_t)pBuff;

    /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
#if HARDWARE_CHECKSUM_CONFIG
    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS | ETH_DMATxDesc_CIC_TCPUDPICMP_Full;
#else
    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;
#endif

    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

    /* Clear TBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    ETH->DMATPDR = 0;

    /* Update the ETHERNET DMA global Tx descriptor with next Tx descriptor */
    /* Chained Mode */
    /* Selects the next DMA Tx descriptor list for next buffer to send */
    DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);
    /* Return SUCCESS */
    return ETH_SUCCESS;
}

/*********************************************************************
 * @fn      ETH_Stop
 *
 * @brief   Disables ENET MAC and DMA reception/transmission.
 *
 * @return  none
 */
void ETH_Stop(void)
{
    ETH_MACTransmissionCmd(DISABLE);
    ETH_FlushTransmitFIFO();
    ETH_MACReceptionCmd(DISABLE);
}

/*********************************************************************
 * @fn      ReInitMACReg
 *
 * @brief   Reinitialize MAC register.
 *
 * @param   none.
 *
 * @return  none.
 */
void ReInitMACReg(void)
{
    uint16_t timeout = 10000;
    uint32_t maccr, macmiiar, macffr, machthr, machtlr;
    uint32_t macfcr, macvlantr, dmaomr;

    /* Wait for sending data to complete */
    while((ETH->DMASR & (7 << 20)) != ETH_DMA_TransmitProcess_Suspended);

    ETH_Stop();

    /* Record the register value  */
    macmiiar = ETH->MACMIIAR;
    maccr = ETH->MACCR;
    macffr = ETH->MACFFR;
    machthr = ETH->MACHTHR;
    machtlr = ETH->MACHTLR;
    macfcr = ETH->MACFCR;
    macvlantr = ETH->MACVLANTR;
    dmaomr = ETH->DMAOMR;

    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();

    /* Software reset */
    ETH_SoftwareReset();

    /* Wait for software reset */
    do{
        Delay_Us(10);
        if( !--timeout )  break;
    }while(ETH->DMABMR & ETH_DMABMR_SR);

    /* Configure MAC address */
    ETH->MACA0HR = (uint32_t)((MACAddr[5]<<8) | MACAddr[4]);
    ETH->MACA0LR = (uint32_t)(MACAddr[0] | (MACAddr[1]<<8) | (MACAddr[2]<<16) | (MACAddr[3]<<24));

    /* Mask the interrupt that Tx good frame count counter reaches half the maximum value */
    ETH->MMCTIMR = ETH_MMCTIMR_TGFM;
    /* Mask the interrupt that Rx good unicast frames counter reaches half the maximum value */
    /* Mask the interrupt that Rx crc error counter reaches half the maximum value */
    ETH->MMCRIMR = ETH_MMCRIMR_RGUFM | ETH_MMCRIMR_RFCEM;

    ETH_DMAITConfig(ETH_DMA_IT_NIS |\
                ETH_DMA_IT_R |\
                ETH_DMA_IT_T |\
                ETH_DMA_IT_AIS |\
                ETH_DMA_IT_RBU,\
                ENABLE);

    ETH_DMATxDescChainInit(DMATxDscrTab, MACTxBuf, ETH_TXBUFNB);
    ETH_DMARxDescChainInit(DMARxDscrTab, MACRxBuf, ETH_RXBUFNB);
    pDMARxSet = DMARxDscrTab;
    pDMATxSet = DMATxDscrTab;

    ETH->MACMIIAR = macmiiar;
    ETH->MACCR = maccr;
    ETH->MACFFR = macffr;
    ETH->MACHTHR = machthr;
    ETH->MACHTLR = machtlr;
    ETH->MACFCR = macfcr;
    ETH->MACVLANTR = macvlantr;
    ETH->DMAOMR = dmaomr;

    ETH_Start( );
}

/*********************************************************************
 * @fn      WCHNET_RecProcess
 *
 * @brief   Receiving related processing
 *
 * @param   none.
 *
 * @return  none.
 */
void WCHNET_RecProcess(void)
{
    if(((ChipId & 0xf0) <= 0x20) && \
            ((ETH->DMAMFBOCR & 0x1FFE0000) != 0))
    {
        ReInitMACReg();
        /* Resume DMA transport */
        ETH->DMARPDR = 0;
        ETH->DMATPDR = 0;
    }
}

/*********************************************************************
 * @fn      WCHNET_ETHIsr
 *
 * @brief   Ethernet Interrupt Service Routine
 *
 * @return  none
 */
void WCHNET_ETHIsr(void)
{
    uint32_t int_sta;

    int_sta = ETH->DMASR;
    if (int_sta & ETH_DMA_IT_AIS)
    {
        if (int_sta & ETH_DMA_IT_RBU)
        {
            WCHNET_RecProcess();
            ETH_DMAClearITPendingBit(ETH_DMA_IT_RBU);
        }
        ETH_DMAClearITPendingBit(ETH_DMA_IT_AIS);
    }

    if( int_sta & ETH_DMA_IT_NIS )
    {
        if( int_sta & ETH_DMA_IT_R )
        {
            /*If you don't use the Ethernet library,
             * you can do some data processing operations here*/
            ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
        }
        if( int_sta & ETH_DMA_IT_T )
        {
            ETH_DMAClearITPendingBit(ETH_DMA_IT_T);
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
    ChipId = DBGMCU_GetCHIPID();
    ETH_Configuration( macAddr );
    ETH_DMATxDescChainInit(DMATxDscrTab, MACTxBuf, ETH_TXBUFNB);
    ETH_DMARxDescChainInit(DMARxDscrTab, MACRxBuf, ETH_RXBUFNB);
    pDMARxSet = DMARxDscrTab;
    pDMATxSet = DMATxDscrTab;
    NVIC_EnableIRQ(ETH_IRQn);
    NVIC_SetPriority(ETH_IRQn, 0);
}

/*********************************************************************
 * @fn      ETH_LibInit
 *
 * @brief   Ethernet library initialization program
 *
 * @return  command status
 */
uint8_t ETH_LibInit( uint8_t *ip, uint8_t *gwip, uint8_t *mask, uint8_t *macaddr )
{
    uint8_t s;
    struct _WCH_CFG  cfg;

    memset(&cfg,0,sizeof(cfg));
    cfg.TxBufSize = ETH_TX_BUF_SZE;
    cfg.TCPMss   = WCHNET_TCP_MSS;
    cfg.HeapSize = WCHNET_MEM_HEAP_SIZE;
    cfg.ARPTableNum = WCHNET_NUM_ARP_TABLE;
    cfg.MiscConfig0 = WCHNET_MISC_CONFIG0;
    cfg.MiscConfig1 = WCHNET_MISC_CONFIG1;
    cfg.net_send = ETH_TxPktChainMode;
    cfg.CheckValid = WCHNET_CFG_VALID;
    s = WCHNET_ConfigLIB(&cfg);
    if( s ){
       return (s);
    }
    s = WCHNET_Init(ip,gwip,mask,macaddr);
    ETH_Init( macaddr );
    return (s);
}

/******************************** endfile @ eth_driver ******************************/
