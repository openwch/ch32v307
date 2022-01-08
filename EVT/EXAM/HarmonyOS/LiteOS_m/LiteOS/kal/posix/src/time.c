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

#define _GNU_SOURCE
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "time_internal.h"
#include "los_debug.h"
#include "los_task.h"
#include "los_swtmr.h"
#include "los_tick.h"
#include "los_context.h"

/* accumulative time delta from discontinuous modify */
STATIC struct timespec g_accDeltaFromSet;

STATIC const UINT16 g_daysInMonth[2][13] = {
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

STATIC const UINT8 g_montbl[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*
 * Time zone information, stored in seconds,
 * negative values indicate the east of UTC,
 * positive values indicate the west of UTC.
 */
long timezone = -8 * 60 * 60; // defaults to CST: 8 hours east of the Prime Meridian

/* internal shared struct tm object for localtime and gmtime */
static struct tm g_tm;

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
    UINT64 nseconds;
    UINT64 tick;
    UINT32 ret;
    const UINT32 nsPerTick = OS_SYS_NS_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND;

    if (!ValidTimeSpec(rqtp)) {
        errno = EINVAL;
        return -1;
    }

    nseconds = (UINT64)rqtp->tv_sec * OS_SYS_NS_PER_SECOND + rqtp->tv_nsec;

    tick = (nseconds + nsPerTick - 1) / nsPerTick; // Round up for ticks

    if (tick >= UINT32_MAX) {
        errno = EINVAL;
        return -1;
    }

    /* PS: skip the first tick because it is NOT a full tick. */
    ret = LOS_TaskDelay(tick ? (UINT32)(tick + 1) : 0);
    if (ret == LOS_OK || ret == LOS_ERRNO_TSK_YIELD_NOT_ENOUGH_TASK) {
        if (rmtp) {
            rmtp->tv_sec = rmtp->tv_nsec = 0;
        }
        return 0;
    }

    /* sleep in interrupt context or in task sched lock state */
    errno = EPERM;
    return -1;
}

int timer_create(clockid_t clockID, struct sigevent *restrict evp, timer_t *restrict timerID)
{
    UINT32 ret;
    UINT32 swtmrID;

    if (!timerID || (clockID != CLOCK_REALTIME)) {
        errno = EINVAL;
        return -1;
    }

    if (!evp || evp->sigev_notify != SIGEV_THREAD || evp->sigev_notify_attributes) {
        errno = ENOTSUP;
        return -1;
    }

    ret = LOS_SwtmrCreate(1, LOS_SWTMR_MODE_ONCE, (SWTMR_PROC_FUNC)evp->sigev_notify_function,
                          &swtmrID, (UINT32)(UINTPTR)evp->sigev_value.sival_ptr
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
                          , OS_SWTMR_ROUSES_IGNORE, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    if (ret != LOS_OK) {
        errno = (ret == LOS_ERRNO_SWTMR_MAXSIZE) ? EAGAIN : EINVAL;
        return -1;
    }

    *timerID = (timer_t)(UINTPTR)swtmrID;
    return 0;
}

int timer_delete(timer_t timerID)
{
    UINT32 swtmrID = (UINT32)(UINTPTR)timerID;
    if (LOS_SwtmrDelete(swtmrID) != LOS_OK) {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

int timer_settime(timer_t timerID, int flags,
                  const struct itimerspec *restrict value,
                  struct itimerspec *restrict oldValue)
{
    UINT32 intSave;
    UINT32 swtmrID = (UINT32)(UINTPTR)timerID;
    SWTMR_CTRL_S *swtmr = NULL;
    UINT32 interval, expiry, ret;

    if (flags != 0) {
        /* flags not supported currently */
        errno = ENOSYS;
        return -1;
    }

    if (value == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (!ValidTimeSpec(&value->it_value) || !ValidTimeSpec(&value->it_interval)) {
        errno = EINVAL;
        return -1;
    }

    expiry = OsTimeSpec2Tick(&value->it_value);
    interval = OsTimeSpec2Tick(&value->it_interval);

    /* if specified interval, it must be same with expiry due to the limitation of liteos-m */
    if (interval && interval != expiry) {
        errno = ENOTSUP;
        return -1;
    }

    if (oldValue) {
        (VOID)timer_gettime(timerID, oldValue);
    }

    ret = LOS_SwtmrStop(swtmrID);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_SWTMR_NOT_STARTED)) {
        errno = EINVAL;
        return -1;
    }

    intSave = LOS_IntLock();
    swtmr = OS_SWT_FROM_SID(swtmrID);
    swtmr->ucMode = (interval ? LOS_SWTMR_MODE_PERIOD : LOS_SWTMR_MODE_NO_SELFDELETE);
    swtmr->uwInterval = (interval ? interval : expiry);

    LOS_IntRestore(intSave);

    if ((value->it_value.tv_sec == 0) && (value->it_value.tv_nsec == 0)) {
        /*
         * 1) when expiry is 0, means timer should be stopped.
         * 2) If timer is ticking, stopping timer is already done before.
         * 3) If timer is created but not ticking, return 0 as well.
         */
        return 0;
    }

    if (LOS_SwtmrStart(swtmr->usTimerID) != LOS_OK) {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

int timer_gettime(timer_t timerID, struct itimerspec *value)
{
    UINT32 tick = 0;
    SWTMR_CTRL_S *swtmr = NULL;
    UINT32 swtmrID = (UINT32)(UINTPTR)timerID;
    UINT32 ret;

    if (value == NULL) {
        errno = EINVAL;
        return -1;
    }

    swtmr = OS_SWT_FROM_SID(swtmrID);

    /* get expire time */
    ret = LOS_SwtmrTimeGet(swtmr->usTimerID, &tick);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_SWTMR_NOT_STARTED)) {
        errno = EINVAL;
        return -1;
    }

    OsTick2TimeSpec(&value->it_value, tick);
    OsTick2TimeSpec(&value->it_interval, (swtmr->ucMode == LOS_SWTMR_MODE_ONCE) ? 0 : swtmr->uwInterval);
    return 0;
}

int timer_getoverrun(timer_t timerID)
{
    (void)timerID;

    errno = ENOSYS;
    return -1;
}

STATIC VOID OsGetHwTime(struct timespec *hwTime)
{
    UINT64 cycle = LOS_SysCycleGet();
    UINT64 nowNsec = (cycle / OS_SYS_CLOCK) * OS_SYS_NS_PER_SECOND +
                     (cycle % OS_SYS_CLOCK) * OS_SYS_NS_PER_SECOND / OS_SYS_CLOCK;

    hwTime->tv_sec = nowNsec / OS_SYS_NS_PER_SECOND;
    hwTime->tv_nsec = nowNsec % OS_SYS_NS_PER_SECOND;
}

STATIC VOID OsGetRealTime(struct timespec *realTime)
{
    UINT32 intSave;
    struct timespec hwTime = {0};
    OsGetHwTime(&hwTime);
    intSave = LOS_IntLock();
    realTime->tv_nsec = hwTime.tv_nsec + g_accDeltaFromSet.tv_nsec;
    realTime->tv_sec = hwTime.tv_sec + g_accDeltaFromSet.tv_sec + (realTime->tv_nsec >= OS_SYS_NS_PER_SECOND);
    realTime->tv_nsec %= OS_SYS_NS_PER_SECOND;
    LOS_IntRestore(intSave);
}

STATIC VOID OsSetRealTime(const struct timespec *realTime)
{
    UINT32 intSave;
    struct timespec hwTime = {0};
    OsGetHwTime(&hwTime);
    intSave = LOS_IntLock();
    g_accDeltaFromSet.tv_nsec = realTime->tv_nsec - hwTime.tv_nsec;
    g_accDeltaFromSet.tv_sec = realTime->tv_sec - hwTime.tv_sec - (g_accDeltaFromSet.tv_nsec < 0);
    g_accDeltaFromSet.tv_nsec = (g_accDeltaFromSet.tv_nsec + OS_SYS_NS_PER_SECOND) % OS_SYS_NS_PER_SECOND;
    LOS_IntRestore(intSave);
}

int clock_settime(clockid_t clockID, const struct timespec *tp)
{
    if (!ValidTimeSpec(tp)) {
        errno = EINVAL;
        return -1;
    }

    switch (clockID) {
        case CLOCK_REALTIME:
            /* we only support the realtime clock currently */
            OsSetRealTime(tp);
            return 0;
        case CLOCK_MONOTONIC_COARSE:
        case CLOCK_REALTIME_COARSE:
        case CLOCK_MONOTONIC_RAW:
        case CLOCK_PROCESS_CPUTIME_ID:
        case CLOCK_BOOTTIME:
        case CLOCK_REALTIME_ALARM:
        case CLOCK_BOOTTIME_ALARM:
        case CLOCK_SGI_CYCLE:
        case CLOCK_TAI:
        case CLOCK_THREAD_CPUTIME_ID:
            errno = ENOTSUP;
            return -1;
        case CLOCK_MONOTONIC:
        default:
            errno = EINVAL;
            return -1;
    }
}

int clock_gettime(clockid_t clockID, struct timespec *tp)
{
    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }

    switch (clockID) {
        case CLOCK_MONOTONIC_RAW:
        case CLOCK_MONOTONIC:
        case CLOCK_MONOTONIC_COARSE:
            OsGetHwTime(tp);
            return 0;
        case CLOCK_REALTIME:
        case CLOCK_REALTIME_COARSE:
            OsGetRealTime(tp);
            return 0;
        case CLOCK_THREAD_CPUTIME_ID:
        case CLOCK_PROCESS_CPUTIME_ID:
        case CLOCK_BOOTTIME:
        case CLOCK_REALTIME_ALARM:
        case CLOCK_BOOTTIME_ALARM:
        case CLOCK_SGI_CYCLE:
        case CLOCK_TAI:
            errno = ENOTSUP;
            return -1;
        default:
            errno = EINVAL;
            return -1;
    }
}

int clock_getres(clockid_t clockID, struct timespec *tp)
{
    if (tp == NULL) {
        errno = EINVAL;
        return -1;
    }

    switch (clockID) {
        case CLOCK_MONOTONIC_RAW:
        case CLOCK_MONOTONIC:
        case CLOCK_REALTIME:
        case CLOCK_MONOTONIC_COARSE:
        case CLOCK_REALTIME_COARSE:
            tp->tv_nsec = OS_SYS_NS_PER_SECOND / OS_SYS_CLOCK;
            tp->tv_sec = 0;
            return 0;
        case CLOCK_THREAD_CPUTIME_ID:
        case CLOCK_PROCESS_CPUTIME_ID:
        case CLOCK_BOOTTIME:
        case CLOCK_REALTIME_ALARM:
        case CLOCK_BOOTTIME_ALARM:
        case CLOCK_SGI_CYCLE:
        case CLOCK_TAI:
            errno = ENOTSUP;
            return -1;
        default:
            errno = EINVAL;
            return -1;
    }
}

int clock_nanosleep(clockid_t clk, int flags, const struct timespec *req, struct timespec *rem)
{
    switch (clk) {
        case CLOCK_REALTIME:
            if (flags == 0) {
                /* we only support the realtime clock currently */
                return nanosleep(req, rem);
            }
            /* fallthrough */
        case CLOCK_MONOTONIC_COARSE:
        case CLOCK_REALTIME_COARSE:
        case CLOCK_MONOTONIC_RAW:
        case CLOCK_MONOTONIC:
        case CLOCK_PROCESS_CPUTIME_ID:
        case CLOCK_BOOTTIME:
        case CLOCK_REALTIME_ALARM:
        case CLOCK_BOOTTIME_ALARM:
        case CLOCK_SGI_CYCLE:
        case CLOCK_TAI:
            if (flags == 0 || flags == TIMER_ABSTIME) {
                return ENOTSUP;
            }
            /* fallthrough */
        case CLOCK_THREAD_CPUTIME_ID:
        default:
            return EINVAL;
    }
}

clock_t clock(void)
{
    clock_t clk;
    struct timespec hwTime;
    OsGetHwTime(&hwTime);

    clk = hwTime.tv_sec * CLOCKS_PER_SEC;
    clk += hwTime.tv_nsec  / (OS_SYS_NS_PER_SECOND / CLOCKS_PER_SEC);

    return clk;
}

time_t time(time_t *timer)
{
    struct timespec ts;

    if (-1 == clock_gettime(CLOCK_REALTIME, &ts)) {
        return (time_t)-1;
    }

    if (timer != NULL) {
        *timer = ts.tv_sec;
    }
    return ts.tv_sec;
}

/*
 * Compute the `struct tm' representation of T,
 * offset OFFSET seconds east of UTC,
 * and store year, yday, mon, mday, wday, hour, min, sec into *TP.
 * Return nonzero if successful.
 */
static INT32 ConvertSecs2Utc(time_t t, INT32 offset, struct tm *tp)
{
    time_t days;
    time_t rem;
    time_t year;
    time_t month;
    time_t yearGuess;

    days = t / SECS_PER_DAY;
    rem = t % SECS_PER_DAY;
    rem += offset;
    while (rem < 0) {
        rem += SECS_PER_DAY;
        --days;
    }
    while (rem >= SECS_PER_DAY) {
        rem -= SECS_PER_DAY;
        ++days;
    }
    tp->tm_hour = rem / SECS_PER_HOUR;
    rem %= SECS_PER_HOUR;
    tp->tm_min = rem / SECS_PER_MIN;
    tp->tm_sec = rem % SECS_PER_MIN;
    /* January 1, 1970 was a Thursday.  */
    tp->tm_wday = (BEGIN_WEEKDAY + days) % DAYS_PER_WEEK;
    if (tp->tm_wday < 0) {
        tp->tm_wday += DAYS_PER_WEEK;
    }
    year = EPOCH_YEAR;

    while ((days < 0) ||
           (days >= (IS_LEAP_YEAR (year) ? DAYS_PER_LEAP_YEAR : DAYS_PER_NORMAL_YEAR))) {
        /* Guess a corrected year, assuming 365 days per year.  */
        yearGuess = year + days / DAYS_PER_NORMAL_YEAR - (days % DAYS_PER_NORMAL_YEAR < 0);

        /* Adjust days and year to match the guessed year.  */
        days -= ((yearGuess - year) * DAYS_PER_NORMAL_YEAR +
                 LEAPS_THRU_END_OF (yearGuess - 1) -
                 LEAPS_THRU_END_OF (year - 1));
        year = yearGuess;
    }
    tp->tm_year = year - TM_YEAR_BASE;
    if (tp->tm_year != year - TM_YEAR_BASE) {
        return 0;
    }
    tp->tm_yday = days;
    const UINT16 *daysInMonth = g_daysInMonth[IS_LEAP_YEAR(year)];
    /* valid month value is 0-11 */
    for (month = 11; days < (long int) daysInMonth[month]; --month) {
        continue;
    }
    days -= daysInMonth[month];
    tp->tm_mon = month;
    tp->tm_mday = days + 1;
    tp->__tm_gmtoff = offset;
    tp->__tm_zone = NULL;
    tp->tm_isdst = 0;
    return 1;
}

struct tm *gmtime_r(const time_t *timep, struct tm *result)
{
    if ((timep == NULL) || (result == NULL)) {
        errno = EFAULT;
        return NULL;
    }
    if (!ConvertSecs2Utc(*timep, 0, result)) {
        errno = EINVAL;
        return NULL;
    }
    return result;
}

struct tm *gmtime(const time_t *timer)
{
    return gmtime_r(timer, &g_tm);
}

struct tm *localtime_r(const time_t *timep, struct tm *result)
{
    if ((timep == NULL) || (result == NULL)) {
        errno = EFAULT;
        return NULL;
    }
    if (!ConvertSecs2Utc(*timep, -timezone, result)) {
        errno = EINVAL;
        return NULL;
    }
    return result;
}

struct tm *localtime(const time_t *timer)
{
    return localtime_r(timer, &g_tm);
}

static time_t ConvertUtc2Secs(struct tm *tm)
{
    time_t seconds = 0;
    INT32 month = 0;
    UINT8 leap = 0;

    INT32 year = (EPOCH_YEAR - TM_YEAR_BASE);
    while (year < tm->tm_year) {
        seconds += SECS_PER_NORMAL_YEAR;
        if (IS_LEAP_YEAR(year + TM_YEAR_BASE)) {
            seconds += SECS_PER_DAY;
        }
        year++;
    }

    if (IS_LEAP_YEAR(tm->tm_year + TM_YEAR_BASE)) {
        leap = 1;
    }
    while (month < tm->tm_mon) {
        if ((month == 1) && leap) {
            seconds += (g_montbl[month] + 1) * SECS_PER_DAY;
        } else {
            seconds += g_montbl[month] * SECS_PER_DAY;
        }
        month++;
    }

    seconds += (tm->tm_mday - 1) * SECS_PER_DAY;
    seconds += tm->tm_hour * SECS_PER_HOUR + tm->tm_min * SECS_PER_MIN + tm->tm_sec;

    seconds -= tm->__tm_gmtoff; // sub time zone to get UTC time
    return seconds;
}

time_t mktime(struct tm *tmptr)
{
    time_t timeInSeconds;
    if (tmptr == NULL) {
        errno = EFAULT;
        return (time_t)-1;
    }

    /* tm_isdst is not supported and is ignored */
    if (tmptr->tm_year < (EPOCH_YEAR - TM_YEAR_BASE) ||
            tmptr->__tm_gmtoff > (-TIME_ZONE_MIN * SECS_PER_MIN) ||
            tmptr->__tm_gmtoff < (-TIME_ZONE_MAX * SECS_PER_MIN) ||
            tmptr->tm_sec > 60 || tmptr->tm_sec < 0 ||      /* Seconds [0-60] */
            tmptr->tm_min > 59 || tmptr->tm_min < 0 ||      /* Minutes [0-59] */
            tmptr->tm_hour > 23 || tmptr->tm_hour < 0 ||    /* Hours [0-23] */
            tmptr->tm_mday > 31 || tmptr->tm_mday < 1 ||    /* Day of the month [1-31] */
            tmptr->tm_mon > 11 || tmptr->tm_mon < 0) {      /* Month [0-11] */
        errno = EOVERFLOW;
        return (time_t)-1;
    }
    timeInSeconds = ConvertUtc2Secs(tmptr);
    /* normalize tm_wday and tm_yday */
    ConvertSecs2Utc(timeInSeconds, tmptr->__tm_gmtoff, tmptr);
    return timeInSeconds;
}

int gettimeofday(struct timeval *tv, void *ptz)
{
    struct timespec ts;
    struct timezone *tz = (struct timezone *)ptz;

    if (tv != NULL) {
        if (-1 == clock_gettime(CLOCK_REALTIME, &ts)) {
            return -1;
        }
        tv->tv_sec = ts.tv_sec;
        tv->tv_usec = ts.tv_nsec / OS_SYS_NS_PER_US;
    }
    if (tz != NULL) {
        tz->tz_minuteswest = timezone / SECS_PER_MIN;
        tz->tz_dsttime = 0;
    }
    return 0;
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    struct timespec ts;
    INT32 rtcTimeZone = timezone;

    if (tv == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (tz != NULL) {
        if ((tz->tz_minuteswest >= TIME_ZONE_MIN) &&
            (tz->tz_minuteswest <= TIME_ZONE_MAX)) {
            rtcTimeZone = tz->tz_minuteswest * SECS_PER_MIN;
        } else {
            errno = EINVAL;
            return -1;
        }
    }

    if (tv->tv_usec >= OS_SYS_US_PER_SECOND) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = tv->tv_sec;
    ts.tv_nsec = tv->tv_usec * OS_SYS_NS_PER_US;
    if (-1 == clock_settime(CLOCK_REALTIME, &ts)) {
        return -1;
    }

    timezone = rtcTimeZone;

    return 0;
}

int usleep(unsigned useconds)
{
    struct timespec specTime = { 0 };
    UINT64 nanoseconds = (UINT64)useconds * OS_SYS_NS_PER_US;

    specTime.tv_sec = (time_t)(nanoseconds / OS_SYS_NS_PER_SECOND);
    specTime.tv_nsec = (long)(nanoseconds % OS_SYS_NS_PER_SECOND);
    return nanosleep(&specTime, NULL);
}

unsigned sleep(unsigned seconds)
{
    struct timespec specTime = { 0 };
    UINT64 nanoseconds = (UINT64)seconds * OS_SYS_NS_PER_SECOND;

    specTime.tv_sec = (time_t)(nanoseconds / OS_SYS_NS_PER_SECOND);
    specTime.tv_nsec = (long)(nanoseconds % OS_SYS_NS_PER_SECOND);
    return nanosleep(&specTime, NULL);
}
