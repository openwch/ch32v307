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
uint32_t volatile LocalTime;

ETH_DMADESCTypeDef *pDMARxSet;
ETH_DMADESCTypeDef *pDMATxSet;

#if( PHY_MODE == USE_10M_BASE )
/* PHY negotiation function */
uint8_t phyLinkStatus = 0;
uint8_t phyStatus = 0;
uint8_t phyLinkCnt = 0;
uint8_t phySucCnt = 0;
uint8_t phyPN = PHY_PN_SWITCH_AUTO;
uint8_t TRDetectStep = 0;
uint8_t TRDetectCnt = 0;
uint8_t LinkTaskPeriod = 50;
uint32_t RandVal = 0;
volatile uint8_t phyLinkReset;
volatile uint32_t phyLinkTime;
/* PHY receive processing */
uint8_t RBUFlag = 0;
uint8_t RecStopFlag = 0;
uint8_t ReInitMACFlag = 0;
uint8_t DuplexMode = 0;
uint8_t PhyPolarityDetect = 0;
uint32_t LinkSuccTime = 0;
extern u8 MACAddr[6];
void ReInitMACReg(void);
#define IDLE_DES_NUM    2       //The number of descriptors that need to be free
#endif

#if ENABLE_POLLING_TO_QUERY_PHY_LINK_STAT
u16 LastPhyStat = 0;
u32 LastQueryPhyTime = 0;
#endif
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
    uint8_t *macaddr=(uint8_t *)(ROM_CFG_USERADR_ID+5);

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
#if ENABLE_POLLING_TO_QUERY_PHY_LINK_STAT
void WCHNET_QueryPhySta(void)
{
    u16 phy_stat;
    if(QUERY_STAT_FLAG){                                         /* Query the PHY link status every 1s */
        LastQueryPhyTime = LocalTime / 1000;
#if((PHY_MODE == USE_MAC_MII) ||(PHY_MODE == USE_MAC_RMII))
        phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
        if(phy_stat != LastPhyStat){
            ETH_PHYLink();
        }
#elif(PHY_MODE == USE_MAC_RGMII)
        ETH_WritePHYRegister( PHY_ADDRESS, 0x1F, 0x0a43 );
        /*In some cases the status is not updated in time,
         * so read this register twice to get the correct status value.*/
        ETH_ReadPHYRegister( PHY_ADDRESS, 0x1A);
        phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, 0x1A) & 0x04;
        if(phy_stat != LastPhyStat){
            ETH_PHYLink();
        }
#endif
    }
}
#endif

#if( PHY_MODE == USE_10M_BASE )
/*********************************************************************
 * @fn      WCHNET_PhyPNProcess
 *
 * @brief   Phy PN Polarity related processing
 *
 * @param   none.
 *
 * @return  none.
 */
void WCHNET_PhyPNProcess(void)
{
    uint32_t PhyVal;

    LinkSuccTime = LocalTime;
    if((ETH->MMCRGUFCR == 0) && (ETH->MMCRFCECR >= 3))
    {
        PhyVal = ETH_ReadPHYRegister(gPHYAddress, PHY_MDIX);
        if((PhyVal >> 2) & 0x01)
            PhyVal &= ~(3 << 2);                //change PHY PN Polarity to normal
        else
            PhyVal |= 1 << 2;                   //change PHY PN Polarity to reverse
        ETH_WritePHYRegister(gPHYAddress, PHY_MDIX, PhyVal);
        ETH->MMCCR |= ETH_MMCCR_CR;             //Counters Reset
        while(ETH->MMCCR & ETH_MMCCR_CR);       //Wait for counters reset to complete
    }
    if(ETH->MMCRGUFCR != 0)
    {
        PhyPolarityDetect = 0;
        /* enable Filter function */
        ETH->MACFFR &= ~(ETH_ReceiveAll_Enable | ETH_PromiscuousMode_Enable);
    }
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
    if(RBUFlag)
    {
        RBUFlag = 0;
        RecStopFlag = 0;
        if(PhyPolarityDetect)
            WCHNET_PhyPNProcess();
        ReInitMACReg();
    }

    if(RecStopFlag)
    {
        if((pDMARxSet == (ETH_DMADESCTypeDef *)ETH->DMACHRDR) && \
        (pDMARxSet->Status&ETH_DMARxDesc_OWN))
        {
            RecStopFlag = 0;
            ETH_MACReceptionCmd(ENABLE);
        }
    }
}

