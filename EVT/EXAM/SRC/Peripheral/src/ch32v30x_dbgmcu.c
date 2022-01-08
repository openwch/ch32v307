/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_dbgmcu.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the DBGMCU firmware functions.
****************************************************************************************/
#include "ch32v30x_dbgmcu.h"

#define IDCODE_DEVID_MASK    ((uint32_t)0x00000FFF)

/*******************************************************************************
* Function Name  : DBGMCU_GetREVID
* Description    : Returns the device revision identifier.
* Input          : None
* Return         : Revision identifier.
*******************************************************************************/
uint32_t DBGMCU_GetREVID(void)
{
   return((*(uint32_t*) 0x1FFFF704) >> 16);
}

/*******************************************************************************
* Function Name  : DBGMCU_GetDEVID
* Description    : Returns the device identifier.
* Input          : None
* Return         : Device identifier.
*******************************************************************************/
uint32_t DBGMCU_GetDEVID(void)
{
   return((*(uint32_t*) 0x1FFFF704) & IDCODE_DEVID_MASK);
}


