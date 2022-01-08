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
    g_testCount++;
    return;
}

#ifdef __RISC_V__
static UINT32 OsShellCmdTaskCntGet(VOID)
{
    UINT32 loop;
    UINT32 taskCnt = 0;
    UINT32 intSave;
    LosTaskCB *taskCB = (LosTaskCB *)NULL;

    intSave = LOS_IntLock();
    for (loop = 0; loop < g_taskMaxNum; loop++) {
        taskCB = (((LosTaskCB *)g_taskCBArray) + loop);
        if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
            continue;
        }
        taskCnt++;
    }
    (VOID)LOS_IntRestore(intSave);
    return taskCnt;
}
#endif

static UINT32 TestCase(VOID)
{
    UINT32 ret;
    UINT8 startIndex;
    UINT8 index;
    UINT8 delIndex;
    UINT8 pro;
    CHAR acName[TASK_NAME_NUM];
    UINT32 auwTestTaskID[LOSCFG_BASE_CORE_TSK_LIMIT];
#ifdef __RISC_V__
    UINT32 taskCnt;
    taskCnt = OsShellCmdTaskCntGet();
#endif
    TSK_INIT_PARAM_S task1 = { 0 };
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.uwStackSize = LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE;

#ifdef __RISC_V__
    g_leavingTaskNum = LOSCFG_BASE_CORE_TSK_LIMIT - taskCnt;
#else
    g_leavingTaskNum = LOSCFG_BASE_CORE_TSK_LIMIT - TASK_EXISTED_NUM;
#endif
    LOS_TaskLock();

    if (LOSCFG_BASE_CORE_SWTMR == 1) {
        startIndex = 1;
        pro = 1;
    } else {
        startIndex = 0;
        pro = 0;
    }

    g_testCount = 0;

    for (index = 0; index < g_leavingTaskNum + startIndex; pro++, index++) {
        if (pro == TASK_PRIO_TEST) {
            pro++;
        }

        if (pro >= OS_TASK_PRIORITY_LOWEST) {
            pro = 0;
        }

        task1.usTaskPrio = pro;
        (void)sprintf_s(acName, TASK_NAME_NUM, "Tsk087_%d", index);
        task1.pcName = acName;

        ret = LOS_TaskCreate(&auwTestTaskID[index], &task1);
        ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    }

    task1.pcName = "Tsk087Err";
    task1.usTaskPrio++;
    ret = LOS_TaskCreate(&auwTestTaskID[index], &task1);
    ICUNIT_TRACK_EQUAL(ret, LOS_ERRNO_TSK_TCB_UNAVAILABLE, ret);

EXIT:
    for (delIndex = 0; delIndex < index - 1 + startIndex; delIndex++) {
        ret = LOS_TaskDelete(auwTestTaskID[delIndex]);
        ICUNIT_TRACK_EQUAL(ret, LOS_OK, ret);
    }

    LOS_TaskUnlock();

    ICUNIT_TRACK_EQUAL(g_testCount, 0, g_testCount);

    return LOS_OK;
}

VOID ItLosTask087(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosTask087", TestCase, TEST_LOS, TEST_TASK, TEST_LEVEL2, TEST_PRESSURE);
}

