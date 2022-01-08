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

#include "hctest.h"
#include "los_config.h"
#include "kernel_test.h"
#include "log.h"

#define RET_OK 0

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixtimer, PosixStringFuncTestSuite);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStringFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStringFuncTestSuiteTearDown(void)
{
    printf("==== [ String TEST ] ====\n\n");
    return TRUE;
}

/* *
 * @tc.number TEST_STRING_STRLEN_001
 * @tc.name   string_strlen error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrlen001, Function | MediumTest | Level1)
{
    char src[] = "helloworld";
    int ret = strlen(src);
    TEST_ASSERT_EQUAL_INT(ret, 10);
}

/* *
 * @tc.number TEST_STRING_STRLEN_002
 * @tc.name   string_strlen error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrlen002, Function | MediumTest | Level1)
{
    char src[] = "hello world";
    int ret = strlen(src);
    TEST_ASSERT_EQUAL_INT(ret, 11);
}

/* *
 * @tc.number TEST_STRING_STRLEN_003
 * @tc.name   string_strlen error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrlen003, Function | MediumTest | Level1)
{
    int ret = strlen("");
    TEST_ASSERT_EQUAL_INT(ret, 0);
}

/* *
 * @tc.number TEST_STRING_STRLEN_004
 * @tc.name   string_strlen error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrlen004, Function | MediumTest | Level1)
{
    char src[] = "hello\0world";
    int ret = strlen(src);
    TEST_ASSERT_EQUAL_INT(ret, 5);
}

/* *
 * @tc.number TEST_STRING_STRLEN_005
 * @tc.name   string_strlen error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrlen005, Function | MediumTest | Level1)
{
    char src[] = "\0helloworld";
    int ret = strlen(src);
    TEST_ASSERT_EQUAL_INT(ret, 0);
}


/* *
 * @tc.number TEST_STRING_STRNCASECMP_001
 * @tc.name   string_strncasecmp error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrncasecmp001, Function | MediumTest | Level1)
{
    char *src[] = {"helloworld", "HElloworld"};
    int ret = strncasecmp(src[0], src[1], 2);
    TEST_ASSERT_EQUAL_INT(RET_OK, ret);
}

/* *
 * @tc.number TEST_STRING_STRNCASECMP_002
 * @tc.name   string_strncasecmp error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrncasecmp002, Function | MediumTest | Level1)
{
    char *src[] = {"helloworld", "he\0lloworld"};
    int ret = strncasecmp(src[0], src[1], 3);
    TEST_ASSERT_GREATER_THAN(RET_OK, ret);
}

/* *
 * @tc.number TEST_STRING_STRNCASECMP_003
 * @tc.name   string_strncasecmp error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrncasecmp003, Function | MediumTest | Level1)
{
    char *src[] = {"helloworld", "he lloworld"};
    int ret = strncasecmp(src[0], src[1], 3);
    TEST_ASSERT_GREATER_THAN(RET_OK, ret);
}

/* *
 * @tc.number TEST_STRING_STRNCASECMP_004
 * @tc.name   string_strncasecmp error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrncasecmp004, Function | MediumTest | Level1)
{
    char *src[] = {"helloworld", "hello World"};
    int ret = strncasecmp(src[0], src[1], 3);
    TEST_ASSERT_EQUAL_INT(RET_OK, ret);
}

/* *
 * @tc.number TEST_STRING_STRNCASECMP_005
 * @tc.name   string_strncasecmp error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrncasecmp005, Function | MediumTest | Level1)
{
    char *src[] = {"helloworld", "\0"};
    int ret = strncasecmp(src[0], src[1], 1);
    TEST_ASSERT_GREATER_THAN(RET_OK, ret);
}

/* *
 * @tc.number TEST_STRING_STRNCMP_001
 * @tc.name   string_strncmp error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrncmp001, Function | MediumTest | Level1)
{
    char *src[] = {"helloworld", "HELloworld"};
    int ret = strncmp(src[0], src[1], 3);
    TEST_ASSERT_GREATER_THAN(RET_OK, ret);
}

/* *
 * @tc.number TEST_STRING_STRNCMP_002
 * @tc.name   string_strncmp error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrncmp002, Function | MediumTest | Level1)
{
    char *src[] = {"helloworld", "he\0lloworld"};
    int ret = strncmp(src[0], src[1], 3);
    TEST_ASSERT_GREATER_THAN(RET_OK, ret);
}

/* *
 * @tc.number TEST_STRING_STRNCMP_003
 * @tc.name   string_strncmp error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrncmp003, Function | MediumTest | Level1)
{
    char *src[] = {"helloworld", "he lloworld"};
    int ret = strncmp(src[0], src[1], 3);
    TEST_ASSERT_GREATER_THAN(RET_OK, ret);
}

/* *
 * @tc.number TEST_STRING_STRNCMP_004
 * @tc.name   string_strncmp error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrncmp004, Function | MediumTest | Level1)
{
    char *src[] = {"helloworld", "hello World"};
    int ret = strncmp(src[0], src[1], 3);
    TEST_ASSERT_EQUAL_INT(RET_OK, ret);
}

/* *
 * @tc.number TEST_STRING_STRNCMP_005
 * @tc.name   string_strncmp error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrncmp005, Function | MediumTest | Level1)
{
    char *src[] = {"helloworld", "\0"};
    int ret = strncmp(src[0], src[1], 3);
    TEST_ASSERT_GREATER_THAN(RET_OK, ret);
}


/* *
 * @tc.number TEST_STRING_STRRCHR_001
 * @tc.name   string_strrchr error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrrchr001, Function | MediumTest | Level1)
{
    char src[] = "hello world";
    char *ret = strrchr(src, '!');
    TEST_ASSERT_EQUAL_PTR(ret, NULL);
    TEST_ASSERT_NULL(ret);
}

/* *
 * @tc.number TEST_STRING_STRRCHR_002
 * @tc.name   string_strrchr error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrrchr002, Function | MediumTest | Level1)
{
    char src[] = "hello world";
    char *ret = strrchr(src, '\0');
    TEST_ASSERT_EQUAL_PTR(ret, src + 11);
    TEST_ASSERT_NOT_NULL(ret);
}

/* *
 * @tc.number TEST_STRING_STRRCHR_003
 * @tc.name   string_strrchr error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrrchr003, Function | MediumTest | Level1)
{
    char src[] = "hello\0world";
    char *ret = strrchr(src, '\0');
    TEST_ASSERT_EQUAL_PTR(ret, src + 5);
    TEST_ASSERT_NOT_NULL(ret);
}

/* *
 * @tc.number TEST_STRING_STRRCHR_004
 * @tc.name   string_strrchr error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrrchr004, Function | MediumTest | Level1)
{
    char src[] = "hello world";
    char *ret = strrchr(src, ' ');
    TEST_ASSERT_EQUAL_PTR(ret, src + 5);
    TEST_ASSERT_NOT_NULL(ret);
}

/* *
 * @tc.number TEST_STRING_STRRCHR_005
 * @tc.name   string_strrchr error test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringFuncTestSuite, testStringStrrchr005, Function | MediumTest | Level1)
{
    char src[] = "hello\0world";
    char *ret = strrchr(src, ' ');
    TEST_ASSERT_EQUAL_PTR(ret, NULL);
    TEST_ASSERT_NULL(ret);
}

RUN_TEST_SUITE(PosixStringFuncTestSuite);

void PosixStringFuncTest02()
{
    LOG("begin PosixStringFuncTest02....");
    RUN_ONE_TESTCASE(testStringStrlen001);
    RUN_ONE_TESTCASE(testStringStrlen002);
    RUN_ONE_TESTCASE(testStringStrlen003);
    RUN_ONE_TESTCASE(testStringStrlen004);
    RUN_ONE_TESTCASE(testStringStrlen005);
    RUN_ONE_TESTCASE(testStringStrncasecmp001);
    RUN_ONE_TESTCASE(testStringStrncasecmp002);
    RUN_ONE_TESTCASE(testStringStrncasecmp003);
    RUN_ONE_TESTCASE(testStringStrncasecmp004);
    RUN_ONE_TESTCASE(testStringStrncasecmp005);
    RUN_ONE_TESTCASE(testStringStrncmp001);
    RUN_ONE_TESTCASE(testStringStrncmp002);
    RUN_ONE_TESTCASE(testStringStrncmp003);
    RUN_ONE_TESTCASE(testStringStrncmp004);
    RUN_ONE_TESTCASE(testStringStrncmp005);
    RUN_ONE_TESTCASE(testStringStrrchr001);
    RUN_ONE_TESTCASE(testStringStrrchr002);
    RUN_ONE_TESTCASE(testStringStrrchr003);
    RUN_ONE_TESTCASE(testStringStrrchr004);
    RUN_ONE_TESTCASE(testStringStrrchr005);

    return;
}