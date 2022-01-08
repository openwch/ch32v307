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
#include "log.h"


#define EQUAL 0

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is PosixStringsFuncTestSuite
 */
LITE_TEST_SUIT(Posix, PosixStrings, PosixStringsFuncTestSuite);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStringsFuncTestSuiteSetUp(void)
{
    LOG("+-------------------------------------------+\n");
    LOG("+------PosixStringsFuncTestSuiteSetUp-------+\n");
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStringsFuncTestSuiteTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    LOG("+-----PosixStringsFuncTestSuiteTearDown-----+\n");
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number SUB_KERNEL_IO_STRINGS_0100
 * @tc.name   strcasecmp basic function test
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringsFuncTestSuite, testStrCaseCmp001, Function | MediumTest | Level1)
{
    int ret = strcasecmp("", "");
    TEST_ASSERT_EQUAL_INT(ret, EQUAL);

    ret = strcasecmp("abcdefg", "abcdefg");
    TEST_ASSERT_EQUAL_INT(ret, EQUAL);

    ret = strcasecmp("abcdefg", "abcdEFg");
    TEST_ASSERT_EQUAL_INT(ret, EQUAL);

    ret = strcasecmp("abcdefg", "abcdEF");
    TEST_ASSERT_NOT_EQUAL(ret, EQUAL);

    ret = strcasecmp("abcdef", "abcdEFg");
    TEST_ASSERT_NOT_EQUAL(ret, EQUAL);
};

RUN_TEST_SUITE(PosixStringsFuncTestSuite);

void PosixStringStrcasecmpFuncTest()
{
    LOG("begin PosixStringStrcasecmpFuncTest....");
    RUN_ONE_TESTCASE(testStrCaseCmp001);

    return;
}