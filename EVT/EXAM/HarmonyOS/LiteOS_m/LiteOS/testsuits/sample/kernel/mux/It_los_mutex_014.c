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
    g_testCount++;

    ret = LOS_MuxPend(g_mutexTest, 50); // 50,  mux pend timeout.
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    g_testCount++;

    LOS_TaskDelay(30); // 30, set delay time.

    ret = LOS_MuxPost(g_mutexTest);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    LOS_TaskDelay(100); // 100, set delay time.
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task;
    TSK_INIT_PARAM_S task2;

    ret = LOS_MuxCreate(&g_mutexTest);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    g_testCount = 0;

    LOS_TaskLock();

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task.usTaskPrio = (TASK_PRIO_TEST - 1); // 1, set new task priority, it is higher than the current task.
    task.pcName = "VMuteB9_1";
    task.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    task.uwResved = 0;

    ret = LOS_TaskCreate(&g_testTaskID01, &task);

    task2.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task2.usTaskPrio = (TASK_PRIO_TEST - 2); // 2, set new task priority, it is higher than the current task.
    task2.pcName = "VMuteB9_2";
    task2.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    task2.uwResved = 0;

    ret = LOS_TaskCreate(&g_testTaskID02, &task2);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    LOS_TaskUnlock();

    ICUNIT_ASSERT_EQUAL(g_testCount, 3, g_testCount); // 3, Here, assert that g_testCount is equal to 3.

    LOS_TaskDelay(30); // 30, set delay time.
    ICUNIT_ASSERT_EQUAL(g_testCount, 4, g_testCount); // 4, Here, assert that g_testCount is equal to 4.

    LOS_TaskDelay(30); // 30, set delay time.

    ret = LOS_TaskDelete(g_testTaskID02);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    ret = LOS_TaskDelete(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_MuxDelete(g_mutexTest);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    return LOS_OK;
}

VOID ItLosMux014(void)
{
    TEST_ADD_CASE("ItLosMux014", Testcase, TEST_LOS, TEST_MUX, TEST_LEVEL1, TEST_FUNCTION);
}

