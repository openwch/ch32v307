/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/07/18
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/**
 * @note
 * The code initializes the TIM1 timer and the RTC module, and then performs calibration on the RTC
 * based on a calibration value.
 * 
 * this code block is checking if the `CalibrationVal` is less than 1000000. If it is, it
 * calculates the `FastSecPer30days` value by subtracting `CalibrationVal` from 1000000 and then
 * multiplying it by the number of seconds in 30 days (3600 seconds * 24 hours * 30 days) divided
 * by 1000000. This `FastSecPer30days` value is then passed to the `RTC_Calibration` function for
 * further processing.
 */
#include "debug.h"

/* Global define */
#define PPM_PER_STEP 0.9536743 // 10^6/2^20.
#define PPM_PER_SEC 0.3858025  // 10^6/(30d*24h*3600s).
/* Global Variable */
volatile uint8_t CalibrationFlag = 0;
volatile uint32_t CalibrationTIMCir = 0, CalibrationVal = 0;

typedef struct
{
    vu8 hour;
    vu8 min;
    vu8 sec;

    vu16 w_year;
    vu8 w_month;
    vu8 w_date;
    vu8 week;
} _calendar_obj;

_calendar_obj calendar;

u8 const table_week[12] = {0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5};
const u8 mon_table[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* Exported_Functions */
u8 RTC_Init(void);
u8 Is_Leap_Year(u16 year);
u8 RTC_Alarm_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec);
u8 RTC_Get(void);
u8 RTC_Get_Week(u16 year, u8 month, u8 day);
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec);

volatile uint8_t Calibration_STA = 0;

void RTC_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      RTC_IRQHandler
 *
 * @brief   This function handles RTC Handler.
 *
 * @return  none
 */
void RTC_IRQHandler(void)
{
    if (CalibrationFlag)
    {
        if (RTC_GetITStatus(RTC_IT_SEC) != RESET) /* Seconds interrupt */
        {
            RTC_Get();
        }
        if (RTC_GetITStatus(RTC_IT_ALR) != RESET) /* Alarm clock interrupt */
        {
            RTC_ClearITPendingBit(RTC_IT_ALR);
            RTC_Get();
        }
        printf("year/month/day/week/hour/min/sec:\r\n");
        printf("%d-%d-%d  %d  %d:%d:%d\r\n", calendar.w_year, calendar.w_month, calendar.w_date,
               calendar.week, calendar.hour, calendar.min, calendar.sec);
    }
    else
    {
        if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
        {
            if (Calibration_STA == 0)
            {
                CalibrationTIMCir = 0;
                TIM1->CNT = 0;
                TIM1->CTLR1 |= TIM_CEN;
                Calibration_STA = 1;
            }
            else if (Calibration_STA == 1)
            {

                TIM1->CTLR1 &= ~TIM_CEN;
                CalibrationVal = TIM1->CNT + CalibrationTIMCir * 65536;
                CalibrationVal < 1000000 ? printf("Calibration val = %ld\n", 1000000 - CalibrationVal) : printf("Calibration val = %ld\n", CalibrationVal - 1000000);
                ;

                TIM1->CNT = 0;
                Calibration_STA = 0;
                CalibrationFlag = 1;
            }
        }
    }
    RTC_ClearITPendingBit(RTC_IT_SEC | RTC_IT_OW);
    RTC_WaitForLastTask();
}

void TIM1_UP_IRQHandler()
{
    if (!CalibrationFlag)
        CalibrationTIMCir += 1;
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}


/*********************************************************************
 * @fn      TIM1_OutCompare_Init
 *
 * @brief   Initializes TIM1 output compare.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_Base_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    TIM1->CNT = 0;
}

/*********************************************************************
 * @fn      RTC_NVIC_Config
 *
 * @brief   Initializes RTC Int.
 *
 * @return  none
 */
static void RTC_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*********************************************************************
 * @fn      RTC_Init
 *
 * @brief   Initializes RTC collection.
 *
 * @return  1 - Init Fail
 *          0 - Init Success
 */
u8 RTC_Init(void)
{
    u8 temp = 0;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_ClearITPendingBit(RTC_IT_SEC);
    /* Is it the first configuration */

    BKP_DeInit();
    RCC_LSEConfig(RCC_LSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET && temp < 250)
    {
        temp++;
        Delay_Ms(20);
    }
    if (temp >= 250)
        return 1;
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE);
    RTC_WaitForLastTask();
    RTC_WaitForSynchro();
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    RTC_WaitForLastTask();
    RTC_EnterConfigMode();
    /*Deliberately speeding up the clock*/
    RTC_SetPrescaler(32766);
    RTC_WaitForLastTask();
    RTC_Set(2019, 10, 8, 13, 58, 55); /* Setup Time */
    RTC_ExitConfigMode();
    BKP_WriteBackupRegister(BKP_DR1, 0XA1A1);

    RTC_NVIC_Config();
    RTC_Get();

    return 0;
}

/*********************************************************************
 * @fn      Is_Leap_Year
 *
 * @brief   Judging whether it is a leap year.
 *
 * @param   year
 *
 * @return  1 - Yes
 *          0 - No
 */
u8 Is_Leap_Year(u16 year)
{
    if (year % 4 == 0)
    {
        if (year % 100 == 0)
        {
            if (year % 400 == 0)
                return 1;
            else
                return 0;
        }
        else
            return 1;
    }
    else
        return 0;
}

/*********************************************************************
 * @fn      RTC_Set
 *
 * @brief   Set Time.
 *
 * @param   Struct of _calendar_obj
 *
 * @return  1 - error
 *          0 - success
 */
