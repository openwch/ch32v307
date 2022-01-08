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
LITE_TEST_SUIT(Posix, Posixtimer, PosixCTypeToupperTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixCTypeToupperTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixCTypeToupperTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_CTYPE_TOUPPER_001
 * @tc.name      : Converts a lowercase letter to uppercase equivalent
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeToupperTest, testCTypeToupper001, Function | MediumTest | Level1)
{
    int a = 'a';
    int ret = toupper(a);
    if (ret == 'A') {
        LOG("[DEMO] posix ctype test case 1:toupper(%c)==%c ok.\n", a, ret);
    } else {
        LOG("[DEMO] posix ctype test case 1:toupper(%c)!=%c  fail.\n", a);
    }
    TEST_ASSERT_TRUE(ret == 'A');
}

/* *
 * @tc.number    : TEST_CTYPE_TOUPPER_002
 * @tc.name      : Converts a lowercase letter to uppercase equivalent
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeToupperTest, testCTypeToupper002, Function | MediumTest | Level1)
{
    int a = 'A';
    int ret = toupper(a);
    if (ret == 'A') {
        LOG("[DEMO] posix ctype test case 2:toupper(%c)==%c ok.\n", a, ret);
    } else {
        LOG("[DEMO] posix ctype test case 2:toupper(%c)!=%c  fail.\n", a);
    }
    TEST_ASSERT_TRUE(ret == 'A');
}

/* *
 * @tc.number    : TEST_CTYPE_TOUPPER_003
 * @tc.name      : Converts a lowercase letter to uppercase equivalent
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeToupperTest, testCTypeToupper003, Function | MediumTest | Level1)
{
    int a = 'z';
    int ret = toupper(a);
    if (ret == 'Z') {
        LOG("[DEMO] posix ctype test case 3:toupper(%c)==%c ok.\n", a, ret);
    } else {
        LOG("[DEMO] posix ctype test case 3:toupper(%c)!=%c  fail.\n", a);
    }
    TEST_ASSERT_TRUE(ret == 'Z');
}

/* *
 * @tc.number    : TEST_CTYPE_TOUPPER_004
 * @tc.name      : Converts a lowercase letter to uppercase equivalent
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeToupperTest, testCTypeToupper004, Function | MediumTest | Level1)
{
    int a = 'Z';
    int ret = toupper(a);
    if (ret == 'Z') {
        LOG("[DEMO] posix ctype test case 4:toupper(%c)==%c ok.\n", a, ret);
    } else {
        LOG("[DEMO] posix ctype test case 4:toupper(%c)!=%c  fail.\n", a);
    }
    TEST_ASSERT_TRUE(ret == 'Z');
}

/* *
 * @tc.number    : TEST_CTYPE_TOUPPER_005
 * @tc.name      : Converts a lowercase letter to uppercase equivalent
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeToupperTest, testCTypeToupper005, Function | MediumTest | Level1)
{
    int a = '1';
    int ret = toupper(a);
    if (ret == '1') {
        LOG("[DEMO] posix ctype test case 5(except):toupper(%c)==%c ok.\n", a, ret);
    } else {
        LOG("[DEMO] posix ctype test case 5(except):toupper(%c)!=%c  fail.\n", a);
    }
    TEST_ASSERT_TRUE(ret == '1');
}

RUN_TEST_SUITE(PosixCTypeToupperTest);

void PosixToupperFuncTest()
{
    LOG("begin PosixToupperFuncTest....");
    RUN_ONE_TESTCASE(testCTypeToupper001);
    RUN_ONE_TESTCASE(testCTypeToupper002);
    RUN_ONE_TESTCASE(testCTypeToupper003);
    RUN_ONE_TESTCASE(testCTypeToupper004);
    RUN_ONE_TESTCASE(testCTypeToupper005);

    return;
}