/*********************************************************************
 * @fn      WCHNET_LinkProcess
 *
 * @brief   link process.
 *
 * @param   none.
 *
 * @return  none.
 */
void WCHNET_LinkProcess( void )
{
    uint16_t phy_anlpar, phy_bmsr, phy_mdix, RegVal;
    phy_anlpar = ETH_ReadPHYRegister(gPHYAddress, PHY_ANLPAR);
    phy_bmsr = ETH_ReadPHYRegister( gPHYAddress, PHY_BMSR);

    if( (phy_anlpar&PHY_ANLPAR_SELECTOR_FIELD) )
    {
        if(TRDetectStep == 0)
        {
            TRDetectStep = 1;
            TRDetectCnt = 1;
            PHY_TR_SWITCH();
            LinkTaskPeriod = RandVal%100 + 50;
            return;
        }
        else if(TRDetectStep == 1)
        {
            TRDetectStep = 2;
            TRDetectCnt = 0;
        }
        if( !(phyLinkStatus&PHY_LINK_WAIT_SUC) )
        {
            if( phyPN == PHY_PN_SWITCH_AUTO )
            {
                PHY_PN_SWITCH(PHY_PN_SWITCH_P);
            }
            else if( phyPN == PHY_PN_SWITCH_P )
            {
                phyLinkStatus = PHY_LINK_WAIT_SUC;
            }
            else
            {
                phyLinkStatus = PHY_LINK_WAIT_SUC;
            }
        }
        else{
            if((phySucCnt++ == 5) && ((phy_bmsr&(1<<5)) == 0))
            {
                phySucCnt = 0;
                if(phyPN == PHY_PN_SWITCH_N)
                    PHY_PN_SWITCH(PHY_PN_SWITCH_P);
                else PHY_PN_SWITCH(PHY_PN_SWITCH_N);
            }
        }
        phyLinkCnt = 0;
    }
    else
    {
        if(TRDetectStep == 1)
        {
            TRDetectCnt++;
            if(TRDetectCnt == 8)
            {
                TRDetectCnt = 0;
                TRDetectStep = 0;
                ETH_WritePHYRegister(gPHYAddress, PHY_MDIX, PHY_PN_SWITCH_AUTO);
                return;
            }
            PHY_TR_SWITCH();
            return;
        }
        if( phyLinkStatus == PHY_LINK_WAIT_SUC )
        {
            if(phyLinkCnt++ == 15 )
            {
                phyLinkCnt = 0;
                phySucCnt = 0;
                TRDetectStep = 0;
                phyLinkStatus = PHY_LINK_INIT;
                PHY_PN_SWITCH(PHY_PN_SWITCH_AUTO);
            }
        }
        else
        {
            if( phyPN == PHY_PN_SWITCH_P )
            {
                if(phyLinkCnt++ == 4 )
                {
                    phyLinkCnt = 0;
                    PHY_PN_SWITCH(PHY_PN_SWITCH_N);
                }
            }
            else if( phyPN == PHY_PN_SWITCH_N )
            {
                if(phyLinkCnt++ == 15 )
                {
                    phyLinkCnt = 0;
                    phySucCnt = 0;
                    TRDetectStep = 0;
                    phyLinkStatus = PHY_LINK_INIT;
                    PHY_PN_SWITCH(PHY_PN_SWITCH_AUTO);
                }
            }
            else{
                if(phyLinkCnt++ == (5000 / PHY_LINK_TASK_PERIOD))
                    PHY_LINK_RESET( );
            }
        }
    }
}

/*********************************************************************
 * @fn      WCHNET_HandlePhyNegotiation
 *
 * @brief   Handle PHY Negotiation.
 *
 * @param   none.
 *
 * @return  none.
 */
