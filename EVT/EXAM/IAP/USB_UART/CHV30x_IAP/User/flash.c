/********************************** (C) COPYRIGHT  *******************************
 * File Name          : flash.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2026/10/15
 * Description        : fast program
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "flash.h"
#include "string.h"

/*********************************************************************
 * @fn      CH32_IAP_Program
 *
 * @brief   adr - the date address
 *          buf - the date buffer
 *
 * @return  none
 */
void CH32_IAP_Program(u32 adr, u32* buf) {

    FLASH_ROM_WRITE(adr, buf, 256);
}



