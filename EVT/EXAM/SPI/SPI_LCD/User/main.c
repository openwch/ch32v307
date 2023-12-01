/*********************************************************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/8/15
 * Description        : Main program body.
 *********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/



/*
 *@Note
 This example reads image data from SPI flash and displays it on the LCD screen.
 You should first put your image in SPI flash, the image format is RGB 565 and big-endian byte order
 Both types of hardware communicate using the SPI interface.
 You can set the lcd screen size and buffer size in config.h
  pins: 
    FLASH_CS   -- PA2
    FLASH_DO   -- PA6(SPI1_MISO)
    FLASH_WP   -- 3.3V
    FLASH_DI   -- PA7(SPI1_MOSI)
    FLASH_CLK  -- PA5(SPI1_SCK)
    FLASH_HOLD -- 3.3V
    
    LCD_LED    -- PB9
    LCD_DC     -- PB10
    LCD_RST    -- PB11
    LCD_CS     -- PB12
    LCD_SCK    -- PB13(SPI2_SCK)
    LCD_MOSI   -- PB15(SPI2_MOSI)

*/


#include "debug.h"
#include "string.h"
#include "lcd.h"
#include "flash.h"
#include "config.h"

/* Global define */


/* Global Variable */

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    uint32_t addr = 0;
    uint16_t i    = 0;

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    SPI_FLASH_Init();
    LCD_Init();

    while (1)
    {
        addr = 0;
        i    = 0;
        while (i < 11)
        {
            i++;
            LCD_drawImageWithSize(0, 0, LCD_W, LCD_H, addr);
            addr += (LCD_W * LCD_H * COLOR_BYTE);
        }
    }
}
