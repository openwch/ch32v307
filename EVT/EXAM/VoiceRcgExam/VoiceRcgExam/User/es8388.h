/********************************** (C) COPYRIGHT *******************************
* File Name          : es8388.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        :
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __ES8388_H
#define __ES8388_H

#include "debug.h"



#define ES8388_ADDR     0x10	//ES8388的器件地址,固定为0x10 

u8 ES8388_Init(void);
u8 ES8388_Write_Reg(u8 reg, u8 val);
u8 ES8388_Read_Reg(u8 reg);
void ES8388_I2S_Cfg(u8 fmt, u8 len);
void ES8388_Set_Volume(u8 volume);
void ES8388_ADDA_Cfg(u8 dacen,u8 adcen);
void ES8388_Output_Cfg(u8 out);
void ES8388_Input_Cfg(u8 in);
#endif
