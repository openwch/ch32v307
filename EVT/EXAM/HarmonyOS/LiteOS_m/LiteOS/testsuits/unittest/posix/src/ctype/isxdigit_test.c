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
LITE_TEST_SUIT(Posix, Posixtimer, PosixCTypeIsxdigitTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixCTypeIsxdigitTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixCTypeIsxdigitTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_CTYPE_ISXDIGIT_001
 * @tc.name      : Checks whether a parameter is a hexadecimal digit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsxdigitTest, testCTypeIsxdigit001, Function | MediumTest | Level1)
{
    int a = '0';
    int ret = isxdigit(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 1:isxdigit(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 1:isxdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISXDIGIT_002
 * @tc.name      : Checks whether a parameter is a hexadecimal digit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsxdigitTest, testCTypeIsxdigit002, Function | MediumTest | Level1)
{
    int a = '5';
    int ret = isxdigit(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 2:isxdigit(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 2:isxdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISXDIGIT_003
 * @tc.name      : Checks whether a parameter is a hexadecimal digit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsxdigitTest, testCTypeIsxdigit003, Function | MediumTest | Level1)
{
    int a = '9';
    int ret = isxdigit(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 3:isxdigit(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 3:isxdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISXDIGIT_004
 * @tc.name      : Checks whether a parameter is a hexadecimal digit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsxdigitTest, testCTypeIsxdigit004, Function | MediumTest | Level1)
{
    int a = 'a';
    int ret = isxdigit(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 1:isxdigit(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 1:isxdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISXDIGIT_005
 * @tc.name      : Checks whether a parameter is a hexadecimal digit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsxdigitTest, testCTypeIsxdigit005, Function | MediumTest | Level1)
{
    int a = 'f';
    int ret = isxdigit(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 1:isxdigit(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 1:isxdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISXDIGIT_006
 * @tc.name      : Checks whether a parameter is a hexadecimal digit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsxdigitTest, testCTypeIsxdigit006, Function | MediumTest | Level1)
{
    int a = 'A';
    int ret = isxdigit(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 1:isxdigit(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 1:isxdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISXDIGIT_007
 * @tc.name      : Checks whether a parameter is a hexadecimal digit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsxdigitTest, testCTypeIsxdigit007, Function | MediumTest | Level1)
{
    int a = 'F';
    int ret = isxdigit(a);
    if (ret != 0) {
        LOG("[DEMO] posix ctype test case 1:isxdigit(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 1:isxdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISXDIGIT_008
 * @tc.name      : Checks whether a parameter is a hexadecimal digit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsxdigitTest, testCTypeIsxdigit008, Function | MediumTest | Level1)
{
    int a = 'F' + 1;
    int ret = isxdigit(a);
    if (ret == 0) {
        LOG("[DEMO] posix ctype test case 5(except):isxdigit(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 5(except):isxdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISXDIGIT_009
 * @tc.name      : Checks whether a parameter is a hexadecimal digit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsxdigitTest, testCTypeIsxdigit009, Function | MediumTest | Level1)
{
    int a = '0' - 1;
    int ret = isxdigit(a);
    if (ret == 0) {
        LOG("[DEMO] posix ctype test case 4(except):isxdigit(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 4(except):isxdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number    : TEST_CTYPE_ISXDIGIT_010
 * @tc.name      : Checks whether a parameter is a hexadecimal digit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCTypeIsxdigitTest, testCTypeIsxdigit010, Function | MediumTest | Level1)
{
    int a = '9' + 1;
    int ret = isxdigit(a);
    if (ret == 0) {
        LOG("[DEMO] posix ctype test case 5(except):isxdigit(%c) ok.\n", a);
    } else {
        LOG("[DEMO] posix ctype test case 5(except):isxdigit(%c) fail.\n", a);
    }
    TEST_ASSERT_EQUAL_INT(0, ret);
}

RUN_TEST_SUITE(PosixCTypeIsxdigitTest);

void PosixIsxdigitFuncTest()
{
    LOG("begin PosixIsxdigitFuncTest....");
    RUN_ONE_TESTCASE(testCTypeIsxdigit001);
    RUN_ONE_TESTCASE(testCTypeIsxdigit002);
    RUN_ONE_TESTCASE(testCTypeIsxdigit003);
    RUN_ONE_TESTCASE(testCTypeIsxdigit004);
    RUN_ONE_TESTCASE(testCTypeIsxdigit005);
    RUN_ONE_TESTCASE(testCTypeIsxdigit006);
    RUN_ONE_TESTCASE(testCTypeIsxdigit007);
    RUN_ONE_TESTCASE(testCTypeIsxdigit008);
    RUN_ONE_TESTCASE(testCTypeIsxdigit009);
    RUN_ONE_TESTCASE(testCTypeIsxdigit010);

    return;
}