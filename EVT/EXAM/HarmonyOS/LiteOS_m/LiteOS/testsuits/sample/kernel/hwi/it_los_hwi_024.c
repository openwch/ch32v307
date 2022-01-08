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
    UINT32 ret;

    TestHwiClear(HWI_NUM_TEST);

    g_testCount++;

    ret = LOS_SemPost(g_usSemID);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    return;

EXIT:
    LOS_TaskDelete(g_testTaskID01);
    LOS_SemDelete(g_usSemID);
    return;
}

static VOID TaskF01(VOID)
{
    UINT32 ret;

    g_testCount++;

    ret = LOS_SemPend(g_usSemID, LOS_WAIT_FOREVER);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    ICUNIT_GOTO_EQUAL(g_testCount, 2, g_testCount, EXIT1); // Compare wiht the expected value 2.
    g_testCount++;

    TestHwiDelete(HWI_NUM_TEST);
    LOS_TaskDelete(g_testTaskID01);

    return;

EXIT1:
    TestHwiDelete(HWI_NUM_TEST);
    LOS_SemDelete(g_usSemID);
    LOS_TaskDelete(g_testTaskID01);
    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    UINT32 semCount = 1;
    HWI_PRIOR_T hwiPrio = 7;
    HWI_MODE_T mode = 0;
    HWI_ARG_T arg = 0;
    TSK_INIT_PARAM_S task;

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task.pcName = "HwiTask024A";
    task.uwStackSize = TASK_STACK_SIZE_TEST;
    task.usTaskPrio = TASK_PRIO_TEST - 1;
    task.uwResved = LOS_TASK_STATUS_DETACHED;

    g_testCount = 0;

    ret = LOS_SemCreate(semCount, &g_usSemID);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_SemPend(g_usSemID, LOS_NO_WAIT);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    ret = LOS_HwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    ret = LOS_TaskCreate(&g_testTaskID01, &task);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    TestHwiTrigger(HWI_NUM_TEST);
    ICUNIT_ASSERT_EQUAL(g_testCount, 3, g_testCount); // Compare wiht the expected value 3.

EXIT1:
    ret = LOS_SemDelete(g_usSemID);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    return LOS_OK;

EXIT2:
    ret = LOS_SemDelete(g_usSemID);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    TestHwiDelete(HWI_NUM_TEST);

    return LOS_OK;
}

VOID ItLosHwi024(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosHwi024", Testcase, TEST_LOS, TEST_HWI, TEST_LEVEL2, TEST_FUNCTION);
}