u8 RTC_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
{
    u16 t;
    u32 seccount = 0;
    if (syear < 1970 || syear > 2099)
        return 1;
    for (t = 1970; t < syear; t++)
    {
        if (Is_Leap_Year(t))
            seccount += 31622400;
        else
            seccount += 31536000;
    }
    smon -= 1;
    for (t = 0; t < smon; t++)
    {
        seccount += (u32)mon_table[t] * 86400;
        if (Is_Leap_Year(syear) && t == 1)
            seccount += 86400;
    }
    seccount += (u32)(sday - 1) * 86400;
    seccount += (u32)hour * 3600;
    seccount += (u32)min * 60;
    seccount += sec;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetCounter(seccount);
    RTC_WaitForLastTask();
    return 0;
}

/*********************************************************************
 * @fn      RTC_Alarm_Set
 *
 * @brief   Set Alarm Time.
 *
 * @param   Struct of _calendar_obj
 *
 * @return  1 - error
 *          0 - success
 */
u8 RTC_Alarm_Set(u16 syear, u8 smon, u8 sday, u8 hour, u8 min, u8 sec)
{
    u16 t;
    u32 seccount = 0;
    if (syear < 1970 || syear > 2099)
        return 1;
    for (t = 1970; t < syear; t++)
    {
        if (Is_Leap_Year(t))
            seccount += 31622400;
        else
            seccount += 31536000;
    }
    smon -= 1;
    for (t = 0; t < smon; t++)
    {
        seccount += (u32)mon_table[t] * 86400;
        if (Is_Leap_Year(syear) && t == 1)
            seccount += 86400;
    }
    seccount += (u32)(sday - 1) * 86400;
    seccount += (u32)hour * 3600;
    seccount += (u32)min * 60;
    seccount += sec;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_SetAlarm(seccount);
    RTC_WaitForLastTask();

    return 0;
}

/*********************************************************************
 * @fn      RTC_Get
 *
 * @brief   Get current time.
 *
 * @return  1 - error
 *          0 - success
 */
u8 RTC_Get(void)
{
    static u16 daycnt = 0;
    u32 timecount = 0;
    u32 temp = 0;
    u16 temp1 = 0;
    timecount = RTC_GetCounter();
    temp = timecount / 86400;
    if (daycnt != temp)
    {
        daycnt = temp;
        temp1 = 1970;
        while (temp >= 365)
        {
            if (Is_Leap_Year(temp1))
            {
                if (temp >= 366)
                    temp -= 366;
                else
                {
                    break;
                }
            }
            else
                temp -= 365;
            temp1++;
        }
        calendar.w_year = temp1;
        temp1 = 0;
        while (temp >= 28)
        {
            if (Is_Leap_Year(calendar.w_year) && temp1 == 1)
            {
                if (temp >= 29)
                    temp -= 29;
                else
                    break;
            }
            else
            {
                if (temp >= mon_table[temp1])
                    temp -= mon_table[temp1];
                else
                    break;
            }
            temp1++;
        }
        calendar.w_month = temp1 + 1;
        calendar.w_date = temp + 1;
    }
    temp = timecount % 86400;
    calendar.hour = temp / 3600;
    calendar.min = (temp % 3600) / 60;
    calendar.sec = (temp % 3600) % 60;
    calendar.week = RTC_Get_Week(calendar.w_year, calendar.w_month, calendar.w_date);
    return 0;
}

/*********************************************************************
 * @fn      RTC_Get_Week
 *
 * @brief   Get the current day of the week.
 *
 * @param   year/month/day
 *
 * @return  week
 */
u8 RTC_Get_Week(u16 year, u8 month, u8 day)
{
    u16 temp2;
    u8 yearH, yearL;

    yearH = year / 100;
    yearL = year % 100;
    if (yearH > 19)
        yearL += 100;
    temp2 = yearL + yearL / 4;
    temp2 = temp2 % 7;
    temp2 = temp2 + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3)
        temp2--;
    return (temp2 % 7);
}

/*********************************************************************
 * @fn      RTC_Calibration
 *
 * @brief   The function `RTC_Calibration` calculates a calibration step value based on a given fast seconds per
 *        30 days value.
 *
 * @param   FastSecPer30days The `FastSecPer30days` parameter represents the number of fast seconds in a
 *        30-day period. This value is used to calculate the calibration step for the RTC (Real-Time Clock)
 *        based on the deviation from the ideal timekeeping.
 *
 * @return  none
 */
void RTC_Calibration(uint16_t FastSecPer30days)
{
    float Deviation = 0.0;
    u8 CalibStep = 0;

    Deviation = FastSecPer30days * PPM_PER_SEC;
    Deviation /= PPM_PER_STEP;
    CalibStep = (u8)Deviation;
    if (Deviation >= (CalibStep + 0.5))
        CalibStep += 1;
    if (CalibStep > 127)
        CalibStep = 127;

    BKP_SetRTCCalibrationValue(CalibStep);
    printf("Calibration cab: %d\n", CalibStep);
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
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();

    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("RTC Test\r\n");
    /* The code is initializing the TIM1 timer and the RTC (Real-Time Clock) module. */
    TIM1_Base_Init(65535, SystemCoreClock / 1000000 - 1);
    RTC_Init();

    while (CalibrationFlag == 0)
        ;
    

    if (CalibrationVal < 1000000)
    {
        uint16_t FastSecPer30days = (1000000 - CalibrationVal)*3600*24*30/1000000;
        RTC_Calibration(FastSecPer30days);
    }
    while (1)
    {
        Delay_Ms(100);
    }
}
