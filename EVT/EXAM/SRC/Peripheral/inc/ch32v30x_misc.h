/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_misc.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/03/06
* Description        : This file contains all the functions prototypes for the 
*                      miscellaneous firmware library functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/  
#ifndef __CH32V30X_MISC_H
#define __CH32V30X_MISC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* CSR_INTSYSCR_INEST_definition */
#define INTSYSCR_INEST_NoEN        0x00   /* interrupt nesting disable(CSR-0x804 bit1 = 0) */
#define INTSYSCR_INEST_EN_2Level   0x01   /* interrupt nesting enable-2 Level(CSR-0x804 bit1 = 1 bit[3:2] = 1) */
#define INTSYSCR_INEST_EN_4Level   0x02   /* interrupt nesting enable-4 Level(CSR-0x804 bit1 = 1 bit[3:2] = 2) */
#define INTSYSCR_INEST_EN_8Level   0x03   /* interrupt nesting enable-8 Level(CSR-0x804 bit1 = 1 bit[3:2] = 3) */

/* Check the configuration of CSR(0x804) in the startup file(.S)
 *   interrupt nesting enable-8 Level(CSR-0x804 bit1 = 1 bit[3:2] = 3)
 *     priority - bit[7:5] - Preemption Priority
 *                bit[4:0] - Reserve
 *   interrupt nesting enable-4 Level(CSR-0x804 bit1 = 1 bit[3:2] = 2)
 *     priority - bit[7:6] - Preemption Priority
 *                bit[5] - Sub priority
 *                bit[4:0] - Reserve
 *   interrupt nesting enable-2 Level(CSR-0x804 bit1 = 1 bit[3:2] = 1)
 *     priority - bit[7] - Preemption Priority
 *                bit[6:5] - Sub priority
 *                bit[4:0] - Reserve
 *   interrupt nesting disable(CSR-0x804 bit1 = 0)
 *     priority - bit[7:5] - Sub priority
 *                bit[4:0] - Reserve
 */

#ifndef INTSYSCR_INEST
#define INTSYSCR_INEST   INTSYSCR_INEST_EN_4Level
#endif

/* NVIC Init Structure definition
 *   interrupt nesting disable(CSR-0x804 bit1 = 0)
 *     NVIC_IRQChannelPreemptionPriority - range is 0.
 *     NVIC_IRQChannelSubPriority - range from 0 to 7.
 *
 *   interrupt nesting enable-2 Level(CSR-0x804 bit1 = 1 bit[3:2] = 1)
 *     NVIC_IRQChannelPreemptionPriority - range from 0 to 1.
 *     NVIC_IRQChannelSubPriority - range from 0 to 3.
 *
 *   interrupt nesting enable-4 Level(CSR-0x804 bit1 = 1 bit[3:2] = 2)
 *     NVIC_IRQChannelPreemptionPriority - range from 0 to 3.
 *     NVIC_IRQChannelSubPriority - range from 0 to 1.
 *
 *   interrupt nesting enable-8 Level(CSR-0x804 bit1 = 1 bit[3:2] = 3)
 *     NVIC_IRQChannelPreemptionPriority - range from 0 to 7.
 *     NVIC_IRQChannelSubPriority - range range is 0.
 */
typedef struct
{
    uint8_t NVIC_IRQChannel;
    uint8_t NVIC_IRQChannelPreemptionPriority;
    uint8_t NVIC_IRQChannelSubPriority;
    FunctionalState NVIC_IRQChannelCmd;
} NVIC_InitTypeDef;

/* Preemption_Priority_Group */
#if (INTSYSCR_INEST == INTSYSCR_INEST_NoEN)
#define NVIC_PriorityGroup_0           ((uint32_t)0x00) /* interrupt nesting disable(CSR-0x804 bit1 = 0) */
#elif (INTSYSCR_INEST == INTSYSCR_INEST_EN_2Level)
#define NVIC_PriorityGroup_1           ((uint32_t)0x01) /* interrupt nesting enable-2 Level(CSR-0x804 bit1 = 1 bit[3:2] = 1) */
#elif (INTSYSCR_INEST == INTSYSCR_INEST_EN_8Level)
#define NVIC_PriorityGroup_3           ((uint32_t)0x03) /* interrupt nesting enable-8 Level(CSR-0x804 bit1 = 1 bit[3:2] = 3) */
#else
#define NVIC_PriorityGroup_2           ((uint32_t)0x02) /* interrupt nesting enable-4 Level(CSR-0x804 bit1 = 1 bit[3:2] = 2) */
#endif

void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);
void NVIC_Init(NVIC_InitTypeDef *NVIC_InitStruct);

#ifdef __cplusplus
}
#endif

#endif 

