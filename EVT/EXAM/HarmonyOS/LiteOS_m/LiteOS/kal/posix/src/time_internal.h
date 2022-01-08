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

#ifndef _TIME_IMPL_H
#define _TIME_IMPL_H

#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "los_debug.h"
#include "los_task.h"
#include "los_swtmr.h"
#include "los_timer.h"
#include "los_context.h"
#include "los_compiler.h"

#define OS_SYS_NS_PER_US 1000
#define OS_SYS_NS_PER_SECOND 1000000000
#define OS_SYS_US_PER_SECOND 1000000
#define OS_SYS_MS_PER_SECOND 1000

#define TM_YEAR_BASE         1900
#define EPOCH_YEAR           1970
#define SECS_PER_MIN         60
#define MINS_PER_HOUR        60
#define SECS_PER_HOUR        3600  /* 60 * 60 */
#define SECS_PER_DAY         86400 /* 60 * 60 * 24 */
#define SECS_PER_NORMAL_YEAR 31536000 /* 60 * 60 * 24 * 365 */
#define DAYS_PER_WEEK        7
#define DAYS_PER_NORMAL_YEAR 365
#define DAYS_PER_LEAP_YEAR   366
#define BEGIN_WEEKDAY        4
#define TIME_ZONE_MAX        720 /* UTC-12:00 , the last time zone */
#define TIME_ZONE_MIN        (-840) /* UTC+14:00 , the first time zone */

/*
 * Nonzero if YEAR is a leap year (every 4 years,
 * except every 100th isn't, and every 400th is).
 */
#ifndef IS_LEAP_YEAR
#define IS_LEAP_YEAR(year) \
    (((year) % 4 == 0) && (((year) % 100 != 0) || ((year) % 400 == 0)))
#endif

#define DIV(a, b) (((a) / (b)) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

/* internal functions */
STATIC INLINE BOOL ValidTimeSpec(const struct timespec *tp)
{
    /* Fail a NULL pointer */
    if (tp == NULL) {
        return FALSE;
    }

    /* Fail illegal nanosecond values */
    if ((tp->tv_nsec < 0) || (tp->tv_nsec >= OS_SYS_NS_PER_SECOND) || (tp->tv_sec < 0)) {
        return FALSE;
    }

    return TRUE;
}

STATIC INLINE UINT32 OsTimeSpec2Tick(const struct timespec *tp)
{
    UINT64 tick, ns;

    ns = (UINT64)tp->tv_sec * OS_SYS_NS_PER_SECOND + tp->tv_nsec;
    /* Round up for ticks */
    tick = (ns * LOSCFG_BASE_CORE_TICK_PER_SECOND + (OS_SYS_NS_PER_SECOND - 1)) / OS_SYS_NS_PER_SECOND;
    if (tick > LOS_WAIT_FOREVER) {
        tick = LOS_WAIT_FOREVER;
    }
    return (UINT32)tick;
}

STATIC INLINE VOID OsTick2TimeSpec(struct timespec *tp, UINT32 tick)
{
    UINT64 ns = ((UINT64)tick * OS_SYS_NS_PER_SECOND) / LOSCFG_BASE_CORE_TICK_PER_SECOND;
    tp->tv_sec = (time_t)(ns / OS_SYS_NS_PER_SECOND);
    tp->tv_nsec = (long)(ns % OS_SYS_NS_PER_SECOND);
}
#endif

