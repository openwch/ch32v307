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
 * @defgroup los_exchook Exception hooks
 * @ingroup kernel
 */

#ifndef _LOS_EXCHOOK_H
#define _LOS_EXCHOOK_H

#include "los_tick.h"
#include "los_debug.h"
#include "los_arch_interrupt.h"
#include "los_interrupt.h"
#include "los_task.h"
#include "los_queue.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 *  @ingroup  los_exchook
 *  @brief: System exception hooks register function.
 *
 *  @par Description:
 *  This API is used to register exception hooks.
 *  Hooks will be invoked in reverse order.
 *
 * @attention:
 * <ul><li>This function should not be call in excHookFn.</li></ul>
 *
 * @param: None.
 *
 * @retval: LOS_OK  success.
 *          LOS_ERRNO_SYS_HOOK_IS_FULL  too many hooks registered. @see LOSCFG_BASE_EXC_HOOK_LIMIT
 *          LOS_ERRNO_SYS_PTR_NULL  excHookFn is null or invalid excType.
 *
 * @par Dependency:
 * <ul><li>los_exchook.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 *
 * */
extern UINT32 LOS_RegExcHook(EXC_TYPE excType, ExcHookFn excHookFn);

/**
 *  @ingroup  los_exchook
 *  @brief: System exception hooks unregister function.
 *
 *  @par Description:
 *  This API is used to unregister exception hooks.
 *
 * @attention:
 * <ul><li>This function should not be call in excHookFn.</li></ul>
 *
 * @param: None.
 *
 * @retval: LOS_OK  success.
 *          LOS_ERRNO_SYS_PTR_NULL  excHookFn is null or invalid excType.
 *
 * @par Dependency:
 * <ul><li>los_exchook.h: the header file that contains the API declaration.</li></ul>
 * @see None.
 *
 * */
extern UINT32 LOS_UnRegExcHook(EXC_TYPE excType, ExcHookFn excHookFn);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_EXCHOOK_H */
