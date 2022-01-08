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

#include "los_pm.h"
#include "securec.h"
#include "los_sched.h"
#include "los_timer.h"
#include "los_memory.h"

#if (LOSCFG_KERNEL_PM == 1)
#define OS_PM_NODE_FREE 0x80000000U
#define OS_PM_LOCK_MAX  0xFFFFU

typedef UINT32 (*Suspend)(VOID);

#if (LOSCFG_KERNEL_PM_DEBUG == 1)
typedef struct {
    CHAR         *name;
    UINT32       count;
    LOS_DL_LIST  list;
} OsPmLockCB;
#endif

typedef struct {
    LOS_SysSleepEnum  pmMode;
    LOS_SysSleepEnum  sysMode;
    UINT16            lock;
    BOOL              isWake;
    LosPmDevice       *device;
    LosPmSysctrl      *sysctrl;
    LosPmTickTimer    *tickTimer;
#if (LOSCFG_BASE_CORE_TICK_WTIMER == 0)
    UINT64            enterSleepTime;
#endif
#if (LOSCFG_KERNEL_PM_DEBUG == 1)
    LOS_DL_LIST       lockList;
#endif
} LosPmCB;

STATIC LosPmCB g_pmCB;
STATIC LosPmSysctrl *g_sysctrl = NULL;
STATIC UINT64 g_pmSleepTime;

#if (LOSCFG_BASE_CORE_TICK_WTIMER == 0)
STATIC VOID OsPmTickTimerStart(LosPmCB *pm)
{
    UINT32 intSave;
    UINT64 currTime, sleepTime, realSleepTime;
    LosPmTickTimer *tickTimer = pm->tickTimer;

    intSave = LOS_IntLock();
    /* Restore the main CPU frequency */
    sleepTime = tickTimer->timerCycleGet();
    tickTimer->timerStop();

    realSleepTime = OS_SYS_CYCLE_TO_NS(sleepTime, tickTimer->freq);
    realSleepTime = OS_SYS_NS_TO_CYCLE(realSleepTime, OS_SYS_CLOCK);
    currTime = pm->enterSleepTime + realSleepTime;
    pm->enterSleepTime = 0;

    OsSchedTimerBaseReset(currTime);
    OsSchedUpdateExpireTime(currTime, FALSE);
    tickTimer->tickUnlock();
    LOS_IntRestore(intSave);
    return;
}

STATIC VOID OsPmTickTimerStop(LosPmCB *pm)
{
    UINT64 sleepCycle;
    UINT64 realSleepTime = g_pmSleepTime;
    LosPmTickTimer *tickTimer = pm->tickTimer;

    if (realSleepTime == 0) {
        return;
    }

    sleepCycle = OS_SYS_CYCLE_TO_NS(realSleepTime, OS_SYS_CLOCK);
    sleepCycle = OS_SYS_NS_TO_CYCLE(sleepCycle, tickTimer->freq);

    /* The main CPU reduces the frequency */
    pm->enterSleepTime = OsGetCurrSchedTimeCycle();
    tickTimer->tickLock();
    tickTimer->timerStart(sleepCycle);
    return;
}
#endif

STATIC VOID OsPmTickTimerResume(LosPmCB *pm)
{
    if ((pm->sysMode == LOS_SYS_DEEP_SLEEP) && (pm->tickTimer->tickUnlock != NULL)) {
        pm->tickTimer->tickUnlock();
    } else {
#if (LOSCFG_BASE_CORE_TICK_WTIMER == 0)
        /* Sys tick timer is restored from low power mode */
        if (pm->enterSleepTime != 0) {
            OsPmTickTimerStart(pm);
        }
#endif
    }
}

STATIC VOID OsPmCpuResume(LosPmCB *pm)
{
    if ((pm->sysMode == LOS_SYS_NORMAL_SLEEP) && (pm->sysctrl->normalResume != NULL)) {
        pm->sysctrl->normalResume();
    } else if ((pm->sysMode == LOS_SYS_LIGHT_SLEEP) && (pm->sysctrl->lightResume != NULL)) {
        pm->sysctrl->lightResume();
    } else if ((pm->sysMode == LOS_SYS_DEEP_SLEEP) && (pm->sysctrl->deepResume != NULL)) {
        pm->sysctrl->deepResume();
    }
}

