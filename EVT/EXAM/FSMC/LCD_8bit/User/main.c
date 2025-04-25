/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2020/04/30
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
   FSMC routine to operate LCD drived by ST7789:
  LCD--PIN:
      ----------------------------------------------------------------
                 GND--VSS
                 VCC--3.3v
      -----------Command pin-------------------------------------------
                 RES --PA15
                 CS  --PD7(NE1)
                 RS  --PD11(A16)
                 WR  --PD5(NWE)
                 RD  --PD4(NOE)
                 BLK --PA12
     ------------Data    pin--------------------------------------------
                 PD14--FSMC_D0
                 PD15--FSMC_D1
                 PD0 --FSMC_D2
                 PD1 --FSMC_D3
                 PE7 --FSMC_D4
                 PE8 --FSMC_D5
                 PE9 --FSMC_D6
                 PE10--FSMC_D7

*/

#include "debug.h"
#include "lcd_init.h"
#include "lcd.h"
#include "pic.h"


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    LCD_Init();
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);

    while(1)
    {
        LCD_ShowChinese(45,40,(u8 *)"ÇßºãÎ¢µç×Ó",DARKBLUE,WHITE,32,0);
        LCD_ShowString(95,80,(u8 *)"WCH",DARKBLUE,WHITE,32,0);
        LCD_ShowString(65,120,(u8 *)"LCD DEMO",DARKBLUE,WHITE,32,0);
        LCD_ShowPicture(0,160,240,129,gImage_WCH);
    }
}







