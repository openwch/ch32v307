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

#include <stdarg.h>
#include "ohos_types.h"
#include "los_config.h"
#include "Public.h"
#include "PCommon.h"
#include "fuzz_posix.h"

extern S_ElementInit g_element[ELEMENT_LEN];
extern int g_iteration;

static int VaFunc(int argsNum, ...)
{
    int sum1 = 0;
    int sum2 = 0;
    va_list argPtr1;
    va_start(argPtr1, argsNum);
    va_list argPtr2;
    va_copy(argPtr2, argPtr1);

    for (int i = 0; i < argsNum; i++) {
        sum1 += va_arg(argPtr1, int);
        sum2 += va_arg(argPtr2, int);
    }
    int sum = sum1 + sum2;

    va_end(argPtr1);
    va_end(argPtr2);

    return sum;
}

int StdargFuzz(void)
{
    int i;
    const int initValue1 = 5;
    const int initValue2 = 10;
    const int initValue3 = 15;
    const int argNum = 3;
    const int count = 2;

    printf("Fuzz test in line [%d] stdarg start\n", __LINE__);

    INIT_FuzzEnvironment();
    CreatPrecondForQueue();

    for (i = 0; i < g_iteration; i++) {
        hi_watchdog_feed();
        heartbeatPrint(i);

        unsigned int stdargVal1 = *((unsigned int *)DT_SetGetU32(&g_element[NUM_0_INDEX], initValue1));
        unsigned int stdargVal2 = *((unsigned int *)DT_SetGetU32(&g_element[NUM_1_INDEX], initValue2));
        unsigned int stdargVal3 = *((unsigned int *)DT_SetGetU32(&g_element[NUM_2_INDEX], initValue3));
        int ret = VaFunc(argNum, stdargVal1, stdargVal2, stdargVal3);
        int sum = (stdargVal1 + stdargVal2 + stdargVal3) * count;
        if (ret == sum) {
            printf("VA_FUNC_TEST(3, %d, %d, %d) = %d", stdargVal1, stdargVal2, stdargVal3, ret);
            return RET_TRUE;
        }
    }

    CleanPrecondForQueue();
    DT_Clear(g_element);
    CLOSE_Log();
    CLEAR_FuzzEnvironment();

    printf("Fuzz test in line [%d] va_start/va_copy/va_end ok\n", __LINE__);

    return 0;
}