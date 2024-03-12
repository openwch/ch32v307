/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/03/05
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *  This example demonstrates control w2812 by using SPI or PWM.
 *
 *  In spi mode
 *  PA7 - w2812
 *  In PWM mode
 *  PA8 - w2812
 */

#include "debug.h"

/* Global define */

#define LED_SPI_MODE 1
#define LED_PWM_MODE 2

#define LED_MODE LED_SPI_MODE
//#define LED_MODE LED_PWM_MODE

#define Pixel_NUM (8)

#define LIST_SIZE(list) (sizeof(list)/sizeof(list[0]))
#define hex2rgb(c) (((c)>>16)&0xff),(((c)>>8)&0xff),((c)&0xff)

/* Global Variable */

#if LED_MODE == LED_SPI_MODE

/** spi mode
 * When you using spi mode, you should make sure your spi work at 6M clock
 */

#define Pixel_PRE_LEN (12u)
#define Pixel_RESET_LEN (25u)
#define COLOR_BUFFER_LEN (((Pixel_NUM)*Pixel_PRE_LEN)+Pixel_RESET_LEN)
#define SPI1_DMA_TX_CH   DMA1_Channel3


uint8_t color_buf[COLOR_BUFFER_LEN] = {0};


/*********************************************************************
 * @fn      convToBit
 *
 * @brief   Convert hex to spi bit
 *
 * @param   res  - the result
 *          input - input data
 *
 * @return  none
 */
void convToBit(uint8_t *res, uint8_t input)
{
    uint8_t mask = 0x80;
    for (int i = 0; i < 4; i++) {
        uint8_t result = (input & mask) ? 0xE : 0x8;
        result <<= 4;
        mask >>= 1;
        result |= (input & mask) ? 0xE : 0x8;
        mask >>= 1;
        res[i] = result;
    }
}


/*********************************************************************
 * @fn      colorToBit
 *
 * @brief   Convert color to spi bit
 *
 * @param   buf  - the result
 *          r  - red channel
 *          g  - green channel
 *          b  - blue channel
 *
 * @return  none
 */
void colorToBit(uint8_t *buf, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t *res = buf;
    convToBit(res, g);
    convToBit(&(res[4]), r);
    convToBit(&(res[8]), b);
}


/*********************************************************************
 * @fn      setPixelColor
 *
 * @brief   Set the pixel color of an LED
 *
 * @param   index - index of LED
 *          r  - red channel
 *          g  - green channel
 *          b  - blue channel
 *
 *
 * @return  none
 */
void setPixelColor(uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t *buf = &(color_buf[index * Pixel_PRE_LEN]);
    colorToBit(buf,r,g,b);
}

/*********************************************************************
 * @fn      SPI_1Lines_HalfDuplex_Init
 *
 * @brief   Configuring the SPI for half-duplex communication.
 *
 * @return  none
 */
void SPI_1Lines_HalfDuplex_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure= {0};
    SPI_InitTypeDef SPI_InitStructure= {0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE );


    // the clock output
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init( GPIOA, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init( SPI1, &SPI_InitStructure );

    SPI_Cmd( SPI1, ENABLE );
}

/*********************************************************************
 * @fn      SPI1_DMA_Init
 *
 * @brief   Initialize DMA for SPI2
 *
 * @return  none
 */
void SPI1_DMA_Init()
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(SPI1_DMA_TX_CH);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)color_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = COLOR_BUFFER_LEN;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(SPI1_DMA_TX_CH, &DMA_InitStructure);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
}
#elif    LED_MODE == LED_PWM_MODE

void DMA1_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

#define CODE_0      (3)
#define CODE_1      (7)
#define RESET_LEN    (60)
#define TIM_DMA_CH1_CH   DMA1_Channel5
#define COLOR_BUFFER_LEN (((Pixel_NUM)*(3*8))+RESET_LEN)

uint16_t color_buf[COLOR_BUFFER_LEN] = {0};

