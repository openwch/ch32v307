/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @defgroup los_tick
 * @ingroup kernel
 */

#ifndef _LOS_TICK_H
#define _LOS_TICK_H

#include "los_error.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @ingroup los_tick
 * Tick error code: The Tick configuration is incorrect.
 *
 * Value: 0x02000400
 *
 * Solution: Change values of the OS_SYS_CLOCK and LOSCFG_BASE_CORE_TICK_PER_SECOND
 * system time configuration modules in Los_config.h.
 */
#define LOS_ERRNO_TICK_CFG_INVALID       LOS_ERRNO_OS_ERROR(LOS_MOD_TICK, 0x00)

/**
 * @ingroup los_tick
 * Tick error code: This error code is not in use temporarily.
 *
 * Value: 0x02000401
 *
 * Solution: None.
 */
#define LOS_ERRNO_TICK_NO_HWTIMER        LOS_ERRNO_OS_ERROR(LOS_MOD_TICK, 0x01)

/**
 * @ingroup los_tick
 * Tick error code: The number of Ticks is too small.
 *
 * Value: 0x02000402
 *
 * Solution: Change values of the OS_SYS_CLOCK and LOSCFG_BASE_CORE_TICK_PER_SECOND
 * system time configuration modules according to the SysTick_Config function.
 */
#define LOS_ERRNO_TICK_PER_SEC_TOO_SMALL LOS_ERRNO_OS_ERROR(LOS_MOD_TICK, 0x02)

/**
 *  @ingroup  los_tick
 *  @brief: System timer cycles get function.
 *
 *  @par Description:
 *  This API is used to get system timer cycles.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param: None.
 *
 * @retval: current system cycles.
 *
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 *
 * */
extern UINT64 LOS_SysCycleGet(VOID);

/**
 * @ingroup los_tick
 * Number of milliseconds in one second.
 */
#define OS_SYS_MS_PER_SECOND   1000

/**
 * @ingroup los_tick
 * Number of microseconds in one second.
 */
#define OS_SYS_US_PER_SECOND   1000000

#define OS_SYS_NS_PER_SECOND   1000000000

#define OS_SYS_NS_PER_US       1000

#define OS_CYCLE_PER_TICK      (OS_SYS_CLOCK / LOSCFG_BASE_CORE_TICK_PER_SECOND)

#define OS_NS_PER_CYCLE        (OS_SYS_NS_PER_SECOND / OS_SYS_CLOCK)

#define OS_MS_PER_TICK         (OS_SYS_MS_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND)

#define OS_US_PER_TICK         (OS_SYS_US_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND)

#define OS_NS_PER_TICK         (OS_SYS_NS_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND)

#define OS_SYS_CYCLE_TO_NS(cycle, freq)  (((cycle) / (freq)) * OS_SYS_NS_PER_SECOND + \
    ((cycle) % OS_SYS_CLOCK) * OS_SYS_NS_PER_SECOND / (freq))

#define OS_SYS_NS_TO_CYCLE(time, freq) (((time) / OS_SYS_NS_PER_SECOND) * (freq) +     \
    (time % OS_SYS_NS_PER_SECOND) * (freq) / OS_SYS_NS_PER_SECOND)

/**
 * @ingroup los_tick
 * System time basic function error code: Null pointer.
 *
 * Value: 0x02000010
 *
 * Solution: Check whether the input parameter is null.
 */
#define LOS_ERRNO_SYS_PTR_NULL                 LOS_ERRNO_OS_ERROR(LOS_MOD_SYS, 0x10)

/**
 * @ingroup los_tick
 * System time basic function error code: Invalid system clock configuration.
 *
 * Value: 0x02000011
 *
 * Solution: Configure a valid system clock in los_config.h.
 */
#define LOS_ERRNO_SYS_CLOCK_INVALID            LOS_ERRNO_OS_ERROR(LOS_MOD_SYS, 0x11)

/**
 * @ingroup los_tick
 * System time basic function error code: This error code is not in use temporarily.
 *
 * Value: 0x02000012
 *
 * Solution: None.
 */
