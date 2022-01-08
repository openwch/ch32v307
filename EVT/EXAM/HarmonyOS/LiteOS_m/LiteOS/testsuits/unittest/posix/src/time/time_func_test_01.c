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

#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include "ohos_types.h"
#include "hctest.h"
#include "los_config.h"
#include "securec.h"
#include "kernel_test.h"
#include "log.h"

#define RET_OK 0

#define SLEEP_ACCURACY 21000  // 20 ms, with 1ms deviation
#define ACCURACY_TEST_LOOPS 3 // loops for accuracy test, than count average value
#define MILLISECONDS_PER_SECOND 1000
#define NANOSECONDS_PER_MILLISECOND 1000000
#define TM_BASE_YEAR 1900
#define TIME_STR_LEN 100

#define INIT_TM(tmSt, year, mon, day, hour, min, sec, wday) \
    do {                                                    \
        (tmSt).tm_sec = (sec);                             \
        (tmSt).tm_min = (min);                             \
        (tmSt).tm_hour = (hour);                           \
        (tmSt).tm_mday = (day);                            \
        (tmSt).tm_mon = (mon);                             \
        (tmSt).tm_year = (year) - 1900;                      \
        (tmSt).tm_wday = wday;                                \
        (tmSt).__tm_gmtoff = 0;                               \
        (tmSt).__tm_zone = "";                              \
    } while (0)

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, PosixTime, PosixTimeFuncTestSuite);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixTimeFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixTimeFuncTestSuiteTearDown(void)
{
    return TRUE;
}

static int KeepRun(int msec)
{
    struct timespec time1 = { 0, 0 };
    struct timespec time2 = { 0, 0 };
    clock_gettime(CLOCK_MONOTONIC, &time1);
    LOG("KeepRun start : tv_sec=%ld, tv_nsec=%ld\n", time1.tv_sec, time1.tv_nsec);
    int loop = 0;
    int runned = 0;
    while (runned < msec) {
        ++loop;
        clock_gettime(CLOCK_MONOTONIC, &time2);
        runned = (time2.tv_sec - time1.tv_sec) * MILLISECONDS_PER_SECOND;
        runned += (time2.tv_nsec - time1.tv_nsec) / NANOSECONDS_PER_MILLISECOND;
    }

    LOG("KeepRun end : tv_sec=%ld, tv_nsec=%ld\n", time2.tv_sec, time2.tv_nsec);
    return loop;
}

static int CheckValueClose(double target, double actual, double accuracy)
{
    double diff = actual - target;
    double pct;
    if (diff < 0) {
        diff = -diff;
    }
    if (actual == 0) {
        return 0;
    } else {
        pct = diff / actual;
    }
    return (pct <= accuracy);
}

static char *TmToStr(const struct tm *stm, char *timeStr, unsigned len)
{
    if (stm == NULL || timeStr == NULL) {
        return "";
    }
    sprintf_s(timeStr, len, "%ld/%d/%d %02d:%02d:%02d WEEK(%d)", stm->tm_year + TM_BASE_YEAR, stm->tm_mon + 1,
        stm->tm_mday, stm->tm_hour, stm->tm_min, stm->tm_sec, stm->tm_wday);
    return timeStr;
}

