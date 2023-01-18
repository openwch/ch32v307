/********************************** (C) COPYRIGHT *******************************
* File Name          : lcd.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains the headers of the TFTLCD.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __LCD_H
#define __LCD_H	

#include"debug.h"
#include"stdlib.h"

#define  delay_ms(x)  Delay_Ms(x)
#define  delay_us(x)  Delay_Us(x)

/* LCD parameter */
typedef struct  
{
    u16 width;
    u16 height;
    u16 id;             //LCD ID
    u8  dir;
    u16 wramcmd;
    u16 setxcmd;
    u16 setycmd;
}_lcd_dev;


extern _lcd_dev lcddev;
extern u16 POINT_COLOR;
extern u16 BACK_COLOR;


/* LCD addr */
typedef struct
{
    vu16 LCD_REG;
    vu16 LCD_RAM;
} LCD_TypeDef;

/* A17, Bank1, sector1 */
#define LCD_BASE        ((u32)(0x60000000 | 0x0003FFFE))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

/* Scan direction definition */
#define L2R_U2D  0
#define L2R_D2U  1
#define R2L_U2D  2
#define R2L_D2U  3

#define U2D_L2R  4
#define U2D_R2L  5
#define D2U_L2R  6
#define D2U_R2L  7

#define DFT_SCAN_DIR  L2R_U2D

/* Colour */
#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40
#define BRRED            0XFC07
#define GRAY             0X8430

#define DARKBLUE         0X01CF
#define LIGHTBLUE        0X7D7C
#define GRAYBLUE         0X5458
 
#define LIGHTGREEN       0X841F
#define LGRAY            0XC618

#define LGRAYBLUE        0XA651
#define LBBLUE           0X2B12

void LCD_Init(void);
void LCD_Clear(u16 Color);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
void LCD_Scan_Dir(u8 dir);
void LCD_Display_Dir(u8 dir);
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);

#endif









