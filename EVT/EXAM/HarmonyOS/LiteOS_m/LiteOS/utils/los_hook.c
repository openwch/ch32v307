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

#include "los_hook.h"
#include "internal/los_hook_types_parse.h"


#if (LOSCFG_DEBUG_HOOK == 1)
#define LOS_HOOK_TYPE_DEF(type, paramList)                  \
    STATIC type##_FN g_fn##type;                            \
    UINT32 type##_RegHook(type##_FN func) {                 \
        if ((func) == NULL) {                               \
            return LOS_ERRNO_HOOK_REG_INVALID;              \
        }                                                   \
        if (g_fn##type) {                                   \
            return LOS_ERRNO_HOOK_POOL_IS_FULL;             \
        }                                                   \
        g_fn##type = (func);                                \
        return LOS_OK;                                      \
    }                                                       \
    UINT32 type##_UnRegHook(type##_FN func) {               \
        if (((func) == NULL) || (g_fn##type != (func))) {   \
            return LOS_ERRNO_HOOK_UNREG_INVALID;            \
        }                                                   \
        g_fn##type = NULL;                                  \
        return LOS_OK;                                      \
    }                                                       \
    VOID type##_CallHook paramList {                        \
        if (g_fn##type) {                                   \
            g_fn##type(PARAM_TO_ARGS paramList);            \
        }                                                   \
    }

LOS_HOOK_ALL_TYPES_DEF;

#undef LOS_HOOK_TYPE_DEF

#endif /* LOSCFG_DEBUG_HOOK */

