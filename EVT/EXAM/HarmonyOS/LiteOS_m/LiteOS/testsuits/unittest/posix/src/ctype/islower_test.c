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

#include <ctype.h>
#include "ohos_types.h"
#include "hctest.h"
#include "los_config.h"
#include "kernel_test.h"
#include "log.h"


/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixtimer, PosixCTypeIslowerTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixCTypeIslowerTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixCTypeIslowerTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_CTYPE_ISLOWER_001
 * @tc.name      : Checks whether a parameter is a lowercase letter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIslowerTest, testCTypeIslower001, Function | MediumTest | Level1)
{
    int a = 'a';
    int ret = islower(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 1:islower(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 1:islower(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(1, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISLOWER_002
 * @tc.name      : Checks whether a parameter is a lowercase letter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIslowerTest, testCTypeIslower002, Function | MediumTest | Level1)
{
    int a = 'z';
    int ret = islower(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 2:islower(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 2:islower(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(1, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISLOWER_003
 * @tc.name      : Checks whether a parameter is a lowercase letter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIslowerTest, testCTypeIslower003, Function | MediumTest | Level1)
{
    int a = 'f';
    int ret = islower(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 3:islower(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 3:islower(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(1, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISLOWER_004
 * @tc.name      : Checks whether a parameter is a lowercase letter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIslowerTest, testCTypeIslower004, Function | MediumTest | Level1)
{
    int a = 'a' - 1;
    int ret = islower(a);
    if (ret == 0) {
        LOG("[DEMO] posix ctype test case 4(except):islower(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 4(except):islower(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISLOWER_005
 * @tc.name      : Checks whether a parameter is a lowercase letter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIslowerTest, testCTypeIslower005, Function | MediumTest | Level1)
{
    int a = 'z' + 1;
    int ret = islower(a);
    if (ret == 0) {
        LOG("[DEMO] posix ctype test case 5(except):islower(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 5(except):islower(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISLOWER_006
 * @tc.name      : Checks whether a parameter is a lowercase letter
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIslowerTest, testCTypeIslower006, Function | MediumTest | Level1)
{
    int a = 'A';
    int ret = islower(a);
    if (ret == 0) {
        LOG("[DEMO] posix ctype test case 6(except):islower(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 6(except):islower(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(0, ret);
}

RUN_TEST_SUITE(PosixCTypeIslowerTest);

void PosixIslowerFuncTest()
{
    LOG("begin PosixIslowerFuncTest....");
    RUN_ONE_TESTCASE(testCTypeIslower001);
    RUN_ONE_TESTCASE(testCTypeIslower002);
    RUN_ONE_TESTCASE(testCTypeIslower003);
    RUN_ONE_TESTCASE(testCTypeIslower004);
    RUN_ONE_TESTCASE(testCTypeIslower005);
    RUN_ONE_TESTCASE(testCTypeIslower006);

    return;
}