STATIC Suspend OsPmCpuSuspend(LosPmCB *pm)
{
    Suspend sysSuspend = NULL;

    /* cpu enter low power mode */
    LOS_ASSERT(pm->sysctrl != NULL);

    if (pm->sysMode == LOS_SYS_NORMAL_SLEEP) {
        sysSuspend = pm->sysctrl->normalSuspend;
    } else if (pm->sysMode == LOS_SYS_LIGHT_SLEEP) {
        sysSuspend = pm->sysctrl->lightSuspend;
    } else if (pm->sysMode == LOS_SYS_DEEP_SLEEP) {
        sysSuspend = pm->sysctrl->deepSuspend;
    } else {
        sysSuspend = pm->sysctrl->shutdownSuspend;
    }

    LOS_ASSERT(sysSuspend != NULL);

    return sysSuspend;
}

STATIC VOID OsPmTickTimerSuspend(LosPmCB *pm)
{
    if (((pm->sysMode == LOS_SYS_DEEP_SLEEP) || (pm->sysMode == LOS_SYS_SHUTDOWN)) &&
        (pm->tickTimer->tickLock != NULL)) {
        pm->tickTimer->tickLock();
    } else {
#if (LOSCFG_BASE_CORE_TICK_WTIMER == 0)
        /* Sys tick timer enter low power mode */
        if (pm->tickTimer == NULL) {
            return;
        }

        if ((pm->tickTimer->timerStart != NULL) &&
            (pm->tickTimer->timerStop != NULL) &&
            (pm->tickTimer->timerCycleGet == NULL) &&
            (pm->tickTimer->freq != 0)) {
            OsPmTickTimerStop(pm);
        }
#endif
    }
}

STATIC VOID OsPmEnter(BOOL isIdle)
{
    UINT32 ret;
    UINT32 intSave;
    Suspend sysSuspend = NULL;
    LosPmCB *pm = &g_pmCB;
    BOOL isTaskLock = FALSE;

    intSave = LOS_IntLock();
    pm->sysMode = pm->pmMode;
    if (isIdle) {
        if ((pm->sysMode != LOS_SYS_NORMAL_SLEEP) && (pm->sysMode != LOS_SYS_LIGHT_SLEEP)) {
            pm->sysMode = LOS_SYS_NORMAL_SLEEP;
        }
    } else {
        if ((pm->sysMode != LOS_SYS_DEEP_SLEEP) && (pm->sysMode != LOS_SYS_SHUTDOWN)) {
            LOS_IntRestore(intSave);
            return;
        }
    }

    if ((pm->sysMode == LOS_SYS_NORMAL_SLEEP) || (pm->sysMode == LOS_SYS_LIGHT_SLEEP)) {
        if (pm->lock > 0) {
            pm->sysMode = LOS_SYS_NORMAL_SLEEP;
        }
    } else if (pm->lock > 0) {
        LOS_IntRestore(intSave);
        return;
    }

    if (pm->sysMode != LOS_SYS_NORMAL_SLEEP) {
        pm->isWake = FALSE;
        LOS_TaskLock();
        isTaskLock = TRUE;

        ret = pm->device->suspend((UINT32)pm->sysMode);
        if (ret != LOS_OK) {
            goto EXIT;
        }
    }

    OsPmTickTimerSuspend(pm);

    sysSuspend = OsPmCpuSuspend(pm);
    LOS_IntRestore(intSave);

    if (!isTaskLock || (isTaskLock && !pm->isWake)) {
        (VOID)sysSuspend();
    }

    intSave = LOS_IntLock();

    OsPmCpuResume(pm);

    OsPmTickTimerResume(pm);

    if (pm->sysMode != LOS_SYS_NORMAL_SLEEP) {
        pm->device->resume((UINT32)pm->sysMode);
    }

    if (pm->pmMode == LOS_SYS_DEEP_SLEEP) {
        pm->pmMode = LOS_SYS_NORMAL_SLEEP;
    }

EXIT:
    LOS_IntRestore(intSave);

    if (isTaskLock) {
        LOS_TaskUnlock();
    }
    return;
}

STATIC VOID OsPmTask(VOID)
{
    OsPmEnter(FALSE);
}

STATIC UINT32 OsPmDeviceRegister(LosPmCB *pm, LosPmDevice *device)
{
    UINT32 intSave;

    if ((device->suspend == NULL) || (device->resume == NULL)) {
        return LOS_ERRNO_PM_INVALID_PARAM;
    }

    intSave = LOS_IntLock();
    pm->device = device;
    LOS_IntRestore(intSave);

    return LOS_OK;
}

