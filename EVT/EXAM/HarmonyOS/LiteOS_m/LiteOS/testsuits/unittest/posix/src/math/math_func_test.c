/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
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

#include <float.h>
#include <math.h>
#include "ohos_types.h"
#include "hctest.h"
#include "los_config.h"
#include "kernel_test.h"
#include "log.h"

#define RET_OK 0
#define TEST_VALUE_X 0
#define TEST_VALUE_Y 1
#define TEST_EXPECTED 2

int DoubleEquals(double a, double b)
{
    if (a == INFINITY && b == INFINITY) {
        return 1;
    }
    if (a == -INFINITY && b == -INFINITY) {
        return 1;
    }
    if (a == INFINITY && b != INFINITY) {
        return 0;
    }
    if (a == -INFINITY && b != -INFINITY) {
        return 0;
    }
    if (isnan(a) && isnan(b)) {
        return 1;
    }
    if (isnan(a) || isnan(b)) {
        return 0;
    }

    return fabs(a - b) < DBL_EPSILON;
}

/* *
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is utils
 * @param        : module name is utilsFile
 * @param        : test suit name is PosixMathFuncTestSuite
 */
LITE_TEST_SUIT(Posix, Posixmath, PosixMathFuncTestSuite);

/* *
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL PosixMathFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/* *
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL PosixMathFuncTestSuiteTearDown(void)
{
    LOG("+-------------------------------------------+\n");
    return TRUE;
}

/* *
 * @tc.number     SUB_KERNEL_MATH_ABS_001
 * @tc.name       test abs api
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathAbs001, Function | MediumTest | Level1)
{
    const int testCount = 3;
    int testValues[] = {-3, 0, 3};
    int expected[] = {3, 0, 3};
    int ret;
    for (int i = 0; i < testCount; ++i) {
        ret = abs(testValues[i]);
        LOG("\n [POSIXTEST][abs]abs(%d) = %d, expected is %d", testValues[i], ret, expected[i]);
        TEST_ASSERT_EQUAL_INT(expected[i], ret);
    }
};

/* *
 * @tc.number     SUB_KERNEL_MATH_ABS_002
 * @tc.name       test abs api for boundary value
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathAbs002, Function | MediumTest | Level1)
{
    const int testCount = 3;
    int testValues[] = {-2147483648, -2147483647, 2147483647};
    int expected[] = {-2147483648, 2147483647, 2147483647};
    int ret;
    for (int i = 0; i < testCount; ++i) {
        ret = abs(testValues[i]);
        LOG("\n [POSIXTEST][abs]abs(%d) = %d, expected is %d", testValues[i], ret, expected[i]);
        TEST_ASSERT_EQUAL_INT(expected[i], ret);
    }
};

/* *
 * @tc.number     SUB_KERNEL_MATH_LOG_001
 * @tc.name       log basic function test
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathLog001, Function | MediumTest | Level1)
{
    const int testCount = 3;
    double testValues[] = { 0.5, 5.5, 1};
    double expected[] = { -0.69314718055994528623, 1.70474809223842527217, 0.00000000000000000000};
    double ret;
    PRINT_EMG("GZHTESTLOG PRINT_EMG: %f, %f, %f", testValues[0], testValues[1], testValues[2]);
    LOG("GZHTESTLOG LOG: %f, %f, %f", testValues[0], testValues[1], testValues[2]);
    for (int i = 0; i < testCount; ++i) {
        ret = log(testValues[i]);
        LOG("\n [POSIXTEST][log]log(%f) = %f, expected is %f", testValues[i], ret, expected[i]);
        TEST_ASSERT_EQUAL_FLOAT(expected[i], ret);
        TEST_ASSERT_TRUE(DoubleEquals(expected[i], ret));
    }
};

/* *
 * @tc.number     SUB_KERNEL_MATH_LOG_002
 * @tc.name       log function test for invalid input
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathLog002, Function | MediumTest | Level1)
{
    const int testCount = 4;
    double testValues[] = { 0, -INFINITY, -2.0, NAN};
    double expected[] = { -INFINITY, NAN, NAN, NAN};
    double ret;

    LOG("\n (math_errhandling & MATH_ERRNO) = %d", (math_errhandling & MATH_ERRNO));
    LOG("\n (math_errhandling & MATH_ERREXCEPT) = %d", (math_errhandling & MATH_ERREXCEPT));

    for (int i = 0; i < testCount; ++i) {
        ret = log(testValues[i]);
        LOG("\n [POSIXTEST][log]log(%f) = %f, expected is %f", testValues[i], ret, expected[i]);
        TEST_ASSERT_EQUAL_FLOAT(expected[i], ret);
    }
};

/* *
 * @tc.number     SUB_KERNEL_MATH_SQRT_001
 * @tc.name       sqrt basic function test
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathSqrt001, Function | MediumTest | Level1)
{
    const int testCount = 3;
    double testValues[] = { 4, 3, 10 };
    double expected[] = { 2.00000000000000000000, 1.73205080756887719318, 3.16227766016837952279 };
    double ret;
    for (int i = 0; i < testCount; ++i) {
        ret = sqrt(testValues[i]);
        LOG("\n [POSIXTEST][sqrt]sqrt(%f) = %f, expected is %f", testValues[i], ret, expected[i]);
        TEST_ASSERT_EQUAL_FLOAT(expected[i], ret);
        TEST_ASSERT_TRUE(DoubleEquals(expected[i], ret));
    }
};

/* *
 * @tc.number     SUB_KERNEL_MATH_SQRT_002
 * @tc.name       sqrt function test for invalid input
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathSqrt002, Function | MediumTest | Level1)
{
    const int testCount = 5;
    double testValues[] = { 0, INFINITY, -2.0, -INFINITY, NAN };
    double expected[] = { 0.00000000000000000000, INFINITY, NAN, NAN, NAN};
    double ret;
    for (int i = 0; i < testCount; ++i) {
        ret = sqrt(testValues[i]);
        LOG("\n [POSIXTEST][sqrt]sqrt(%f) = %f, expected is %f", testValues[i], ret, expected[i]);
        TEST_ASSERT_EQUAL_FLOAT(expected[i], ret);
    }
};

/* *
 * @tc.number     SUB_KERNEL_MATH_POW_001
 * @tc.name       pow basic function test
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathPow001, Function | MediumTest | Level1)
{
    double testValues[][TEST_EXPECTED + 1] = {
        {1, 1.12, 1.00000000000000000000},
        {2.8, 2.14, 9.0556199394902243682},
        {3.6, 3.16, 57.26851244563656706532},
        {678.88, 0, 1.00000000000000000000}
    };

    const int testCount = sizeof(testValues) / (sizeof(double) * 3);
    double ret;
    for (int i = 0; i < testCount; ++i) {
        ret = pow(testValues[i][TEST_VALUE_X], testValues[i][TEST_VALUE_Y]);
        LOG("\n [POSIXTEST][pow]pow1(%f,%f) = %f, expected is %f", testValues[i][TEST_VALUE_X],
            testValues[i][TEST_VALUE_Y], ret, testValues[i][TEST_EXPECTED]);
        TEST_ASSERT_EQUAL_FLOAT(testValues[i][TEST_EXPECTED], ret);
        TEST_ASSERT_TRUE(DoubleEquals(testValues[i][TEST_EXPECTED], ret));
    }
};


/* *
 * @tc.number     SUB_KERNEL_MATH_POW_002
 * @tc.name       pow basic function test for range input
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathPow002, Function | MediumTest | Level1)
{
    double testValues[][TEST_EXPECTED + 1] = {
        {1, NAN, 1.00000000000000000000},
        {-1, -INFINITY, 1.00000000000000000000},
        {-0.5, -INFINITY, INFINITY},
        {3.5, -INFINITY, 0},
        {0.5, INFINITY, 0},
        {-3.5, INFINITY, INFINITY},
        {-INFINITY, -5, -0.00000000000000000000},
        {-INFINITY, -4.37, 0.00000000000000000000},
        {-INFINITY, 7, -INFINITY},
        {-INFINITY, 4, INFINITY},
        {INFINITY, -4, 0.00000000000000000000},
        {INFINITY, 4, INFINITY}
    };

    const int testCount = sizeof(testValues) / (sizeof(double) * 3);
    double ret;
    for (int i = 0; i < testCount; ++i) {
        ret = pow(testValues[i][TEST_VALUE_X], testValues[i][TEST_VALUE_Y]);
        TEST_ASSERT_EQUAL_FLOAT(testValues[i][TEST_EXPECTED], ret);
        TEST_ASSERT_TRUE(DoubleEquals(testValues[i][TEST_EXPECTED], ret));
        LOG("\n [POSIXTEST][pow]pow1(%f,%f) = %f, expected is %f", testValues[i][TEST_VALUE_X],
            testValues[i][TEST_VALUE_Y], ret, testValues[i][TEST_EXPECTED]);
    }
};

/* *
 * @tc.number     SUB_KERNEL_MATH_POW_003
 * @tc.name       pow basic function test for invalid input
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathPow003, Function | MediumTest | Level1)
{
    double testValues[][TEST_EXPECTED + 1] = {
        {0.0, -7, -HUGE_VAL},
        {-0.0, -6.22, -HUGE_VAL},
        {-7.23, 3.57, NAN},
        {121223, 5674343, HUGE_VAL}
    };

    const int testCount = sizeof(testValues) / (sizeof(double) * 3);
    double ret;
    for (int i = 0; i < testCount; ++i) {
        ret = pow(testValues[i][TEST_VALUE_X], testValues[i][TEST_VALUE_Y]);
        TEST_ASSERT_EQUAL_FLOAT(testValues[i][TEST_EXPECTED], ret);
        LOG("\n [POSIXTEST][pow]pow1(%f,%f) = %f, expected is %f", testValues[i][TEST_VALUE_X],
            testValues[i][TEST_VALUE_Y], ret, testValues[i][TEST_EXPECTED]);
        TEST_ASSERT_TRUE(DoubleEquals(testValues[i][TEST_EXPECTED], ret));
    }
};

/* *
 * @tc.number     SUB_KERNEL_MATH_ROUND_001
 * @tc.name       round basic function test
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathRound001, Function | MediumTest | Level1)
{
    double testValues[] = { -0.5, 2.5, -3.812345679812345, -0.125, 0.125};
    double expected[] = { -1.00000000000000000000, 3.00000000000000000000, -4.00000000000000000000,
        0.00000000000000000000, 0.00000000000000000000};
    const int testCount = sizeof(testValues) / sizeof(double);
    double ret;
    for (int i = 0; i < testCount; ++i) {
        ret = round(testValues[i]);
        LOG("\n [POSIXTEST][round]round1(%f) = %f, expected is %f", testValues[i], ret, expected[i]);
        TEST_ASSERT_EQUAL_FLOAT(expected[i], ret);
        TEST_ASSERT_TRUE(DoubleEquals(expected[i], ret));
    }
};

/* *
 * @tc.number     SUB_KERNEL_MATH_ROUND_002
 * @tc.name       round basic function test
 * @tc.desc       [C- SOFTWARE -0100]
 */
