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

#ifndef _LOS_HOOK_H
#define _LOS_HOOK_H

#include "internal/los_hook_types.h"
#include "los_config.h"
#include "los_error.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if (LOSCFG_DEBUG_HOOK == 1)
/**
 * @ingroup los_hook
 * Hook error code: The hook pool is insufficient.
 *
 * Value: 0x02001f00
 *
 * Solution: Deregister the registered hook.
 */
#define LOS_ERRNO_HOOK_POOL_IS_FULL             LOS_ERRNO_OS_ERROR(LOS_MOD_HOOK, 0x00)

/**
 * @ingroup los_hook
 * Hook error code: Invalid parameter.
 *
 * Value: 0x02001f01
 *
 * Solution: Check the input parameters of LOS_HookReg.
 */
#define LOS_ERRNO_HOOK_REG_INVALID              LOS_ERRNO_OS_ERROR(LOS_MOD_HOOK, 0x01)

/**
 * @ingroup los_hook
 * Hook error code: Invalid parameter.
 *
 * Value: 0x02001f02
 *
 * Solution: Check the input parameters of LOS_HookUnReg.
 */
#define LOS_ERRNO_HOOK_UNREG_INVALID            LOS_ERRNO_OS_ERROR(LOS_MOD_HOOK, 0x02)

/**
 * @ingroup los_hook
 * @brief Registration of hook function.
 *
 * @par Description:
 * This API is used to register hook function.
 *
 * @attention
 * <ul>
 * <li> None.</li>
 * </ul>
 *
 * @param hookType  [IN] Register the type of the hook.
 * @param hookFn  [IN] The function to be registered.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_hook.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_HookReg(hookType, hookFn)           hookType##_RegHook(hookFn)

/**
 * @ingroup los_hook
 * @brief Deregistration of hook function.
 *
 * @par Description:
 * This API is used to deregister hook function.
 *
 * @attention
 * <ul>
 * <li> None.</li>
 * </ul>
 *
 * @param hookType  [IN] Deregister the type of the hook.
 * @param hookFn  [IN] The function to be deregistered.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>los_hook.h: the header file that contains the API declaration.</li></ul>
 * @see
 */
#define LOS_HookUnReg(hookType, hookFn)         hookType##_UnRegHook(hookFn)

/**
 * Call hook functions.
 */
#define OsHookCall(hookType, ...)               hookType##_CallHook(__VA_ARGS__)

#else
#define LOS_HookReg(hookType, hookFn)
#define LOS_HookUnReg(hookType, hookFn)
#define OsHookCall(hookType, ...)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_HOOK_H */