void WCHNET_HandlePhyNegotiation(void)
{
    if(phyLinkReset)              /* After the PHY link is disconnected, wait 500ms before turning on the PHY clock*/
    {
        if( LocalTime - phyLinkTime >= 500 )
        {
            phyLinkReset = 0;
            EXTEN->EXTEN_CTR |= EXTEN_ETH_10M_EN;
            PHY_LINK_RESET();
        }
    }
    else {
        if( !phyStatus )          /* Handling PHY Negotiation Exceptions */
        {
            ACCELERATE_LINK_PROCESS();
            if( LocalTime - phyLinkTime >= LinkTaskPeriod )
            {
                UPDATE_LINKTASKPERIOD();
                phyLinkTime = LocalTime;
                WCHNET_LinkProcess( );
            }
            if(ReInitMACFlag) ReInitMACFlag = 0;
        }
        else{                     /* PHY link complete */
            if(ReInitMACFlag)
            {
                if( LocalTime - phyLinkTime >= 5 * PHY_LINK_TASK_PERIOD ){
                    u32 phy_stat;
                    ReInitMACFlag = 0;
                    phy_stat = ETH_ReadPHYRegister( gPHYAddress, PHY_BMSR);
                    if((phy_stat&PHY_Linked_Status) == 0)
                    {
                        WCHNET_PhyStatus( phy_stat );
                        PHY_LINK_RESET();
                    }
                }
            }
            if(PhyPolarityDetect)
            {
                if( LocalTime - LinkSuccTime >= 2 * PHY_LINK_TASK_PERIOD )
                {
                    WCHNET_PhyPNProcess();
                }
            }
        }
    }
}
#endif

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

#if( PHY_MODE == USE_10M_BASE )
    WCHNET_HandlePhyNegotiation();
    WCHNET_RecProcess();
#endif

#if ENABLE_POLLING_TO_QUERY_PHY_LINK_STAT
    WCHNET_QueryPhySta();                   /* Query external PHY status */
#endif
}

#if( PHY_MODE == USE_10M_BASE )
/*********************************************************************
 * @fn      ETH_LedLinkSet
 *
 * @brief   set eth link led,setbit 0 or 1,the link led turn on or turn off
 *
 * @return  none
 */
void ETH_LedLinkSet( uint8_t mode )
{
    if( mode == LED_OFF )
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_0);
    }
    else
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_0);
    }
}

/*********************************************************************
 * @fn      ETH_LedDataSet
 *
 * @brief   set eth data led,setbit 0 or 1,the data led turn on or turn off
 *
 * @return  none
 */
void ETH_LedDataSet( uint8_t mode )
{
    if( mode == LED_OFF )
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_1);
    }
    else
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_1);
    }
}

/*********************************************************************
 * @fn      ETH_LedConfiguration
 *
 * @brief   set eth data and link led pin
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_LedConfiguration(void)
{
    GPIO_InitTypeDef  GPIO={0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    GPIO.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC,&GPIO);
    ETH_LedDataSet(LED_OFF);
    ETH_LedLinkSet(LED_OFF);
}

/*********************************************************************
 * @fn      ETH_SetClock
 *
 * @brief   Set ETH Clock(60MHZ).
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_SetClock(void)
{
    RCC_PLL3Cmd(DISABLE);
    RCC_PREDIV2Config(RCC_PREDIV2_Div2);                             /* HSE = 8M */
    RCC_PLL3Config(RCC_PLL3Mul_15);                                  /* 4M*15 = 60MHz */
    RCC_PLL3Cmd(ENABLE);
    while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLL3RDY));
}
#elif( PHY_MODE == USE_MAC_MII )
/*********************************************************************
 * @fn      ETH_MIIPinInit
 *
 * @brief   PHY MII interface GPIO initialization.
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_MIIPinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_Output(GPIOA, GPIO_Pin_2);                                                 /* MDIO */
    GPIO_Output(GPIOC, GPIO_Pin_1);                                                 /* MDC */

    GPIO_Input(GPIOC, GPIO_Pin_3);                                                  /* TXCLK */
    GPIO_Output(GPIOB, GPIO_Pin_11);                                                /* TXEN */
    GPIO_Output(GPIOB, GPIO_Pin_12);                                                /* TXD0 */
    GPIO_Output(GPIOB, GPIO_Pin_13);                                                /* TXD1 */
    GPIO_Output(GPIOC, GPIO_Pin_2);                                                 /* TXD2 */
    GPIO_Output(GPIOB, GPIO_Pin_8);                                                 /* TXD3 */
    GPIO_Input(GPIOA, GPIO_Pin_1);                                                  /* RXC */
    GPIO_Input(GPIOA, GPIO_Pin_7);                                                  /* RXDV */
    GPIO_Input(GPIOC, GPIO_Pin_4);                                                  /* RXD0 */
    GPIO_Input(GPIOC, GPIO_Pin_5);                                                  /* RXD1 */
    GPIO_Input(GPIOB, GPIO_Pin_0);                                                  /* RXD2 */
    GPIO_Input(GPIOB, GPIO_Pin_1);                                                  /* RXD3 */
    GPIO_Input(GPIOB, GPIO_Pin_10);                                                 /* RXER */

    GPIO_Output(GPIOA, GPIO_Pin_0);                                                 /* CRS */
    GPIO_Output(GPIOA, GPIO_Pin_3);                                                 /* COL */
}

