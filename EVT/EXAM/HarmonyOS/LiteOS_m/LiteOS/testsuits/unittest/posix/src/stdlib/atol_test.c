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
LITE_TEST_SUIT(Posix, Posixtimer, PosixStdlibAtolTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibAtolTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibAtolTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_STDLIB_ATOL_001
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtolTest, testStdlibAtol001, Function | MediumTest | Level1)
{
    const long value = atol("2147483647");
    if (value == 2147483647) {
        LOG("[DEMO] posix stdlib test case 1:atol(%ld) ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 1:atol(%ld) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT32(2147483647, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOL_002
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtolTest, testStdlibAtol002, Function | MediumTest | Level1)
{
    const long value = atol("-2147483648");
    if (value == -2147483648) {
        LOG("[DEMO] posix stdlib test case 2:atol(%ld) ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 2:atol(%ld) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT32(-2147483648, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOL_003
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtolTest, testStdlibAtol003, Function | MediumTest | Level1)
{
    const long value = atol("100");
    if (value == 100) {
        LOG("[DEMO] posix stdlib test case 3:atol(%ld) ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 3:atol(%ld) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT32(100, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOL_004
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtolTest, testStdlibAtol004, Function | MediumTest | Level1)
{
    const long value = atol("2147483648");
    if (value != 2147483648) {
        LOG("[DEMO] posix stdlib test case 4(except):atol(%ld) != 2147483648 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 4(except):atol(%ld) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT32(-2147483648, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOL_005
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtolTest, testStdlibAtol005, Function | MediumTest | Level1)
{
    const long value = atol("-2147483649");
    if (value == 2147483647) {
        LOG("[DEMO] posix stdlib test case 5(except):atoi(%d) != -2147483649 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 5(except):atoi(%d) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT32(2147483647, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOL_006
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtolTest, testStdlibAtol006, Function | MediumTest | Level1)
{
    const long value = atol("+100");
    if (value == 100) {
        LOG("[DEMO] posix stdlib test case 6:atol(%ld) == +100 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 6:atol(%ld) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT32(100, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOL_007
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtolTest, testStdlibAtol007, Function | MediumTest | Level1)
{
    const long value = atol("-100");
    if (value == -100) {
        LOG("[DEMO] posix stdlib test case 7:atol(%ld) == -100 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 7:atoi(%ld) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT32(-100, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOL_008
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtolTest, testStdlibAtol008, Function | MediumTest | Level1)
{
    long value = atol("+-100");
    if (value == 0) {
        LOG("[DEMO] posix stdlib test case 8(except):atol(%ld) ==  +-100 ok.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 8(except):atol(%ld) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT32(0, value);
}

/* *
 * @tc.number    : TEST_STDLIB_ATOL_009
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibAtolTest, testStdlibAtol009, Function | MediumTest | Level1)
{
    long value = atol("12+-100");
    if (value == 12) {
        LOG("[DEMO] posix stdlib test case 9(except):atol(%ld) ok == 12+-100.\n", value);
    } else {
        LOG("[DEMO] posix stdlib test case 9(except):atol(%ld) fail.\n", value);
    }
    TEST_ASSERT_EQUAL_INT32(12, value);
}

RUN_TEST_SUITE(PosixStdlibAtolTest);

void PosixStdlibAtolFuncTest()
{
    LOG("begin PosixStdlibAtolFuncTest....");
    RUN_ONE_TESTCASE(testStdlibAtol001);
    RUN_ONE_TESTCASE(testStdlibAtol002);
    RUN_ONE_TESTCASE(testStdlibAtol003);
    RUN_ONE_TESTCASE(testStdlibAtol004);
    RUN_ONE_TESTCASE(testStdlibAtol005);
    RUN_ONE_TESTCASE(testStdlibAtol006);
    RUN_ONE_TESTCASE(testStdlibAtol007);
    RUN_ONE_TESTCASE(testStdlibAtol008);
    RUN_ONE_TESTCASE(testStdlibAtol009);

    return;
}