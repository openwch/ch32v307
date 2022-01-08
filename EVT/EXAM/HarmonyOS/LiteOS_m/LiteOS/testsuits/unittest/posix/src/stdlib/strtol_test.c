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
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
#include "log.h"

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixtimer, PosixStdlibStrtolTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibStrtolTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibStrtolTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_001
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol001, Function | MediumTest | Level1)
{
    char nPtr16[] = " 10";
    char *endPtr16 = NULL;
    long ret = strtol(nPtr16, &endPtr16, 16);
    if (ret == 16) {
        LOG("[DEMO] posix stdlib test case 1:strtol(base=16) ret:%ld,%s, endPtr:%s ok.\n", ret, nPtr16, endPtr16);
    } else {
        LOG("[DEMO] posix stdlib test case 1:strtol(base=16) ret:%ld,%s fail.\n", ret, nPtr16);
    }
    TEST_ASSERT_EQUAL_INT32(16, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr16, "");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_002
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol002, Function | MediumTest | Level1)
{
    char nPtr16[] = "0x10";
    char *endPtr16 = NULL;
    long ret = strtol(nPtr16, &endPtr16, 0);
    if (ret == 16) {
        LOG("[DEMO] posix stdlib test case 2:strtol(base=16) ret:%ld,%s, endPtr:%s ok.\n", ret, nPtr16, endPtr16);
    } else {
        LOG("[DEMO] posix stdlib test case 2:strtol(base=16) ret:%ld,%s fail.\n", ret, nPtr16);
    }
    TEST_ASSERT_EQUAL_INT32(16, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr16, "");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_003
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol003, Function | MediumTest | Level1)
{
    char nPtr10[] = "10";
    char *endPtr10 = NULL;
    long ret = strtol(nPtr10, &endPtr10, 10);
    if (ret == 10) {
        LOG("[DEMO] posix stdlib test case 3:strtol(base=10) ret:%ld,%s, endPtr:%s ok.\n", ret, nPtr10, endPtr10);
    } else {
        LOG("[DEMO] posix stdlib test case 3:strtol(base=10) ret:%ld,%s fail.\n", ret, nPtr10);
    }
    TEST_ASSERT_EQUAL_INT32(10, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr10, "");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_004
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol004, Function | MediumTest | Level1)
{
    char nPtr10[] = "-10";
    char *endPtr10 = NULL;
    long ret = strtol(nPtr10, &endPtr10, 10);
    if (ret == -10) {
        LOG("[DEMO] posix stdlib test case 4:strtol(base=10) ret:%ld,%s, endPtr:%s ok.\n", ret, nPtr10, endPtr10);
    } else {
        LOG("[DEMO] posix stdlib test case 4:strtol(base=10) ret:%ld,%s fail.\n", ret, nPtr10);
    }
    TEST_ASSERT_EQUAL_INT32(-10, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr10, "");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_005
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol005, Function | MediumTest | Level1)
{
    char nPtr10_3[] = "10";
    char *endPtr10_3 = NULL;
    long ret = strtol(nPtr10_3, &endPtr10_3, 0);
    if (ret == 10) {
        LOG("[DEMO] posix stdlib test case 5:strtol(base=0) ret:%ld,%s, endPtr:%s ok.\n", ret, nPtr10_3, endPtr10_3);
    } else {
        LOG("[DEMO] posix stdlib test case 5:strtol(base=0) ret:%ld,%s fail.\n", ret, nPtr10_3);
    }
    TEST_ASSERT_EQUAL_INT32(10, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr10_3, "");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_006
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol006, Function | MediumTest | Level1)
{
    char nPtr8[] = "10";
    char *endPtr8 = NULL;
    long ret = strtol(nPtr8, &endPtr8, 8);
    if (ret == 8) {
        LOG("[DEMO] posix stdlib test case 6:strtol(base=8) ret:%ld,%s, endPtr:%s ok.\n", ret, nPtr8, endPtr8);
    } else {
        LOG("[DEMO] posix stdlib test case 6:strtol(base=8) ret:%ld,%s fail.\n", ret, nPtr8);
    }
    TEST_ASSERT_EQUAL_INT32(8, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr8, "");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_007
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol007, Function | MediumTest | Level1)
{
    char nPtr8_2[] = "010";
    char *endPtr8_2 = NULL;
    long ret = strtol(nPtr8_2, &endPtr8_2, 8);
    if (ret == 8) {
        LOG("[DEMO] posix stdlib test case 7:strtol(base=8) ret:%ld,%s, endPtr:%s ok.\n", ret, nPtr8_2, endPtr8_2);
    } else {
        LOG("[DEMO] posix stdlib test case 7:strtol(base=8) ret:%ld,%s fail.\n", ret, nPtr8_2);
    }
    TEST_ASSERT_EQUAL_INT32(8, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr8_2, "");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_008
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol008, Function | MediumTest | Level1)
{
    char nPtr8_3[] = "010";
    char *endPtr8_3 = NULL;
    long ret = strtol(nPtr8_3, &endPtr8_3, 0);
    if (ret == 8) {
        LOG("[DEMO] posix stdlib test case 8:strtol(base=8) ret:%ld,%s, endPtr:%s ok.\n", ret, nPtr8_3, endPtr8_3);
    } else {
        LOG("[DEMO] posix stdlib test case 8:strtol(base=8) ret:%ld,%s fail.\n", ret, nPtr8_3);
    }
    TEST_ASSERT_EQUAL_INT32(8, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr8_3, "");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_009
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol009, Function | MediumTest | Level1)
{
    char nPtr2[] = "10";
    char *endPtr2 = NULL;
    long ret = strtol(nPtr2, &endPtr2, 2);
    if (ret == 2) {
        LOG("[DEMO] posix stdlib test case 9:strtol(base=2) ret:%ld,%s, endPtr:%s ok.\n", ret, nPtr2, endPtr2);
    } else {
        LOG("[DEMO] posix stdlib test case 9:strtol(base=2) ret:%ld,%s fail.\n", ret, nPtr2);
    }
    TEST_ASSERT_EQUAL_INT32(2, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr2, "");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_010
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol010, Function | MediumTest | Level1)
{
    char nPtr[] = "12 0110 0XDEFE 0666 -1.6";
    char *endPtr = NULL;
    long ret = strtol(nPtr, &endPtr, 10);
    if (ret == 12) {
        LOG("[DEMO] posix stdlib test case 10:strtol(base=10) ret:%ld, %s, endPtr:%s ok.\n", ret, endPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 10:strtol(base=10) ret:%ld, %s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_INT32(12, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 0110 0XDEFE 0666 -1.6");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOL_011
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol011, Function | MediumTest | Level1)
{
    char nPtr[] = "12 1.5";
    char *endPtr = NULL;
    long ret = strtol(nPtr, &endPtr, 65);
    if (ret == 67) {
        LOG("[DEMO] posix stdlib test case 11:strtol(base=65) ret:%ld, %s, endPtr:%s ok.\n", ret, endPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 11:strtol(base=65) ret:%ld, %s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_INT32(67, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 1.5");
}

LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol012, Function | MediumTest | Level1)
{
    char nPtr[] = "2147483647 -2147483648";
    char *endPtr = NULL;
    long ret = strtol(nPtr, &endPtr, 10);
    if (ret == 2147483647) {
        LOG("[DEMO] posix stdlib test case 12:strtol(base=10) ret:%ld, %s, endPtr:%s ok.\n", ret, endPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 12:strtol(base=10) ret:%ld, %s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_INT32(2147483647, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, " -2147483648");
}

/* *
 * @tc.number    : TEST_STDLIB_ATOL_002
 * @tc.name      : convert string to long integer
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtolTest, testStdlibStrtol013, Function | MediumTest | Level1)
{
    char nPtr[] = " -2147483648";
    char *endPtr = NULL;
    long ret = strtol(nPtr, &endPtr, 10);
    if (ret == -2147483648) {
        LOG("[DEMO] posix stdlib test case 13:strtol(base=10) ret:%ld, %s, endPtr:%s ok.\n", ret, endPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 13:strtol(base=10) ret:%ld, %s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_INT32(-2147483648, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, "");
}

RUN_TEST_SUITE(PosixStdlibStrtolTest);

void PosixStdlibStrtolFuncTest()
{
    LOG("begin PosixStdlibStrtolFuncTest....");
    RUN_ONE_TESTCASE(testStdlibStrtol001);
    RUN_ONE_TESTCASE(testStdlibStrtol002);
    RUN_ONE_TESTCASE(testStdlibStrtol003);
    RUN_ONE_TESTCASE(testStdlibStrtol004);
    RUN_ONE_TESTCASE(testStdlibStrtol005);
    RUN_ONE_TESTCASE(testStdlibStrtol006);
    RUN_ONE_TESTCASE(testStdlibStrtol007);
    RUN_ONE_TESTCASE(testStdlibStrtol008);
    RUN_ONE_TESTCASE(testStdlibStrtol009);
    RUN_ONE_TESTCASE(testStdlibStrtol010);
    RUN_ONE_TESTCASE(testStdlibStrtol011);
    RUN_ONE_TESTCASE(testStdlibStrtol012);
    RUN_ONE_TESTCASE(testStdlibStrtol013);

    return;
}