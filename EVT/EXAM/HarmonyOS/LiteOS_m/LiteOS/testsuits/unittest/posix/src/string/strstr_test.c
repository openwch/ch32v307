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
/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixtimer, PosixStringStrStrTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStringStrStrTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStringStrStrTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_001
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr001, Function | MediumTest | Level1)
{
    const char destS[] = "string this is string";
    const char srcS[] = "string";

    char *ret = strstr(destS, srcS);
    if (strcmp(ret, destS) == 0) {
        LOG("[DEMO] posix string test case 1:strstr(%s) %s ok.\n", srcS, destS);
    } else {
        LOG("[DEMO] posix string test case 1:strstr(%s) %s fail.\n", srcS, destS);
    }
    TEST_ASSERT_EQUAL_STRING(ret, destS);
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_002
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr002, Function | MediumTest | Level1)
{
    const char destS[] = "string this is string";
    const char srcS[] = "this is";

    char *ret = strstr(destS, srcS);
    if (strcmp(ret, "this is string") == 0) {
        LOG("[DEMO] posix string test case 2:strstr(%s) %s ok.\n", srcS, destS);
    } else {
        LOG("[DEMO] posix string test case 2:strstr(%s) %s fail.\n", srcS, destS);
    }
    TEST_ASSERT_EQUAL_STRING(ret, "this is string");
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_003
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr003, Function | MediumTest | Level1)
{
    const char dest[] = "hello world !";
    const char srcT[] = "\0hello";

    char *ret = strstr(dest, srcT);
    if (strcmp(ret, dest) == 0) {
        LOG("[DEMO] posix string test case 3:strstr(%s) %s ok.\n", srcT, dest);
    } else {
        LOG("[DEMO] posix string test case 3:strstr(%s) %s fail.\n", srcT, dest);
    }
    TEST_ASSERT_EQUAL_STRING(ret, dest);
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_004
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr004, Function | MediumTest | Level1)
{
    const char dest[] = "hello world !";
    const char src[] = "heAlo";

    char *ret = strstr(dest, src);
    if (ret == NULL) {
        LOG("[DEMO] posix string test case 4(except):strstr(%s) %s ok.\n", src, dest);
    } else {
        LOG("[DEMO] posix string test case 4(except):strstr(%s) %s fail.\n", src, dest);
    }
    TEST_ASSERT_NULL(ret);
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_005
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr005, Function | MediumTest | Level1)
{
    const char dest[] = "hello world !";
    const char src[] = "hellm";

    char *ret = strstr(dest, src);
    if (ret == NULL) {
        LOG("[DEMO] posix string test case 5(except):strstr(%s) %s ok.\n", src, dest);
    } else {
        LOG("[DEMO] posix string test case 5(except):strstr(%s) %s fail.\n", src, dest);
    }
    TEST_ASSERT_NULL(ret);
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_006
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr006, Function | MediumTest | Level1)
{
    const char destS[] = "string this is string";
    const char srcOne[] = "t"; // one byte

    char *ret = strstr(destS, srcOne);
    if (strcmp(ret, "tring this is string") == 0) {
        LOG("[DEMO] posix string test case 6:strstr(%s) %s ok.\n", srcOne, destS);
    } else {
        LOG("[DEMO] posix string test case 6:strstr(%s) %s fail.\n", srcOne, destS);
    }
    TEST_ASSERT_EQUAL_STRING(ret, "tring this is string");
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_007
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr007, Function | MediumTest | Level1)
{
    const char destS[] = "string this is string";
    const char srcOne[] = "m"; // one byte

    char *ret = strstr(destS, srcOne);
    if (ret == NULL) {
        LOG("[DEMO] posix string test case 7(except):strstr(%s) %s ok.\n", srcOne, destS);
    } else {
        LOG("[DEMO] posix string test case 7(except):strstr(%s) %s fail.\n", srcOne, destS);
    }
    TEST_ASSERT_NULL(ret);
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_008
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr008, Function | MediumTest | Level1)
{
    const char destS[] = "string this is string";
    const char srcTwo[] = "th"; // two byte

    char *ret = strstr(destS, srcTwo);
    if (strcmp(ret, "this is string") == 0) {
        LOG("[DEMO] posix string test case 8:strstr(%s) %s ok.\n", srcTwo, destS);
    } else {
        LOG("[DEMO] posix string test case 8:strstr(%s) %s fail.\n", srcTwo, destS);
    }
    TEST_ASSERT_EQUAL_STRING(ret, "this is string");
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_009
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr009, Function | MediumTest | Level1)
{
    const char destS[] = "string this is string";
    const char srcTwo2[] = "tm"; // two byte

    char *ret = strstr(destS, srcTwo2);
    if (ret == NULL) {
        LOG("[DEMO] posix string test case 9(except):strstr(%s) %s ok.\n", srcTwo2, destS);
    } else {
        LOG("[DEMO] posix string test case 9(except):strstr(%s) %s fail.\n", srcTwo2, destS);
    }
    TEST_ASSERT_NULL(ret);
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_010
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr010, Function | MediumTest | Level1)
{
    const char destS[] = "string this is string";
    const char srcThree[] = "thi"; // three byte

    char *ret = strstr(destS, srcThree);
    if (strcmp(ret, "this is string") == 0) {
        LOG("[DEMO] posix string test case 10:strstr(%s) %s ok.\n", srcThree, destS);
    } else {
        LOG("[DEMO] posix string test case 10:strstr(%s) %s fail.\n", srcThree, destS);
    }
    TEST_ASSERT_EQUAL_STRING(ret, "this is string");
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_011
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr011, Function | MediumTest | Level1)
{
    const char destS[] = "string this is string";
    const char srcThree[] = "thm"; // three byte

    char *ret = strstr(destS, srcThree);
    if (ret == NULL) {
        LOG("[DEMO] posix string test case 11(except):strstr(%s) %s ok.\n", srcThree, destS);
    } else {
        LOG("[DEMO] posix string test case 11(except):strstr(%s) %s fail.\n", srcThree, destS);
    }
    TEST_ASSERT_NULL(ret);
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_012
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr012, Function | MediumTest | Level1)
{
    const char destS[] = "string this is string";
    const char srcFour[] = "this"; // four byte

    char *ret = strstr(destS, srcFour);
    if (strcmp(ret, "this is string") == 0) {
        LOG("[DEMO] posix string test case 12:strstr(%s) %s ok.\n", srcFour, destS);
    } else {
        LOG("[DEMO] posix string test case 12:strstr(%s) %s fail.\n", srcFour, destS);
    }
    TEST_ASSERT_EQUAL_STRING(ret, "this is string");
}

/* *
 * @tc.number    : TEST_STRING_STRSTR_013
 * @tc.name      : find the first occurrence of sub-string in a string
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStringStrStrTest, testStringStrStr013, Function | MediumTest | Level1)
{
    const char destS[] = "string this is string";
    const char srcFour[] = "thim"; // four byte

    char *ret = strstr(destS, srcFour);
    if (ret == NULL) {
        LOG("[DEMO] posix string test case 13(except):strstr(%s) %s ok.\n", srcFour, destS);
    } else {
        LOG("[DEMO] posix string test case 13(except):strstr(%s) %s fail.\n", srcFour, destS);
    }
    TEST_ASSERT_NULL(ret);
}

RUN_TEST_SUITE(PosixStringStrStrTest);