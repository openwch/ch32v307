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
#include "stdlib.h"
#include "string.h"
#include "log.h"

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixtimer, PosixStdlibAtoiTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibAtoiTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibAtoiTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_STDLIB_ATOI_001
 * @tc.name      : convert string to integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtoiTest, testStdlibAtoi001, Function | MediumTest | Level1)
{
    int value = atoi("2147483647");
    if (value == 2147483647) {
        LOG("[DEMO] posix stdlib test case 1:atoi(%d) ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 1:atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT(2147483647, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOI_002
 * @tc.name      : convert string to integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtoiTest, testStdlibAtoi002, Function | MediumTest | Level1)
{
    int value = atoi("-2147483648");
    if (value == -2147483648) {
        LOG("[DEMO] posix stdlib test case 2:atoi(%d) ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 2:atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT(-2147483648, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOI_003
 * @tc.name      : convert string to integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtoiTest, testStdlibAtoi003, Function | MediumTest | Level1)
{
    int value = atoi("100");
    if (value == 100) {
        LOG("[DEMO] posix stdlib test case 3:atoi(%d) ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 3:atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT(100, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOI_004
 * @tc.name      : convert string to integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtoiTest, testStdlibAtoi004, Function | MediumTest | Level1)
{
    int value = atoi("2147483648");
    if (value == -2147483648) {
        LOG("[DEMO] posix stdlib test case 4(except):atoi(%d) != 2147483648 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 4(except):atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT(-2147483648, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOI_005
 * @tc.name      : convert string to integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtoiTest, testStdlibAtoi005, Function | MediumTest | Level1)
{
    int value = atoi("-2147483649");
    if (value == 2147483647) {
        LOG("[DEMO] posix stdlib test case 5(except):atoi(%d) != -2147483649 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 5(except):atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT(2147483647, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOI_006
 * @tc.name      : convert string to integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtoiTest, testStdlibAtoi006, Function | MediumTest | Level1)
{
    int value = atoi("+100");
    if (value == 100) {
        LOG("[DEMO] posix stdlib test case 6:atoi(%d) == +100 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 6:atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT(100, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOI_007
 * @tc.name      : convert string to integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtoiTest, testStdlibAtoi007, Function | MediumTest | Level1)
{
    int value = atoi("-100");
    if (value == -100) {
        LOG("[DEMO] posix stdlib test case 7:atoi(%d) == -100 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 7:atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT(-100, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOI_008
 * @tc.name      : convert string to integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtoiTest, testStdlibAtoi008, Function | MediumTest | Level1)
{
    int value = atoi("+-100");
    if (value == 0) {
        LOG("[DEMO] posix stdlib test case 8(except):atoi(%d) ==  +-100 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 8(except):atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT(0, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOI_009
 * @tc.name      : convert string to integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtoiTest, testStdlibAtoi009, Function | MediumTest | Level1)
{
    int value = atoi("12+-100");
    if (value == 12) {
        LOG("[DEMO] posix stdlib test case 9(except):atoi(%d) ok == 12+-100.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 9(except):atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT(12, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOI_010
 * @tc.name      : convert string to integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtoiTest, testStdlibAtoi010, Function | MediumTest | Level1)
{
    int value = atoi("21474836470");
    if (value == -10) {
        LOG("[DEMO] posix stdlib test case 10(except):atoi(%d) ok == 21474836470.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 10(except):atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT(-10, value);
}

RUN_TEST_SUITE(PosixStdlibAtoiTest);

void PosixStdlibAtoiFuncTest()
{
    LOG("begin PosixStdlibAtoiFuncTest....");
    RUN_ONE_TESTCASE(testStdlibAtoi001);
    RUN_ONE_TESTCASE(testStdlibAtoi002);
    RUN_ONE_TESTCASE(testStdlibAtoi003);
    RUN_ONE_TESTCASE(testStdlibAtoi004);
    RUN_ONE_TESTCASE(testStdlibAtoi005);
    RUN_ONE_TESTCASE(testStdlibAtoi006);
    RUN_ONE_TESTCASE(testStdlibAtoi007);
    RUN_ONE_TESTCASE(testStdlibAtoi008);
    RUN_ONE_TESTCASE(testStdlibAtoi009);
    RUN_ONE_TESTCASE(testStdlibAtoi010);

    return;
}