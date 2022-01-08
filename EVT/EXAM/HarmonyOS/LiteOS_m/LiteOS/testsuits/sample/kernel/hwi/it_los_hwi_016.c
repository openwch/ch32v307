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
#include "it_los_hwi.h"


static VOID HwiF01(VOID)
{
    TestHwiClear(HWI_NUM_TEST);
    g_testCount++;
    return;
}

static VOID TaskF03(VOID)
{
    ICUNIT_GOTO_EQUAL(g_testCount, 4, g_testCount, EXIT); // Compare wiht the expected value 4.
    g_testCount++;

    LOS_TaskDelete(g_testTaskID03);
    return;

EXIT:
    TestHwiDelete(HWI_NUM_TEST);
    LOS_TaskDelete(g_testTaskID02);
    LOS_TaskDelete(g_testTaskID01);
    LOS_TaskDelete(g_testTaskID03);
    return;
}

static VOID TaskF02(VOID)
{
    ICUNIT_GOTO_EQUAL(g_testCount, 3, g_testCount, EXIT); // Compare wiht the expected value 3.
    g_testCount++;

    LOS_TaskDelete(g_testTaskID02);
    return;

EXIT:
    TestHwiDelete(HWI_NUM_TEST);
    LOS_TaskDelete(g_testTaskID03);
    LOS_TaskDelete(g_testTaskID01);
    LOS_TaskDelete(g_testTaskID02);
    return;
}

static VOID TaskF01(VOID)
{
    g_testCount++;

    TestHwiTrigger(HWI_NUM_TEST);

    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT); // Compare wiht the expected value 2.
    g_testCount++;

    LOS_TaskDelete(g_testTaskID01);
    TestHwiDelete(HWI_NUM_TEST);
    return;

EXIT:
    TestHwiDelete(HWI_NUM_TEST);
    LOS_TaskDelete(g_testTaskID03);
    LOS_TaskDelete(g_testTaskID02);
    LOS_TaskDelete(g_testTaskID01);
    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    HWI_PRIOR_T hwiPrio = 1;
    HWI_MODE_T mode = 0;
    HWI_ARG_T arg = 0;
    TSK_INIT_PARAM_S task;

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task.pcName = "HwiTsk016A";
    task.uwStackSize = TASK_STACK_SIZE_TEST;
    task.usTaskPrio = TASK_PRIO_TEST - 3; // 3, set new task priority, it is higher than the current task.
    task.uwResved = LOS_TASK_STATUS_DETACHED;

    g_testCount = 0;

    LOS_TaskLock();

    ret = LOS_TaskCreate(&g_testTaskID01, &task);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF02;
    task.pcName = "HwiTsk016B";
    task.uwStackSize = TASK_STACK_SIZE_TEST;
    task.usTaskPrio = TASK_PRIO_TEST - 2; // 2, set new task priority, it is higher than the current task.
    task.uwResved = LOS_TASK_STATUS_DETACHED;

    ret = LOS_TaskCreate(&g_testTaskID02, &task);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF03;
    task.pcName = "HwiTsk016C";
    task.uwStackSize = TASK_STACK_SIZE_TEST;
    task.usTaskPrio = TASK_PRIO_TEST - 1;
    task.uwResved = LOS_TASK_STATUS_DETACHED;

    ret = LOS_TaskCreate(&g_testTaskID03, &task);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    ret = LOS_HwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT3);

    LOS_TaskUnlock();
    ICUNIT_ASSERT_EQUAL(g_testCount, 5, g_testCount); // Compare wiht the expected value 5.

    TestHwiDelete(HWI_NUM_TEST);
    return LOS_OK;

EXIT3:
    LOS_TaskDelete(g_testTaskID03);
    LOS_TaskDelete(g_testTaskID02);
    LOS_TaskDelete(g_testTaskID01);
    return LOS_OK;

EXIT2:
    LOS_TaskDelete(g_testTaskID02);
    LOS_TaskDelete(g_testTaskID01);
    return LOS_OK;

EXIT1:
    LOS_TaskDelete(g_testTaskID01);
    return LOS_OK;
}

VOID ItLosHwi016(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosHwi016", Testcase, TEST_LOS, TEST_HWI, TEST_LEVEL2, TEST_FUNCTION);
}
