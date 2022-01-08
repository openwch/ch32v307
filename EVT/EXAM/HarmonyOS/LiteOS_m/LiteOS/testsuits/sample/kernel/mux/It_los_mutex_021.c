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

#include "osTest.h"
#include "It_los_mux.h"


static VOID TaskF01(void)
{
    UINT32 ret;

    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 1, g_testCount); // 1, Here, assert that g_testCount is equal to 1.

    ret = LOS_MuxPend(g_mutexTest, LOS_WAIT_FOREVER);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    g_testCount++;

    ret = LOS_MuxPost(g_mutexTest);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 2, g_testCount); // 2, Here, assert that g_testCount is equal to 2.
    g_testCount++;

    LOS_TaskDelay(50); // 50, set delay time.
}

VOID TaskF02(void)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1;

    g_testCount++;

    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.pcName = "LosMB2_2";
    task1.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    task1.usTaskPrio = TASK_PRIO_TEST - 3; // 3, set new task priority, it is higher than the current task.
    task1.uwResved = 0;

    ret = LOS_TaskCreate(&g_testTaskID02, &task1);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    LOS_TaskDelay(1); // 1, set delay time.

    ret = LOS_MuxPend(g_mutexTest, LOS_WAIT_FOREVER);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    ret = LOS_MuxPost(g_mutexTest);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);
    ICUNIT_ASSERT_EQUAL_VOID(g_testCount, 3, g_testCount); // 3, Here, assert that g_testCount is equal to 3.
    g_testCount++;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task;

    g_testCount = 0;

    ret = LOS_MuxCreate(&g_mutexTest);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF02;
    task.usTaskPrio = (TASK_PRIO_TEST - 1); // 1, set new task priority, it is higher than the current task.
    task.pcName = "LosMB2_1";
    task.uwStackSize = TASK_STACK_SIZE_TEST * 2; // 2, Used to represent larger stack space  
    task.uwResved = 0;

    ret = LOS_TaskCreate(&g_testTaskID01, &task);

    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    LOS_TaskDelay(1); // 1, set delay time.
    ICUNIT_ASSERT_EQUAL(g_testCount, 4, g_testCount); // 4, Here, assert that g_testCount is equal to 4.

    ret = LOS_MuxDelete(g_mutexTest);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_TaskDelete(g_testTaskID02);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    ret = LOS_TaskDelete(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_TSK_NOT_CREATED, ret);
    return LOS_OK;
}

VOID ItLosMux021(void)
{
    TEST_ADD_CASE("ItLosMux021", Testcase, TEST_LOS, TEST_MUX, TEST_LEVEL1, TEST_FUNCTION);
}

