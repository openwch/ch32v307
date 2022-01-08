/*
 * Copyright (c) 2021 Nuclei Limited. All rights reserved.
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

#include <stdio.h>
#include <stdarg.h>
#include "los_arch.h"
#include "los_task.h"
#include "los_debug.h"
#include "ch32v30x.h"
#include "los_arch_context.h"
#include "los_arch_interrupt.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

UINT32 g_intCount = 0;

// LosExcInfo g_excInfo;
LITE_OS_SEC_TEXT_INIT VOID HalHwiInit(VOID)
{
    // already setup interrupt vectors
}

/*****************************************************************************
 Function    : HalHwiCreate
 Description : create hardware interrupt
 Input       : hwiNum       --- hwi num to create
               hwiPrio      --- priority of the hwi
               mode         --- hwi interrupt mode, between vector or non-vector
               handler      --- hwi handler
               arg          --- set trig mode of the hwi handler
                                Level Triggerred = 0
                                Postive/Rising Edge Triggered = 1
                                Negtive/Falling Edge Triggered = 3
 Output      : None
 Return      : LOS_OK on success or error code on failure
 *****************************************************************************/
 UINT32 HalHwiCreate(HWI_HANDLE_T hwiNum,
                                     HWI_PRIOR_T hwiPrio,
                                     HWI_MODE_T mode,
                                     HWI_PROC_FUNC handler,
                                     HWI_ARG_T arg)
{
    if (hwiNum > SOC_INT_MAX){
        return OS_ERRNO_HWI_NUM_INVALID;
    }
//    if (mode > ECLIC_VECTOR_INTERRUPT){
//        return OS_ERRNO_HWI_MODE_INVALID;
//    }
//    if (arg > ECLIC_NEGTIVE_EDGE_TRIGGER){
//        return OS_ERRNO_HWI_ARG_INVALID;
//    }

    NVIC_SetPriority(hwiNum,(UINT8)hwiPrio);
    NVIC_EnableIRQ(hwiNum);

    return LOS_OK;
}

/*****************************************************************************
 Function    : HalHwiDelete
 Description : Delete hardware interrupt
 Input       : hwiNum   --- hwi num to delete
 Return      : LOS_OK on success or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT UINT32 HalHwiDelete(HWI_HANDLE_T hwiNum)
{
    NVIC_DisableIRQ(hwiNum);
    return LOS_OK;
}

/* ****************************************************************************
 Function    : HalHwiDefaultHandler
 Description : default handler of the hardware interrupt
 Input       : None
 Output      : None
 Return      : None
 **************************************************************************** */
LITE_OS_SEC_TEXT_INIT VOID HalHwiDefaultHandler(VOID)
{
    PRINT_ERR("default handler\n");
    while (1) {
    }
}

/* ****************************************************************************
 Function    : HalDisplayTaskInfo
 Description : display the task list
 Input       : None
 Output      : None
 Return      : None
 **************************************************************************** */
VOID HalDisplayTaskInfo(VOID)
{
    TSK_INFO_S taskInfo;
    UINT32 index;
    UINT32 ret;

    PRINTK("ID  Pri    Status     name \r\n");
    PRINTK("--  ---    ---------  ----\r\n");

    for (index = 0; index < LOSCFG_BASE_CORE_TSK_LIMIT; index++) {
        ret = LOS_TaskInfoGet(index, &taskInfo);
        if (ret != LOS_OK) {
            continue;
        }
        PRINTK("%d    %d     %s      %s \r\n",
               taskInfo.uwTaskID, taskInfo.usTaskPrio, OsConvertTskStatus(taskInfo.usTaskStatus), taskInfo.acName);
    }
    return;
}

/* ****************************************************************************
 Function    : HalUnalignedAccessFix
 Description : Unaligned acess fixes are not supported by default
 Input       : None
 Output      : None
 Return      : None
 **************************************************************************** */
WEAK UINT32 HalUnalignedAccessFix(UINTPTR mcause, UINTPTR mepc, UINTPTR mtval, VOID *sp)
{
    /* Unaligned acess fixes are not supported by default */
    PRINTK("Unaligned acess fixes are not support by default!\r\n");
    return LOS_NOK;
}

__attribute__((always_inline)) inline VOID HalIntEnter(VOID)
{
    g_intCount += 1;
}

__attribute__((always_inline)) inline VOID HalIntExit(VOID)
{
    g_intCount -= 1;
}

__attribute__((always_inline)) inline UINT32 HalIsIntActive(VOID)
{
    return (g_intCount > 0);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
