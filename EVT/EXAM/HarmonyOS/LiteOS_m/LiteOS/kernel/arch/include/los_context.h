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

/**
 * @defgroup los_context hardware
 * @ingroup kernel
 */

#ifndef _LOS_CONTEXT_H
#define _LOS_CONTEXT_H

#include "los_compiler.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @ingroup  los_context
 * @brief: Task stack initialization.
 *
 * @par Description:
 * This API is used to initialize the task stack.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  taskID     [IN] Type#UINT32: TaskID.
 * @param  stackSize  [IN] Type#UINT32: Total size of the stack.
 * @param  topStack   [IN] Type#VOID *: Top of task's stack.
 *
 * @retval: context Type#TaskContext *.
 * @par Dependency:
 * <ul><li>los_context.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern VOID *HalTskStackInit(UINT32 taskID, UINT32 stackSize, VOID *topStack);

/**
 * @ingroup  los_context
 * @brief: Function to sys exit.
 *
 * @par Description:
 * This API is used to sys exit.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  None.
 *
 * @retval: None.
 * @par Dependency:
 * <ul><li>los_context.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
LITE_OS_SEC_TEXT_MINOR NORETURN VOID HalSysExit(VOID);

/**
 * @ingroup  los_context
 * @brief: Task scheduling Function.
 *
 * @par Description:
 * This API is used to scheduling task.
 *
 * @attention:
 * <ul><li>None.</li></ul>
 *
 * @param  None.
 *
 * @retval: None.
 * @par Dependency:
 * <ul><li>los_context.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 */
extern VOID HalTaskSchedule(VOID);

typedef VOID (*OS_TICK_HANDLER)(VOID);
UINT32 HalStartSchedule(OS_TICK_HANDLER handler);

UINT32 HalIntLock(VOID);
#define LOS_IntLock HalIntLock

VOID HalIntRestore(UINT32 intSave);
#define LOS_IntRestore  HalIntRestore

UINT32 HalIntUnLock(VOID);
#define LOS_IntUnLock HalIntUnLock

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_CONTEXT_H */
