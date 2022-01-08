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
#include "It_los_task.h"


static VOID TaskF01(VOID)
{
    UINT32 tick1;
    UINT32 tick2;

    ICUNIT_GOTO_EQUAL(g_testCount, 0, g_testCount, EXIT);
    g_testCount++;

    tick1 = LOS_TickCountGet();
    LOS_TaskDelay(10); // 10, set delay time
    tick2 = LOS_TickCountGet();
    tick2 = tick2 - tick1;

    if ((tick2 != 10) && (tick2 != 11)) { // 10, 11 delay time
        ICUNIT_GOTO_EQUAL(tick2, 0, tick2, EXIT); // 0, Here, assert that result is equal to 0.
    }

    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT); // 2, Here, assert that g_testCount is equal to 2.
    g_testCount++;

EXIT:
    LOS_TaskDelete(g_testTaskID01);

    return;
}

static VOID TaskF02(VOID)
{
    UINT32 ret;
    UINT32 semHandle;
    UINT32 tick1;
    UINT32 tick2;

    ret = LOS_SemCreate(0, &semHandle);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ICUNIT_GOTO_EQUAL(g_testCount, 3, g_testCount, EXIT); // 3, Here, assert that g_testCount is equal to 3.
    g_testCount++;

    tick1 = LOS_TickCountGet();
    ret = LOS_SemPend(semHandle, 10); // 10, suspend with wait time.
    tick2 = LOS_TickCountGet();
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_SEM_TIMEOUT, ret, EXIT);

    tick2 = tick2 - tick1;
    if ((tick2 != 10) && (tick2 != 11)) { // 10, 11 delay time
        ICUNIT_GOTO_EQUAL(tick2, 0, tick2, EXIT); // 0, Here, assert that result is equal to 0.
    }
    ICUNIT_GOTO_EQUAL(g_testCount, 5, g_testCount, EXIT); // 5, Here, assert that g_testCount is equal to 5.
    g_testCount++;

    ret = LOS_SemDelete(semHandle);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

EXIT:
    LOS_SemDelete(semHandle);
    LOS_TaskDelete(g_testTaskID01);
    return;
}

static UINT32 TestCase(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1 = { 0 };

    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.uwStackSize = TASK_STACK_SIZE_TEST;
    task1.pcName = "Tsk081A";
    task1.usTaskPrio = TASK_PRIO_TEST - 2; // 2, set new task priority, it is higher than the current task.

    g_testCount = 0;

    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ICUNIT_ASSERT_EQUAL(g_testCount, 1, g_testCount);
    g_testCount++;

    ret = LOS_TaskSuspend(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_TaskResume(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    LOS_TaskDelay(10); // 10, set delay time
    ICUNIT_ASSERT_EQUAL(g_testCount, 3, g_testCount); // 3, Here, assert that g_testCount is equal to 3.

    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF02;

    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ICUNIT_ASSERT_EQUAL(g_testCount, 4, g_testCount); // 4, Here, assert that g_testCount is equal to 4.
    g_testCount++;

    ret = LOS_TaskSuspend(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_TaskResume(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    LOS_TaskDelay(11); // 11, set delay time
    ICUNIT_ASSERT_EQUAL(g_testCount, 6, g_testCount); // 6, Here, assert that g_testCount is equal to 6.
    return LOS_OK;
}

VOID ItLosTask081() // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosTask081", TestCase, TEST_LOS, TEST_TASK, TEST_LEVEL1, TEST_FUNCTION);
}