#elif( PHY_MODE == USE_MAC_RMII )
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

#elif( PHY_MODE == USE_MAC_RGMII )
/*********************************************************************
 * @fn      ETH_RGMIIPinInit
 *
 * @brief   PHY RGMII interface GPIO initialization.
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_RGMIIPinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    GPIOB->CFGHR &= ~(0xff<<16);
    GPIOB->CFGHR |= (0xbb<<16);
    GPIOB->CFGLR &= ~(0xff<<4);

    GPIO_Output(GPIOA, GPIO_Pin_2);
    GPIO_Output(GPIOA, GPIO_Pin_3);
    GPIO_Output(GPIOA, GPIO_Pin_7);
    GPIO_Output(GPIOC, GPIO_Pin_4);
    GPIO_Output(GPIOC, GPIO_Pin_5);
    GPIO_Output(GPIOB, GPIO_Pin_0);

    GPIO_Input(GPIOC, GPIO_Pin_0);
    GPIO_Input(GPIOC, GPIO_Pin_1);
    GPIO_Input(GPIOC, GPIO_Pin_2);
    GPIO_Input(GPIOC, GPIO_Pin_3);
    GPIO_Input(GPIOA, GPIO_Pin_0);
    GPIO_Input(GPIOA, GPIO_Pin_1);

    GPIO_Input(GPIOB, GPIO_Pin_1);                                 /* 125m in */

    GPIO_Input(GPIOC, GPIO_Pin_7);                                 /* interrupt pin */
}
#endif

#if(((PHY_MODE == USE_MAC_RMII) || \
        (PHY_MODE == USE_MAC_RGMII)) && \
        !ENABLE_POLLING_TO_QUERY_PHY_LINK_STAT)
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
#if( PHY_MODE == USE_MAC_RGMII )
    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x0a42 );
    RegValue = ETH_ReadPHYRegister(gPHYAddress, 0x12);
    /* enable Link Status Change Interrupt and
     * Auto-Negotiation Completed Interrupt*/
    RegValue |= (1<<4)|(1<<3);
    ETH_WritePHYRegister(gPHYAddress, 0x12, RegValue );
    /* Clear the Interrupt status */
    ETH_WritePHYRegister( gPHYAddress, 0x1F, 0x0a43 );
    ETH_ReadPHYRegister( gPHYAddress, 0x1D);
#elif( PHY_MODE == USE_MAC_RMII )
    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x07 );
    /* Configure RMII mode */
    RegValue = ETH_ReadPHYRegister(gPHYAddress, 0x10);
    RegValue |= 0x01 << 3;
    ETH_WritePHYRegister(gPHYAddress, 0x10, RegValue );
    /* Configure interrupt function */
    RegValue = ETH_ReadPHYRegister(gPHYAddress, 0x13);
    RegValue |= 0x0f << 11;
    ETH_WritePHYRegister(gPHYAddress, 0x13, RegValue );
    /* Clear the Interrupt status */
    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x00 );
    ETH_ReadPHYRegister( gPHYAddress, 0x1E);
#endif
}
#endif

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
    u32 phy_stat;

