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
#include <regex.h>
#include "log.h"

#define EQUAL 0

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is PosixStrCaseCmpFuncTestSuite
 */
LITE_TEST_SUIT(Posix, PosixRegexTest, PosixRegexFuncTestSuite);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixRegexFuncTestSuiteSetUp(void)
{
    LOG("+-------------------------------------------+\n");
    LOG("+--------PosixRegexFuncTestSuiteSetUp-------+\n");
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixRegexFuncTestSuiteTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    LOG("+-------PosixRegexFuncTestSuiteTearDown-----+\n");
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

void TestRegex(int flag, const char *pattern, const char *buf, const int expectedStatus, const char *expectedRes)
{
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regex_t reg;
    char res[64];
    int j = 0;
    regcomp(&reg, pattern, flag);
    int status = regexec(&reg, buf, nmatch, pmatch, 0);
    TEST_ASSERT_EQUAL_INT(status, expectedStatus);
    if (status == REG_NOMATCH) {
        LOG("no match");
    } else if (status == 0) {
        LOG("Match:");
        for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
            putchar(buf[i]);
            res[j] = buf[i];
            j++;
        }
        res[j] = 0;
        LOG("\n");
        TEST_ASSERT_EQUAL_STRING(res, expectedRes);
    }
    regfree(&reg);
}

void TestRegcomp(int flag, const char *pattern, const int expectedStatus)
{
    regex_t reg;
    int status = regcomp(&reg, pattern, flag);
    LOG("pattern : %s ,real status : %d \n", pattern, status);
    TEST_ASSERT_EQUAL_INT(status, expectedStatus);
    regfree(&reg);
}

/* *
 * @tc.number    SUB_KERNEL_SYS_REGCOMP_0100
 * @tc.name      test regcomp/regexec/regfree cflags = Extended
 * @tc.desc      [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixRegexFuncTestSuite, testRegexExtended001, Function | MediumTest | Level1)
{
    TestRegex(REG_EXTENDED, "^\\w+([-+.]\\w+)*@\\w+([-.]\\w+)*.\\w+([-.]\\w+)*$", "harmony20000925@abcdef.com", REG_OK,
        "harmony20000925@abcdef.com");

    TestRegex(REG_EXTENDED, "^addr=([^&]*)", "huawei123&sex=girl&age=18\r\naddr=bantian&hobby=movie", REG_NOMATCH,
        NULL);
}

/* *
 * @tc.number    SUB_KERNEL_SYS_REGCOMP_0200
 * @tc.name      test regcomp/regexec/regfree cflags = ICASE
 * @tc.desc      [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixRegexFuncTestSuite, testRegexIcase001, Function | MediumTest | Level1)
{
    TestRegex(REG_ICASE, "HARMONY[1-9]", "harmony20000925@abcdef.com", REG_OK, "harmony2");

    TestRegex(REG_ICASE, "HARMONY([1-9])", "harmony20000925@abcdef.com", REG_NOMATCH, NULL);
}

/* *
 * @tc.number    SUB_KERNEL_SYS_REGCOMP_0300
 * @tc.name      test regcomp/regexec/regfree cflags = NEWLINE
 * @tc.desc      [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixRegexFuncTestSuite, testRegexNewline001, Function | MediumTest | Level1)
{
    TestRegex(REG_EXTENDED | REG_NEWLINE, "^addr=([^&]*)", "huawei123&sex=girl&age=18\r\naddr=bantian&hobby=movie",
        REG_OK, "addr=bantian");

    TestRegex(REG_EXTENDED | REG_NEWLINE, "^addr=([^&]*)", "huawei123&sex=girl&age=18&addr=bantian&hobby=movie",
        REG_NOMATCH, NULL);
}

/* *
 * @tc.number    SUB_KERNEL_SYS_REGCOMP_0400
 * @tc.name      test regcomp/regexec/regfree cflags = NOSUB
 * @tc.desc      [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixRegexFuncTestSuite, testRegexNosub001, Function | MediumTest | Level1)
{
    int cflags = REG_ICASE | REG_NOSUB;
    regex_t reg;
    const char *buf = "harmony20000925@abcdef.com";
    const char *pattern2 = "HARMONY[1-9]";
    regcomp(&reg, pattern2, cflags);
    int status = regexec(&reg, buf, (size_t)0, NULL, 0);
    TEST_ASSERT_EQUAL_INT(status, 0);
    regfree(&reg);
}

/* *
 * @tc.number    SUB_KERNEL_SYS_REGCOMP_0500
 * @tc.name      test regcomp cflags = NOSUB
 * @tc.desc      [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixRegexFuncTestSuite, testRegcomp001, Function | MediumTest | Level1)
{
    TestRegcomp(REG_EXTENDED, "[[.]", REG_ECOLLATE);
    TestRegcomp(REG_EXTENDED, "[[:class:", REG_ECTYPE);
    TestRegcomp(REG_EXTENDED, "[abcdefg", REG_EBRACK);
    TestRegcomp(REG_EXTENDED, "\\x{4e00-\\x{9fa5}", REG_EBRACE);
    TestRegcomp(REG_EXTENDED, "*abcdefg", REG_BADRPT);
}

RUN_TEST_SUITE(PosixRegexFuncTestSuite);

void PosixRegexFuncTest()
{
    LOG("begin PosixRegexFuncTest....");
    RUN_ONE_TESTCASE(testRegexExtended001);
    RUN_ONE_TESTCASE(testRegexIcase001);
    RUN_ONE_TESTCASE(testRegexNewline001);
    RUN_ONE_TESTCASE(testRegexNosub001);
    RUN_ONE_TESTCASE(testRegcomp001);

    return;
}