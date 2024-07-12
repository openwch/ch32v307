/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/07/08
 * Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
 *CH32V317 Low power consumption mode related routines.Compared to other CH32V30X series products,
 *before entering the low power consumption mode,it is necessary to turn off the power supply of
 *the ETH-PHY to reduce power consumption.the HSE needs to be used as system clock source.
 *@ sleep or stop mode:
 *EXTI_Line0(PA0)
 *This routine demonstrates WFI\WFE enters sleep or stop mode, PA0 pin input high level triggers external
 *interrupt EXTI_Line0 exits sleep or stop mode,Program execution continues after wake-up.
 *@ standby mode:
 *WKUP(PA0)
 *This routine demonstrates that WFI enters the standby mode, the rising edge of the WKUP (PA0) pin
 *exits the standby mode,Program reset after wake-up.
 *Note: In order to reduce power consumption as much as possible, it is recommended to set the unused
 *GPIO to pull-down mode.
 */
#include "string.h"
#include "eth_driver.h"

/* Low power consumption Mode Definition */
#define  SLEEP_MODE   0
#define  STOP_MODE    1
#define  STANDBY_MODE 2

#define  LP_MODE  SLEEP_MODE
//#define  LP_MODE  STOP_MODE
//#define  LP_MODE  STANDBY_MODE

/* Execute with WFI or WFE Definition */
#define  Enter_WFI   0
#define  Enter_WFE   1

#define  Enter_MODE  Enter_WFI
//#define  Enter_MODE  Enter_WFE


/*********************************************************************
 * @fn      EXTI_INT_INIT
 *
 * @brief  Initializes EXTI0 collection.
 *
 * @return  none
 */
void EXTI0_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    /* GPIOA.0 ----> EXTI_Line0 */
    GPIO_EXTILineConfig( GPIO_PortSourceGPIOA, GPIO_PinSource0 );
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
#if(Enter_MODE == Enter_WFI)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
#elif(Enter_MODE == Enter_WFE)
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
#endif
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init( &EXTI_InitStructure );

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NVIC_InitStructure );
}

/*********************************************************************
 * @fn      GPIO_IPD_Init
 *
 * @brief   To reduce power consumption, unused GPIOs need to be set as pull-down inputs.
 *
 * @return  none
 */
void GPIO_IPD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    GPIO_PinRemapConfig( GPIO_Remap_SWJ_Disable, ENABLE );
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                            RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO , ENABLE );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    GPIO_Init( GPIOB, &GPIO_InitStructure );
    GPIO_Init( GPIOC, &GPIO_InitStructure );
    GPIO_Init( GPIOD, &GPIO_InitStructure );
    GPIO_Init( GPIOE, &GPIO_InitStructure );

    /* The pins used by ETH-PHY(PE7-PE15 and PD8) needs to be configured as floating inputs */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
                                  GPIO_Pin_12| GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOE, &GPIO_InitStructure );
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOD, &GPIO_InitStructure );
}


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    Delay_Ms(200);
    ETH_PHY_PowerDown();           //Turn off the power supply of the ETH-PHY
    GPIO_IPD_Init();

#if(LP_MODE != STANDBY_MODE)
    EXTI0_INT_INIT();
#endif
    USART_Printf_Init(115200);     //USART initialize
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

#if(LP_MODE == SLEEP_MODE)
    printf("Sleep Mode Test\r\n");
    if(((RCC->CFGR0 & RCC_SWS)==0x04)||((RCC->CFGR0 & RCC_PLLSRC) == RCC_PLLSRC))
    {
        RCC_HSICmd(DISABLE);
    }
    #if(Enter_MODE == Enter_WFI)
        __WFI();
    #elif(Enter_MODE == Enter_WFE)
        __WFE();
    #endif
#endif

#if(LP_MODE == STOP_MODE)
    printf("Stop Mode Test\r\n");
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    #if(Enter_MODE == Enter_WFI)
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    #elif(Enter_MODE == Enter_WFE)
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
        SystemInit();
    #endif
#endif

#if(LP_MODE == STANDBY_MODE)
    printf("Standby Mode Test\r\n");
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    if(PWR_GetFlagStatus(PWR_FLAG_WU) == SET)
    {
        printf("\r\n Standby wake up reset \r\n");
    }
    else
    {
        printf("\r\n Power on reset \r\n");
        PWR_WakeUpPinCmd(ENABLE);
        PWR_EnterSTANDBYMode();
    }
#endif
    printf("\r\n #################### \r\n");
    while(1)
    {
        Delay_Ms(1000);
        printf("Run in main\r\n");
    }
}
/*********************************************************************
 * @fn      EXTI0_IRQHandler
 *
 * @brief   This function handles EXTI0 exception.
 *
 * @return  none
 */
#if((LP_MODE != STANDBY_MODE) && (Enter_MODE == Enter_WFI))
__attribute__((interrupt("WCH-Interrupt-fast")))
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
    {
        SystemInit();
        printf("EXTI0 Wake_up\r\n");
        EXTI_ClearITPendingBit(EXTI_Line0);     /* Clear Flag */
    }
}
#endif
