/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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

#include "los_exc_info.h"
#include "securec.h"
#include "los_debug.h"
#include "los_interrupt.h"
#include "los_task.h"
#include "los_exchook.h"
#include "los_membox.h"
#include "los_memory.h"


#if (LOSCFG_PLATFORM_EXC == 1)
#define EXC_MSG_ARRAY_INIT_VALUE   0xff
#define EXC_INT_STATUS_LEN        (OS_NVIC_INT_ENABLE_SIZE + OS_NVIC_INT_PEND_SIZE + \
                                   OS_NVIC_INT_ACT_SIZE + OS_NVIC_INT_PRI_SIZE + \
                                   OS_NVIC_EXCPRI_SIZE + OS_NVIC_SHCSR_SIZE + \
                                   OS_NVIC_INT_CTRL_SIZE)

UINT32 g_excQueueMaxNum;
UINT32 g_excMemMaxNum;
VOID *g_excContent = NULL;
UINT32 g_excArraySize = MAX_EXC_MEM_SIZE;
UINT8 g_excMsgArray[MAX_EXC_MEM_SIZE];
STATIC ExcInfoArray g_excArray[OS_EXC_TYPE_MAX];

STATIC UINT32 OsExcSaveIntStatus(UINT32 type, VOID *arg)
{
    UINT32 ret;
    UINTPTR excContentEnd = (UINTPTR)MAX_INT_INFO_SIZE + (UINTPTR)g_excContent;

    (VOID)arg;

    *((UINT32 *)g_excContent) = type;
    g_excContent = (UINT8 *)g_excContent + sizeof(UINT32);

    *((UINT32 *)g_excContent) = EXC_INT_STATUS_LEN;
    g_excContent = (UINT8 *)g_excContent + sizeof(UINT32);
    /* save IRQ ENABLE reg group */
    ret = memcpy_s(g_excContent, excContentEnd - (UINTPTR)g_excContent,
                   (const VOID *)OS_NVIC_SETENA_BASE, OS_NVIC_INT_ENABLE_SIZE);
    if (ret != EOK) {
        return LOS_NOK;
    }
    g_excContent = (UINT8 *)g_excContent + OS_NVIC_INT_ENABLE_SIZE;

    /* save IRQ PEND reg group */
    ret = memcpy_s(g_excContent, excContentEnd - (UINTPTR)g_excContent,
                   (const VOID *)OS_NVIC_SETPEND_BASE, OS_NVIC_INT_PEND_SIZE);
    if (ret != EOK) {
        return LOS_NOK;
    }
    g_excContent = (UINT8 *)g_excContent + OS_NVIC_INT_PEND_SIZE;

    /* save IRQ ACTIVE reg group */
    ret = memcpy_s(g_excContent, excContentEnd - (UINTPTR)g_excContent,
                   (const VOID *)OS_NVIC_INT_ACT_BASE, OS_NVIC_INT_ACT_SIZE);
    if (ret != EOK) {
        return LOS_NOK;
    }
    g_excContent = (UINT8 *)g_excContent + OS_NVIC_INT_ACT_SIZE;

    /* save IRQ Priority reg group */
    ret = memcpy_s(g_excContent, excContentEnd - (UINTPTR)g_excContent,
                   (const VOID *)OS_NVIC_PRI_BASE, OS_NVIC_INT_PRI_SIZE);
    g_excContent = (UINT8 *)g_excContent + OS_NVIC_INT_PRI_SIZE;

    /* save Exception Priority reg group */
    ret = memcpy_s(g_excContent, excContentEnd - (UINTPTR)g_excContent,
                   (const VOID *)OS_NVIC_EXCPRI_BASE, OS_NVIC_EXCPRI_SIZE);
    if (ret != EOK) {
        return LOS_NOK;
    }
    g_excContent = (UINT8 *)g_excContent + OS_NVIC_EXCPRI_SIZE;

    /* save IRQ Handler & SHCSR */
    ret = memcpy_s(g_excContent, excContentEnd - (UINTPTR)g_excContent,
                   (const VOID *)OS_NVIC_SHCSR, OS_NVIC_SHCSR_SIZE);
    if (ret != EOK) {
        return LOS_NOK;
    }
    g_excContent = (UINT8 *)g_excContent + OS_NVIC_SHCSR_SIZE;

    /* save IRQ Control & ICSR */
    ret = memcpy_s(g_excContent, excContentEnd - (UINTPTR)g_excContent,
                   (const VOID *)OS_NVIC_INT_CTRL, OS_NVIC_INT_CTRL_SIZE);
    if (ret != EOK) {
        return LOS_NOK;
    }
    g_excContent = (UINT8 *)g_excContent + OS_NVIC_INT_CTRL_SIZE;

    return LOS_OK;
}

