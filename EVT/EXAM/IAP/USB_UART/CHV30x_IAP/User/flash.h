/********************************** (C) COPYRIGHT  *******************************
 * File Name          : falsh.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2020/12/16
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/
#ifndef __FLASH_H
#define __FLASH_H

#include "ch32v30x_it.h"
#include "stdio.h"


void CH32_IAP_Program(u32 adr, u32* buf);


#endif