STATIC UINT32 OsPmTickTimerRegister(LosPmCB *pm, LosPmTickTimer *tickTimer)
{
    UINT32 intSave;

    intSave = LOS_IntLock();
#if (LOSCFG_BASE_CORE_TICK_WTIMER == 0)
    pm->enterSleepTime = 0;
#endif
    pm->tickTimer = tickTimer;
    LOS_IntRestore(intSave);
    return LOS_OK;
}

STATIC UINT32 OsPmSysctrlRegister(LosPmCB *pm, LosPmSysctrl *sysctrl)
{
    UINT32 intSave;

    if (sysctrl->normalSuspend == NULL) {
        return LOS_ERRNO_PM_INVALID_PARAM;
    }

    intSave = LOS_IntLock();
    pm->sysctrl = sysctrl;
    LOS_IntRestore(intSave);

    return LOS_OK;
}

UINT32 LOS_PmRegister(LOS_PmNodeType type, VOID *node)
{
    LosPmCB *pm = &g_pmCB;

    if (node == NULL) {
        return LOS_ERRNO_PM_INVALID_PARAM;
    }

    switch (type) {
        case LOS_PM_TYPE_DEVICE:
            return OsPmDeviceRegister(pm, (LosPmDevice *)node);
        case LOS_PM_TYPE_TICK_TIMER:
            return OsPmTickTimerRegister(pm, (LosPmTickTimer *)node);
        case LOS_PM_TYPE_SYSCTRL:
            return OsPmSysctrlRegister(pm, (LosPmSysctrl *)node);
        default:
            break;
    }

    return LOS_ERRNO_PM_INVALID_TYPE;
}

STATIC UINT32 OsPmDeviceUnregister(LosPmCB *pm, LosPmDevice *device)
{
    UINT32 intSave;

    intSave = LOS_IntLock();
    if (pm->device == device) {
        pm->device = NULL;
        pm->pmMode = LOS_SYS_NORMAL_SLEEP;
        LOS_IntRestore(intSave);
        return LOS_OK;
    }

    LOS_IntRestore(intSave);
    return LOS_ERRNO_PM_INVALID_NODE;
}

STATIC UINT32 OsPmTickTimerUnregister(LosPmCB *pm, LosPmTickTimer *tickTimer)
{
    UINT32 intSave;

    intSave = LOS_IntLock();
    if (pm->tickTimer == tickTimer) {
        pm->tickTimer = NULL;
        if ((pm->pmMode != LOS_SYS_NORMAL_SLEEP) && (pm->pmMode != LOS_SYS_LIGHT_SLEEP)) {
            pm->pmMode = LOS_SYS_NORMAL_SLEEP;
        }
        LOS_IntRestore(intSave);
        return LOS_OK;
    }

    LOS_IntRestore(intSave);
    return LOS_ERRNO_PM_INVALID_NODE;
}

STATIC UINT32 OsPmSysctrlUnregister(LosPmCB *pm, LosPmSysctrl *sysctrl)
{
    UINT32 intSave;
    VOID *freeNode = NULL;

    intSave = LOS_IntLock();
    if (pm->sysctrl == sysctrl) {
        if (pm->sysctrl == g_sysctrl) {
            freeNode = (VOID *)pm->sysctrl;
            g_sysctrl = NULL;
        }

        pm->sysctrl = NULL;
        LOS_IntRestore(intSave);

        if (freeNode != NULL) {
            (VOID)LOS_MemFree((VOID *)OS_SYS_MEM_ADDR, freeNode);
        }
        return LOS_OK;
    }

    LOS_IntRestore(intSave);
    return LOS_ERRNO_PM_INVALID_NODE;
}

UINT32 LOS_PmUnregister(LOS_PmNodeType type, VOID *node)
{
    LosPmCB *pm = &g_pmCB;

    if (node == NULL) {
        return LOS_ERRNO_PM_INVALID_PARAM;
    }

    switch (type) {
        case LOS_PM_TYPE_DEVICE:
            return OsPmDeviceUnregister(pm, (LosPmDevice *)node);
        case LOS_PM_TYPE_TICK_TIMER:
            return OsPmTickTimerUnregister(pm, (LosPmTickTimer *)node);
        case LOS_PM_TYPE_SYSCTRL:
            return OsPmSysctrlUnregister(pm, (LosPmSysctrl *)node);
        default:
            break;
    }

    return LOS_ERRNO_PM_INVALID_TYPE;
}

