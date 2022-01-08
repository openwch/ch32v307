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

#define RET_TRUE 1
#define RET_FALSE 0

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixctype, PosixCtypeFuncTestSuite);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixCtypeFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixCtypeFuncTestSuiteTearDown(void)
{
    printf("==== [ Ctype TEST ] ====\n\n");
    return TRUE;
}

/* *
 * @tc.number TEST_CTYPE_ISALNUM_001
 * @tc.name   ctype_isalnum test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsalnum001, Function | MediumTest | Level1)
{
    int src = 'A';
    int ret = isalnum(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISALNUM_002
 * @tc.name   ctype_isalnum test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsalnum002, Function | MediumTest | Level1)
{
    int src = '1';
    int ret = isalnum(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISALNUM_003
 * @tc.name   ctype_isalnum test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsalnum003, Function | MediumTest | Level1)
{
    int src = '@';
    int ret = isalnum(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISALNUM_004
 * @tc.name   ctype_isalnum test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsalnum004, Function | MediumTest | Level1)
{
    int src = ' ';
    int ret = isalnum(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISALNUM_005
 * @tc.name   ctype_isalnum test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsalnum005, Function | MediumTest | Level1)
{
    int src = '\f'; // 0x0c 14
    int ret = isalnum(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}


/* *
 * @tc.number TEST_CTYPE_ISASCII_001
 * @tc.name   ctype_isascii test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsascii001, Function | MediumTest | Level1)
{
    const int src = -1;
    int ret = isascii(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISASCII_002
 * @tc.name   ctype_isascii test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsascii002, Function | MediumTest | Level1)
{
    const int src = 0;
    int ret = isascii(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISASCII_003
 * @tc.name   ctype_isascii test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsascii003, Function | MediumTest | Level1)
{
    const int src = 127;
    int ret = isascii(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISASCII_004
 * @tc.name   ctype_isascii test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsascii004, Function | MediumTest | Level1)
{
    const int src = 128;
    int ret = isascii(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISASCII_005
 * @tc.name   ctype_isascii test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsascii005, Function | MediumTest | Level1)
{
    int src = '\f'; // 0x0c 14
    int ret = isascii(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISPRINT_001
 * @tc.name   ctype_isprint test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsprint001, Function | MediumTest | Level1)
{
    int src = 'A';
    int ret = isprint(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISPRINT_002
 * @tc.name   ctype_isprint test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsprint002, Function | MediumTest | Level1)
{
    int src = '1';
    int ret = isprint(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISPRINT_003
 * @tc.name   ctype_isprint test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsprint003, Function | MediumTest | Level1)
{
    int src = '@';
    int ret = isprint(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISPRINT_004
 * @tc.name   ctype_isprint test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsprint004, Function | MediumTest | Level1)
{
    int src = ' ';
    int ret = isprint(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISPRINT_005
 * @tc.name   ctype_isprint test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsprint005, Function | MediumTest | Level1)
{
    int src = '\f'; // 0x0c
    int ret = isprint(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}


/* *
 * @tc.number TEST_CTYPE_ISSPACE_001
 * @tc.name   Ctype_isspace test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsspace001, Function | MediumTest | Level1)
{
    int src = 'A';
    int ret = isspace(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISSPACE_002
 * @tc.name   Ctype_isspace test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsspace002, Function | MediumTest | Level1)
{
    int src = '1';
    int ret = isspace(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISSPACE_003
 * @tc.name   Ctype_isspace test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsspace003, Function | MediumTest | Level1)
{
    int src = '@';
    int ret = isspace(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISSPACE_004
 * @tc.name   Ctype_isspace test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsspace004, Function | MediumTest | Level1)
{
    int src = ' ';
    int ret = isspace(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISSPACE_005
 * @tc.name   Ctype_isspace test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsspace005, Function | MediumTest | Level1)
{
    int src = '\t';
    int ret = isspace(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}


/* *
 * @tc.number TEST_CTYPE_ISUPPER_001
 * @tc.name   Ctype_isupper test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsupper001, Function | MediumTest | Level1)
{
    int src = 'A';
    int ret = isupper(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISUPPER_002
 * @tc.name   Ctype_isupper test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsupper002, Function | MediumTest | Level1)
{
    int src = 'a';
    int ret = isupper(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISUPPER_003
 * @tc.name   Ctype_isupper test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsupper003, Function | MediumTest | Level1)
{
    const int src = 0x45;
    int ret = isupper(src);
    TEST_ASSERT_NOT_EQUAL(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISUPPER_004
 * @tc.name   Ctype_isupper test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsupper004, Function | MediumTest | Level1)
{
    int src = ' ';
    int ret = isupper(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

/* *
 * @tc.number TEST_CTYPE_ISUPPER_005
 * @tc.name   Ctype_isupper test with not exist pid
 * @tc.desc   [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixCtypeFuncTestSuite, testCtypeIsupper005, Function | MediumTest | Level1)
{
    int src = '\t';
    int ret = isupper(src);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

RUN_TEST_SUITE(PosixCtypeFuncTestSuite);

void PosixCtypeFuncTest()
{
    LOG("begin PosixCtypeFuncTest....");
    RUN_ONE_TESTCASE(testCtypeIsalnum001);
    RUN_ONE_TESTCASE(testCtypeIsalnum002);
    RUN_ONE_TESTCASE(testCtypeIsalnum003);
    RUN_ONE_TESTCASE(testCtypeIsalnum004);
    RUN_ONE_TESTCASE(testCtypeIsalnum005);
    RUN_ONE_TESTCASE(testCtypeIsascii001);
    RUN_ONE_TESTCASE(testCtypeIsascii002);
    RUN_ONE_TESTCASE(testCtypeIsascii003);
    RUN_ONE_TESTCASE(testCtypeIsascii004);
    RUN_ONE_TESTCASE(testCtypeIsascii005);
    RUN_ONE_TESTCASE(testCtypeIsprint001);
    RUN_ONE_TESTCASE(testCtypeIsprint002);
    RUN_ONE_TESTCASE(testCtypeIsprint003);
    RUN_ONE_TESTCASE(testCtypeIsprint004);
    RUN_ONE_TESTCASE(testCtypeIsprint005);
    RUN_ONE_TESTCASE(testCtypeIsspace001);
    RUN_ONE_TESTCASE(testCtypeIsspace002);
    RUN_ONE_TESTCASE(testCtypeIsspace003);
    RUN_ONE_TESTCASE(testCtypeIsspace004);
    RUN_ONE_TESTCASE(testCtypeIsspace005);
    RUN_ONE_TESTCASE(testCtypeIsupper001);
    RUN_ONE_TESTCASE(testCtypeIsupper002);
    RUN_ONE_TESTCASE(testCtypeIsupper003);
    RUN_ONE_TESTCASE(testCtypeIsupper004);
    RUN_ONE_TESTCASE(testCtypeIsupper005);

    return;
}