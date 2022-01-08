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

#include "los_debug.h"
#include "stdarg.h"
#include "los_context.h"


#if (LOSCFG_KERNEL_PRINTF == 1)
STATIC const CHAR *g_logString[] = {
    "EMG",
    "COMMON",
    "ERR",
    "WARN",
    "INFO",
    "DEBUG",
};
#endif

STATIC ExcHookFn g_excHook;
STATIC BACK_TRACE_HOOK g_backTraceHook = NULL;

VOID OsBackTraceHookSet(BACK_TRACE_HOOK hook)
{
    if (g_backTraceHook == NULL) {
        g_backTraceHook = hook;
    }
}

VOID OsBackTraceHookCall(UINTPTR *LR, UINT32 LRSize, UINT32 jumpCount, UINTPTR SP)
{
    if (g_backTraceHook != NULL) {
        g_backTraceHook(LR, LRSize, jumpCount, SP);
    } else {
        PRINT_ERR("Record LR failed, because of g_backTraceHook is not registered, "
                  "should call OSBackTraceInit firstly\n");
    }
}

VOID OsExcHookRegister(ExcHookFn excHookFn)
{
    UINT32 intSave = LOS_IntLock();
    if (!g_excHook) {
        g_excHook = excHookFn;
    }
    LOS_IntRestore(intSave);
}

VOID OsDoExcHook(EXC_TYPE excType)
{
    UINT32 intSave = LOS_IntLock();
    if (g_excHook) {
        g_excHook(excType);
    }
    LOS_IntRestore(intSave);
}

#if (LOSCFG_KERNEL_PRINTF == 1)
INT32 OsLogLevelCheck(INT32 level)
{
    if (level > PRINT_LEVEL) {
        return LOS_NOK;
    }

    if ((level != LOG_COMMON_LEVEL) && ((level > LOG_EMG_LEVEL) && (level <= LOG_DEBUG_LEVEL))) {
        PRINTK("[%s]", g_logString[level]);
    }

    return LOS_OK;
}
#endif

#if (LOSCFG_KERNEL_PRINTF > 1)
WEAK VOID HalConsoleOutput(LogModuleType type, INT32 level, const CHAR *fmt, ...)
{
}
#endif