#if( PHY_MODE == USE_10M_BASE )
    u16 phy_anlpar;
    phy_anlpar = ETH_ReadPHYRegister( gPHYAddress, PHY_ANLPAR);
    phy_stat = ETH_ReadPHYRegister( gPHYAddress, PHY_BSR);

    if((phy_stat&(PHY_Linked_Status))&&(phy_anlpar == 0)){           /* restart negotiation */
        EXTEN->EXTEN_CTR &= ~EXTEN_ETH_10M_EN;
        phyLinkReset = 1;
        phyLinkTime = LocalTime;
        return;
    }
    WCHNET_PhyStatus( phy_stat );

    if( (phy_stat&PHY_Linked_Status) && (phy_stat&PHY_AutoNego_Complete) )
    {
        phy_stat = ETH_ReadPHYRegister( gPHYAddress, PHY_STATUS );
        if( phy_stat & (1<<2) )
        {
            ETH->MACCR |= ETH_Mode_FullDuplex;
        }
        else
        {
            if( (phy_anlpar&PHY_ANLPAR_SELECTOR_FIELD) != PHY_ANLPAR_SELECTOR_VALUE )
            {
                ETH->MACCR |= ETH_Mode_FullDuplex;
            }
            else
            {
                ETH->MACCR &= ~ETH_Mode_FullDuplex;
            }
        }
        ETH->MACCR &= ~(ETH_Speed_100M|ETH_Speed_1000M);
        phyStatus = PHY_Linked_Status;

        /* disable Filter function */
        ETH->MACFFR |= (ETH_ReceiveAll_Enable | ETH_PromiscuousMode_Enable);

        ETH->MMCCR |= ETH_MMCCR_CR;             //Counters Reset
        while(ETH->MMCCR & ETH_MMCCR_CR);       //Wait for counters reset to complete
        PhyPolarityDetect = 1;
        LinkSuccTime = LocalTime;
        ETH_Start( );
    }
    else
    {
        EXTEN->EXTEN_CTR &= ~EXTEN_ETH_10M_EN;
        phyLinkReset = 1;
        phyLinkTime = LocalTime;
    }
    DuplexMode = (ETH->MACCR >> 11) & 0x01;  /* Record duplex mode*/
#elif(PHY_MODE == USE_MAC_MII)
    phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
    LastPhyStat = phy_stat;
    WCHNET_PhyStatus( phy_stat );
    if( (phy_stat&PHY_Linked_Status) && (phy_stat&PHY_AutoNego_Complete) )
    {
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
        ETH_Start( );
    }
#elif(PHY_MODE == USE_MAC_RMII)
    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x00 );
    phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
#if ENABLE_POLLING_TO_QUERY_PHY_LINK_STAT
    LastPhyStat = phy_stat;
#endif
    WCHNET_PhyStatus( phy_stat );
    if( (phy_stat&PHY_Linked_Status) && (phy_stat&PHY_AutoNego_Complete) )
    {
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
        ETH_Start( );
    }
    phy_stat = ETH_ReadPHYRegister( gPHYAddress, 0x1E);   /* Clear the Interrupt status */
#elif(PHY_MODE == USE_MAC_RGMII)
    ETH_WritePHYRegister( gPHYAddress, 0x1F, 0x0a43 );
    /*In some cases the status is not updated in time,
     * so read this register twice to get the correct status value.*/
    ETH_ReadPHYRegister( gPHYAddress, 0x1A);
    phy_stat = ETH_ReadPHYRegister( gPHYAddress, 0x1A);
#if ENABLE_POLLING_TO_QUERY_PHY_LINK_STAT
    LastPhyStat = phy_stat & 0x04;
#endif
    WCHNET_PhyStatus( phy_stat );

    if( phy_stat & 0x04 )
    {
        if( phy_stat & 0x08 )
        {
            ETH->MACCR |= ETH_Mode_FullDuplex;
        }
        else
        {
            ETH->MACCR &= ~ETH_Mode_FullDuplex;
        }
        if( (phy_stat & 0x30) == 0x00 )
        {
            ETH->MACCR &= ~(ETH_Speed_100M|ETH_Speed_1000M);
        }
        else if( (phy_stat & 0x30) == 0x10 )
        {
            ETH->MACCR &= ~(ETH_Speed_100M|ETH_Speed_1000M);
            ETH->MACCR |= ETH_Speed_100M;
        }
        else if( (phy_stat & 0x30) == 0x20 )
        {
            ETH->MACCR &= ~(ETH_Speed_100M|ETH_Speed_1000M);
            ETH->MACCR |= ETH_Speed_1000M;
        }
        ETH_Start( );
    }
    phy_stat = ETH_ReadPHYRegister( gPHYAddress, 0x1D);   /* Clear the Interrupt status */
#endif
}

