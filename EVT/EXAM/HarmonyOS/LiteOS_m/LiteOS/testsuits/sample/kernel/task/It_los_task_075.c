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


static VOID TaskF02(VOID)
{
    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT1);
    g_testCount++;

    while (g_testCount <= 2) { // 2, wait until g_testCount is greater to 2.
    }

    ICUNIT_GOTO_EQUAL(g_testCount, 3, g_testCount, EXIT2); // 3, Here, assert that g_testCount is equal to 3.
    g_testCount++;

    LOS_TaskDelete(g_testTaskID02);

    return;

EXIT1:
    LOS_TaskDelete(g_testTaskID02);
EXIT2:
    LOS_TaskDelete(g_testTaskID01);
    return;
}

static VOID TaskF01(VOID)
{
    UINT32 ret;

    g_testCount++;

    ret = LOS_TaskDelay(1);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT); // 2, Here, assert that g_testCount is equal to 2.
    g_testCount++;

    LOS_TaskDelete(g_testTaskID01);

    return;

EXIT:
    LOS_TaskDelete(g_testTaskID01);
    LOS_TaskDelete(g_testTaskID02);
    return;
}

static UINT32 TestCase(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1 = { 0 };
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.uwStackSize = TASK_STACK_SIZE_TEST;
    task1.pcName = "Tsk075A";
    task1.usTaskPrio = TASK_PRIO_TEST - 2; // 2, set new task priority, it is higher than the current task.

    g_testCount = 0;

    LOS_TaskLock();

    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT3);

    ret = LOS_TaskDelay(2); // 2, set delay time
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_TSK_DELAY_IN_LOCK, ret, EXIT2);

    ICUNIT_GOTO_EQUAL(g_testCount, 0, g_testCount, EXIT2);

    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF02;
    task1.pcName = "Tsk075B";
    task1.usTaskPrio = TASK_PRIO_TEST - 1;

    ret = LOS_TaskCreate(&g_testTaskID02, &task1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    ret = LOS_TaskDelay(2); // 2, set delay time
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_TSK_DELAY_IN_LOCK, ret);

    ICUNIT_GOTO_EQUAL(g_testCount, 0, g_testCount, EXIT1);

    LOS_TaskUnlock();

    ICUNIT_ASSERT_EQUAL(g_testCount, 4, g_testCount); // 4, Here, assert that g_testCount is equal to 4.

    return LOS_OK;

EXIT1:
    LOS_TaskDelete(g_testTaskID02);
EXIT2:
    LOS_TaskDelete(g_testTaskID01);
EXIT3:
    LOS_TaskLock();
    return LOS_OK;
}

VOID ItLosTask075(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosTask075", TestCase, TEST_LOS, TEST_TASK, TEST_LEVEL1, TEST_FUNCTION);
}

