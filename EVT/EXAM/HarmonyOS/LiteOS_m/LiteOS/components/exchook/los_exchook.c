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

#include "los_exchook.h"
#include "los_context.h"

#ifndef LOSCFG_BASE_EXC_HOOK_LIMIT
#define LOSCFG_BASE_EXC_HOOK_LIMIT  16
#endif

struct Node {
    ExcHookFn excHookFn;
    struct Node *next;
};

STATIC struct Node g_excNodes[LOSCFG_BASE_EXC_HOOK_LIMIT];
STATIC struct Node *g_excHeads[EXC_TYPE_END + 1]; /* EXC_TYPE_END is used for the free list. */

STATIC VOID DoExcHookInRegOrder(EXC_TYPE excType, struct Node *node)
{
    if (node != NULL) {
        DoExcHookInRegOrder(excType, node->next);
        node->excHookFn(excType);
    }
}

STATIC VOID DoExcHook(EXC_TYPE excType)
{
    UINT32 intSave;
    if (excType >= EXC_TYPE_END) {
        return;
    }
    intSave = LOS_IntLock();
    DoExcHookInRegOrder(excType, g_excHeads[excType]);
    LOS_IntRestore(intSave);
}

STATIC struct Node *GetFreeNode(VOID)
{
    struct Node *node = NULL;
    int i;
    if (g_excHeads[EXC_TYPE_END] == NULL) {
        if (g_excNodes[0].excHookFn != NULL) {
            /* no free node now */
            return NULL;
        } else {
            /* Initialize the free list */
            for (i = 0; i < LOSCFG_BASE_EXC_HOOK_LIMIT; ++i) {
                g_excNodes[i].next = g_excHeads[EXC_TYPE_END];
                g_excHeads[EXC_TYPE_END] = &g_excNodes[i];
            }
            OsExcHookRegister(DoExcHook);
        }
    }

    node = g_excHeads[EXC_TYPE_END];
    g_excHeads[EXC_TYPE_END] = node->next;
    return node;
}

UINT32 LOS_RegExcHook(EXC_TYPE excType, ExcHookFn excHookFn)
{
    UINT32 intSave;
    struct Node *node = NULL;
    if (excType >= EXC_TYPE_END || excHookFn == NULL) {
        return LOS_ERRNO_SYS_PTR_NULL;
    }

    intSave = LOS_IntLock();
    node = GetFreeNode();
    if (node == NULL) {
        LOS_IntRestore(intSave);
        return LOS_ERRNO_SYS_HOOK_IS_FULL;
    }

    node->excHookFn = excHookFn;
    node->next = g_excHeads[excType];
    g_excHeads[excType] = node;
    LOS_IntRestore(intSave);
    return LOS_OK;
}

UINT32 LOS_UnRegExcHook(EXC_TYPE excType, ExcHookFn excHookFn)
{
    UINT32 intSave;
    struct Node *node = NULL;
    struct Node *preNode = NULL;
    if (excType >= EXC_TYPE_END || excHookFn == NULL) {
        return LOS_ERRNO_SYS_PTR_NULL;
    }

    intSave = LOS_IntLock();
    for (node = g_excHeads[excType]; node != NULL; node = node->next) {
        if (node->excHookFn == excHookFn) {
            if (preNode) {
                preNode->next = node->next;
            } else {
                g_excHeads[excType] = node->next;
            }
            node->excHookFn = NULL;
            node->next = g_excHeads[EXC_TYPE_END];
            g_excHeads[EXC_TYPE_END] = node;
        }
        preNode = node;
    }
    LOS_IntRestore(intSave);
    return LOS_OK;
}