#if( PHY_MODE == USE_10M_BASE )
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
    ETH_InitTypeDef ETH_InitStructure;
    uint16_t timeout = 10000;
    uint16_t RegVal;
    uint32_t tmpreg = 0;

    /* Wait for sending data to complete */
    while((ETH->DMASR & (7 << 20)) != ETH_DMA_TransmitProcess_Suspended);

    PHY_TR_REVERSE();

    /* Software reset */
    ETH_SoftwareReset();
    /* Wait for software reset */
    do{
        Delay_Us(10);
        if( !--timeout )  break;
    }while(ETH->DMABMR & ETH_DMABMR_SR);

    /* ETHERNET Configuration */
    /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
    ETH_StructInit(&ETH_InitStructure);
    /* Fill ETH_InitStructure parameters */
    /*------------------------   MAC   -----------------------------------*/
    ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
    ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
    #if HARDWARE_CHECKSUM_CONFIG
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
    #endif
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    /* Filter function configuration */
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
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
    /* Configure Ethernet */
    /*---------------------- Physical layer configuration -------------------*/
    /* Set the SMI interface clock, set as the main frequency divided by 42  */
    tmpreg = ETH->MACMIIAR;
    tmpreg &= MACMIIAR_CR_MASK;
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div42;
    ETH->MACMIIAR = (uint32_t)tmpreg;

    /*------------------------ MAC register configuration  ----------------------- --------------------*/
    tmpreg = ETH->MACCR;
    tmpreg &= MACCR_CLEAR_MASK;
    tmpreg |= (uint32_t)(ETH_InitStructure.ETH_AutoNegotiation |
    ETH_InitStructure.ETH_Watchdog |
    ETH_InitStructure.ETH_Jabber |
    ETH_InitStructure.ETH_InterFrameGap |
    ETH_InitStructure.ETH_CarrierSense |
    ETH_InitStructure.ETH_Speed |
    ETH_InitStructure.ETH_ReceiveOwn |
    ETH_InitStructure.ETH_LoopbackMode |
    ETH_InitStructure.ETH_Mode |
    ETH_InitStructure.ETH_ChecksumOffload |
    ETH_InitStructure.ETH_RetryTransmission |
    ETH_InitStructure.ETH_AutomaticPadCRCStrip |
    ETH_InitStructure.ETH_BackOffLimit |
    ETH_InitStructure.ETH_DeferralCheck);
    /* Write MAC Control Register */
    ETH->MACCR = (uint32_t)tmpreg;
    #if( PHY_MODE == USE_10M_BASE )
    ETH->MACCR |= ETH_Internal_Pull_Up_Res_Enable;  /*Turn on the internal pull-up resistor*/
    #endif
    ETH->MACFFR = (uint32_t)(ETH_InitStructure.ETH_ReceiveAll |
    ETH_InitStructure.ETH_SourceAddrFilter |
    ETH_InitStructure.ETH_PassControlFrames |
    ETH_InitStructure.ETH_BroadcastFramesReception |
    ETH_InitStructure.ETH_DestinationAddrFilter |
    ETH_InitStructure.ETH_PromiscuousMode |
    ETH_InitStructure.ETH_MulticastFramesFilter |
    ETH_InitStructure.ETH_UnicastFramesFilter);
    /*--------------- ETHERNET MACHTHR and MACHTLR Configuration ---------------*/
    /* Write to ETHERNET MACHTHR */
    ETH->MACHTHR = (uint32_t)ETH_InitStructure.ETH_HashTableHigh;
    /* Write to ETHERNET MACHTLR */
    ETH->MACHTLR = (uint32_t)ETH_InitStructure.ETH_HashTableLow;
    /*----------------------- ETHERNET MACFCR Configuration --------------------*/
    /* Get the ETHERNET MACFCR value */
    tmpreg = ETH->MACFCR;
    /* Clear xx bits */
    tmpreg &= MACFCR_CLEAR_MASK;
    tmpreg |= (uint32_t)((ETH_InitStructure.ETH_PauseTime << 16) |
    ETH_InitStructure.ETH_ZeroQuantaPause |
    ETH_InitStructure.ETH_PauseLowThreshold |
    ETH_InitStructure.ETH_UnicastPauseFrameDetect |
    ETH_InitStructure.ETH_ReceiveFlowControl |
    ETH_InitStructure.ETH_TransmitFlowControl);
    ETH->MACFCR = (uint32_t)tmpreg;

    ETH->MACVLANTR = (uint32_t)(ETH_InitStructure.ETH_VLANTagComparison |
    ETH_InitStructure.ETH_VLANTagIdentifier);

    tmpreg = ETH->DMAOMR;
    tmpreg &= DMAOMR_CLEAR_MASK;
    tmpreg |= (uint32_t)(ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame |
    ETH_InitStructure.ETH_ReceiveStoreForward |
    ETH_InitStructure.ETH_FlushReceivedFrame |
    ETH_InitStructure.ETH_TransmitStoreForward |
    ETH_InitStructure.ETH_TransmitThresholdControl |
    ETH_InitStructure.ETH_ForwardErrorFrames |
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames |
    ETH_InitStructure.ETH_ReceiveThresholdControl |
    ETH_InitStructure.ETH_SecondFrameOperate);
    ETH->DMAOMR = (uint32_t)tmpreg;

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
                    ETH_DMA_IT_RBU |\
                    ETH_DMA_IT_PHYLINK,\
                    ENABLE);

    ETH_DMATxDescChainInit(DMATxDscrTab, MACTxBuf, ETH_TXBUFNB);
    ETH_DMARxDescChainInit(DMARxDscrTab, MACRxBuf, ETH_RXBUFNB);
    pDMARxSet = DMARxDscrTab;
    pDMATxSet = DMATxDscrTab;

    ETH->MACCR &= ~ETH_Mode_FullDuplex;     //configure working mode based on the link result
    if(DuplexMode)
    {
        ETH->MACCR |= ETH_Mode_FullDuplex;
    }

    ETH->MACCR &= ~(ETH_Speed_100M|ETH_Speed_1000M);

    ETH_Start( );

    PHY_TR_REVERSE();

    if(!phyStatus)
    {
        PHY_LINK_RESET();
    }

    ReInitMACFlag = 1;
    phyLinkTime = LocalTime;
}
#endif

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
#if( PHY_MODE == USE_10M_BASE )
    ETH->MACCR |= ETH_Internal_Pull_Up_Res_Enable;  /*Turn on the internal pull-up resistor*/