VOID LOS_PmWakeSet(VOID)
{
    UINT32 intSave;
    LosPmCB *pm = &g_pmCB;

    intSave = LOS_IntLock();
    pm->isWake = TRUE;
    LOS_IntRestore(intSave);
    return;
}

LOS_SysSleepEnum LOS_PmModeGet(VOID)
{
    LOS_SysSleepEnum mode;
    LosPmCB *pm = &g_pmCB;

    UINT32 intSave = LOS_IntLock();
    mode = pm->pmMode;
    LOS_IntRestore(intSave);

    return mode;
}

UINT32 LOS_PmModeSet(LOS_SysSleepEnum mode)
{
    UINT32 intSave;
    UINT32 taskID;
    UINT32 ret;
    LosPmCB *pm = &g_pmCB;
    INT32 sleepMode = (INT32)mode;
    TSK_INIT_PARAM_S taskInitParam = { 0 };

    if ((sleepMode < 0) || (sleepMode > LOS_SYS_SHUTDOWN)) {
        return LOS_ERRNO_PM_INVALID_MODE;
    }

    intSave = LOS_IntLock();
    if ((mode != LOS_SYS_NORMAL_SLEEP) && (pm->device == NULL)) {
        LOS_IntRestore(intSave);
        return LOS_ERRNO_PM_DEVICE_NULL;
    }

    if ((mode == LOS_SYS_LIGHT_SLEEP) && (pm->sysctrl->lightSuspend == NULL)) {
        LOS_IntRestore(intSave);
        return LOS_ERRNO_PM_HANDLER_NULL;
    }

    if ((mode == LOS_SYS_DEEP_SLEEP) && (pm->sysctrl->deepSuspend == NULL)) {
        LOS_IntRestore(intSave);
        return LOS_ERRNO_PM_HANDLER_NULL;
    }

    if ((mode == LOS_SYS_SHUTDOWN) && (pm->sysctrl->shutdownSuspend == NULL)) {
        LOS_IntRestore(intSave);
        return LOS_ERRNO_PM_HANDLER_NULL;
    }

    if ((mode == LOS_SYS_DEEP_SLEEP) || (mode == LOS_SYS_SHUTDOWN)) {
        if ((pm->tickTimer == NULL) ||
            (pm->tickTimer->tickLock == NULL) ||
            (pm->tickTimer->tickUnlock == NULL)) {
            LOS_IntRestore(intSave);
            return LOS_ERRNO_PM_TICK_TIMER_NULL;
        }
    }

    pm->pmMode = mode;
    LOS_IntRestore(intSave);

    if ((mode == LOS_SYS_DEEP_SLEEP) || (mode == LOS_SYS_SHUTDOWN)) {
        taskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)OsPmTask;
        taskInitParam.uwStackSize = LOSCFG_KERNEL_PM_TASK_STACKSIZE;
        taskInitParam.pcName = "pm";
        taskInitParam.usTaskPrio = LOSCFG_KERNEL_PM_TASK_PTIORITY;
        ret = LOS_TaskCreate(&taskID, &taskInitParam);
        if (ret != LOS_OK) {
            return ret;
        }
    }

    return LOS_OK;
}

#if (LOSCFG_KERNEL_PM_DEBUG == 1)
VOID LOS_PmLockInfoShow(VOID)
{
    UINT32 intSave;
    LosPmCB *pm = &g_pmCB;
    OsPmLockCB *lock = NULL;
    LOS_DL_LIST *head = &pm->lockList;
    LOS_DL_LIST *list = head->pstNext;

    PRINTK("Name                          Count\n\r");

    intSave = LOS_IntLock();
    while (list != head) {
        lock = LOS_DL_LIST_ENTRY(list, OsPmLockCB, list);
        PRINTK("%-30s%5u\n\r", lock->name, lock->count);
        list = list->pstNext;
    }
    LOS_IntRestore(intSave);

    return;
}
#endif

