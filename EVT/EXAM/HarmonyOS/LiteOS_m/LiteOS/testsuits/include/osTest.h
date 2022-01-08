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

#ifndef _OSTEST_H
#define _OSTEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iCunit.h"

#include "los_debug.h"
#include "los_config.h"

#include "los_interrupt.h"
#include "los_arch_interrupt.h"
#include "los_task.h"
#include "los_sem.h"
#include "los_event.h"
#include "los_memory.h"
#include "los_queue.h"
#include "los_cpup.h"
#include "los_tick.h"
#include "los_swtmr.h"
#include "los_mux.h"
#include "securec.h"
#include "securectype.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define PRINTF(fmt, args...) \
    do {                     \
        printf(fmt, ##args); \
    } while (0)

#define LITEOS_BASE_TEST 1

#ifndef LOS_KERNEL_TEST_NOT_SMOKE
#define LOS_KERNEL_TEST_NOT_SMOKE 1
#endif
#define LOS_KERNEL_CORE_TASK_TEST 1
#define LOS_KERNEL_IPC_MUX_TEST 1
#define LOS_KERNEL_IPC_SEM_TEST 1
#define LOS_KERNEL_IPC_EVENT_TEST 1
#define LOS_KERNEL_IPC_QUEUE_TEST 1
#define LOS_KERNEL_CORE_SWTMR_TEST 1
#ifndef LOS_KERNEL_HWI_TEST
#define LOS_KERNEL_HWI_TEST 1
#endif
#define LOS_KERNEL_FS_TEST 0
#define LOS_KERNEL_MEM_TEST 1
#define LOS_KERNEL_TICKLESS_TEST 0

#define LITEOS_CMSIS_TEST 0
#define LOS_CMSIS2_CORE_TASK_TEST 0
#define LOS_CMSIS2_IPC_MUX_TEST 0
#define LOS_CMSIS2_IPC_SEM_TEST 0
#define LOS_CMSIS2_IPC_EVENT_TEST 0
#define LOS_CMSIS2_IPC_MSG_TEST 0
#define LOS_CMSIS2_CORE_SWTMR_TEST 0
#define LOS_CMSIS2_HWI_TEST 0

#define LOSCFG_TEST_LLT 0
#define LOSCFG_TEST_MUCH_LOG 0

extern UINT32 volatile g_testCount;
extern UINT32 g_testTskHandle;
extern UINT32 g_testTaskID01;
extern UINT32 g_testTaskID02;
extern UINT32 g_testTaskID03;
extern UINT32 g_testTaskID04;
extern UINT32 g_hwiNum1;
extern UINT32 g_hwiNum2;
extern UINT32 g_usSemID;
extern UINT32 g_usSemID2;
extern UINT32 g_mutexTest;

extern UINT16 g_usSwTmrID;
extern UINT32 g_usSemID;
extern UINT32 g_testQueueID01;
extern UINT32 g_testQueueID02;
extern UINT32 g_testQueueID03;
extern UINT32 g_testTskHandle;
extern UINT32 g_leavingTaskNum;
extern UINT32 g_testTaskIdArray[LOSCFG_BASE_CORE_TSK_LIMIT];
extern UINT16 g_index;
extern UINT32 g_usSemID3[];
extern EVENT_CB_S g_exampleEvent;

#ifndef LOS_TASK_MIN_TEST_STACK_SIZE
#define LOS_TASK_MIN_TEST_STACK_SIZE LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE + 0x20
#endif

#ifndef TASK_TEST
#define TASK_TEST
#endif

#ifndef SWTMR_TEST
#define SWTMR_TEST
#endif

#ifndef QUEUE_TEST
#define QUEUE_TEST
#endif

#ifndef SEM_TEST
#define SEM_TEST
#endif

#ifndef EVENT_TEST
#define EVENT_TEST
#endif

#ifndef HWI_TEST
#define HWI_TEST
#endif

#ifndef TICK_TEST
#define TICK_TEST
#endif

#ifndef EXC_TEST
#define EXC_TEST
#endif

#ifndef MEMCHECK_TEST
#define MEMCHECK_TEST
#endif

#ifndef MACRO_BASE_FUNCTION_TEST
#define MACRO_BASE_FUNCTION_TEST
#endif

#ifndef MACRO_EXC_TEST
#define MACRO_EXC_TEST
#endif

#ifndef MACRO_PRESSURE_TEST
#define MACRO_PRESSURE_TEST
#endif

#ifdef MACRO_PREESSURE_TEST
#define DIVISOR 1
#else
#define DIVISOR 10
#endif

#ifndef TEST_CASE_RES_CHECK
#define TEST_CASE_RES_CHECK
#endif

#ifndef TST_IT
#define TST_IT
#endif

#define TEST_PT YES

#if (YES == OS_INCLUDE_DYNMEM)
#define TEST_BIG YES
#else
#define TEST_BIG NO
#endif

#define TASK_PRIO_TEST 25
#define TASK_PRIO_TEST_NORMAL 20

#define TASK_STACK_SIZE_TEST 0x400
#define TASK_LOOP_NUM 0x100000
#define QUEUE_LOOP_NUM 100
#define HWI_LOOP_NUM 100
#define SWTMR_LOOP_NUM 1000
#define TASK_NAME_NUM 10
#define TEST_TASK_RUNTIME 0x100000
#define TEST_SWTMR_RUNTIME 0x1000000
#define TEST_HWI_RUNTIME 0x100000
#define TEST_TASK_STACK_SIZE LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE
#define TEST_TASK_PRIORITY_LOW osPriorityBelowNormal  // tskIDLE_PRIORITY + 10
#define TEST_TASK_PRIORITY_NORMAL osPriorityNormal    // tskIDLE_PRIORITY + 11
#define TEST_TASK_PRIORITY_HIGH osPriorityAboveNormal // tskIDLE_PRIORITY + 12
#define TEST_TASK_PRIORITY_MAX osPriorityAboveNormal5

#define LOS_TASK_STATUS_DETACHED 0x0100

#define LOS_SYS_MS_PER_SECOND 1000    // Number of ms in one second.
#define LOS_SYS_US_PER_SECOND 1000000 // Number of us in one second.

#define LOS_MS_PER_TICK (LOS_SYS_MS_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND)

#ifdef __RISC_V__
#define OS_TSK_TEST_STACK_SIZE 0x9000
#else
#define OS_TSK_TEST_STACK_SIZE 0x1000
#endif
#define TASK_CMSIS_STACK_SIZE_TEST 0x600

#define OS_EVENT_TIMEOUT_MAX_VAL 0xFFFFFFFF

#if (LOSCFG_BASE_CORE_SWTMR == 1)
#define TASK_EXISTED_NUM 3
#else
#define TASK_EXISTED_NUM 2
#endif


#if (LOSCFG_BASE_CORE_SWTMR == 1)
#define QUEUE_EXISTED_NUM 1
#else
#define QUEUE_EXISTED_NUM 0
#endif

#define HWI_NUM_INT_NEG (-4)
#define HWI_NUM_INT0 0
#define HWI_NUM_INT1 1
#define HWI_NUM_INT2 2
#define HWI_NUM_INT3 3
#define HWI_NUM_INT4 4
#define HWI_NUM_INT5 5
#define HWI_NUM_INT6 6
#define HWI_NUM_INT7 7
#define HWI_NUM_INT11 11
#define HWI_NUM_INT12 12
#define HWI_NUM_INT13 13
#define HWI_NUM_INT14 14
#define HWI_NUM_INT15 15
#define HWI_NUM_INT16 16
#define HWI_NUM_INT17 17
#define HWI_NUM_INT18 18
#define HWI_NUM_INT21 21
#define HWI_NUM_INT22 22
#define HWI_NUM_INT23 23
#define HWI_NUM_INT24 24
#define HWI_NUM_INT25 25
#define HWI_NUM_INT26 26
#define HWI_NUM_INT27 27
#define HWI_NUM_INT28 28
#define HWI_NUM_INT30 30
#define HWI_NUM_INT31 31
#define HWI_NUM_INT32 32
#define HWI_NUM_INT33 33
#define HWI_NUM_INT45 45
#define HWI_NUM_INT50 50
#define HWI_NUM_INT55 55
#define HWI_NUM_INT60 60
#define HWI_NUM_INT58 58
#define HWI_NUM_INT59 59

#define HWI_NUM_INT75 75
#define HWI_NUM_INT71 71
#define HWI_NUM_INT72 72
#define HWI_NUM_INT73 73

#ifdef __RISC_V__
#define HWI_NUM_TEST 32
#define HWI_NUM_TEST0 33
#define HWI_NUM_TEST1 34
#define HWI_NUM_TEST2 35
#define HWI_NUM_TEST3 36
#else
#define HWI_NUM_TEST0 HWI_NUM_INT1
#define HWI_NUM_TEST HWI_NUM_INT7
#define HWI_NUM_TEST1 HWI_NUM_INT11
#define HWI_NUM_TEST2 HWI_NUM_INT12
#define HWI_NUM_TEST3 HWI_NUM_INT14
#endif

#define LOSCFG_BASE_IPC_QUEUE_CONFIG LOSCFG_BASE_IPC_QUEUE_LIMIT
#define LOSCFG_BASE_IPC_SEM_CONFIG LOSCFG_BASE_IPC_SEM_LIMIT
#define LOSCFG_BASE_CORE_SWTMR_CONFIG LOSCFG_BASE_CORE_SWTMR_LIMIT
#define LOSCFG_BASE_CORE_TSK_CONFIG LOSCFG_BASE_CORE_TSK_LIMIT
#define dprintf printf
#define IT_SEM_COUNT_MAX OS_SEM_COUNTING_MAX_COUNT

extern EVENT_CB_S g_pstEventCb01;
extern EVENT_CB_S g_pstEventCb02;
extern EVENT_CB_S g_pstEventCb03;


extern UINT32 TEST_TskDelete(UINT32 taskID);
extern UINT32 TestSemDelete(UINT32 semHandle);
extern UINT32 TestHwiDelete(UINT32 hwiNum);
extern VOID TEST_HwiDeleteAll(VOID);
extern VOID TestHwiTrigger(UINT32 hwiNum);
extern VOID TestHwiClear(UINT32 hwiNum);
#ifdef __RISC_V__
extern UINT64 LosCpuCycleGet(VOID);
#else
typedef struct tagHwiHandleForm {
    HWI_PROC_FUNC pfnHook;
    UINT32 uwPrioMask;
} HWI_HANDLE_FORM_S;
#endif
#define TEST_HwiCreate(ID, prio, mode, Func, arg) HalHwiCreate(ID, prio, mode, Func, arg)
#define uart_printf_func printf

extern VOID ItSuiteLosTask(void);
extern VOID ItSuiteLosQueue(void);
extern VOID ItSuiteLosMux(void);
extern VOID ItSuiteLosEvent(void);
extern VOID ItSuiteLosSem(void);
extern VOID ItSuiteLosSwtmr(void);
extern VOID ItSuiteLosHwi(void);
extern VOID ItSuiteLosMem(void);
extern VOID ItSuite_Los_FatFs(void);

extern VOID ItSuite_Cmsis_Lostask(void);
extern VOID ItSuite_Cmsis_Lostask_add(void);
extern VOID ItSuite_CMSIS_Losmsg(void);
extern VOID ItSuite_CMSIS_Losmsg_add(void);
extern VOID ItSuite_CMSIS_Signal(void);
extern VOID ItSuite_CMSIS_Signal_add(void);
extern VOID ItSuite_Cmsis_LosMemBox(void);
extern VOID ItSuite_Cmsis_LosMutex(void);
extern VOID ItSuite_Cmsis_LosMutex_add(void);
extern VOID ItSuite_Cmsis_LosRobin(void);
extern VOID ItSuite_Cmsis_LosRobin_add(void);
extern VOID ItSuite_CMSIS_Losmail(void);
extern VOID ItSuite_CMSIS_Losmail_add(void);
extern VOID ItSuite_Cmsis_LosSem(void);
extern VOID ItSuite_Cmsis_LosSem_add(void);
extern VOID ItSuite_CMSIS_Swtmr(void);
extern VOID ItSuite_CMSIS_Swtmr_add(void);
extern VOID ItSuite_Cmsis_Hwi_M3(VOID);
extern VOID ItSuite_Cmsis_Hwi_M3_add(void);
extern VOID ItSuite_CMSIS_Wait(void);
extern VOID ItSuite_CMSIS_Wait_add(VOID);

#define OS_MS_TO_TICK(ms) (((ms) * (UINT64)LOSCFG_BASE_CORE_TICK_PER_SECOND) / 1000)

extern UINT32 g_usSemID;
extern UINT32 g_auwTestTaskID[LOSCFG_BASE_CORE_TSK_LIMIT];

extern LosQueueCB *g_pstAllQueue;
extern UINT32 g_taskMaxNum;

extern LITE_OS_SEC_BSS_INIT LOS_DL_LIST g_stUnusedSemList;

extern LosTask g_losTask;
extern VOID LOS_Schedule(VOID);
extern LosTaskCB *g_taskCBArray;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _OSTEST_H */
