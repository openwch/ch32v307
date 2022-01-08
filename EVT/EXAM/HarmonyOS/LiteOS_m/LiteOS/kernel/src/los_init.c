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

#include "stdarg.h"
#include "los_arch.h"
#include "los_config.h"
#include "los_debug.h"
#include "los_memory.h"
#include "los_mux.h"
#include "los_queue.h"
#include "los_sem.h"

#if (LOSCFG_PLATFORM_HWI == 1)
#include "los_interrupt.h"
#endif

#if (LOSCFG_KERNEL_TRACE == 1)
#include "los_debug.h"
#endif

#if (LOSCFG_BASE_CORE_SWTMR == 1)
#include "los_swtmr.h"
#endif

#if (LOSCFG_BASE_CORE_CPUP == 1)
#include "los_cpup.h"
#endif

#if (LOSCFG_PLATFORM_EXC == 1)
#include "los_exc_info.h"
#endif

#if (LOSCFG_BACKTRACE_TYPE != 0)
#include "los_backtrace.h"
#endif

#if (LOSCFG_KERNEL_PM == 1)
#include "los_pm.h"
#endif

/*****************************************************************************
 Function    : LOS_Reboot
 Description : system exception, die in here, wait for watchdog.
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT VOID LOS_Reboot(VOID)
{
    OsDoExcHook(EXC_REBOOT);
    HalSysExit();
}

LITE_OS_SEC_TEXT_INIT VOID LOS_Panic(const CHAR *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    PRINT_ERR(fmt, ap);
    va_end(ap);
    HalSysExit();
}


/*****************************************************************************
 Function    : OsRegister
 Description : Configuring the maximum number of tasks
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT static VOID OsRegister(VOID)
{
    g_taskMaxNum = LOSCFG_BASE_CORE_TSK_LIMIT + 1; /* Reserved 1 for IDLE */

    return;
}

LITE_OS_SEC_TEXT_INIT UINT32 LOS_Start(VOID)
{
    return HalStartSchedule(OsTickHandler);
}

/*****************************************************************************
 Function    : LOS_KernelInit
 Description : System kernel initialization function, configure all system modules
 Input       : None
 Output      : None
 Return      : LOS_OK on success or error code on failure
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT UINT32 LOS_KernelInit(VOID)
{
    UINT32 ret;
    PRINTK("entering kernel init...\n");

#if (LOSCFG_BACKTRACE_TYPE != 0)
    OSBackTraceInit();
#endif

    OsRegister();
    ret = OsMemSystemInit();
    if (ret != LOS_OK) {
        PRINT_ERR("OsMemSystemInit error %d\n", ret);
        return ret;
    }

    HalArchInit();

    ret = OsTaskInit();
    if (ret != LOS_OK) {
        PRINT_ERR("OsTaskInit error\n");
        return ret;
    }

#if (LOSCFG_BASE_CORE_TSK_MONITOR == 1)
    OsTaskMonInit();
#endif

#if (LOSCFG_BASE_CORE_CPUP == 1)
    ret = OsCpupInit();
    if (ret != LOS_OK) {
        PRINT_ERR("OsCpupInit error\n");
        return ret;
    }
#endif

#if (LOSCFG_BASE_IPC_SEM == 1)
    ret = OsSemInit();
    if (ret != LOS_OK) {
        return ret;
    }
#endif

#if (LOSCFG_BASE_IPC_MUX == 1)
    ret = OsMuxInit();
    if (ret != LOS_OK) {
        return ret;
    }
#endif

#if (LOSCFG_BASE_IPC_QUEUE == 1)
    ret = OsQueueInit();
    if (ret != LOS_OK) {
        PRINT_ERR("OsQueueInit error\n");
        return ret;
    }
#endif

#if (LOSCFG_BASE_CORE_SWTMR == 1)
    ret = OsSwtmrInit();
    if (ret != LOS_OK) {
        PRINT_ERR("OsSwtmrInit error\n");
        return ret;
    }
#endif

    ret = OsIdleTaskCreate();
    if (ret != LOS_OK) {
        return ret;
    }

#if (LOSCFG_KERNEL_TRACE == 1)
    ret = LOS_TraceInit();
    if (ret != LOS_OK) {
        PRINT_ERR("LOS_TraceInit error\n");
        return ret;
    }
#endif

#if (LOSCFG_KERNEL_PM == 1)
    ret = OsPmInit();
    if (ret != LOS_OK) {
        PRINT_ERR("Pm init failed!\n");
        return ret;
    }
#endif

#ifdef LOSCFG_TEST
    //ret = los_TestInit();
    //if (ret != LOS_OK) {
    //    PRINT_ERR("los_TestInit error\n");
    //    return ret;
    //}
#endif

#if (LOSCFG_PLATFORM_EXC == 1)
    OsExcMsgDumpInit();
#endif

    return LOS_OK;
}

