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
#include "It_los_swtmr.h"


static VOID Case1()
{
    UINT32 ret;

    while (g_swtmrCountC != SWTMR_LOOP_NUM) {
        ret = LOS_EventRead(&g_eventCB3, 0x1, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
        ICUNIT_ASSERT_EQUAL_VOID(ret, 0x1, ret);
    }
}

static VOID Case2()
{
    UINT32 ret;

    while (g_swtmrCountB != SWTMR_LOOP_NUM) {
        ret = LOS_EventRead(&g_eventCB2, 0x11, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
        ICUNIT_ASSERT_EQUAL_VOID(ret, 0x11, ret);
    }
}

static VOID Case3()
{
    UINT32 ret;

    while (g_swtmrCountA != SWTMR_LOOP_NUM) {
        g_testCount++;
        ret = LOS_EventRead(&g_eventCB1, 0x111, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
        ICUNIT_ASSERT_EQUAL_VOID(ret, 0x111, ret);
    }
}

VOID Case4()
{
    UINT32 ret;

    g_swtmrCountC++;
    ret = LOS_EventWrite(&g_eventCB3, 0x1);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    if (g_swtmrCountC == SWTMR_LOOP_NUM) {
        LOS_TaskDelete(g_testTaskID03);
    }
}

VOID Case5()
{
    UINT32 ret;

    g_swtmrCountB++;
    ret = LOS_EventWrite(&g_eventCB2, 0x11);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    if (g_swtmrCountB == SWTMR_LOOP_NUM) {
        LOS_TaskDelete(g_testTaskID02);
    }
}

VOID Case6()
{
    UINT32 ret;

    g_swtmrCountA++;
    ret = LOS_EventWrite(&g_eventCB1, 0x111);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    if (g_swtmrCountA == SWTMR_LOOP_NUM) {
        LOS_TaskDelete(g_testTaskID01);
    }
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1;

    g_testCount = 0;
    g_swtmrCountA = 0;
    g_swtmrCountB = 0;
    g_swtmrCountC = 0;

    ret = LOS_EventInit(&g_eventCB1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_EventInit(&g_eventCB2);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_EventInit(&g_eventCB3);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    // 1, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(1, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)Case6, &g_swtmrId1, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    // 2, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(2, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)Case5, &g_swtmrId2, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    // 3, Timeout interval of a periodic software timer.
    ret = LOS_SwtmrCreate(3, LOS_SWTMR_MODE_PERIOD, (SWTMR_PROC_FUNC)Case4, &g_swtmrId3, 0xffff
#if (LOSCFG_BASE_CORE_SWTMR_ALIGN == 1)
        , OS_SWTMR_ROUSES_ALLOW, OS_SWTMR_ALIGN_INSENSITIVE
#endif
    );
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    LOS_TaskLock();

    (void)memset_s(&task1, sizeof(TSK_INIT_PARAM_S), 0, sizeof(TSK_INIT_PARAM_S));
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)Case3;
    task1.pcName = "TskNameA";
    task1.usTaskPrio = TASK_PRIO_TEST_NORMAL;
    task1.uwStackSize = TASK_STACK_SIZE_TEST * 2; // 2, set statck size 

    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)Case2;
    task1.pcName = "TskNameB";
    task1.usTaskPrio = TASK_PRIO_TEST_NORMAL - 1;
    ret = LOS_TaskCreate(&g_testTaskID02, &task1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)Case1;
    task1.pcName = "TskNameC";
    // 2, set new task priority, it is higher than the current task.
    task1.usTaskPrio = TASK_PRIO_TEST_NORMAL - 2;
    ret = LOS_TaskCreate(&g_testTaskID03, &task1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    LOS_SwtmrStart(g_swtmrId3);
    LOS_SwtmrStart(g_swtmrId2);
    LOS_SwtmrStart(g_swtmrId1);

    LOS_TaskUnlock();

    while (1) {
        if (g_swtmrCountA == SWTMR_LOOP_NUM) {
            ICUNIT_GOTO_EQUAL(g_testCount, g_swtmrCountA, g_testCount, EXIT);

            ret = LOS_SwtmrDelete(g_swtmrId1);
            ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

            g_swtmrCountA = 0;
        }

        if (g_swtmrCountB == SWTMR_LOOP_NUM) {
            ret = LOS_SwtmrDelete(g_swtmrId2);
            ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

            g_swtmrCountB = 0;
        }

        if (g_swtmrCountC == SWTMR_LOOP_NUM) {
            ret = LOS_SwtmrDelete(g_swtmrId3);
            ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

            g_swtmrCountC = 0;
            break;
        }
    }

    ret = LOS_EventDestroy(&g_eventCB1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_EventDestroy(&g_eventCB2);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_EventDestroy(&g_eventCB3);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    return LOS_OK;

EXIT:
    LOS_SwtmrDelete(g_swtmrId3);
    LOS_SwtmrDelete(g_swtmrId2);
    LOS_SwtmrDelete(g_swtmrId1);
    LOS_TaskDelete(g_testTaskID02);
    LOS_TaskDelete(g_testTaskID03);

    return LOS_OK;
}

VOID ItLosSwtmr035() // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosSwtmr035", Testcase, TEST_LOS, TEST_SWTMR, TEST_LEVEL3, TEST_FUNCTION);
}

