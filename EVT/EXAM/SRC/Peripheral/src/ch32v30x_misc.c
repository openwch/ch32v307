/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_misc.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/03/06
* Description        : This file provides all the miscellaneous firmware functions .
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x_misc.h"

__IO uint32_t NVIC_Priority_Group = 0;

/*********************************************************************
 * @fn      NVIC_PriorityGroupConfig
 *
 * @brief   Configures the priority grouping - pre-emption priority and subpriority.
 *
 * @param   NVIC_PriorityGroup - specifies the priority grouping bits length.
 *            NVIC_PriorityGroup_0 - 0 bits for pre-emption priority
 *                                   3 bits for subpriority
 *            NVIC_PriorityGroup_1 - 1 bits for pre-emption priority
 *                                   2 bits for subpriority
 *            NVIC_PriorityGroup_2 - 2 bits for pre-emption priority
 *                                   1 bits for subpriority
 *            NVIC_PriorityGroup_3 - 3 bits for pre-emption priority
 *                                   0 bits for subpriority
 *
 * @return  none
 */
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
    NVIC_Priority_Group = NVIC_PriorityGroup;
}

/*********************************************************************
 * @fn      NVIC_Init
 *
 * @brief   Initializes the NVIC peripheral according to the specified parameters in
 *        the NVIC_InitStruct.
 *
 * @param   NVIC_InitStruct - pointer to a NVIC_InitTypeDef structure that contains the
 *        configuration information for the specified NVIC peripheral.
 *            interrupt nesting disable(CSR-0x804 bit1 = 0)
 *              NVIC_IRQChannelPreemptionPriority - range is 0.
 *              NVIC_IRQChannelSubPriority - range from 0 to 7.
 *
 *            interrupt nesting enable-2 Level(CSR-0x804 bit1 = 1 bit[3:2] = 1)
 *              NVIC_IRQChannelPreemptionPriority - range from 0 to 1.
 *              NVIC_IRQChannelSubPriority - range from 0 to 3.
 *
 *            interrupt nesting enable-4 Level(CSR-0x804 bit1 = 1 bit[3:2] = 2)
 *              NVIC_IRQChannelPreemptionPriority - range from 0 to 3.
 *              NVIC_IRQChannelSubPriority - range from 0 to 1.
 *
 *            interrupt nesting enable-8 Level(CSR-0x804 bit1 = 1 bit[3:2] = 3)
 *              NVIC_IRQChannelPreemptionPriority - range from 0 to 7.
 *              NVIC_IRQChannelSubPriority - range range is 0.
 *
 * @return  none
 */
void NVIC_Init(NVIC_InitTypeDef *NVIC_InitStruct)
{
#if (INTSYSCR_INEST == INTSYSCR_INEST_NoEN)
    if(NVIC_Priority_Group == NVIC_PriorityGroup_0)
    {
        NVIC_SetPriority(NVIC_InitStruct->NVIC_IRQChannel, NVIC_InitStruct->NVIC_IRQChannelSubPriority << 4);
    }
#elif (INTSYSCR_INEST == INTSYSCR_INEST_EN_2Level)
    if(NVIC_Priority_Group == NVIC_PriorityGroup_1)
    {
        if(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority < 2)
        {
            NVIC_SetPriority(NVIC_InitStruct->NVIC_IRQChannel, (NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << 7) | (NVIC_InitStruct->NVIC_IRQChannelSubPriority << 5));
        }
    }
#elif (INTSYSCR_INEST == INTSYSCR_INEST_EN_4Level)
    if(NVIC_Priority_Group == NVIC_PriorityGroup_2)
    {
        if(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority < 4)
        {
            NVIC_SetPriority(NVIC_InitStruct->NVIC_IRQChannel, (NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << 6) | (NVIC_InitStruct->NVIC_IRQChannelSubPriority << 5));
        }
    }
#elif (INTSYSCR_INEST == INTSYSCR_INEST_EN_8Level)
    if(NVIC_Priority_Group == NVIC_PriorityGroup_3)
    {
        if(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority < 8)
        {
            NVIC_SetPriority(NVIC_InitStruct->NVIC_IRQChannel, (NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << 5) );
        }
    }
#endif

    if(NVIC_InitStruct->NVIC_IRQChannelCmd != DISABLE)
    {
        NVIC_EnableIRQ(NVIC_InitStruct->NVIC_IRQChannel);
    }
    else
    {
        NVIC_DisableIRQ(NVIC_InitStruct->NVIC_IRQChannel);
    }
}
