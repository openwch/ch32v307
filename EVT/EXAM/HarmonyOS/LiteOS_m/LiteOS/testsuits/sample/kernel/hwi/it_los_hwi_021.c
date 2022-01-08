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


static VOID TaskF02(VOID)
{
    ICUNIT_GOTO_EQUAL(g_testCount, 3, g_testCount, EXIT); // Compare wiht the expected value 3.
    g_testCount++;

EXIT:
    LOS_TaskDelete(g_testTaskID02);
    return;
}

static VOID HwiF01(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task;

    TestHwiClear(HWI_NUM_TEST);

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF02;
    task.pcName = "HwiTsk021B";
    task.uwStackSize = TASK_STACK_SIZE_TEST;
    task.usTaskPrio = TASK_PRIO_TEST - 1;
    task.uwResved = LOS_TASK_STATUS_DETACHED;

    g_testCount++;

    ret = LOS_TaskCreate(&g_testTaskID02, &task);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    return;

EXIT:
    LOS_TaskDelete(g_testTaskID01);
    return;
}

static VOID TaskF01(VOID)
{
    g_testCount++;

    TestHwiTrigger(HWI_NUM_TEST);

    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT); // Compare wiht the expected value 2.
    g_testCount++;

EXIT:
    TestHwiDelete(HWI_NUM_TEST);
    LOS_TaskDelete(g_testTaskID01);
    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    HWI_PRIOR_T hwiPrio = 7;
    HWI_MODE_T mode = 0;
    HWI_ARG_T arg = 0;
    TSK_INIT_PARAM_S task;

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task.pcName = "HwiTsk021A";
    task.uwStackSize = TASK_STACK_SIZE_TEST;
    task.usTaskPrio = TASK_PRIO_TEST - 2; // 2, set new task priority, it is higher than the current task.
    task.uwResved = LOS_TASK_STATUS_DETACHED;

    g_testCount = 0;

    ret = LOS_HwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_TaskCreate(&g_testTaskID01, &task);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ICUNIT_ASSERT_EQUAL(g_testCount, 4, g_testCount); // Compare wiht the expected value 4.

    return LOS_OK;

EXIT:
    TestHwiDelete(HWI_NUM_TEST);
    return LOS_OK;
}

VOID ItLosHwi021(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosHwi021", Testcase, TEST_LOS, TEST_HWI, TEST_LEVEL2, TEST_FUNCTION);
}
