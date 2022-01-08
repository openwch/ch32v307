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
#include "limits.h"
#include "log.h"

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is CmsisTaskFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixtimer, PosixStdlibStrtoulTest);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibStrtoulTestSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixStdlibStrtoulTestTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOUL_001
 * @tc.name      : convert string by strtoul
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoulTest, testStdlibStrtoul001, Function | MediumTest | Level1)
{
    char nPtr[] = "12 0110 0XDEFE 0666 4294967295 4294967296 12.34";
    char *endPtr = NULL;
    unsigned long ret = strtoul(nPtr, &endPtr, 10);
    if (ret == 12UL) {
        LOG("[DEMO] posix stdlib test case 1:strtoul(base=10) ret:%lu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 1:strtoul(base=10) ret:%lu,%s fail.\n", ret, nPtr);
    }
    TEST_ASSERT_EQUAL_UINT32(12UL, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 0110 0XDEFE 0666 4294967295 4294967296 12.34");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOUL_002
 * @tc.name      : convert string by strtoul
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoulTest, testStdlibStrtoul002, Function | MediumTest | Level1)
{
    char nPtr[] = " 0110 0XDEFE 0666 4294967295 4294967296 12.34";
    char *endPtr = NULL;
    unsigned long ret = strtoul(nPtr, &endPtr, 2);
    if (ret == 6UL) {
        LOG("[DEMO] posix stdlib test case 2:strtoul(base=2) ret:%lu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 2:strtoul(base=2) ret:%lu,%s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_UINT32(6UL, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 0XDEFE 0666 4294967295 4294967296 12.34");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOUL_003
 * @tc.name      : convert string by strtoul
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoulTest, testStdlibStrtoul003, Function | MediumTest | Level1)
{
    char nPtr[] = " 0XDEFE 0666 4294967295 4294967296 12.34";
    char *endPtr = NULL;
    unsigned long ret = strtoul(nPtr, &endPtr, 16);
    if (ret == 0XDEFEUL) {
        LOG("[DEMO] posix stdlib test case 3:strtoul(base=16) ret:%lu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 3:strtoul(base=16) ret:%lu,%s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_UINT32(0XDEFEUL, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 0666 4294967295 4294967296 12.34");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOUL_004
 * @tc.name      : convert string by strtoul
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoulTest, testStdlibStrtoul004, Function | MediumTest | Level1)
{
    char nPtr[] = " 0666 4294967295 4294967296 12.34";
    char *endPtr = NULL;
    unsigned long ret = strtoul(nPtr, &endPtr, 8);
    if (ret == 0666UL) {
        LOG("[DEMO] posix stdlib test case 4:strtoul(base=8) ret:%lu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 4:strtoul(base=8) ret:%lu,%s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_UINT32(0666UL, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 4294967295 4294967296 12.34");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOUL_005
 * @tc.name      : convert string by strtoul
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoulTest, testStdlibStrtoul005, Function | MediumTest | Level1)
{
    char nPtr[] = " 4294967295 4294967296 12.34";
    char *endPtr = NULL;
    unsigned long ret = strtoul(nPtr, &endPtr, 0);
    if (ret == 4294967295UL) {
        LOG("[DEMO] posix stdlib test case 5:strtoul(base=0) ret:%lu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 5:strtoul(base=0) ret:%lu,%s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_UINT32(4294967295UL, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 4294967296 12.34");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOUL_006
 * @tc.name      : convert string by strtoul
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoulTest, testStdlibStrtoul006, Function | MediumTest | Level1)
{
    char nPtr[] = " 4294967296 12.34";
    char *endPtr = NULL;
    unsigned long ret = strtoul(nPtr, &endPtr, 0);
    if (ret == 0UL) {
        LOG("[DEMO] posix stdlib test case 6:strtoul(base=0) ret:%lu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 6:strtoul(base=0) ret:%lu,%s fail.\n", ret, endPtr);
    }

    TEST_ASSERT_EQUAL_UINT32(ULONG_MAX, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 12.34");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOUL_007
 * @tc.name      : convert string by strtoul
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoulTest, testStdlibStrtoul007, Function | MediumTest | Level1)
{
    char nPtr[] = " 12.34";
    char *endPtr = NULL;
    unsigned long ret = strtoul(nPtr, &endPtr, 65);
    if (ret == 67UL) {
        LOG("[DEMO] posix stdlib test case 7:strtoul(base=65) ret:%lu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 7:strtoul(base=65) ret:%lu,%s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_UINT32(67UL, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, ".34");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOUL_008
 * @tc.name      : convert string by strtoul
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoulTest, testStdlibStrtoul008, Function | MediumTest | Level1)
{
    char nPtr[] = ".34";
    char *endPtr = NULL;
    unsigned long ret = strtoul(nPtr, &endPtr, 0);
    if (ret == 0UL) {
        LOG("[DEMO] posix stdlib test case 8:strtoul(base=0) ret:%lu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 8:strtoul(base=0) ret:%lu,%s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_UINT32(0UL, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, ".34");
}

/**
 * @tc.number    : TEST_STDLIB_STRTOUL_009
 * @tc.name      : convert string by strtoul
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoulTest, testStdlibStrtoul009, Function | MediumTest | Level1)
{
    char nPtr[] = "0XDEFE 0666";
    char *endPtr = NULL;
    unsigned long ret = strtoul(nPtr, &endPtr, 0);
    if (ret == 0XDEFE) {
        LOG("[DEMO] posix stdlib test case 9:strtoul(base=0) ret:%lu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 9:strtoul(base=0) ret:%lu,%s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_UINT32(0XDEFE, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, " 0666");
}

/* *
 * @tc.number    : TEST_STDLIB_STRTOUL_010
 * @tc.name      : convert string by strtoul
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixStdlibStrtoulTest, testStdlibStrtoul010, Function | MediumTest | Level1)
{
    char nPtr[] = " 0666";
    char *endPtr = NULL;
    unsigned long ret = strtoul(nPtr, &endPtr, 0);
    if (ret == 0666) {
        LOG("[DEMO] posix stdlib test case 9:strtoul(base=0) ret:%lu,%s, endPtr:%s ok.\n", ret, nPtr, endPtr);
    } else {
        LOG("[DEMO] posix stdlib test case 9:strtoul(base=0) ret:%lu,%s fail.\n", ret, endPtr);
    }
    TEST_ASSERT_EQUAL_UINT32(0666, ret);
    TEST_ASSERT_EQUAL_STRING(endPtr, "");
}

RUN_TEST_SUITE(PosixStdlibStrtoulTest);

void PosixStdlibStrtoulFuncTest()
{
    LOG("begin PosixStdlibStrtoulFuncTest....");
    RUN_ONE_TESTCASE(testStdlibStrtoul001);
    RUN_ONE_TESTCASE(testStdlibStrtoul002);
    RUN_ONE_TESTCASE(testStdlibStrtoul003);
    RUN_ONE_TESTCASE(testStdlibStrtoul004);
    RUN_ONE_TESTCASE(testStdlibStrtoul005);
    RUN_ONE_TESTCASE(testStdlibStrtoul006);
    RUN_ONE_TESTCASE(testStdlibStrtoul007);
    RUN_ONE_TESTCASE(testStdlibStrtoul008);
    RUN_ONE_TESTCASE(testStdlibStrtoul009);
    RUN_ONE_TESTCASE(testStdlibStrtoul010);

    return;
}