/* *
 * @tc.number SUB_KERNEL_TIME_USLEEP_001
 * @tc.name   usleep accuracy test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeUSleep001, Function | MediumTest | Level1)
{
    // wifiiot无法支持10ms以下的sleep
    int interval[] = {15*1000, 20*1000, 30*1000, 300*1000};
    for (unsigned int j = 0; j < sizeof(interval) / sizeof(int); j++) {
        LOG("\ntest interval:%d\n", interval[j]);
        struct timespec time1 = { 0 }, time2 = { 0 };
        long duration; // unit: us
        double d = 0.0;
        for (int i = 1; i <= ACCURACY_TEST_LOOPS; i++) {
            clock_gettime(CLOCK_MONOTONIC, &time1);
            int rt = usleep(interval[j]);
            clock_gettime(CLOCK_MONOTONIC, &time2);
            TEST_ASSERT_EQUAL_INT(RET_OK, rt);
            duration = (time2.tv_sec - time1.tv_sec) * 1000000 + (time2.tv_nsec - time1.tv_nsec) / 1000;
            LOG("testloop %d, actual usleep duration: %ld us\n", i, duration);
            d += duration;
        }
        d = d / ACCURACY_TEST_LOOPS; // average
        LOG("interval:%u, average duration: %.2f\n", interval[j], d);
        TEST_ASSERT_GREATER_OR_EQUAL(interval[j], d);
        TEST_ASSERT_INT32_WITHIN(SLEEP_ACCURACY, interval[j], d);
    }
}

/* *
 * @tc.number SUB_KERNEL_TIME_USLEEP_002
 * @tc.name   usleep test for special delay
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeUSleep002, Function | MediumTest | Level1)
{
    struct timespec time1 = { 0 };
    struct timespec time2 = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &time1);
    int rt = usleep(0);
    clock_gettime(CLOCK_MONOTONIC, &time2);
    TEST_ASSERT_EQUAL_INT(RET_OK, rt);
    long long duration = (time2.tv_sec - time1.tv_sec) * 1000000 + (time2.tv_nsec - time1.tv_nsec) / 1000;
    LOG("\n usleep(0), actual usleep duration: %lld us\n", duration);
    TEST_ASSERT_LESS_OR_EQUAL_INT64(1000, duration);
}

/* *
 * @tc.number     SUB_KERNEL_TIME_GMTIME_001
 * @tc.name       test gmtime api
 * @tc.desc       [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeGmtime001, Function | MediumTest | Level1)
{
    time_t time1 = 18880;
    char timeStr[TIME_STR_LEN] = {0};
    LOG("\nsizeof(time_t) = %d, sizeof(struct tm) = %d", sizeof(time_t), sizeof(struct tm));
    struct tm *stm = gmtime(&time1);
    TEST_ASSERT_EQUAL_STRING("1970/1/1 05:14:40 WEEK(4)", TmToStr(stm, timeStr, TIME_STR_LEN));

    time1 = LONG_MAX;
    stm = gmtime(&time1);
    LOG("\n LONG_MAX = %lld, cvt result : %s", time1, TmToStr(stm, timeStr, TIME_STR_LEN));
    TEST_ASSERT_EQUAL_STRING("2038/1/19 03:14:07 WEEK(2)", TmToStr(stm, timeStr, TIME_STR_LEN));

    time1 = LONG_MAX - 1;
    stm = gmtime(&time1);
    LOG("\n LONG_MAX - 1 = %lld, cvt result : %s", time1, TmToStr(stm, timeStr, TIME_STR_LEN));
    TEST_ASSERT_EQUAL_STRING("2038/1/19 03:14:06 WEEK(2)", TmToStr(stm, timeStr, TIME_STR_LEN));

    time1 = LONG_MIN;
    stm = gmtime(&time1);
    LOG("\n LONG_MIN  = %lld, cvt result : %s", time1, TmToStr(stm, timeStr, TIME_STR_LEN));
    TEST_ASSERT_EQUAL_STRING("1901/12/13 20:45:52 WEEK(5)", TmToStr(stm, timeStr, TIME_STR_LEN));

    time1 = LONG_MIN + 1;
    stm = gmtime(&time1);
    LOG("\n LONG_MIN + 1  = %lld, cvt result : %s", time1, TmToStr(stm, timeStr, TIME_STR_LEN));
    TEST_ASSERT_EQUAL_STRING("1901/12/13 20:45:53 WEEK(5)", TmToStr(stm, timeStr, TIME_STR_LEN));
};

/* *
 * @tc.number     SUB_KERNEL_TIME_LOCALTIME_001
 * @tc.name       test localtime api
 * @tc.desc       [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeLocaltime001, Function | MediumTest | Level1)
{
    char cTime[32];
    time_t tStart;
    time_t tEnd;

    struct timeval timeSet = {
        .tv_sec = 86399,
        .tv_usec = 0
    };

    int ret = settimeofday(&timeSet, NULL);
    time(&tStart);
    sleep(2);
    time(&tEnd);
    TEST_ASSERT_EQUAL_INT(0, ret);

    struct tm *tmStart = localtime(&tStart);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", tmStart);
    TEST_ASSERT_EQUAL_STRING("23:59:59", cTime);
    LOG("\n time_t=%lld, first time:%s", tStart, cTime);
    struct tm *tmEnd = localtime(&tEnd);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", tmEnd);
    TEST_ASSERT_EQUAL_STRING("00:00:01", cTime);
    LOG("\n time_t=%lld, first time:%s", tEnd, cTime);
}

/* *
 * @tc.number     SUB_KERNEL_TIME_LOCALTIME_002
 * @tc.name       test localtime api
 * @tc.desc       [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeLocaltime002, Function | MediumTest | Level1)
{
    char cTime[32];
    time_t tStart = LONG_MAX;
    struct tm *tmStart = localtime(&tStart);
    strftime(cTime, sizeof(cTime), "%y-%m-%d %H:%M:%S", tmStart);
    TEST_ASSERT_EQUAL_STRING("38-01-19 11:14:07", cTime);
    LOG("\n time_t=%lld, first time:%s", tStart, cTime);

    tStart = LONG_MIN;
    tmStart = localtime(&tStart);
    strftime(cTime, sizeof(cTime), "%y-%m-%d %H:%M:%S", tmStart);
    TEST_ASSERT_EQUAL_STRING("01-12-14 04:45:52", cTime);
    LOG("\n time_t=%lld, first time:%s", tStart, cTime);

    tStart = 0;
    tmStart = localtime(&tStart);
    strftime(cTime, sizeof(cTime), "%y-%m-%d %H:%M:%S", tmStart);
    TEST_ASSERT_EQUAL_STRING("70-01-01 08:00:00", cTime);
    LOG("\n time_t=%lld, first time:%s", tStart, cTime);

    tStart = -1;
    tmStart = localtime(&tStart);
    strftime(cTime, sizeof(cTime), "%y-%m-%d %H:%M:%S", tmStart);
    TEST_ASSERT_EQUAL_STRING("70-01-01 07:59:59", cTime);
    LOG("\n time_t=%lld, first time:%s", tStart, cTime);
}

/* *
 * @tc.number     SUB_KERNEL_TIME_LOCALTIMER_001
 * @tc.name       localtime_r api base test
 * @tc.desc       [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeLocaltimer001, Function | MediumTest | Level1)
{
    char cTime[32];
    time_t tStart;
    time_t tEnd;
    struct tm tmrStart = { 0 };
    struct tm tmrEnd = { 0 };
    struct timeval tSet = {
        .tv_sec = 86399,
        .tv_usec = 0
    };

    int ret = settimeofday(&tSet, NULL);
    time(&tStart);
    sleep(2);
    time(&tEnd);
    TEST_ASSERT_EQUAL_INT(0, ret);

    struct tm *tmrStartPtr = localtime_r(&tStart, &tmrStart);
    struct tm *tmrEndPtr = localtime_r(&tEnd, &tmrEnd);

    strftime(cTime, sizeof(cTime), "%H:%M:%S", &tmrStart);
    TEST_ASSERT_EQUAL_STRING("23:59:59", cTime);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", tmrStartPtr);
    TEST_ASSERT_EQUAL_STRING("23:59:59", cTime);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", &tmrEnd);
    TEST_ASSERT_EQUAL_STRING("00:00:01", cTime);
    strftime(cTime, sizeof(cTime), "%H:%M:%S", tmrEndPtr);
    TEST_ASSERT_EQUAL_STRING("00:00:01", cTime);
}

/* *
 * @tc.number     SUB_KERNEL_TIME_LOCALTIMER_002
 * @tc.name       test localtime_r api for range
 * @tc.desc       [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeLocaltimer002, Function | MediumTest | Level1)
{
    char cTime[32];
    struct tm tmrResult = { 0 };

    time_t tStart = LONG_MAX;
    struct tm *tmStart = localtime_r(&tStart, &tmrResult);
    strftime(cTime, sizeof(cTime), "%y-%m-%d %H:%M:%S", tmStart);
    TEST_ASSERT_EQUAL_STRING("38-01-19 11:14:07", cTime);
    LOG("\n time_t=%lld, first time:%s", tStart, cTime);

    tStart = LONG_MIN;
    tmStart = localtime_r(&tStart, &tmrResult);
    strftime(cTime, sizeof(cTime), "%y-%m-%d %H:%M:%S", tmStart);
    TEST_ASSERT_EQUAL_STRING("01-12-14 04:45:52", cTime);
    LOG("\n time_t=%lld, first time:%s", tStart, cTime);

    tStart = 0;
    tmStart = localtime_r(&tStart, &tmrResult);
    strftime(cTime, sizeof(cTime), "%y-%m-%d %H:%M:%S", tmStart);
    TEST_ASSERT_EQUAL_STRING("70-01-01 08:00:00", cTime);
    LOG("\n time_t=%lld, first time:%s", tStart, cTime);

    tStart = -1;
    tmStart = localtime_r(&tStart, &tmrResult);
    strftime(cTime, sizeof(cTime), "%y-%m-%d %H:%M:%S", tmStart);
    TEST_ASSERT_EQUAL_STRING("70-01-01 07:59:59", cTime);
    LOG("\n time_t=%lld, first time:%s", tStart, cTime);
}

/* *
 * @tc.number     SUB_KERNEL_TIME_MKTIME_001
 * @tc.name       mktime api base test
 * @tc.desc       [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeMktime001, Function | MediumTest | Level1)
{
    // default time zone east 8
    struct tm timeptr = { 0 };
    time_t testTime = 18880;
    char timeStr[TIME_STR_LEN] = {0};

    INIT_TM(timeptr, 2020, 7, 9, 18, 10, 0, 7);
    time_t timeRet = mktime(&timeptr);
    LOG("\n 2020-7-9 18:10:00, mktime Ret = %lld", timeRet);
    TEST_ASSERT_EQUAL_INT(1596967800, timeRet);

    INIT_TM(timeptr, 1970, 0, 1, 8, 0, 0, 0);
    timeRet = mktime(&timeptr);
    LOG("\n 1970-1-1 08:00:00, mktime Ret = %lld", timeRet);
    TEST_ASSERT_EQUAL_INT(0, timeRet);

    struct tm *stm = localtime(&testTime);
    LOG("\n testTime 18880, tm : %s", TmToStr(stm, timeStr, TIME_STR_LEN));
    timeRet = mktime(stm);
    TEST_ASSERT_EQUAL_INT(18880, timeRet);
    LOG("\n input 18880, mktime Ret = %lld", timeRet);

    testTime = LONG_MAX;
    stm = localtime(&testTime);
    LOG("\n testTime LONG_MAX, tm : %s", TmToStr(stm, timeStr, TIME_STR_LEN));
    timeRet = mktime(stm);
    TEST_ASSERT_EQUAL_INT(LONG_MAX, timeRet);
    LOG("\n input LONG_MAX, mktime Ret = %lld", timeRet);

    testTime = 0;
    stm = localtime(&testTime);
    LOG("\n testTime 0, tm : %s", TmToStr(stm, timeStr, TIME_STR_LEN));
    timeRet = mktime(stm);
    TEST_ASSERT_EQUAL_INT(0, timeRet);
    LOG("\n input 0, mktime Ret = %lld", timeRet);
}

/* *
 * @tc.number     SUB_KERNEL_TIME_MKTIME_002
 * @tc.name       mktime api test for invalid input
 * @tc.desc       [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeMktime002, Function | MediumTest | Level1)
{
    struct tm timeptr = { 0 };
    LOG("\n sizeof(time_t) = %d", sizeof(time_t));
    INIT_TM(timeptr, 1969, 7, 9, 10, 10, 0, 7);
    time_t timeRet = mktime(&timeptr);
    LOG("\n 1800-8-9 10:10:00, mktime Ret lld = %lld", timeRet);
    TEST_ASSERT_EQUAL_INT(0, timeRet);
}


/* *
 * @tc.number     SUB_KERNEL_TIME_STRFTIME_001
 * @tc.name       test strftime api
 * @tc.desc       [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeStrftime001, Function | MediumTest | Level3)
{
    char buffer[80];
    time_t mtime = 0;
    size_t ftime = 0;

    mtime = LONG_MAX;
    ftime = strftime(buffer, 80, "%y-%m-%d %H:%M:%S", localtime(&mtime));
    TEST_ASSERT_GREATER_THAN_INT(0, ftime);
    TEST_ASSERT_EQUAL_STRING("38-01-19 11:14:07", buffer);
    LOG("\nresult: %s, expected : %s", buffer, "38-01-19 11:14:07");

    mtime = LONG_MIN;
    ftime = strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&mtime));
    TEST_ASSERT_GREATER_THAN_INT(0, ftime);
    TEST_ASSERT_EQUAL_STRING("1901-12-14 04:45:52", buffer);
    LOG("\nresult: %s, expected : %s", buffer, "1901-12-14 04:45:52");

    mtime = 18880;
    ftime = strftime(buffer, 80, "%F %T", localtime(&mtime));
    TEST_ASSERT_GREATER_THAN_INT(0, ftime);
    TEST_ASSERT_EQUAL_STRING("1970-01-01 13:14:40", buffer);
    LOG("\nresult: %s, expected : %s", buffer, "1970-01-01 13:14:40");

    mtime = 18880;
    ftime = strftime(buffer, 80, "%D %w %H:%M:%S", localtime(&mtime));
    TEST_ASSERT_GREATER_THAN_INT(0, ftime);
    TEST_ASSERT_EQUAL_STRING("01/01/70 4 13:14:40", buffer);
    LOG("\nresult: %s, expected : %s", buffer, "01/01/70 4 13:14:40");
};

/* *
 * @tc.number     SUB_KERNEL_TIME_STRFTIME_002
 * @tc.name       test strftime api base case
 * @tc.desc       [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeStrftime002, Function | MediumTest | Level3)
{
    char buffer[80];
    time_t mtime = 18880;
    size_t ftime = 0;
    struct tm *tmTime = localtime(&mtime);

    ftime = strftime(buffer, 80, "%Ex %EX %A", tmTime);
    TEST_ASSERT_GREATER_THAN_INT(0, ftime);
    TEST_ASSERT_EQUAL_STRING("01/01/70 13:14:40 Thursday", buffer);
    LOG("\nresult: %s, expected : %s", buffer, "01/01/70 13:14:40 Thursday");

    ftime = strftime(buffer, 80, "%x %X", tmTime);
    TEST_ASSERT_GREATER_THAN_INT(0, ftime);
    TEST_ASSERT_EQUAL_STRING("01/01/70 13:14:40", buffer);
    LOG("\nresult: %s, expected : %s", buffer, "01/01/70 13:14:40");

    ftime = strftime(buffer, 80, "%D %A %H:%M:%S", tmTime);
    TEST_ASSERT_GREATER_THAN_INT(0, ftime);
    TEST_ASSERT_EQUAL_STRING("01/01/70 Thursday 13:14:40", buffer);
    LOG("\nresult: %s, expected : %s", buffer, "01/01/70 Thursday 13:14:40");
};

/* *
 * @tc.number     SUB_KERNEL_TIME_STRFTIME_003
 * @tc.name       test strftime api for abnormal input
 * @tc.desc       [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimeStrftime003, Function | MediumTest | Level3)
{
    char buffer[80];
    time_t mtime = 18880;
    size_t ftime = 0;
    struct tm *tmTime = localtime(&mtime);

    ftime = strftime(buffer, 12, "%Y-%m-%d %H:%M:%S", tmTime);
    TEST_ASSERT_EQUAL_INT(0, ftime);
    LOG("\nresult: %s, expected : %s", buffer, "1970-01-01 13:14:40");

    ftime = strftime(buffer, 80, "", tmTime);
    TEST_ASSERT_EQUAL_INT(0, ftime);
    LOG("\nresult: %s, expected : %s", buffer, "1970-01-01 13:14:40");

    ftime = strftime(buffer, 19, "%Y-%m-%d %H:%M:%S", tmTime);
    TEST_ASSERT_EQUAL_INT(0, ftime);
    LOG("\nresult: %s, expected : %s", buffer, "1970-01-01 13:14:40");

    ftime = strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", tmTime);
    TEST_ASSERT_EQUAL_INT(19, ftime);
    TEST_ASSERT_EQUAL_STRING("1970-01-01 13:14:40", buffer);
    LOG("\nresult: %s, expected : %s", buffer, "1970-01-01 13:14:40");

    ftime = strftime(buffer, 80, "%F %T %Z", tmTime);
    TEST_ASSERT_EQUAL_INT(0, ftime);
    LOG("\nresult: %s, expected : %s", buffer, "1970-01-01 13:14:40");
};

/* *
 * @tc.number SUB_KERNEL_TIME_API_TIMES_0100
 * @tc.name   test times basic
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixTimeFuncTestSuite, testTimes, Function | MediumTest | Level1)
{
    const int testClockt = 100;
    const int msPerClock = 10;
    struct tms start = { 0 };
    struct tms end = { 0 };
    clock_t stTime = times(&start);
    LOG("start_clock: stTime: %ld", stTime);
    LOG("start_clock: tms_utime: %ld, tms_stime: %ld, tms_cutime:%ld, tms_cstime:%ld", start.tms_utime,
        start.tms_stime, start.tms_cutime, start.tms_cstime);

    KeepRun(testClockt * msPerClock);

    clock_t endTime = times(&end);
    LOG("end_clock: endTime: %ld", endTime);
    LOG("end_clock: tms_utime: %ld, tms_stime: %ld, tms_cutime:%ld, tms_cstime:%ld", end.tms_utime, end.tms_stime,
        end.tms_cutime, end.tms_cstime);

    LOG("Real Time: %ld, User Time %ld, System Time %ld\n", (long)(endTime - stTime),
        (long)(end.tms_utime - start.tms_utime), (long)(end.tms_stime - start.tms_stime));

    if (!CheckValueClose((end.tms_utime - start.tms_utime), testClockt, 0.02)) {
        TEST_FAIL();
    }
    if (!CheckValueClose((endTime - stTime), testClockt, 0.02)) {
        TEST_FAIL();
    }
}

RUN_TEST_SUITE(PosixTimeFuncTestSuite);

void PosixTimeFuncTest()
{
    LOG("begin PosixTimeFuncTest....");

    RUN_ONE_TESTCASE(testTimeUSleep001);
    RUN_ONE_TESTCASE(testTimeUSleep002);

    RUN_ONE_TESTCASE(testTimeGmtime001);
    RUN_ONE_TESTCASE(testTimeLocaltime001);
    RUN_ONE_TESTCASE(testTimeLocaltime002);
    RUN_ONE_TESTCASE(testTimeLocaltimer001);
    RUN_ONE_TESTCASE(testTimeLocaltimer002);
    RUN_ONE_TESTCASE(testTimeMktime001);
    RUN_ONE_TESTCASE(testTimeMktime002);
    RUN_ONE_TESTCASE(testTimeStrftime001);
    RUN_ONE_TESTCASE(testTimeStrftime002);
    RUN_ONE_TESTCASE(testTimeStrftime003);

    return;
}
