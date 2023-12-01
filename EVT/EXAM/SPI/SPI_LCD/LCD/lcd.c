/*********************************************************************
 * File Name          : lcd.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/8/15
 * Description        : file for lcd screen.
 *********************************************************************************
* Copyright (c) 2023 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "lcd.h"
#include "debug.h"

static uint16_t width;
static uint16_t height;

#define LCD_SOFT_RESET          (0x01)
#define LCD_READ_ID             (0x04)
#define LCD_READ_DISPLAY_STATUS (0x09)
#define LCD_ON                  (0x28)
#define LCD_OFF                 (0x29)

#define LCD_SET_X               (0x2a)
#define LCD_SET_Y               (0x2B)
#define LCD_MEM_WRITE           (0x2C)
#define LCD_MEM_READ            (0x2e)

static uint16_t color_buffer[2][FILL_BUFFER_COUNT] = {0};

/*********************************************************************
 * @fn      LCD_WR_REG
 *
 * @brief   Write an 8-bit command to the LCD screen
 * 
 * @param   data - Command value to be written
 *
 * @return  none
 */
static void LCD_WR_REG(uint8_t data)
{
    LCD_CS_CLR;
    LCD_DC_CLR;
    SPI2_Write(data);
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_WR_DATA
 *
 * @brief   Write an 8-bit data to the LCD screen
 * 
 * @param   data - the data to write
 * 
 */
static void LCD_WR_DATA(uint8_t data)
{
    LCD_CS_CLR;
    LCD_DC_SET;
    SPI2_Write(data);
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_WriteReg
 * 
 * @brief   write a data to the register
 * 
 * @param   LCD_Reg - register address
 *          LCD_RegValue - the data to write
 * 
 * @return  none
 */
static void LCD_WriteReg(uint8_t LCD_Reg, uint8_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/*********************************************************************
 * @fn      Lcd_WriteData_16Bit
 * 
 * @brief   write two bytes to the lcd screen
 * 
 * @param   Data - the data to write
 * 
 * @return  none
 */
void Lcd_WriteData_16Bit(uint16_t Data)
{
    LCD_CS_CLR;
    LCD_DC_SET;
    SPI2_Write(Data >> 8);
    SPI2_Write(Data);
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_GPIOInit
 * 
 * @brief   Configuring the control gpio for the lcd screen
 * 
 * @return  none
 */
void LCD_GPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(LCD_LED_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(LCD_DC_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(LCD_RST_RCC, ENABLE);
    RCC_APB2PeriphClockCmd(LCD_CS_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = LCD_LED_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_LED_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LCD_DC_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_DC_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LCD_RST_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = LCD_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(LCD_CS_PORT, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      LCD_RESET
 * 
 * @brief   reset the lcd screen by the rst pin
 * 
 * @return  none
 */
void LCD_RESET(void)
{
    LCD_RST_CLR;
    Delay_Ms(100);
    LCD_RST_SET;
    Delay_Ms(50);
}

/*********************************************************************
 * @fn      LCD_Init
 * 
 * @brief   Initialization LCD screen
 * 
 * @return  none
 */
void LCD_Init(void)
{
    SPI2_Init();
    SPI2_DMA_Init();
    LCD_GPIOInit();
    LCD_RESET();
    LCD_LED_SET;

    LCD_WR_REG(0xCF);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC9);  //C1
    LCD_WR_DATA(0X30);
    LCD_WR_REG(0xED);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0X12);
    LCD_WR_DATA(0X81);
    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x85);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x7A);
    LCD_WR_REG(0xCB);
    LCD_WR_DATA(0x39);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x02);
    LCD_WR_REG(0xF7);
    LCD_WR_DATA(0x20);
    LCD_WR_REG(0xEA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xC0);   //Power control
    LCD_WR_DATA(0x1B);  //VRH[5:0]
    LCD_WR_REG(0xC1);   //Power control
    LCD_WR_DATA(0x00);  //SAP[2:0];BT[3:0] 01
    LCD_WR_REG(0xC5);   //VCM control
    LCD_WR_DATA(0x30);  //3F
    LCD_WR_DATA(0x30);  //3C
    LCD_WR_REG(0xC7);   //VCM control2
    LCD_WR_DATA(0XB7);
    LCD_WR_REG(0x36);  // Memory Access Control
    LCD_WR_DATA(0x08);
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);
    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1A);
    LCD_WR_REG(0xB6);  // Display Function Control
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0xA2);
    LCD_WR_REG(0xF2);  // 3Gamma Function Disable
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0x26);  //Gamma curve selected
    LCD_WR_DATA(0x01);
    LCD_WR_REG(0xE0);  //Set Gamma
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x2A);
    LCD_WR_DATA(0x28);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x54);
    LCD_WR_DATA(0XA9);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0XE1);  //Set Gamma
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x15);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x2B);
    LCD_WR_DATA(0x56);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x3f);
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xef);
    LCD_WR_REG(0x11);  //Exit Sleep
    Delay_Ms(120);
    LCD_WR_REG(0x29);  //display on

    LCD_direction(USE_HORIZONTAL);  
}

