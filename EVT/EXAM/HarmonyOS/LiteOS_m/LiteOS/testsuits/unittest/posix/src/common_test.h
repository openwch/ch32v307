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

#ifndef KERNEL_TEST_H
#define KERNEL_TEST_H

#define TESTCOUNT_NUM_1 1
#define TESTCOUNT_NUM_2 2
#define TESTCOUNT_NUM_3 3
#define TESTCOUNT_NUM_4 4
#define TESTCOUNT_NUM_5 5

#define DELAY_TICKS_1 1
#define DELAY_TICKS_5 5
#define DELAY_TICKS_10 10

#define TEST_TASK_STACK_SIZE 0x600
#define TASK_LOOP_NUM 0x10000000
#define TEST_TIME 10
#define THREAD_COUNT_MIN 3
#define THREAD_COUNT_MAX 30
#define THREAD_STACK_SPACE_MAX 4096

#define ICUNIT_GOTO_NOT_EQUAL(param, value, retcode, label)    \
    do {                                                       \
        if ((param) == (value)) {                              \
            TEST_ASSERT_NOT_EQUAL(param, value);               \
            printf("\nret = %d, expect = %d\n", param, value); \
            goto label;                                        \
        }                                                      \
    } while (0)

#define ICUNIT_GOTO_EQUAL(param, value, retcode, label)        \
    do {                                                       \
        if ((param) != (value)) {                              \
            TEST_ASSERT_EQUAL(param, value);                   \
            printf("\nret = %d, expect = %d\n", param, value); \
            goto label;                                        \
        }                                                      \
    } while (0)

#define ICUNIT_GOTO_STRING_EQUAL(str1, str2, retcode, label) \
    do {                                                     \
        if (strcmp(str1, str2) != 0) {                       \
            TEST_ASSERT_EQUAL(retcode, retcode + 1);         \
            goto label;                                      \
        }                                                    \
    } while (0)

#define ICUNIT_ASSERT_EQUAL_VOID(param, value, retcode) \
    do {                                                \
        if ((param) != (value)) {                       \
            TEST_ASSERT_EQUAL(param, value);            \
            return;                                     \
        }                                               \
    } while (0)

#define ICUNIT_TRACK_EQUAL(param, value, retcode)              \
    do {                                                       \
        if ((param) != (value)) {                              \
            TEST_ASSERT_EQUAL(param, value);                   \
            printf("\nret = %d, expect = %d\n", param, value); \
        }                                                      \
    } while (0)
#define ICUNIT_TRACK_NOT_EQUAL(param, value, retcode)          \
    do {                                                       \
        if ((param) == (value)) {                              \
            TEST_ASSERT_NOT_EQUAL(param, value);               \
            printf("\nret = %d, expect = %d\n", param, value); \
        }                                                      \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
#endif
#endif /* __cplusplus */
#endif
