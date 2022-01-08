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


/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixtimer, PosixCTypeIsdigitTest);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixCTypeIsdigitTestSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixCTypeIsdigitTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/**
 * @tc.number    : TEST_CTYPE_ISDIGIT_001
 * @tc.name      : Checks whether a parameter is a decimal digit (0-9)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsdigitTest, testCTypeIsdigit001, Function | MediumTest | Level1)
{
    int a = '0';
    int ret = isdigit(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 1:isdigit(%c) ok.\n", a);
    }
    else {
        LOG("[DEMO] posix ctype test case 1:isdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(1, ret);
}

/**
 * @tc.number    : TEST_CTYPE_ISDIGIT_002
 * @tc.name      : Checks whether a parameter is a decimal digit (0-9)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsdigitTest, testCTypeIsdigit002, Function | MediumTest | Level1)
{
    int a = '5';
    int ret = isdigit(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 2:isdigit(%c) ok.\n", a);
    }
    else {
        LOG("[DEMO] posix ctype test case 2:isdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(1, ret);
}

/**
 * @tc.number    : TEST_CTYPE_ISDIGIT_003
 * @tc.name      : Checks whether a parameter is a decimal digit (0-9)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsdigitTest, testCTypeIsdigit003, Function | MediumTest | Level1)
{
    int a = '9';
    int ret = isdigit(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 3:isdigit(%c) ok.\n", a);
    }
    else {
        LOG("[DEMO] posix ctype test case 3:isdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(1, ret);
}

/**
 * @tc.number    : TEST_CTYPE_ISDIGIT_004
 * @tc.name      : Checks whether a parameter is a decimal digit (0-9)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsdigitTest, testCTypeIsdigit004, Function | MediumTest | Level1)
{
    int a = '0' - 1;
    int ret = isdigit(a);
    if (ret == 0) {
        LOG("[DEMO] posix ctype test case 4(except):isdigit(%c) ok.\n", a);
    }
    else {
        LOG("[DEMO] posix ctype test case 4(except):isdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/**
 * @tc.number    : TEST_CTYPE_ISDIGIT_005
 * @tc.name      : Checks whether a parameter is a decimal digit (0-9)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsdigitTest, testCTypeIsdigit005, Function | MediumTest | Level1)
{
    int a = '9' + 1;
    int ret = isdigit(a);
    if (ret == 0) {
        LOG("[DEMO] posix ctype test case 5(except):isdigit(%c) ok.\n", a);
    }
    else {
        LOG("[DEMO] posix ctype test case 5(except):isdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(0, ret);
}

RUN_TEST_SUITE(PosixCTypeIsdigitTest);


void PosixIsdigitFuncTest()
{
    LOG("begin PosixIsdigitFuncTest....");
    RUN_ONE_TESTCASE(testCTypeIsdigit001);
    RUN_ONE_TESTCASE(testCTypeIsdigit002);
    RUN_ONE_TESTCASE(testCTypeIsdigit003);
    RUN_ONE_TESTCASE(testCTypeIsdigit004);
    RUN_ONE_TESTCASE(testCTypeIsdigit005);

    return;
}

