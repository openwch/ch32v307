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

#include <stdlib.h>
#include <errno.h>
#include "ohos_types.h"
#include "hctest.h"
#include "los_config.h"
#include "kernel_test.h"
#include "log.h"

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is PosixSysFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixsystem, PosixSysFuncTestSuite);


/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixSysFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixSysFuncTestSuiteTearDown(void)
{
    printf("+Hello this is a System function test+\n");
    return TRUE;
}

/* *
 * @tc.number    : SUB_KERNEL_POSIX_strerror_OPERATION_001
 * @tc.name      : Memony operation for strerror test
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixSysFuncTestSuite, testOsSysStrerror001, Function | MediumTest | Level1)
{
    for (int i = EPERM; i < EHWPOISON; i++) {
        char *s = strerror(i);
        TEST_ASSERT_NOT_NULL(s);
    }

    LOG("strerror(-1) = %s\n", strerror(-1));
    TEST_ASSERT_EQUAL_STRING("No error information", strerror(-1));
    LOG("strerror(0) = %s\n", strerror(0));
    TEST_ASSERT_EQUAL_STRING("No error information", strerror(0));
    LOG("strerror(2) = %s\n", strerror(2));
    TEST_ASSERT_EQUAL_STRING("No such file or directory", strerror(2));
    LOG("strerror(10) = %s\n", strerror(10));
    TEST_ASSERT_EQUAL_STRING("No child process", strerror(10));
};


RUN_TEST_SUITE(PosixSysFuncTestSuite);

void PosixStrerrorTest()
{
    LOG("begin PosixStrerrorTest....");
    RUN_ONE_TESTCASE(testOsSysStrerror001);

    return;
}