/*********************************************************************
 * @fn      LCD_SetWindows
 * 
 * @brief   Setting LCD display window
 * 
 * @param   xStar -  the bebinning x coordinate of the LCD display window
 *          yStar -  the bebinning y coordinate of the LCD display window
 *          xEnd -  the endning x coordinate of the LCD display window
 *          yEnd -  the endning y coordinate of the LCD display window
 * 
 * @return  none
 */
void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{
    LCD_WR_REG(LCD_SET_X);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(0x00FF & xStar);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(0x00FF & xEnd);

    LCD_WR_REG(LCD_SET_Y);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(0x00FF & yStar);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(0x00FF & yEnd);
    LCD_WR_REG(LCD_MEM_WRITE);  
}

/*********************************************************************
 * @fn      LCD_direction
 * 
 * @brief   Setting the display direction of LCD screen
 * 
 * @param   direction -   direction:0-0 degree  1-90 degree  2-180 degree  3-270 degree
 * 
 * @return  none
 */
void LCD_direction(uint8_t direction)
{
    switch (direction)
    {
        case 0:
            width  = LCD_W;
            height = LCD_H;
            LCD_WriteReg(0x36, (1 << 3) | (0 << 6) | (0 << 7));  //BGR==1,MY==0,MX==0,MV==0
            break;
        case 1:
            width  = LCD_H;
            height = LCD_W;
            LCD_WriteReg(0x36, (1 << 3) | (0 << 7) | (1 << 6) | (1 << 5));  //BGR==1,MY==1,MX==0,MV==1
            break;
        case 2:
            width  = LCD_W;
            height = LCD_H;
            LCD_WriteReg(0x36, (1 << 3) | (1 << 6) | (1 << 7));  //BGR==1,MY==0,MX==0,MV==0
            break;
        case 3:
            width  = LCD_H;
            height = LCD_W;
            LCD_WriteReg(0x36, (1 << 3) | (1 << 7) | (1 << 5));  //BGR==1,MY==1,MX==0,MV==1
            break;
        default:
            break;
    }
}

/*********************************************************************
 * @fn      LCD_Clear
 * 
 * @brief   Full screen filled LCD screen
 * 
 * @param   Color -   Filled color
 * 
 * @return  none
 */
void LCD_Clear(uint16_t Color)
{
    unsigned int i, m;
    LCD_SetWindows(0, 0, width - 1, height - 1);
    LCD_CS_CLR;
    LCD_DC_SET;
    for (i = 0; i < height; i++)
    {
        for (m = 0; m < width; m++)
        {
            Lcd_WriteData_16Bit(Color);
        }
    }
    LCD_CS_SET;
}

/*********************************************************************
 * @fn      LCD_drawImageWithSize
 * 
 * @brief   read image from SPI Flash and display on the LCD screen
 * 
 * @param   x -  the x coordinate of the LCD screen
 *          y - the y coordinate of the LCD screen
 *          width -  the image width
 *          height - the image height
 *          addr - the image address in SPI flash
 * 
 * @return  none
 */
void LCD_drawImageWithSize(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t addr)
{
    uint32_t m  = 0;
    uint16_t x2 = (x + width) - 1;
    uint16_t y2 = (y + height) - 1;
    LCD_WR_REG(LCD_SET_X);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(0x00FF & x);
    LCD_WR_DATA(x2 >> 8);
    LCD_WR_DATA(0x00FF & x2);

    LCD_WR_REG(LCD_SET_Y);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(0x00FF & y);
    LCD_WR_DATA(y2 >> 8);
    LCD_WR_DATA(0x00FF & y2);

    LCD_WR_REG(LCD_MEM_WRITE);
    LCD_CS_CLR;
    LCD_DC_SET;

    uint32_t count = width * height;

    uint32_t fill_count = count / FILL_BUFFER_COUNT;
    uint32_t else_count = count % FILL_BUFFER_COUNT;
    SPI_Flash_Read_dma_start(addr);

    SPI1_Read_DMA((uint8_t *)color_buffer[m], FILL_BUFFER_SIZE);

    while (fill_count > 0)
    {
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
            ;
        fill_count--;
        SPI2_Write_DMA((uint8_t *)color_buffer[m], FILL_BUFFER_SIZE);
        m = !m;
        SPI1_Read_DMA((uint8_t *)color_buffer[m], FILL_BUFFER_SIZE);
    }
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
        ;
    SPI2_Write_DMA((uint8_t *)color_buffer[m], else_count * COLOR_BYTE);
    SPI_Flash_Read_dma_end();
}