#endif
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

    /* Reset the physical layer */
    ETH_WritePHYRegister(PHYAddress, PHY_BCR, PHY_Reset);
#if( PHY_MODE == USE_10M_BASE )
    ETH_WritePHYRegister(PHYAddress, PHY_MDIX, PHY_PN_SWITCH_AUTO);
#endif
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
#if( PHY_MODE == USE_10M_BASE )
    ETH_SetClock( );
    /* Enable internal 10BASE-T PHY*/
    EXTEN->EXTEN_CTR |= EXTEN_ETH_10M_EN;    /* Enable 10M Ethernet physical layer   */
#elif( PHY_MODE == USE_MAC_MII)
    /* Enable MII GPIO */
    ETH_MIIPinInit( );
#elif( PHY_MODE == USE_MAC_RMII)
    /* Enable RMII GPIO */
    ETH_RMIIPinInit();
#elif( PHY_MODE == USE_MAC_RGMII )
    /* Enable 1G MAC*/
    EXTEN->EXTEN_CTR |= EXTEN_ETH_RGMII_SEL;
    RCC_ETH1GCLKConfig(RCC_ETH1GCLKSource_PB1_IN);
    RCC_ETH1G_125Mcmd(ENABLE);
    /* Enable RGMII GPIO */
    ETH_RGMIIPinInit();
#endif
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
    /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
    ETH_StructInit(&ETH_InitStructure);
    /* Fill ETH_InitStructure parameters */
    /*------------------------   MAC   -----------------------------------*/
    ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
#if( PHY_MODE == USE_10M_BASE )
    ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
#elif((PHY_MODE == USE_MAC_MII) || (PHY_MODE == USE_MAC_RMII))
    ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
#elif( PHY_MODE == USE_MAC_RGMII )
    ETH_InitStructure.ETH_Speed = ETH_Speed_1000M;
#endif
#if HARDWARE_CHECKSUM_CONFIG
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif
    ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
    ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    /* Filter function configuration */
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
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

#if( PHY_MODE == USE_10M_BASE )
    ETH_DMAITConfig(ETH_DMA_IT_NIS |\
                ETH_DMA_IT_R |\
                ETH_DMA_IT_T |\
                ETH_DMA_IT_AIS |\
                ETH_DMA_IT_RBU |\
                ETH_DMA_IT_PHYLINK,\
                ENABLE);
