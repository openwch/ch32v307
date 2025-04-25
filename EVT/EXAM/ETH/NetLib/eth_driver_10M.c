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
uint32_t ChipId = 0;

ETH_DMADESCTypeDef *pDMARxSet;
ETH_DMADESCTypeDef *pDMATxSet;

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
uint8_t volatile phyLinkReset;
uint32_t volatile phyLinkTime;

/* PHY receive processing */
uint8_t ReInitMACFlag = 0;
uint8_t PhyPolarityDetect = 0;
uint32_t LinkSuccTime = 0;

/*Parameters used when connecting to
 * devices that do not support auto-negotiation.*/
uint8_t phyNegoStat = 0;
uint32_t phyNegoLinkTime = 0;

extern u8 MACAddr[6];
void ReInitMACReg(void);

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
    if(((ChipId & 0xf0) == 0x20) && \
            ((ETH->DMAMFBOCR & 0x1FFE0000) != 0))
    {
        ReInitMACReg();
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
    WCHNET_HandlePhyNegotiation();
    WCHNET_RecProcess();
}

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

/*********************************************************************
 * @fn      ETH_LinkUpCfg
 *
 * @brief   When the PHY is connected, configure the relevant functions.
 *
 * @param   regval  BMSR register value
 *
 * @return  none.
 */
void ETH_LinkUpCfg(uint16_t regval)
{
    WCHNET_PhyStatus( regval );
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
    u16 phy_bsr, phy_stat, phy_anlpar, phy_bcr;

    phy_bsr = ETH_ReadPHYRegister( gPHYAddress, PHY_BSR);
    phy_bcr = ETH_ReadPHYRegister( gPHYAddress, PHY_BCR);
    phy_anlpar = ETH_ReadPHYRegister( gPHYAddress, PHY_ANLPAR);

    if(phy_bsr & PHY_Linked_Status)   //LinkUp
    {
        if(phy_bcr & PHY_AutoNegotiation)   //determine whether auto-negotiation is enable
        {
            if(phy_anlpar == 0)
            {
                if(phy_bsr & PHY_AutoNego_Complete)
                {
                    ETH->MACCR &= ~ETH_Mode_FullDuplex;
                    ETH_LinkUpCfg(phy_bsr);
                    if((ChipId & 0xf0) == 0x20)
                    {
                        if(phyNegoStat & TURN_PN_POLARITY)
                        {
                            //select N polarity
                            ETH_WritePHYRegister(gPHYAddress, PHY_MDIX, PHY_PN_SWITCH_N);
                        }
                        else {
                            //select P polarity
                            ETH_WritePHYRegister(gPHYAddress, PHY_MDIX, PHY_PN_SWITCH_P);
                            phyNegoStat |= LAST_NEGO_STAT;
                        }
                        phyNegoLinkTime = LocalTime;
                    }
                }
                else{
                    PHY_LINK_RESET();
                }
            }
            else {
                phyNegoStat = 0;
                if(phy_bsr & PHY_AutoNego_Complete)
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
                    ETH_LinkUpCfg(phy_bsr);
                }
                else{
                    EXTEN->EXTEN_CTR &= ~EXTEN_ETH_10M_EN;
                    phyLinkReset = 1;
                    phyLinkTime = LocalTime;
                }
            }
        }
        else {
            ETH->MACCR &= ~ETH_Mode_FullDuplex;
            ETH_LinkUpCfg(phy_bsr);
        }
    }
    else {                              //LinkDown
        WCHNET_PhyStatus( phy_bsr );
        EXTEN->EXTEN_CTR &= ~EXTEN_ETH_10M_EN;
        phyLinkReset = 1;
        phyLinkTime = LocalTime;
        if((ChipId & 0xf0) == 0x20)
        {
            if(phyNegoStat & LAST_NEGO_STAT)
            {
                if(LocalTime - phyNegoLinkTime < 100)
                {
                    phyNegoLinkTime = 0;
                    //Clear any existing error count values.
                    ETH->DMAMFBOCR;
                    phyNegoStat ^= TURN_PN_POLARITY;
                }
            }
        }
    }
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
    uint16_t RegVal;
    uint32_t maccr, macmiiar, macffr, machthr, machtlr;
    uint32_t macfcr, macvlantr, dmaomr;

    /* Wait for sending data to complete */
    while((ETH->DMASR & (7 << 20)) != ETH_DMA_TransmitProcess_Suspended);

    PHY_TR_REVERSE();

    /* Record the register value */
    macmiiar = ETH->MACMIIAR;
    maccr = ETH->MACCR;
    macffr = ETH->MACFFR;
    machthr = ETH->MACHTHR;
    machtlr = ETH->MACHTLR;
    macfcr = ETH->MACFCR;
    macvlantr = ETH->MACVLANTR;
    dmaomr = ETH->DMAOMR;

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
                    ETH_DMA_IT_RBU |\
                    ETH_DMA_IT_PHYLINK,\
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

    PHY_TR_REVERSE();

    if(!phyStatus)
    {
        PHY_LINK_RESET();
    }

    ReInitMACFlag = 1;
    phyLinkTime = LocalTime;
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
    /* Set the SMI interface clock, set as the main frequency divided by 42  */
    ETH->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div42;

    /*------------------------ MAC register configuration  -------------------------------------------*/
    ETH->MACCR = (uint32_t)(ETH_InitStruct->ETH_Watchdog |
                  ETH_InitStruct->ETH_Jabber |
                  ETH_InitStruct->ETH_InterFrameGap |
                  ETH_InitStruct->ETH_ChecksumOffload |
                  ETH_InitStruct->ETH_AutomaticPadCRCStrip |
                  ETH_InitStruct->ETH_LoopbackMode |
                  ETH_Internal_Pull_Up_Res_Enable);

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
    ETH_WritePHYRegister(PHYAddress, PHY_MDIX, PHY_PN_SWITCH_AUTO);
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
    ETH_SetClock( );

    /* Enable internal 10BASE-T PHY*/
    EXTEN->EXTEN_CTR |= EXTEN_ETH_10M_EN;    /* Enable 10M Ethernet physical layer   */

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
                ETH_DMA_IT_RBU |\
                ETH_DMA_IT_PHYLINK,\
                ENABLE);
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

    int_sta = ETH->DMASR;
    if (int_sta & ETH_DMA_IT_AIS)
    {
        if (int_sta & ETH_DMA_IT_RBU)
        {
            if((ChipId & 0xf0) == 0x10)
            {
                ((ETH_DMADESCTypeDef *)(((ETH_DMADESCTypeDef *)(ETH->DMACHRDR))->Buffer2NextDescAddr))->Status = ETH_DMARxDesc_OWN;

                /* Resume DMA reception */
                ETH->DMARPDR = 0;
            }
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
        if( int_sta & ETH_DMA_IT_PHYLINK)
        {
            ETH_PHYLink( );
            ETH_DMAClearITPendingBit(ETH_DMA_IT_PHYLINK);
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
    Delay_Ms(100);
    ChipId = DBGMCU_GetCHIPID();
    ETH_LedConfiguration( );
    RandVal = (macAddr[3]^macAddr[4]^macAddr[5]) * 214017 + 2531017;
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
    cfg.led_link = ETH_LedLinkSet;
    cfg.led_data = ETH_LedDataSet;
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
