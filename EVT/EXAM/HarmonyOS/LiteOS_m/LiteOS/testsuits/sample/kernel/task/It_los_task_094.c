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


static VOID TaskF03(UINT32 arg)
{
    ICUNIT_GOTO_EQUAL(arg, -1, arg, EXIT);
    g_testCount++;
EXIT:
    return;
}

static VOID TaskF02(UINT32 arg)
{
    ICUNIT_GOTO_EQUAL(arg, 0, arg, EXIT);
    g_testCount++;
EXIT:
    return;
}

static VOID TaskF01(UINT32 arg1)
{
    ICUNIT_GOTO_EQUAL(arg1, 0xffff, arg1, EXIT);
    g_testCount++;
EXIT:
    return;
}

static UINT32 TestCase(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1 = { 0 };
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.uwStackSize = TASK_STACK_SIZE_TEST;
    task1.pcName = "Tsk094A";
    task1.usTaskPrio = TASK_PRIO_TEST - 2; // 2, set new task priority, it is higher than the current task.
    task1.uwResved = 0;
    task1.uwArg = 0xffff;

    g_testCount = 0;
    ret = LOS_TaskCreateOnly(&g_testTaskID01, &task1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    ICUNIT_GOTO_EQUAL(g_testCount, 0, g_testCount, EXIT);

    LOS_TaskResume(g_testTaskID01);

    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT);

    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF02;
    task1.uwArg = 0;
    task1.pcName = "Tsk094B";
    task1.usTaskPrio = TASK_PRIO_TEST - 3; // 3, set new task priority base on testsuite task`s priority.
    ret = LOS_TaskCreateOnly(&g_testTaskID02, &task1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT);

    LOS_TaskResume(g_testTaskID02);

    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT); // 2, Here, assert that g_testCount is equal to 2.

    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF03;
    task1.uwArg = -1;
    task1.pcName = "Tsk094C";
    task1.usTaskPrio = TASK_PRIO_TEST - 4; // 4, set new task priority base on testsuite task`s priority.
    ret = LOS_TaskCreateOnly(&g_testTaskID03, &task1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT); // 2, Here, assert that g_testCount is equal to 2.

    LOS_TaskResume(g_testTaskID03);

    ICUNIT_GOTO_EQUAL(g_testCount, 3, g_testCount, EXIT); // 3, Here, assert that g_testCount is equal to 3.

    LOS_TaskDelay(10); // 10, set delay time

EXIT:
    LOS_TaskDelete(g_testTaskID01);
    LOS_TaskDelete(g_testTaskID02);
    LOS_TaskDelete(g_testTaskID03);
    return LOS_OK;
}

VOID ItLosTask094(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosTask094", TestCase, TEST_LOS, TEST_TASK, TEST_LEVEL0, TEST_FUNCTION);
}