#define LOS_ERRNO_SYS_MAXNUMOFCORES_IS_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_SYS, 0x12)

/**
 * @ingroup los_tick
 * System time error code: This error code is not in use temporarily.
 *
 * Value: 0x02000013
 *
 * Solution: None.
 */
#define LOS_ERRNO_SYS_PERIERRCOREID_IS_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_SYS, 0x13)

/**
 * @ingroup los_tick
 * System time error code: This error code is not in use temporarily.
 *
 * Value: 0x02000014
 *
 * Solution: None.
 */
#define LOS_ERRNO_SYS_HOOK_IS_FULL             LOS_ERRNO_OS_ERROR(LOS_MOD_SYS, 0x14)

/**
 * @ingroup los_tick
 * system time structure.
 */
typedef struct TagSysTime {
    UINT16  uwYear;    /**< value 1970 ~ 2038 or 1970 ~ 2100 */
    UINT8   ucMonth;   /**< value 1 - 12 */
    UINT8   ucDay;     /**< value 1 - 31 */
    UINT8   ucHour;    /**< value 0 - 23 */
    UINT8   ucMinute;  /**< value 0 - 59 */
    UINT8   ucSecond;  /**< value 0 - 59 */
    UINT8   ucWeek;    /**< value 0 - 6  */
} SYS_TIME_S;

/**
 * @ingroup los_tick
 * @brief Obtain the number of Ticks.
 *
 * @par Description:
 * This API is used to obtain the number of Ticks.
 * @attention
 * <ul>
 * <li>None</li>
 * </ul>
 *
 * @param  None
 *
 * @retval UINT64 The number of Ticks.
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 * @see None
 */
extern UINT64 LOS_TickCountGet(VOID);

/**
 * @ingroup los_tick
 * @brief Obtain the number of cycles in one second.
 *
 * @par Description:
 * This API is used to obtain the number of cycles in one second.
 * @attention
 * <ul>
 * <li>None</li>
 * </ul>
 *
 * @param  None
 *
 * @retval UINT32 Number of cycles obtained in one second.
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 * @see None
 */
extern UINT32 LOS_CyclePerTickGet(VOID);

/**
 * @ingroup los_tick
 * @brief Convert Ticks to milliseconds.
 *
 * @par Description:
 * This API is used to convert Ticks to milliseconds.
 * @attention
 * <ul>
 * <li>The number of milliseconds obtained through the conversion is 32-bit.</li>
 * </ul>
 *
 * @param  ticks  [IN] Number of Ticks. The value range is (0,OS_SYS_CLOCK).
 *
 * @retval UINT32 Number of milliseconds obtained through the conversion. Ticks are successfully converted to
 * milliseconds.
 * @par  Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_MS2Tick
 */
extern UINT32 LOS_Tick2MS(UINT32 ticks);

/**
 * @ingroup los_tick
 * @brief Convert milliseconds to Ticks.
 *
 * @par Description:
 * This API is used to convert milliseconds to Ticks.
 * @attention
 * <ul>
 * <li>If the parameter passed in is equal to 0xFFFFFFFF, the retval is 0xFFFFFFFF. Pay attention to the value to be
 * converted because data possibly overflows.</li>
 * </ul>
 *
 * @param  millisec  [IN] Number of milliseconds.
 *
 * @retval UINT32 Number of Ticks obtained through the conversion.
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 * @see LOS_Tick2MS
 */
extern UINT32 LOS_MS2Tick(UINT32 millisec);

/**
 * @ingroup los_tick
 * Ticks per second
 */
extern UINT32    g_ticksPerSec;

/**
 * @ingroup los_tick
 * Cycles per Second
 */
extern UINT32    g_uwCyclePerSec;

/**
 * @ingroup los_tick
 * Cycles per Tick
 */
extern UINT32    g_cyclesPerTick;

/**
 * @ingroup los_tick
 * System Clock
 */
extern UINT32    g_sysClock;

