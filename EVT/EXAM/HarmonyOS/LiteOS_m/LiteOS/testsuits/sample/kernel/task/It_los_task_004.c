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
    return;
}

static UINT32 TestCase(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1 = { 0 };
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.uwStackSize = LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE - LOSCFG_STACK_POINT_ALIGN_SIZE;
    task1.pcName = "Tsk004A";
    task1.usTaskPrio = TASK_PRIO_TEST - 2; // 2, set new task priority, it is higher than the current task.

    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_TSK_STKSZ_TOO_SMALL, ret);

    return LOS_OK;
}

VOID ItLosTask004(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosTask004", TestCase, TEST_LOS, TEST_TASK, TEST_LEVEL0, TEST_FUNCTION);
}