STATIC UINT32 OsExcMemMsgGet(UINT32 type, VOID *arg)
{
    UINT32 count;
    UINT32 memNum = *(UINT32 *)arg;
    UINTPTR memMsgEnd = (UINTPTR)g_excContent + MAX_MEM_INFO_SIZE;

    *((UINT32 *)g_excContent) = type;
    g_excContent = (UINT8 *)g_excContent + sizeof(UINT32);

    *((UINT32 *)g_excContent) = sizeof(MemInfoCB) * memNum;
    g_excContent = (UINT8 *)g_excContent + sizeof(UINT32);

    count = OsMemboxExcInfoGet(memNum, g_excContent);
    if (count < memNum) {
        g_excContent = (UINT8 *)g_excContent + sizeof(MemInfoCB) * count;
        (VOID)OsMemExcInfoGet(memNum - count, g_excContent);
    }
    g_excContent = (VOID *)memMsgEnd;

    return LOS_OK;
}

STATIC UINT32 OsExcContentGet(UINT32 type, VOID *arg)
{
    UINTPTR excContentEnd = MAX_EXC_MEM_SIZE + (UINTPTR)g_excMsgArray;
    errno_t ret;

    (VOID)arg;

    /* save exception info */
    *((UINT32 *)g_excContent) = type;
    g_excContent = (UINT8 *)g_excContent + sizeof(UINT32);
    *((UINT32 *)g_excContent) = sizeof(ExcInfo) + sizeof(EXC_CONTEXT_S);

    g_excContent = (UINT8 *)g_excContent + sizeof(UINT32);
    ret = memcpy_s((VOID *)g_excContent, excContentEnd - (UINTPTR)g_excContent,
                   (VOID *)&g_excInfo, sizeof(ExcInfo));
    if (ret != EOK) {
        return LOS_NOK;
    }
    g_excContent = (UINT8 *)g_excContent + sizeof(ExcInfo);

    ret = memcpy_s((VOID *)g_excContent, excContentEnd - (UINTPTR)g_excContent,
                   g_excInfo.context, sizeof(EXC_CONTEXT_S));
    if (ret != EOK) {
        return LOS_NOK;
    }
    g_excContent = (UINT8 *)g_excContent + sizeof(EXC_CONTEXT_S);

    return LOS_OK;
}

STATIC UINT32 OsExcHookFunc(UINT32 type, UINT32 index, UINT32 loop, UINT32 len, EXC_INFO_SAVE_CALLBACK func)
{
    UINT32 i;

    *((UINT32 *)g_excContent) = (UINT32)type;
    g_excContent = (UINT8 *)g_excContent + sizeof(UINT32);

    *((UINT32 *)g_excContent) = len * loop;
    g_excContent = (UINT8 *)g_excContent + sizeof(UINT32);

    for (i = index; i < loop; i++) {
        (VOID)func(i, g_excContent);
        g_excContent = (UINT8 *)g_excContent + len;
    }

    return LOS_OK;
}

#if (LOSCFG_BASE_IPC_QUEUE == 1)
STATIC UINT32 OsExcQueueMsgGet(UINT32 type, VOID *arg)
{
    UINT32 len = sizeof(QUEUE_INFO_S);
    UINT32 loop = *(UINT32 *)arg;

    return OsExcHookFunc(type, 1, loop, len, (EXC_INFO_SAVE_CALLBACK)LOS_QueueInfoGet);
}
#endif