/*********************************************************************
 * @fn      TIM1_Init
 *
 * @brief   Initialize TIM1
 *
 * @return  none
 */
void TIM1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseInitStructure.TIM_Period = 10 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = SystemCoreClock / 8000000 - 1;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init( TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig( TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);
    TIM_Cmd( TIM1, ENABLE);
}

/*********************************************************************
 * @fn      DMA1_Init
 *
 * @brief   Initialize DMA for TIM1 ch1
 *
 * @return  none
 */
void DMA1_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit( TIM_DMA_CH1_CH);
    DMA_Cmd( TIM_DMA_CH1_CH, DISABLE);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &TIM1->CH1CVR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) color_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = COLOR_BUFFER_LEN;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init( TIM_DMA_CH1_CH, &DMA_InitStructure);

    DMA_Cmd( TIM_DMA_CH1_CH, DISABLE);

    DMA_ITConfig( DMA1_Channel5, DMA_IT_TC, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/*********************************************************************
 * @fn      setPixelColor
 *
 * @brief   Set the pixel color of an LED
 *
 * @param   id - index of LED
 *          r  - red channel
 *          g  - green channel
 *          b  - blue channel
 *
 *
 * @return  none
 */
void setPixelColor(uint16_t id, uint8_t r, uint8_t g, uint8_t b)
{
    int i = 0, j = id * 24u;
    if (id >= Pixel_NUM) {
        return;
    }

    for (i = 0; i < 8; i++) {
        if (g & (0x80 >> i)) {
            color_buf[j] = CODE_1;
        } else {
            color_buf[j] = CODE_0;
        }
        j++;
    }
    for (i = 0; i < 8; i++) {
        if (r & (0x80 >> i)) {
            color_buf[j] = CODE_1;
        } else {
            color_buf[j] = CODE_0;
        }
        j++;
    }
    for (i = 0; i < 8; i++) {
        if (b & (0x80 >> i)) {
            color_buf[j] = CODE_1;
        } else {
            color_buf[j] = CODE_0;
        }
        j++;
    }
}
#endif

/*********************************************************************
 * @fn      ws2812_sync
 *
 * @brief   Write data to LEDs
 *
 * @return  none
 */
void w2812_sync()
{
#if LED_MODE == LED_SPI_MODE
    while(SPI1_DMA_TX_CH->CNTR!=0);
    DMA_ClearFlag(DMA1_FLAG_TC3);
    DMA_Cmd(SPI1_DMA_TX_CH, DISABLE);
    SPI1_DMA_TX_CH->CNTR = COLOR_BUFFER_LEN;
    DMA_Cmd(SPI1_DMA_TX_CH, ENABLE);
#elif  LED_MODE == LED_PWM_MODE
    while(DMA_GetCurrDataCounter(TIM_DMA_CH1_CH)!=0);
    DMA_SetCurrDataCounter( TIM_DMA_CH1_CH, COLOR_BUFFER_LEN);
    DMA_Cmd( TIM_DMA_CH1_CH, ENABLE);
    TIM_Cmd( TIM1, ENABLE);
#endif
}

#define MAX_STEP (200)
/*********************************************************************
 * @fn      interpolateColors
 *
 * @brief   Set the pixel color of an LED
 *
 * @param   color1 - The color at the beginning
 *          color2 - The color at the ending
 *          step   -
 *
 *
 * @return  color
 */
uint32_t interpolateColors(uint32_t color1, uint32_t color2, uint8_t step)
{
    uint8_t r1 = (color1 >> 16) & 0xFF;
    uint8_t g1 = (color1 >> 8) & 0xFF;
    uint8_t b1 = color1 & 0xFF;

    uint8_t r2 = (color2 >> 16) & 0xFF;
    uint8_t g2 = (color2 >> 8) & 0xFF;
    uint8_t b2 = color2 & 0xFF;

    float tmp1 = (r2 - r1) * (step / (float) MAX_STEP);
    float tmp2 = (g2 - g1) * (step / (float) MAX_STEP);
    float tmp3 = (b2 - b1) * (step / (float) MAX_STEP);

    uint8_t r = (uint8_t) (r1 + tmp1);
    uint8_t g = (uint8_t) (g1 + tmp2);
    uint8_t b = (uint8_t) (b1 + tmp3);

    return ((uint32_t) r << 16) | ((uint32_t) g << 8) | b;
}

uint32_t color_list[] = {
        0xff0000,
        0x00ff00,
        0x0000ff,
        0xffff00,
        0xff00ff,
        0x00ffff,
        0xffffff,
//        0x000000,
        };

/*********************************************************************
 * @fn      led_example_0
 *
 * @brief   In this example, all leds will gradually change from one color to another
 *
 * @return  none
 */
void led_example_0(void)
{
    uint32_t i = 0, j = 0;
    uint32_t c = 0x0f0f00;
    while(1) {
        c = color_list[i];
        uint32_t next_color = color_list[i+1];
        i++;
        if((i+1)>=LIST_SIZE(color_list)) {
            i=0;
        }
        for(j = 0; j<MAX_STEP;j+=1) {
            uint32_t color = interpolateColors(c,next_color,j);
            for (int var = 0; var < Pixel_NUM; ++var) {
                setPixelColor(var,hex2rgb(color));

            }
            w2812_sync();
            Delay_Ms(10);

        }

    }
}

/*********************************************************************
 * @fn      led_example_1
 *
 * @brief   In this example, all leds will blink
 *
 * @return  none
 */
void led_example_1(void)
{
    uint32_t i = 0;
    uint32_t c = 0x0f0f00;
    while(1) {
        c = color_list[i];
        printf("color[%d] %08x  %08x \n",i,c,color_list[i]);
        i++;
        if(i>=LIST_SIZE(color_list)) {
            i=0;
        }
        for (int var = 0; var < Pixel_NUM; ++var) {
            setPixelColor(var,hex2rgb(c));
        }
        w2812_sync();
        Delay_Ms(100);

    }
}

/*********************************************************************
 * @fn      led_example_2
 *
 * @brief   In this example, the leds will blink one by one
 *
 * @return  none
 */
void led_example_2(void)
{
    uint32_t i = 0;
    uint32_t c = 0x0f0f00;
    while(1) {
        c = color_list[i];
        printf("color[%d] %08x  %08x \n",i,c,color_list[i]);
        i++;
        if(i>=LIST_SIZE(color_list)) {
            i=0;
        }

        for (int var = 0; var < Pixel_NUM; var+=1) {
            setPixelColor(var, hex2rgb(c));
            w2812_sync();
            Delay_Ms(100);
            setPixelColor(var,0,0,0);
            w2812_sync();
        }
    }
}
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    uint16_t i = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_Init();
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);

    // Turn off all LEDs
    for (i = 0; i < Pixel_NUM; i++) {
        setPixelColor(i, 0, 0, 0);
    }
#if LED_MODE == LED_SPI_MODE
    SPI_1Lines_HalfDuplex_Init();
    SPI1_DMA_Init();
    DMA_Cmd(SPI1_DMA_TX_CH, ENABLE);
#elif  LED_MODE == LED_PWM_MODE
    TIM1_Init();
    DMA1_Init();
    DMA_Cmd( TIM_DMA_CH1_CH, ENABLE);
#endif

//    led_example_0();
    led_example_2();

}

#if  LED_MODE == LED_PWM_MODE
void DMA1_Channel5_IRQHandler(void)
{
    if (DMA_GetFlagStatus( DMA1_FLAG_TC5)) {
        TIM_Cmd( TIM1, DISABLE);
        DMA_Cmd( TIM_DMA_CH1_CH, DISABLE);
        DMA_ClearFlag( DMA1_FLAG_TC5);
    }
}
#endif
