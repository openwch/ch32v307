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


static UINT32 g_taskCountArray[LOSCFG_BASE_CORE_TSK_LIMIT] = {0};

static VOID TaskF01(VOID)
{
    ICUNIT_TRACK_EQUAL(g_taskCountArray[g_index], g_testCount, g_taskCountArray[g_index]);
    g_testCount++;
    g_index++;
    LOS_TaskDelete(g_testTaskIdArray[g_index - 1]);
}

static UINT32 TestCase(VOID)
{
    UINT32 ret;
    UINT8 startIndex;
    UINT8 delIndex;
    UINT16 pro;
    CHAR acName[TASK_NAME_NUM];
    TSK_INIT_PARAM_S task1 = { 0 };
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.uwStackSize = LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE + 0x100;

    if (LOSCFG_BASE_CORE_SWTMR == 1) {
        startIndex = 3; // 3, init the index  according to LOSCFG_BASE_CORE_SWTMR.
        pro = 1;
    } else {
        startIndex = 0;
        pro = 0;
    }

    g_testCount = 0;
    g_index = startIndex;

    LOS_TaskLock();

    for (g_index = startIndex; g_index < LOSCFG_BASE_CORE_TSK_LIMIT; pro++, g_testCount++, g_index++) {
        pro %= OS_TASK_PRIORITY_LOWEST;
        task1.usTaskPrio = pro;
        (void)sprintf_s(acName, TASK_NAME_NUM, "Tsk083_%d", g_index);
        task1.pcName = acName;
        ret = LOS_TaskCreate(&g_testTaskIdArray[g_index], &task1);
        ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

        g_taskCountArray[g_index] = g_testCount;
    }

    g_testCount = 0;
    g_index = startIndex;
    LOS_TaskUnlock();
    LOS_TaskDelay(20); // 20, set delay time

    return LOS_OK;

EXIT:
    for (delIndex = startIndex; delIndex < g_index - 1 + startIndex; delIndex++) {
        LOS_TaskDelete(g_testTaskIdArray[delIndex]);
    }

    LOS_TaskUnlock();

    return LOS_OK;
}

VOID ItLosTask083(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosTask083", TestCase, TEST_LOS, TEST_TASK, TEST_LEVEL2, TEST_PRESSURE);
}

