/********************************** (C) COPYRIGHT *******************************
* File Name          : lcd_init.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/01/03
* Description        : This file contains the headers of the TFTLCD.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "debug.h"

#define USE_HORIZONTAL 0  //Set the display in landscape or portrait mode; 0 or 1 is portrait 2 or 3 is landscape


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 320

#else
#define LCD_W 320
#define LCD_H 240
#endif


/* LCD addr */
typedef struct
{
    vu8 LCD_REG;
    vu8 LCD_DAT;
} LCD_TypeDef;

/* A16, Bank1, sector1 */
#define LCD_BASE       ((u32)(0x60000000 | 0x0000FFFF))
#define LCD            ((LCD_TypeDef *) LCD_BASE)


#define  ST7789_CMD_RDDID 0x04  //Command to read the ID


//-----------------LCD port definition----------------

#define	LCD_RES_Set()	GPIOA->BSHR=1<<15    //Reset			       PA15
#define	LCD_BLK_Set()	GPIOA->BSHR=1<<12    //Turn on the backlight   PA12
#define	LCD_RES_Clr()	GPIOA->BCR=1<<15     //No reset		           PA15
#define	LCD_BLK_Clr()	GPIOA->BCR=1<<12     //Turn off the backlight  PA12




void LCD_GPIO_Init(void);
void LCD_WR_DATA8(u8 dat);
void LCD_WR_DATA(u16 dat);
void LCD_WR_REG(u8 dat);
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_Init(void);
u16 ST7789_ReadID(void);
#endif




