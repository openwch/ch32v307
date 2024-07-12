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
uint16_t gPHYAddress;

/*********************************************************************
 * @fn      ETH_SetClock
 *
 * @brief   Set the Ethernet related clock
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_SetClock(void)
{
    /* Only support HSE_VALUE: 8MHz.
     * If you select other HSE value,you should
     * change the configuration below*/
    RCC_PLL3Cmd(DISABLE);
    RCC_PREDIV2Config(RCC_PREDIV2_Div1); // HSE = 8M
    RCC_PLL3Config(RCC_PLL3Mul_12_5); // 8M*12.5 = 100MHz
    RCC_MCOConfig(RCC_MCO_PLL3CLK);
    RCC_PLL3Cmd(ENABLE);
    while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLL3RDY));
    RCC_AHBPeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
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

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE|RCC_APB2Periph_AFIO, ENABLE);
    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);
    GPIO_Output(GPIOD, GPIO_Pin_8);                             /* MDIO */
    GPIO_Output(GPIOE, GPIO_Pin_15);                            /* MDC */

    GPIO_Output(GPIOE, GPIO_Pin_8);                             /* TXEN */
    GPIO_Output(GPIOE, GPIO_Pin_11);                            /* TXD0 */
    GPIO_Output(GPIOE, GPIO_Pin_10);                            /* TXD1 */

    GPIO_Output(GPIOE, GPIO_Pin_12);                            /* REFCLK */
    GPIO_Input(GPIOE, GPIO_Pin_9);                              /* CRSDV */
    GPIO_Input(GPIOE, GPIO_Pin_14);                             /* RXD0 */
    GPIO_Input(GPIOE, GPIO_Pin_13);                             /* RXD1 */
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
    tmpreg |= (uint32_t)ETH_MACMIIAR_CR_Div16;
    ETH->MACMIIAR = (uint32_t)tmpreg;

    /*------------------------ MAC register configuration  ----------------------- --------------------*/
    tmpreg = ETH->MACCR;
    tmpreg &= MACCR_CLEAR_MASK;
    tmpreg |= (uint32_t)(ETH_InitStruct->ETH_Watchdog |
                  ETH_InitStruct->ETH_Jabber |
                  ETH_InitStruct->ETH_InterFrameGap |
                  ETH_InitStruct->ETH_ChecksumOffload |
                  ETH_InitStruct->ETH_AutomaticPadCRCStrip |
                  ETH_InitStruct->ETH_DeferralCheck |
                  (1 << 20));
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
                     ETH_InitStruct->ETH_UnicastPauseFrameDetect |
                     ETH_InitStruct->ETH_ReceiveFlowControl |
                     ETH_InitStruct->ETH_TransmitFlowControl);
    ETH->MACFCR = (uint32_t)tmpreg;

    ETH->MACVLANTR = (uint32_t)(ETH_InitStruct->ETH_VLANTagComparison |
                               ETH_InitStruct->ETH_VLANTagIdentifier);

    tmpreg = ETH->DMAOMR;
    tmpreg &= DMAOMR_CLEAR_MASK;
    tmpreg |= (uint32_t)(ETH_InitStruct->ETH_DropTCPIPChecksumErrorFrame |
                    ETH_InitStruct->ETH_FlushReceivedFrame |
                    ETH_InitStruct->ETH_TransmitStoreForward |
                    ETH_InitStruct->ETH_ForwardErrorFrames |
                    ETH_InitStruct->ETH_ForwardUndersizedGoodFrames);
    ETH->DMAOMR = (uint32_t)tmpreg;

    /* Reset the physical layer */
    ETH_WritePHYRegister(PHYAddress, PHY_BCR, PHY_Reset);
    return ETH_SUCCESS;
}

/*********************************************************************
 * @fn      ETH_Configuration
 *
 * @brief   Ethernet configure.
 *
 * @return  none
 */
void ETH_Configuration( void )
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

    /* Set the Ethernet related clock */
    ETH_SetClock();

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
    ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
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
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Enable;
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Enable;
    /* Configure Ethernet */
    ETH_RegInit( &ETH_InitStructure, gPHYAddress );

    /* Mask the interrupt that Tx good frame count counter reaches half the maximum value */
    ETH->MMCTIMR = ETH_MMCTIMR_TGFM;
    /* Mask the interrupt that Rx good unicast frames counter reaches half the maximum value */
    /* Mask the interrupt that Rx crc error counter reaches half the maximum value */
    ETH->MMCRIMR = ETH_MMCRIMR_RGUFM | ETH_MMCRIMR_RFCEM;
}

/*********************************************************************
 * @fn      ETH_PHY_PowerDown
 *
 * @brief   ETH-PHY power down.
 *
 * @return  none
 */
void ETH_PHY_PowerDown( )
{
    uint16_t regval;

    ETH_Configuration();
    Delay_Ms(100);
    /* enable PHY power down*/
    regval = ETH_ReadPHYRegister( gPHYAddress, 0);
    regval |= 0x1 << 11;
    ETH_WritePHYRegister(gPHYAddress, 0, regval );
}


/******************************** endfile @ eth_driver ******************************/
