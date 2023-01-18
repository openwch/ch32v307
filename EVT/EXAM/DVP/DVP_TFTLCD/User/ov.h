/********************************** (C) COPYRIGHT *******************************
* File Name          : ov.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains the headers of the OV2640.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __OV_H
#define __OV_H

#include "debug.h"

#ifndef NULL
#define NULL                    0
#endif

#ifndef VOID
#define VOID                    void
#endif
#ifndef CONST
#define CONST                   const
#endif
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char           BOOLEAN;
#endif
#ifndef CHAR
typedef char                    CHAR;
#endif
#ifndef INT8
typedef char                    INT8;
#endif
#ifndef INT16
typedef short                   INT16;
#endif
#ifndef INT32
typedef long                    INT32;
#endif
#ifndef UINT8
typedef unsigned char           UINT8;
#endif
#ifndef UINT16
typedef unsigned short          UINT16;
#endif
#ifndef UINT32
typedef unsigned long           UINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile  UINT8V;
#endif
#ifndef UINT16V
typedef unsigned short volatile UINT16V;
#endif
#ifndef UINT32V
typedef unsigned long volatile  UINT32V;
#endif

#ifndef PVOID
typedef void                    *PVOID;
#endif
#ifndef PCHAR
typedef char                    *PCHAR;
#endif
#ifndef PCHAR
typedef const char              *PCCHAR;
#endif
#ifndef PINT8
typedef char                    *PINT8;
#endif
#ifndef PINT16
typedef short                   *PINT16;
#endif
#ifndef PINT32
typedef long                    *PINT32;
#endif
#ifndef PUINT8
typedef unsigned char           *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short          *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long           *PUINT32;
#endif
#ifndef PUINT8V
typedef volatile unsigned char  *PUINT8V;
#endif
#ifndef PUINT16V
typedef volatile unsigned short *PUINT16V;
#endif
#ifndef PUINT32V
typedef volatile unsigned long  *PUINT32V;
#endif


//RGB565 PIXEL 320*240
#define RGB565_ROW_NUM   320
#define RGB565_COL_NUM   480   //Col * 2
#define OV2640_RGB565_HEIGHT   320
#define OV2640_RGB565_WIDTH	   240

//JPEG PIXEL 1024 * 768
#define OV2640_JPEG_HEIGHT	768
#define OV2640_JPEG_WIDTH	1024


//PC13-RESET  PC3-PWDN
#define OV_RESET_SET   {GPIOC->BSHR = GPIO_Pin_13;}
#define OV_RESET_CLR   {GPIOC->BCR = GPIO_Pin_13;}
#define OV_PWDN_SET    {GPIOC->BSHR = GPIO_Pin_3;}
#define OV_PWDN_CLR    {GPIOC->BCR = GPIO_Pin_3;}

//OV2640��ID
#define SCCB_ID   			    0X60
#define OV2640_MID				0X7FA2
#define OV2640_PID				0X2642

#define IIC_SCL_IN  {GPIOB->CFGHR&=0XFFFFF0FF;GPIOB->CFGHR|=8<<8;}
#define IIC_SCL_OUT {GPIOB->CFGHR&=0XFFFFF0FF;GPIOB->CFGHR|=3<<8;}
#define IIC_SDA_IN  {GPIOB->CFGHR&=0XFFFF0FFF;GPIOB->CFGHR|=8<<12;}
#define IIC_SDA_OUT {GPIOB->CFGHR&=0XFFFF0FFF;GPIOB->CFGHR|=3<<12;}

#define IIC_SDA_SET   {GPIOB->BSHR = GPIO_Pin_11;}
#define IIC_SDA_CLR   {GPIOB->BCR = GPIO_Pin_11;}
#define IIC_SCL_SET   {GPIOB->BSHR = GPIO_Pin_10;}
#define IIC_SCL_CLR   {GPIOB->BCR = GPIO_Pin_10;}

//SDA In
#define SDA_IN_R      (GPIOB->INDR & GPIO_Pin_11)


void SCCB_GPIO_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
UINT8 SCCB_WR_Byte(UINT8 data);
UINT8 SCCB_RD_Byte(void);
UINT8 SCCB_WR_Reg(UINT8 Reg_Adr,UINT8 Reg_Val);
UINT8 SCCB_RD_Reg(UINT8 Reg_Adr);

void RGB565_Mode_Init(void);
void JPEG_Mode_Init(void);
UINT8 OV2640_Init(void);
void OV2640_JPEG_Mode(void);
void OV2640_RGB565_Mode(void);
UINT8 OV2640_OutSize_Set(UINT16 Image_width,UINT16 Image_height);
void OV2640_Speed_Set(UINT8 Pclk_Div, UINT8 Xclk_Div);



#endif

