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
LITE_TEST_SUIT(Posix, Posixtimer, PosixStdlibAtollTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibAtollTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibAtollTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_STDLIB_ATOLL_001
 * @tc.name      : convert string to long long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtollTest, testStdlibAtoll001, Function | MediumTest | Level1)
{
    long long value = atoll("9223372036854775807");
    if (value == 9223372036854775807LL) {
        LOG("[DEMO] posix stdlib test case 1:atoll(%lld) ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 1:atoll(%lld) fail.\n", value);
    }
    TEST_ASSERT_TRUE(value == 9223372036854775807LL);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOLL_002
 * @tc.name      : convert string to long long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtollTest, testStdlibAtoll002, Function | MediumTest | Level1)
{
    long long value = atoll("-9223372036854775808");
    if (value == -9223372036854775808LL) {
        LOG("[DEMO] posix stdlib test case 2:atoll(%lld) ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 2:atoll(%lld) fail.\n", value);
    }
    TEST_ASSERT_TRUE(value == -9223372036854775808LL);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOLL_003
 * @tc.name      : convert string to long long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtollTest, testStdlibAtoll003, Function | MediumTest | Level1)
{
    long long value = atoll("100");
    if (value == 100LL) {
        LOG("[DEMO] posix stdlib test case 3:atoll(%lld) ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 3:atoll(%lld) fail.\n", value);
    }
    TEST_ASSERT_TRUE(value == 100LL);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOLL_004
 * @tc.name      : convert string to long long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtollTest, testStdlibAtoll004, Function | MediumTest | Level1)
{
    long long value = atoll("9223372036854775808");
    if (value == -9223372036854775808LL) {
        LOG("[DEMO] posix stdlib test case 4(except):atoll(%lld) != 9223372036854775808 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 4(except):atoll(%lld) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT64(LLONG_MIN, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOLL_005
 * @tc.name      : convert string to long long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtollTest, testStdlibAtoll005, Function | MediumTest | Level1)
{
    long long value = atoll("-9223372036854775809");
    if (value == 9223372036854775807LL) {
        LOG("[DEMO] posix stdlib test case 5(except):atoll(%lld) != -9223372036854775809 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 5(except):atoll(%lld) fail.\n", value);
    }

    TEST_ASSERT_EQUAL_INT64(LLONG_MAX, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOLL_006
 * @tc.name      : convert string to long long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtollTest, testStdlibAtoll006, Function | MediumTest | Level1)
{
    long long value = atoll("+100");
    if (value == 100LL) {
        LOG("[DEMO] posix stdlib test case 6:atoll(%lld) == +100 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 6:atoll(%lld) fail.\n", value);
    }
    TEST_ASSERT_TRUE(value == 100LL);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOLL_007
 * @tc.name      : convert string to long long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtollTest, testStdlibAtoll007, Function | MediumTest | Level1)
{
    long long value = atoll("-100");
    if (value == -100LL) {
        LOG("[DEMO] posix stdlib test case 7:atoll(%lld) == -100 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 7:atoll(%lld) fail.\n", value);
    }
    TEST_ASSERT_TRUE(value == -100LL);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOLL_008
 * @tc.name      : convert string to long long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtollTest, testStdlibAtoll008, Function | MediumTest | Level1)
{
    long long value = atoll("+-100");
    if (value == 0LL) {
        LOG("[DEMO] posix stdlib test case 8(except):atoll(%lld) ==  +-100 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 8(except):atoll(%lld) fail.\n", value);
    }
    TEST_ASSERT_TRUE(value == 0LL);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOLL_009
 * @tc.name      : convert string to long long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtollTest, testStdlibAtoll009, Function | MediumTest | Level1)
{
    long long value = atoll("12+-100");
    if (value == 12LL) {
        LOG("[DEMO] posix stdlib test case 9(except):atoll(%lld) ok == 12+-100.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 9(except):atoll(%lld) fail.\n", value);
    }
    TEST_ASSERT_TRUE(value == 12LL);
}

RUN_TEST_SUITE(PosixStdlibAtollTest);

void PosixStdlibAtollFuncTest()
{
    LOG("begin PosixStdlibAtollFuncTest....");
    RUN_ONE_TESTCASE(testStdlibAtoll001);
    RUN_ONE_TESTCASE(testStdlibAtoll002);
    RUN_ONE_TESTCASE(testStdlibAtoll003);
    RUN_ONE_TESTCASE(testStdlibAtoll004);
    RUN_ONE_TESTCASE(testStdlibAtoll005);
    RUN_ONE_TESTCASE(testStdlibAtoll006);
    RUN_ONE_TESTCASE(testStdlibAtoll007);
    RUN_ONE_TESTCASE(testStdlibAtoll008);
    RUN_ONE_TESTCASE(testStdlibAtoll009);

    return;
}