#else
    ETH_DMAITConfig(ETH_DMA_IT_NIS |\
                ETH_DMA_IT_R |\
                ETH_DMA_IT_T |\
                ETH_DMA_IT_AIS |\
                ETH_DMA_IT_RBU,\
                ENABLE);
#if(PHY_MODE == USE_MAC_RMII)
#if ENABLE_POLLING_TO_QUERY_PHY_LINK_STAT
    uint16_t RegValue;
    ETH_WritePHYRegister(gPHYAddress, 0x1F, 0x07 );
    /* Configure RMII mode */
    RegValue = ETH_ReadPHYRegister(gPHYAddress, 0x10);
    RegValue |= 0x01 << 3;
    ETH_WritePHYRegister(gPHYAddress, 0x10, RegValue );
#else
    /* Configure the PHY interrupt function, the supported chip is: CH182H RMII */
    PHY_InterruptInit( );
    /* Configure EXTI Line7. */
    EXTI_Line_Init( );
#endif
#elif( PHY_MODE == USE_MAC_RGMII)
    /* Configure the polarity and delay of TXC */
    RGMII_TXC_Delay(0, 4);
#if !ENABLE_POLLING_TO_QUERY_PHY_LINK_STAT
    /* Configure the PHY interrupt function, the supported chip is: RTL8211FS */
    PHY_InterruptInit( );
    /* Configure EXTI Line7. */
    EXTI_Line_Init( );
#endif
#endif
#endif
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
 * @fn      WCHNET_ETHIsr
 *
 * @brief   Ethernet Interrupt Service Routine
 *
 * @return  none
 */
void WCHNET_ETHIsr(void)
{
    uint32_t int_sta;
#if( PHY_MODE == USE_10M_BASE )
    uint8_t i;
    ETH_DMADESCTypeDef *TempDesPtr;
#endif

    int_sta = ETH->DMASR;
    if (int_sta & ETH_DMA_IT_AIS)
    {
        if (int_sta & ETH_DMA_IT_RBU)
        {
#if( PHY_MODE == USE_10M_BASE )
            RBUFlag = 1;
            ETH_MACReceptionCmd(DISABLE);
#endif
            ETH_DMAClearITPendingBit(ETH_DMA_IT_RBU);
        }
        ETH_DMAClearITPendingBit(ETH_DMA_IT_AIS);
    }

    if( int_sta & ETH_DMA_IT_NIS )
    {
        if( int_sta & ETH_DMA_IT_R )
        {
#if( PHY_MODE == USE_10M_BASE )
            TempDesPtr = (ETH_DMADESCTypeDef *)(ETH->DMACHRDR);
            for( i = 0; i < (IDLE_DES_NUM + 1); i++)
            {
                if((TempDesPtr->Status & ETH_DMARxDesc_OWN) == RESET)     //Descriptor is occupied
                {
                    if((RecStopFlag == 0) && (RBUFlag == 0))
                    {
                        RecStopFlag = 1;
                        ETH_MACReceptionCmd(DISABLE);
                    }
                    break;
                }
                TempDesPtr = (ETH_DMADESCTypeDef *)(TempDesPtr->Buffer2NextDescAddr);
            }
#endif
            /*If you don't use the Ethernet library,
             * you can do some data processing operations here*/
            ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
        }
        if( int_sta & ETH_DMA_IT_T )
        {
            ETH_DMAClearITPendingBit(ETH_DMA_IT_T);
        }
#if( PHY_MODE == USE_10M_BASE )
        if( int_sta & ETH_DMA_IT_PHYLINK)
        {
            ETH_PHYLink( );
            ETH_DMAClearITPendingBit(ETH_DMA_IT_PHYLINK);
        }
#endif
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
#if( PHY_MODE == USE_10M_BASE )
    Delay_Ms(100);
    ETH_LedConfiguration( );
    RandVal = (macAddr[3]^macAddr[4]^macAddr[5]) * 214017 + 2531017;
#endif
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
#if( PHY_MODE == USE_10M_BASE )
    cfg.led_link = ETH_LedLinkSet;
    cfg.led_data = ETH_LedDataSet;
#endif
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