UINT32 LOS_PmLockRequest(const CHAR *name)
{
    UINT32 intSave;
    UINT32 ret = LOS_ERRNO_PM_NOT_LOCK;
    LosPmCB *pm = &g_pmCB;
#if (LOSCFG_KERNEL_PM_DEBUG == 1)
    OsPmLockCB *listNode = NULL;
    OsPmLockCB *lock = NULL;
    LOS_DL_LIST *head = &pm->lockList;
    LOS_DL_LIST *list = head->pstNext;

    if (name == NULL) {
        return LOS_ERRNO_PM_INVALID_PARAM;
    }
#endif

    intSave = LOS_IntLock();
#if (LOSCFG_KERNEL_PM_DEBUG == 1)
    while (list != head) {
        listNode = LOS_DL_LIST_ENTRY(list, OsPmLockCB, list);
        if (strcmp(name, listNode->name) == 0) {
            lock = listNode;
            break;
        }

        list = list->pstNext;
    }

    if (lock == NULL) {
        lock = LOS_MemAlloc((VOID *)OS_SYS_MEM_ADDR, sizeof(OsPmLockCB));
        if (lock == NULL) {
            LOS_IntRestore(intSave);
            return LOS_NOK;
        }
        lock->name = (CHAR *)name;
        lock->count = 1;
        LOS_ListTailInsert(head, &lock->list);
    } else if (lock->count < OS_PM_LOCK_MAX) {
        lock->count++;
    }
#endif

    if (pm->lock < OS_PM_LOCK_MAX) {
        pm->lock++;
        ret = LOS_OK;
    }

    LOS_IntRestore(intSave);
    return ret;
}

UINT32 LOS_PmLockRelease(const CHAR *name)
{
    UINT32 intSave;
    UINT32 ret = LOS_ERRNO_PM_NOT_LOCK;
    LosPmCB *pm = &g_pmCB;
#if (LOSCFG_KERNEL_PM_DEBUG == 1)
    OsPmLockCB *lock = NULL;
    OsPmLockCB *listNode = NULL;
    LOS_DL_LIST *head = &pm->lockList;
    LOS_DL_LIST *list = head->pstNext;
    VOID *lockFree = NULL;

    if (name == NULL) {
        return LOS_ERRNO_PM_INVALID_PARAM;
    }
#endif

    intSave = LOS_IntLock();
#if (LOSCFG_KERNEL_PM_DEBUG == 1)
    while (list != head) {
        listNode = LOS_DL_LIST_ENTRY(list, OsPmLockCB, list);
        if (strcmp(name, listNode->name) == 0) {
            lock = listNode;
            break;
        }

        list = list->pstNext;
    }

    if (lock == NULL) {
        LOS_IntRestore(intSave);
        return LOS_ERRNO_PM_NOT_LOCK;
    } else if (lock->count > 0) {
        lock->count--;
        if (lock->count == 0) {
            LOS_ListDelete(&lock->list);
            lockFree = lock;
        }
    }
#endif

    if (pm->lock > 0) {
        pm->lock--;
        ret = LOS_OK;
    }

    LOS_IntRestore(intSave);
#if (LOSCFG_KERNEL_PM_DEBUG == 1)
    (VOID)LOS_MemFree((VOID *)OS_SYS_MEM_ADDR, lockFree);
#endif
    return ret;
}

STATIC VOID OsPmSleepTimeSet(UINT64 sleepTime)
{
    g_pmSleepTime = sleepTime;
}

UINT32 OsPmInit(VOID)
{
    UINT32 ret;
    LosPmCB *pm = &g_pmCB;

    (VOID)memset_s(pm, sizeof(LosPmCB), 0, sizeof(LosPmCB));

    pm->pmMode = LOS_SYS_NORMAL_SLEEP;
#if (LOSCFG_KERNEL_PM_DEBUG == 1)
    LOS_ListInit(&pm->lockList);
#endif

    ret = OsSchedRealSleepTimeSet(OsPmSleepTimeSet);
    if (ret != LOS_OK) {
        return ret;
    }

    ret = OsPmEnterHandlerSet(OsPmEnter);
    if (ret != LOS_OK) {
        return ret;
    }

    g_sysctrl = (LosPmSysctrl *)LOS_MemAlloc((VOID *)OS_SYS_MEM_ADDR, sizeof(LosPmSysctrl));
    if (g_sysctrl == NULL) {
        return LOS_NOK;
    }

    (VOID)memset_s(g_sysctrl, sizeof(LosPmSysctrl), 0, sizeof(LosPmSysctrl));
    g_sysctrl->normalSuspend = HalEnterSleep;

    ret = LOS_PmRegister(LOS_PM_TYPE_SYSCTRL, (VOID *)g_sysctrl);
    if (ret != LOS_OK) {
        (VOID)LOS_MemFree((VOID *)OS_SYS_MEM_ADDR, g_sysctrl);
        g_sysctrl = NULL;
    }

    return ret;
}
#endif
