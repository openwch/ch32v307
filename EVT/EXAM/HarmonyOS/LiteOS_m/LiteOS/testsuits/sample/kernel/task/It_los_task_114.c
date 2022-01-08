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


static VOID HwiF01(VOID)
{
    TestHwiClear(HWI_NUM_TEST);

    // 3, modify task priority, base on testsuite task`s priority.
    LOS_CurTaskPriSet(TASK_PRIO_TEST - 3);

    g_testCount++;
}

static VOID TaskF01(VOID)
{
    UINT32 ret;
    HWI_PRIOR_T hwiPrio = 3;
    HWI_MODE_T mode = 0;
    HWI_ARG_T arg = 0;

    ret = LOS_HwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    TestHwiTrigger(HWI_NUM_TEST);

    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT);
    ret = LOS_TaskPriGet(g_testTaskID01);

    // 3, Assert this result is consistent with the priority that has been set.
    ICUNIT_ASSERT_EQUAL_VOID(ret, TASK_PRIO_TEST - 3, ret);

EXIT:
    TestHwiDelete(HWI_NUM_TEST);
    LOS_TaskDelete(g_testTaskID01);
    return;
}

static UINT32 TestCase(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1 = { 0 };
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.uwStackSize = TASK_STACK_SIZE_TEST;
    task1.pcName = "Tsk114A";
    task1.usTaskPrio = TASK_PRIO_TEST - 2; // 2, set new task priority, it is higher than the current task.
    task1.uwResved = LOS_TASK_STATUS_DETACHED;

    g_testCount = 0;
    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    LOS_TaskDelay(10); // 10, set delay time

    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT);

    return LOS_OK;

EXIT:
    LOS_TaskDelete(g_testTaskID01);

    return LOS_OK;
}

VOID ItLosTask114(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosTask114", TestCase, TEST_LOS, TEST_TASK, TEST_LEVEL0, TEST_FUNCTION);
}

