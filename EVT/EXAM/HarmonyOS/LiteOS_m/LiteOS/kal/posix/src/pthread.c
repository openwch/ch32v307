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

#include "pthread.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <securec.h>
#include "los_config.h"
#include "los_task.h"

#define PTHREAD_NAMELEN 16

typedef struct {
    void *(*startRoutine)(void *);
    void *param;
    char name[PTHREAD_NAMELEN];
}PthreadData;

static void *PthreadEntry(UINT32 param)
{
    PthreadData *pthreadData = (PthreadData *)(UINTPTR)param;
    void *(*startRoutine)(void *) = pthreadData->startRoutine;
    void *ret = startRoutine(pthreadData->param);
    free(pthreadData);
    return ret;
}

static inline int IsPthread(pthread_t thread)
{
    return ((UINT32)thread <= LOSCFG_BASE_CORE_TSK_LIMIT) &&
           (OS_TCB_FROM_TID((UINT32)thread)->taskEntry == PthreadEntry);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*startRoutine)(void *), void *arg)
{
    TSK_INIT_PARAM_S taskInitParam = {0};
    PthreadData *pthreadData = NULL;
    UINT32 taskID;

    if ((thread == NULL) || (startRoutine == NULL)) {
        return EINVAL;
    }

    taskInitParam.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    taskInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    if (attr) {
        if (attr->detachstate == PTHREAD_CREATE_DETACHED) {
            return ENOTSUP;
        }
        if (attr->stackaddr_set) {
            return ENOTSUP;
        }
        if (attr->stacksize_set) {
            taskInitParam.uwStackSize = attr->stacksize;
        }
        taskInitParam.usTaskPrio = (UINT16)attr->schedparam.sched_priority;
    }

    pthreadData = (PthreadData *)malloc(sizeof(PthreadData));
    if (pthreadData == NULL) {
        return ENOMEM;
    }

    pthreadData->startRoutine  = startRoutine;
    pthreadData->param         = arg;
    taskInitParam.pcName       = pthreadData->name;
    taskInitParam.pfnTaskEntry = PthreadEntry;
    taskInitParam.uwArg   = (UINT32)(UINTPTR)pthreadData;

    if (LOS_TaskCreateOnly(&taskID, &taskInitParam) != LOS_OK) {
        free(pthreadData);
        return EINVAL;
    }

    /* set pthread default name */
    (void)sprintf_s(taskInitParam.pcName, PTHREAD_NAMELEN, "pthread%u", taskID);

    (void)LOS_TaskResume(taskID);

    *thread = (pthread_t)taskID;
    return 0;
}

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param)
{
    if ((param == NULL) || (param->sched_priority < OS_TASK_PRIORITY_HIGHEST) ||
        (param->sched_priority >= OS_TASK_PRIORITY_LOWEST) || !IsPthread(thread)) {
        return EINVAL;
    }

    /* Only support SCHED_RR policy now */
    if (policy != SCHED_RR) {
        return ENOTSUP;
    }

    if (LOS_TaskPriSet((UINT32)thread, (UINT16)param->sched_priority) != LOS_OK) {
        return EINVAL;
    }

    return 0;
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param)
{
    UINT32 prio;

    if ((policy == NULL) || (param == NULL) || !IsPthread(thread)) {
        return EINVAL;
    }

    prio = LOS_TaskPriGet((UINT32)thread);
    if (prio == OS_INVALID) {
        return EINVAL;
    }

    *policy = SCHED_RR;
    param->sched_priority = prio;
    return 0;
}

pthread_t pthread_self(void)
{
    return (pthread_t)LOS_CurTaskIDGet();
}

int pthread_cancel(pthread_t thread)
{
    if (!IsPthread(thread)) {
        return EINVAL;
    }

    return ENOSYS;
}

int pthread_join(pthread_t thread, void **retval)
{
    UINT32 taskStatus;

    if (!IsPthread(thread)) {
        return EINVAL;
    }

    if (retval) {
        /* retrieve thread exit code is not supported currently */
        return ENOTSUP;
    }

    if (thread == pthread_self()) {
        return EDEADLK;
    }

    while (LOS_TaskStatusGet((UINT32)thread, &taskStatus) == LOS_OK) {
        (void)LOS_TaskDelay(10); /* 10: Waiting for the end of thread execution. */
    }

    return 0;
}

int pthread_detach(pthread_t thread)
{
    if (!IsPthread(thread)) {
        return EINVAL;
    }

    return ENOSYS;
}

void pthread_exit(void *retVal)
{
    (void)retVal;
    LosTaskCB *tcb = OS_TCB_FROM_TID(LOS_CurTaskIDGet());
    free((PthreadData *)(UINTPTR)tcb->arg);
    (void)LOS_TaskDelete(LOS_CurTaskIDGet());
}

int pthread_setname_np(pthread_t thread, const char *name)
{
    char *taskName = LOS_TaskNameGet((UINT32)thread);
    if (taskName == NULL || !IsPthread(thread)) {
        return EINVAL;
    }
    if (strnlen(name, PTHREAD_NAMELEN) >= PTHREAD_NAMELEN) {
        return ERANGE;
    }
    (void)strcpy_s(taskName, PTHREAD_NAMELEN, name);
    return 0;
}

int pthread_getname_np(pthread_t thread, char *buf, size_t buflen)
{
    int ret;

    const char *name = LOS_TaskNameGet((UINT32)thread);
    if (name == NULL || !IsPthread(thread)) {
        return EINVAL;
    }
    if (buflen > strlen(name)) {
        ret = strcpy_s(buf, buflen, name);
        if (ret == 0) {
            return 0;
        }
    }
    return ERANGE;
}
