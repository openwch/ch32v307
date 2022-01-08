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

#include "ohos_types.h"
#include "hctest.h"
#include "los_config.h"
#include "kernel_test.h"
#include "ctype.h"
#include "errno.h"
#include "limits.h"
#include "stdlib.h"
#include "string.h"
#include "log.h"

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixtimer, PosixStdlibStrtoullTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibStrtoullTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibStrtoullTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOULL_001
 * @tc.name      : convert string by Strtoull
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoullTest, testStdlibStrtoull001, Function | MediumTest | Level1)
{
    char nPtr[] = "12 0110 0XDEFE 0666 1.6";
    char *endPtr = NULL;
    unsigned long long ret = strtoull(nPtr, &endPtr, 10);
    if (ret == 12ULL) {
        LOG("[DEMO] posix stdlib test case 1:strtoull(base=10) ret:%llu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 1:strtoull(base=10) ret:%llu,%s fail.\n", ret, nPtr);
    }
    TEST_ASSERT_TRUE(ret == 12ULL);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 0110 0XDEFE 0666 1.6");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOULL_002
 * @tc.name      : convert string by Strtoull
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoullTest, testStdlibStrtoull002, Function | MediumTest | Level1)
{
    char nPtr[] = " 0110 0XDEFE 0666 1.6";
    char *endPtr = NULL;
    unsigned long long ret = strtoull(nPtr, &endPtr, 2);
    if (ret == 6ULL) {
        LOG("[DEMO] posix stdlib test case 2:strtoull(base=2) ret:%llu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 2:strtoull(base=2) ret:%llu,%s fail.\n", ret, nPtr);
    }
    TEST_ASSERT_TRUE(ret == 6ULL);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 0XDEFE 0666 1.6");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOULL_003
 * @tc.name      : convert string by Strtoull
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoullTest, testStdlibStrtoull003, Function | MediumTest | Level1)
{
    char nPtr[] = " 0XDEFE 0666 1.6";
    char *endPtr = NULL;
    unsigned long long ret = strtoull(nPtr, &endPtr, 16);
    if (ret == 0XDEFEULL) {
        LOG("[DEMO] posix stdlib test case 3:strtoull(base=16) ret:%llu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 3:strtoull(base=16) ret:%llu,%s fail.\n", ret, nPtr);
    }
    TEST_ASSERT_TRUE(ret == 0XDEFEULL);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 0666 1.6");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOULL_004
 * @tc.name      : convert string by Strtoull
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoullTest, testStdlibStrtoull004, Function | MediumTest | Level1)
{
    char nPtr[] = " 0666 1.6";
    char *endPtr = NULL;
    unsigned long long ret = strtoull(nPtr, &endPtr, 8);
    if (ret == 0666ULL) {
        LOG("[DEMO] posix stdlib test case 4:strtoull(base=8) ret:%llu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 4:strtoull(base=8) ret:%llu,%s fail.\n", ret, nPtr);
    }
    TEST_ASSERT_TRUE(ret == 0666ULL);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 1.6");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOULL_005
 * @tc.name      : convert string by Strtoull
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoullTest, testStdlibStrtoull005, Function | MediumTest | Level1)
{
    char nPtr[] = " 1.6";
    char *endPtr = NULL;
    unsigned long long ret = strtoull(nPtr, &endPtr, 65);
    if (ret == 1ULL) {
        LOG("[DEMO] posix stdlib test case 5:strtoull(base=65) ret:%llu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 5:strtoull(base=65) ret:%llu,%s fail.\n", ret, nPtr);
    }
    TEST_ASSERT_TRUE(ret == 1ULL);
    TEST_ASSERT_EQUAL_STRING(endPtr, ".6");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOULL_006
 * @tc.name      : convert string by Strtoull
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoullTest, testStdlibStrtoull006, Function | MediumTest | Level1)
{
    char nPtr[] = ".6";
    char *endPtr = NULL;
    unsigned long long ret = strtoull(nPtr, &endPtr, 0);
    if (ret == 0ULL) {
        LOG("[DEMO] posix stdlib test case 6:strtoull(base=0) ret:%llu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 6:strtoull(base=0) ret:%llu,%s fail.\n", ret, nPtr);
    }
    TEST_ASSERT_TRUE(ret == 0ULL);
    TEST_ASSERT_EQUAL_STRING(endPtr, ".6");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOULL_007
 * @tc.name      : convert string by Strtoull
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoullTest, testStdlibStrtoull007, Function | MediumTest | Level1)
{
    char nPtr[] = "18446744073709551615 18446744073709551616";
    char *endPtr = NULL;
    unsigned long long ret = strtoull(nPtr, &endPtr, 10);
    if (ret == 18446744073709551615ULL) {
        LOG("[DEMO] posix stdlib test case 7:strtoull(base=10) ret:%llu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 7:strtoull(base=10) ret:%llu,%s fail.\n", ret, nPtr);
    }
    TEST_ASSERT_TRUE(ret == 18446744073709551615ULL);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 18446744073709551616");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOULL_008
 * @tc.name      : convert string by Strtoull
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoullTest, testStdlibStrtoull008, Function | MediumTest | Level1)
{
    char nPtr[] = " 18446744073709551616";
    char *endPtr = NULL;
    unsigned long long ret = strtoull(nPtr, &endPtr, 10);
    if (ret == 0ULL) {
        LOG("[DEMO] posix stdlib test case 8:strtoull(base=10) ret:%llu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 8:strtoull(base=10) ret:%llu,%s fail.\n", ret, nPtr);
    }

    TEST_ASSERT_EQUAL_UINT64(ULLONG_MAX, ret);
    TEST_ASSERT_EQUAL_INT(errno, ERANGE);
    TEST_ASSERT_EQUAL_STRING(endPtr, "");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOULL_009
 * @tc.name      : convert string by Strtoull
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoullTest, testStdlibStrtoull009, Function | MediumTest | Level1)
{
    char nPtr[] = "0XDEFE 0666";
    char *endPtr = NULL;
    unsigned long long ret = strtoull(nPtr, &endPtr, 0);
    if (ret == 0XDEFEULL) {
        LOG("[DEMO] posix stdlib test case 9:strtoull(base=0) ret:%llu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 9:strtoull(base=0) ret:%llu,%s fail.\n", ret, nPtr);
    }
    TEST_ASSERT_TRUE(ret == 0XDEFEULL);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 0666");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOULL_010
 * @tc.name      : convert string by Strtoull
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoullTest, testStdlibStrtoull010, Function | MediumTest | Level1)
{
    char nPtr[] = " 0666";
    char *endPtr = NULL;
    unsigned long long ret = strtoull(nPtr, &endPtr, 0);
    if (ret == 0666ULL) {
        LOG("[DEMO] posix stdlib test case 10:strtoull(base=0) ret:%llu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 10:strtoull(base=0) ret:%llu,%s fail.\n", ret, nPtr);
    }
    TEST_ASSERT_TRUE(ret == 0666ULL);
    TEST_ASSERT_EQUAL_STRING(endPtr, "");
}

RUN_TEST_SUITE(PosixStdlibStrtoullTest);


void PosixStdlibStrtoullFuncTest()
{
    LOG("begin PosixStdlibStrtoullFuncTest....");
    RUN_ONE_TESTCASE(testStdlibStrtoull001);
    RUN_ONE_TESTCASE(testStdlibStrtoull002);
    RUN_ONE_TESTCASE(testStdlibStrtoull003);
    RUN_ONE_TESTCASE(testStdlibStrtoull004);
    RUN_ONE_TESTCASE(testStdlibStrtoull005);
    RUN_ONE_TESTCASE(testStdlibStrtoull006);
    RUN_ONE_TESTCASE(testStdlibStrtoull007);
    RUN_ONE_TESTCASE(testStdlibStrtoull008);
    RUN_ONE_TESTCASE(testStdlibStrtoull009);
    RUN_ONE_TESTCASE(testStdlibStrtoull010);

    return;
}