/**
 * @ingroup  los_tick
 * @brief Handle the system tick timeout.
 *
 * @par Description:
 * This API is called when the system tick timeout and triggers the interrupt.
 *
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param none.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern VOID OsTickHandler(VOID);

/**
 * @ingroup los_tick
 * Define the CPU Tick structure.
 */
typedef struct TagCpuTick {
    UINT32 cntHi; /* < Upper 32 bits of the tick value */
    UINT32 cntLo; /* < Lower 32 bits of the tick value */
} CpuTick;

/**
 * @ingroup los_tick
 * Number of operable bits of a 32-bit operand
 */
#define OS_SYS_MV_32_BIT       32

/**
 * @ingroup los_tick
 * Number of milliseconds in one second.
 */
#define OS_SYS_MS_PER_SECOND   1000

/**
 * @ingroup los_tick
 * Number of microseconds in one second.
 */
#define OS_SYS_US_PER_SECOND   1000000

/**
 * @ingroup los_tick
 * The maximum length of name.
 */
#define OS_SYS_APPVER_NAME_MAX 64

/**
 * @ingroup los_tick
 * The magic word.
 */
#define OS_SYS_MAGIC_WORD      0xAAAAAAAA

/**
 * @ingroup los_tick
 * The initialization value of stack space.
 */
#define OS_SYS_EMPTY_STACK     0xCACACACA

/**
 * @ingroup los_tick
 * @brief Convert cycles to milliseconds.
 *
 * @par Description:
 * This API is used to convert cycles to milliseconds.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  cpuTick  [IN]  Number of CPU cycles.
 * @param  msHi     [OUT] Upper 32 bits of the number of milliseconds.
 * @param  msLo     [OUT] Lower 32 bits of the number of milliseconds.
 *
 * @retval #LOS_ERRNO_SYS_PTR_NULL    0x02000011: Invalid parameter.
 * @retval #LOS_OK                   0:  Cycles are successfully converted to microseconds.
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern UINT32 OsCpuTick2MS(CpuTick *cpuTick, UINT32 *msHi, UINT32 *msLo);

/**
 * @ingroup los_tick
 * @brief Convert cycles to microseconds.
 *
 * @par Description:
 * This API is used to convert cycles to microseconds.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  cpuTick  [IN]  Number of CPU cycles.
 * @param  usHi     [OUT] Upper 32 bits of the number of microseconds.
 * @param  usLo     [OUT] Lower 32 bits of the number of microseconds.
 *
 * @retval #LOS_ERRNO_SYS_PTR_NULL    0x02000011: Invalid parameter.
 * @retval #LOS_OK                   0: Cycles are successfully converted to microseconds.
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern UINT32 OsCpuTick2US(CpuTick *cpuTick, UINT32 *usHi, UINT32 *usLo);

/**
 * @ingroup los_tick
 * @brief Convert cycles to milliseconds.
 *
 * @par Description:
 * This API is used to convert cycles to milliseconds.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  cycle     [IN] Number of cycles.
 *
 * @retval Number of milliseconds obtained through the conversion.    Cycles are successfully converted to milliseconds.
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
STATIC_INLINE UINT64 OsCycle2MS(UINT64 cycle)
{
    return (UINT64)((cycle / (g_sysClock / OS_SYS_MS_PER_SECOND)));
}

/**
 * @ingroup los_tick
 * @brief Convert cycles to microseconds.
 *
 * @par Description:
 * This API is used to convert cycles to microseconds.
 * @attention
 * <ul>
 * <li>None.</li>
 * </ul>
 *
 * @param  cycle     [IN] Number of cycles.
 *
 * @retval Number of microseconds obtained through the conversion. Cycles are successfully converted to microseconds.
 * @par Dependency:
 * <ul><li>los_tick.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
STATIC_INLINE UINT64 OsCycle2US(UINT64 cycle)
{
    UINT64 tmp = g_sysClock / OS_SYS_US_PER_SECOND;
    if (tmp == 0) {
        return 0;
    }
    return (UINT64)(cycle / tmp);
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_TICK_H */
