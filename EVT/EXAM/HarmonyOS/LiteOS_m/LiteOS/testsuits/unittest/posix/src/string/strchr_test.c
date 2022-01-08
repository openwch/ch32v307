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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixtimer, PosixStringStrchrTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStringStrchrTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStringStrchrTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_STRING_STRCHR_001
 * @tc.name      : find the first occurrence of a character in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrchrTest, testStringStrchr001, Function | MediumTest | Level1)
{
    char src[] = "hello !! world";
    char *ret = strchr(src, '!');
    if (strcmp(ret, "!! world") == 0) {
        LOG("[DEMO] posix string test case 1:strchr(!) %s ok.\n", src);
    } else {
        LOG("[DEMO] posix string test case 1:strchr(!) %s fail.\n", src);
    }
    TEST_ASSERT_EQUAL_STRING(ret, "!! world");
}

/* *
 * @tc.number    : TEST_STRING_STRCHR_002
 * @tc.name      : find the first occurrence of a character in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrchrTest, testStringStrchr002, Function | MediumTest | Level1)
{
    char src[] = "hello !! world";
    char *ret = strchr(src, 'l');
    if (strcmp(ret, "llo !! world") == 0) {
        LOG("[DEMO] posix string test case 2:strchr(l) %s ok.\n", src);
    } else {
        LOG("[DEMO] posix string test case 2:strchr(l) %s fail.\n", src);
    }
    TEST_ASSERT_EQUAL_STRING(ret, "llo !! world");
}

/* *
 * @tc.number    : TEST_STRING_STRCHR_003
 * @tc.name      : find the first occurrence of a character in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrchrTest, testStringStrchr003, Function | MediumTest | Level1)
{
    char src[] = "hello !! world";
    char *ret = strchr(src, '\0');
    if (ret != NULL) {
        LOG("[DEMO] posix string test case 3:strchr(\'\\0\') %s ok.\n", src);
    } else {
        LOG("[DEMO] posix string test case 3:strchr(\'\\0\') %s fail.\n", src);
    }
    TEST_ASSERT_NOT_NULL(ret);
}

/* *
 * @tc.number    : TEST_STRING_STRCHR_004
 * @tc.name      : find the first occurrence of a character in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrchrTest, testStringStrchr004, Function | MediumTest | Level1)
{
    char src[] = "hello !! world";
    char *ret = strchr(src, '?');
    if (ret == NULL) {
        LOG("[DEMO] posix string test case 4(except):strchr(?) %s ok.\n", src);
    } else {
        LOG("[DEMO] posix string test case 4(except):strchr(?) %s fail.\n", src);
    }
    TEST_ASSERT_NULL(ret);
}

/* *
 * @tc.number    : TEST_STRING_STRCHR_005
 * @tc.name      : find the first occurrence of a character in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrchrTest, testStringStrchr005, Function | MediumTest | Level1)
{
    char src[] = "hello !! world";
    char *ret = strchr(src, 'm');
    if (ret == NULL) {
        LOG("[DEMO] posix string test case 5(except):strchr(m) %s ok.\n", src);
    } else {
        LOG("[DEMO] posix string test case 5(except):strchr(m) %s fail.\n", src);
    }
    TEST_ASSERT_NULL(ret);
}

/* *
 * @tc.number    : TEST_STRING_STRCHR_005
 * @tc.name      : find the first occurrence of a character in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrchrTest, testStringStrchr006, Function | MediumTest | Level1)
{
    char src[] = "hello !! world";
    char *ret = strchr(src, 0);
    if (ret != NULL) {
        LOG("[DEMO] posix string test case 6(except):strchr(0) %s ok.\n", src);
    } else {
        LOG("[DEMO] posix string test case 6(except):strchr(0) %s fail.\n", src);
    }
    TEST_ASSERT_NOT_NULL(ret);
}

RUN_TEST_SUITE(PosixStringStrchrTest);

void PosixStringStrchrTest()
{
    LOG("begin PosixStringStrchrTest....");
    RUN_ONE_TESTCASE(testStringStrchr001);
    RUN_ONE_TESTCASE(testStringStrchr002);
    RUN_ONE_TESTCASE(testStringStrchr003);
    RUN_ONE_TESTCASE(testStringStrchr004);
    RUN_ONE_TESTCASE(testStringStrchr005);
    RUN_ONE_TESTCASE(testStringStrchr006);

    return;
}