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
#include <string.h>
#include <stdarg.h>
#include "log.h"

#define RET_OK 1
#define MAX_ARG_NUM 2
#define MAX_STRING_LEN 6
/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixtimer, PosixStdargFuncTestSuite);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdargFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdargFuncTestSuiteTearDown(void)
{
    printf("==== [ Stdarg TEST ] ====\n\n");
    return TRUE;
}


int VaFunc(int argsNum, ...)
{
    va_list vaP1;
    (void)va_start(vaP1, argsNum);

    va_list vaP2;
    (void)va_copy(vaP2, vaP1);

    for (int i = 0; i < argsNum; i++) {
        if (i < 1) {
            TEST_ASSERT_EQUAL_INT(10, va_arg(vaP2, int));
        }

        if (i == 1) {
            TEST_ASSERT_EQUAL_INT(65, va_arg(vaP2, int));
        }

        if (i > 1) {
            TEST_ASSERT_EQUAL_STRING("hello world", va_arg(vaP2, char *));
        }
    }

    (void)va_end(vaP1);
    (void)va_end(vaP2);

    return RET_OK;
}

/* *
 * @tc.number STDARG_API_TEST_001
 * @tc.name   stdarg api test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdargFuncTestSuite, testStdarg001, Function | MediumTest | Level1)
{
    int ret = VaFunc(1, 10);
    TEST_ASSERT_EQUAL_INT(RET_OK, ret);
}

/* *
 * @tc.number STDARG_API_TEST_002
 * @tc.name   stdarg api test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdargFuncTestSuite, testStdarg002, Function | MediumTest | Level1)
{
    int ret = VaFunc(2, 10, 'A');
    TEST_ASSERT_EQUAL_INT(RET_OK, ret);
}

/* *
 * @tc.number STDARG_API_TEST_003
 * @tc.name   stdarg api test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdargFuncTestSuite, testStdarg003, Function | MediumTest | Level1)
{
    int ret = VaFunc(3, 10, 'A', "hello world");
    TEST_ASSERT_EQUAL_INT(RET_OK, ret);
}

/* *
 * @tc.number STDARG_API_TEST_003
 * @tc.name   stdarg api test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdargFuncTestSuite, testStdarg004, Function | MediumTest | Level1)
{
    int ret = VaFunc(3, 10, 'A', "hello world", '\0');
    TEST_ASSERT_EQUAL_INT(RET_OK, ret);
}

RUN_TEST_SUITE(PosixStdargFuncTestSuite);


void PosixStdargFuncTest()
{
    LOG("begin PosixStdargFuncTest....");
    RUN_ONE_TESTCASE(testStdarg001);
    RUN_ONE_TESTCASE(testStdarg002);
    RUN_ONE_TESTCASE(testStdarg003);
    RUN_ONE_TESTCASE(testStdarg004);

    return;
}