LITE_TEST_CASE(PosixMathFuncTestSuite, testMathRound002, Function | MediumTest | Level1)
{
    double testValues[] = { NAN, -INFINITY, INFINITY, 0};
    double expected[] = { NAN, -INFINITY, INFINITY, 0.00000000000000000000};
    const int testCount = sizeof(testValues) / sizeof(double);
    double ret;
    for (int i = 0; i < testCount; ++i) {
        ret = round(testValues[i]);
        LOG("\n [POSIXTEST][round]round1(%f) = %f, expected is %f", testValues[i], ret, expected[i]);
        TEST_ASSERT_EQUAL_FLOAT(expected[i], ret);
        TEST_ASSERT_TRUE(DoubleEquals(expected[i], ret));
    }
};

RUN_TEST_SUITE(PosixMathFuncTestSuite);

void PosixMathFuncTest()
{
    LOG("begin PosixMathFuncTest....");
    RUN_ONE_TESTCASE(testMathAbs001);
    RUN_ONE_TESTCASE(testMathAbs002);
    RUN_ONE_TESTCASE(testMathLog001);
    RUN_ONE_TESTCASE(testMathLog002);
    RUN_ONE_TESTCASE(testMathSqrt001);
    RUN_ONE_TESTCASE(testMathSqrt002);
    RUN_ONE_TESTCASE(testMathPow001);
    RUN_ONE_TESTCASE(testMathPow002);
    RUN_ONE_TESTCASE(testMathPow003);
    RUN_ONE_TESTCASE(testMathRound001);
    RUN_ONE_TESTCASE(testMathRound002);

    return;
}