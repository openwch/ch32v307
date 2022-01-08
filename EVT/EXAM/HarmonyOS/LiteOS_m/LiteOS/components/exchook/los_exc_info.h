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

#include "los_config.h"
#include "los_task.h"
#include "los_queue.h"
#include "los_memory.h"
#include "los_arch_interrupt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if (LOSCFG_PLATFORM_EXC == 1)
#define INFO_TYPE_AND_SIZE      8

#define MAX_SCENE_INFO_SIZE     (INFO_TYPE_AND_SIZE + sizeof(ExcInfo) + sizeof(EXC_CONTEXT_S))
#define MAX_TSK_INFO_SIZE       (INFO_TYPE_AND_SIZE + sizeof(TSK_INFO_S) * (LOSCFG_BASE_CORE_TSK_LIMIT + 1))

#if (LOSCFG_BASE_IPC_QUEUE == 1)
#define MAX_QUEUE_INFO_SIZE     (INFO_TYPE_AND_SIZE + sizeof(QUEUE_INFO_S) * LOSCFG_BASE_IPC_QUEUE_LIMIT)
#else
#define MAX_QUEUE_INFO_SIZE     (0)
#endif

#if (LOSCFG_BASE_CORE_EXC_TSK_SWITCH == 1)
#define MAX_SWITCH_INFO_SIZE    (INFO_TYPE_AND_SIZE + (sizeof(UINT32) + sizeof(CHAR) * LOS_TASK_NAMELEN) * OS_TASK_SWITCH_INFO_COUNT)
#else
#define MAX_SWITCH_INFO_SIZE    (0)
#endif

#define MAX_MEM_INFO_SIZE       (INFO_TYPE_AND_SIZE + sizeof(MemInfoCB) * OS_SYS_MEM_NUM)
#define MAX_EXC_MEM_SIZE        (INFO_TYPE_AND_SIZE + MAX_SCENE_INFO_SIZE + MAX_TSK_INFO_SIZE + MAX_QUEUE_INFO_SIZE + MAX_INT_INFO_SIZE + MAX_SWITCH_INFO_SIZE + MAX_MEM_INFO_SIZE)

typedef enum {
    OS_EXC_TYPE_CONTEXT     = 0,
    OS_EXC_TYPE_TSK         = 1,
    OS_EXC_TYPE_QUE         = 2,
    OS_EXC_TYPE_NVIC        = 3,
    OS_EXC_TYPE_TSK_SWITCH  = 4,
    OS_EXC_TYPE_MEM         = 5,
    OS_EXC_TYPE_MAX         = 6
} ExcInfoType;

typedef struct {
    ExcInfoType     flag;
    UINT32          length;
    ExcInfo         info;
    EXC_CONTEXT_S   context;
} ExcContextInfoArray;

typedef struct {
    ExcInfoType flag;
    UINT32      length;
    TSK_INFO_S  taskInfo[LOSCFG_BASE_CORE_TSK_LIMIT + 1];
} ExcTaskInfoArray;

typedef struct {
    ExcInfoType  flag;
    UINT32       length;
    QUEUE_INFO_S queueInfo[LOSCFG_BASE_CORE_TSK_LIMIT];
} ExcQueueInfoArray;

typedef struct {
    UINT32              totalLen;
    ExcContextInfoArray excInfo;
    ExcTaskInfoArray    taskInfo;
    ExcQueueInfoArray   queueInfo;
} ExcMsgArray;

typedef UINT32 (*EXC_INFO_SAVE_CALLBACK)(UINT32, VOID *);

typedef struct {
    ExcInfoType             type;
    UINT32                  valid;
    EXC_INFO_SAVE_CALLBACK  fnExcInfoCb;
    VOID                    *arg;
} ExcInfoArray;

VOID OsExcMsgDumpInit(VOID);
extern UINT8 g_excMsgArray[MAX_EXC_MEM_SIZE];
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