STATIC UINT32 OsExcTaskMsgGet(UINT32 type, VOID *arg)
{
    UINT32 len = sizeof(TSK_INFO_S);
    UINT32 loop = *(UINT32 *)arg;

    return OsExcHookFunc(type, 0, loop, len, (EXC_INFO_SAVE_CALLBACK)LOS_TaskInfoGet);
}

#if (LOSCFG_BASE_CORE_EXC_TSK_SWITCH == 1)
STATIC UINT32 OsExcTskSwitchMsgGet(UINT32 type, VOID *arg)
{
    UINT32 len;
    UINT32 loop;
    UINT32 taskSwitchCount;
    TaskSwitchInfo *taskSwitchInfo = NULL;
    UINT32 i;

    /* not necessary, just for macro int library */
    taskSwitchInfo = (TaskSwitchInfo *)arg;
    taskSwitchCount = taskSwitchInfo->cntInfo.maxCnt;
    len = sizeof(UINT32) + (sizeof(CHAR) * LOS_TASK_NAMELEN);
    if (taskSwitchInfo->cntInfo.isFull) {
        i = taskSwitchInfo->idx;
        loop = i + taskSwitchCount;
    } else {
        i = 0;
        loop = taskSwitchInfo->idx;
    }

    return OsExcHookFunc(type, i, loop, len, (EXC_INFO_SAVE_CALLBACK)LOS_TaskSwitchInfoGet);
}
#endif

VOID OsExcRegister(ExcInfoType type, EXC_INFO_SAVE_CALLBACK func, VOID *arg)
{
    ExcInfoArray *excInfo = NULL;
    if ((type >= OS_EXC_TYPE_MAX) || (func == NULL)) {
        PRINT_ERR("HalExcRegister ERROR!\n");
        return;
    }
    excInfo = &(g_excArray[type]);
    if (excInfo->valid == TRUE) {
        return;
    }

    excInfo->type = type;
    excInfo->fnExcInfoCb = func;
    excInfo->arg = arg;
    excInfo->valid = TRUE;
}

STATIC VOID OsExcMsgDump(VOID)
{
    UINT32 index;

    /* Ignore the return code when matching CSEC rule 6.6(4). */
    (VOID)memset_s(g_excMsgArray, g_excArraySize, EXC_MSG_ARRAY_INIT_VALUE, g_excArraySize);

    *((UINT32 *)g_excContent) = MAX_EXC_MEM_SIZE;  /* The total length of exception information. */
    g_excContent = (UINT8 *)g_excContent + sizeof(UINT32);

    for (index = 0; index < OS_EXC_TYPE_MAX; index++) {
        if (!g_excArray[index].valid) {
            continue;
        }
        g_excArray[index].fnExcInfoCb(g_excArray[index].type, g_excArray[index].arg);
    }

    *((UINT32 *)g_excContent) = OS_EXC_TYPE_MAX;
    g_excContent = (UINT8 *)g_excContent + sizeof(UINT32);
    return;
}

VOID OsExcMsgDumpInit(VOID)
{
    g_excQueueMaxNum = LOSCFG_BASE_IPC_QUEUE_LIMIT;
    g_excMemMaxNum = OS_SYS_MEM_NUM;
    g_excContent = (VOID *)g_excMsgArray;

    OsExcRegister(OS_EXC_TYPE_CONTEXT, OsExcContentGet, NULL);
    OsExcRegister(OS_EXC_TYPE_TSK, OsExcTaskMsgGet, &g_taskMaxNum);
#if (LOSCFG_BASE_IPC_QUEUE == 1)
    OsExcRegister(OS_EXC_TYPE_QUE, OsExcQueueMsgGet, &g_excQueueMaxNum);
#endif
    OsExcRegister(OS_EXC_TYPE_NVIC, OsExcSaveIntStatus, NULL);
#if (LOSCFG_BASE_CORE_EXC_TSK_SWITCH == 1)
    OsExcRegister(OS_EXC_TYPE_TSK_SWITCH, OsExcTskSwitchMsgGet, &g_taskSwitchInfo);
#endif
    OsExcRegister(OS_EXC_TYPE_MEM, OsExcMemMsgGet, &g_excMemMaxNum);

    (VOID)LOS_RegExcHook(EXC_INTERRUPT, (ExcHookFn)OsExcMsgDump);
}
#endif


