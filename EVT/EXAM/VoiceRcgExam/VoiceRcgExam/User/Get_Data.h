/********************************** (C) COPYRIGHT *******************************
* File Name          : Get_Data.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        :
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef _GET_DATA_H
#define _GET_DATA_H
#include "debug.h"

#define USE_ES8388 1

#if USE_ES8388
#define SampleDelay         410
#endif





void voice_init(void);
void DMA_Rx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize );
#endif

