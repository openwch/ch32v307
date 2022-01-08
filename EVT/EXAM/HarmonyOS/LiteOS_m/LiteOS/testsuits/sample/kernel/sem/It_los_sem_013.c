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
#include "It_los_sem.h"


static VOID TaskF01(void)
{
    UINT32 ret;

    g_testCount++;
    ret = LOS_SemPend(g_usSemID, LOS_WAIT_FOREVER);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    g_testCount++;
    LOS_TaskDelete(g_testTaskID01);
}

static VOID TaskF02(void)
{
    UINT32 ret;

    g_testCount++;
    ret = LOS_SemPend(g_usSemID2, LOS_WAIT_FOREVER);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    g_testCount++;
    LOS_TaskDelete(g_testTaskID02);
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task = { 0 };

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task.pcName = "SemTsk13";
    task.uwStackSize = TASK_STACK_SIZE_TEST;
    task.usTaskPrio = TASK_PRIO_TEST - 1;

    g_testCount = 0;

    ret = LOS_SemCreate(0, &g_usSemID);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_SemCreate(0, &g_usSemID2);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_TaskCreate(&g_testTaskID01, &task);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    ICUNIT_GOTO_EQUAL(g_testCount, 1, g_testCount, EXIT3);

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF02;
    task.pcName = "Sem13_1";
    task.usTaskPrio = TASK_PRIO_TEST - 2; // 2, set new task priority, it is higher than the current task.
    ret = LOS_TaskCreate(&g_testTaskID02, &task);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT3);

    ICUNIT_TRACK_EQUAL(g_testCount, 2, g_testCount); // 2, Here, assert that g_testCount is equal to 2.

    LOS_TaskDelete(g_testTaskID02);
EXIT3:
    LOS_TaskDelete(g_testTaskID01);
EXIT2:
    ret = LOS_SemDelete(g_usSemID2);
    ICUNIT_TRACK_EQUAL(ret, LOS_OK, ret);
EXIT:
    ret = LOS_SemDelete(g_usSemID);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    return LOS_OK;
}


VOID ItLosSem013(void)
{
    TEST_ADD_CASE("ItLosSem013", Testcase, TEST_LOS, TEST_SEM, TEST_LEVEL1, TEST_FUNCTION);
}

