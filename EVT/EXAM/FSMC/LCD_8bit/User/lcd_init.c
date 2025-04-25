/********************************** (C) COPYRIGHT *******************************
* File Name          : lcd_init.c
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/04/03
* Description        : This file contains the headers of the TFTLCD.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "lcd_init.h"
#include "debug.h"

/*********************************************************************
 * @fn      LCD_GPIO_Init
 *
 * @brief   Init LCD GPIO
 *
 * @param   none
 *
 * @return  none
 */

void LCD_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_12|GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_11|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);


    readWriteTiming.FSMC_AddressSetupTime = 0x00;
    readWriteTiming.FSMC_AddressHoldTime = 0x01;
    readWriteTiming.FSMC_DataSetupTime = 0x0f;
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    writeTiming.FSMC_AddressSetupTime = 0x00;
    writeTiming.FSMC_AddressHoldTime = 0x01;
    writeTiming.FSMC_DataSetupTime = 0x03;
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;

    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
}


/*********************************************************************
 * @fn      LCD_RD_DATA8
 *
 * @brief   Read data
 *
 * @return  dat - read data
 */

u8 LCD_RD_DATA8(void)
{
    vu8 dat;
    dat=LCD->LCD_DAT;
    return dat;
}

/*********************************************************************
 * @fn      LCD_WR_DATA8
 *
 * @brief   Write data
 *
 * @param   dat
 *
 * @return  none
 */

void LCD_WR_DATA8(u8 dat)
{
    LCD->LCD_DAT=dat;
}

/*********************************************************************
 * @fn      LCD_WR_DATA
 *
 * @brief   Write 16 bit data
 *
 * @param   dat
 *
 * @return  none
 */

void LCD_WR_DATA(u16 dat)
{
    LCD->LCD_DAT=dat>>8;
    LCD->LCD_DAT=dat;
}

/*********************************************************************
 * @fn      LCD_WR_REG
 *
 * @brief   Write value to register
 *
 * @param   dat - value
 *
 * @return  none
 */

void LCD_WR_REG(u8 dat)
{
    LCD->LCD_REG=dat;
}

/*********************************************************************
 * @fn      LCD_Address_Set
 *
 * @brief   Set the start and end addresses
 *
 * @param   x1 - The starting address of the column
 *          x2 - The end address of the column
 *          y1 - The starting address of the line
 *          y2 - The end address of the line
 *
 * @return  none
 */

void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
    if(USE_HORIZONTAL==0)
	{
        LCD_WR_REG(0x2a);
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);
	}
	else
	{
		LCD_WR_REG(0x2a);
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);
	}
}

/*********************************************************************
 * @fn      LCD_Init
 *
 * @brief   Init LCD
 *
 * @return  none
 */

void LCD_Init(void)
{
    u16 ID;
    LCD_GPIO_Init();
	LCD_RES_Clr();
	Delay_Ms(100);
	LCD_RES_Set();
	Delay_Ms(100);

	LCD_BLK_Set();//Turn on the backlight
	Delay_Ms(100);
	
   ID=ST7789_ReadID();
   printf("LCDID:%X",ID);
   while(ID!=0x8552 ){};

    LCD_WR_REG(0x11);
    Delay_Ms(120);
    LCD_WR_REG(0x36);
    if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
    else LCD_WR_DATA8(0xA0);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA8(0x35);

    LCD_WR_REG(0xBB);
    LCD_WR_DATA8(0x19);

    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0x2C);

    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x01);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x12);

    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x20);

    LCD_WR_REG(0xC6);
    LCD_WR_DATA8(0x0F);

    LCD_WR_REG(0xD0);
    LCD_WR_DATA8(0xA4);
    LCD_WR_DATA8(0xA1);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2B);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x54);
    LCD_WR_DATA8(0x4C);
    LCD_WR_DATA8(0x18);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x0B);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x23);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0xD0);
    LCD_WR_DATA8(0x04);
    LCD_WR_DATA8(0x0C);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x13);
    LCD_WR_DATA8(0x2C);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x44);
    LCD_WR_DATA8(0x51);
    LCD_WR_DATA8(0x2F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x1F);
    LCD_WR_DATA8(0x20);
    LCD_WR_DATA8(0x23);

    LCD_WR_REG(0x21);

    LCD_WR_REG(0x29);
}


/*********************************************************************
 * @fn      ST7789_ReadID
 *
 * @brief   Read LCD ID
 *
 * @param   none
 *
 * @return  ID - LCD ID
 */
u16 ST7789_ReadID(void)
{
    u8 id1,id2,id3 ;
    u16 ID;
    LCD_WR_REG(0x04);
    LCD_RD_DATA8();
    id1 = LCD_RD_DATA8();
    id2 = LCD_RD_DATA8();
    while(id1!=id2);
    id3 = LCD_RD_DATA8();
    ID=(id1<<8)|id3;
    return ID;
}






