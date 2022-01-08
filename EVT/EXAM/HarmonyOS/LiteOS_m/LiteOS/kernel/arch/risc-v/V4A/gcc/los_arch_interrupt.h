/*
 * Copyright (c) 2013-2020, Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
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
#ifndef _LOS_HWI_H
#define _LOS_HWI_H

//#include "nuclei_sdk_soc.h"
#include "ch32v30x.h"
#include "los_compiler.h"
#include "los_config.h"
#include "los_interrupt.h"

#include "../../V4A/gcc/los_arch_context.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
/**
 * @ingroup los_hwi
 * Count of Nuclei system interrupt vector.
 */
#define SOC_INT_MAX  103
#define OS_RISCV_SYS_VECTOR_CNT   16

/**
 * @ingroup los_hwi
 * Count of Nuclei interrupt vector maxium, which is configurable.
 */
#define OS_RISCV_CUSTOM_IRQ_VECTOR_CNT  SOC_INT_MAX

/**
 * @ingroup los_hwi
 * Count of Nuclei interrupt vector.
 */
#define OS_RISCV_VECTOR_CNT                  (OS_RISCV_SYS_VECTOR_CNT + OS_RISCV_CUSTOM_IRQ_VECTOR_CNT)

/**
 * Maximum number of supported hardware devices that generate hardware interrupts.
 */
#define OS_HWI_MAX_NUM        (OS_RISCV_VECTOR_CNT-1)

extern VOID HalHwiDefaultHandler(VOID);

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Invalid interrupt number.
 *
 * Value: 0x02000900
 *
 * Solution: Ensure that the interrupt number is valid. The value range of the interrupt number applicable
 * for a risc-v platform is [0, OS_RISCV_VECTOR_CNT].
 */
#define OS_ERRNO_HWI_NUM_INVALID                 LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x00)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Null hardware interrupt handling function.
 *
 * Value: 0x02000901
 *
 * Solution: Pass in a valid non-null hardware interrupt handling function.
 */
#define OS_ERRNO_HWI_PROC_FUNC_NULL              LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x01)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Insufficient interrupt resources for hardware interrupt creation.
 *
 * Value: 0x02000902
 *
 * Solution: Increase the configured maximum number of supported hardware interrupts.
 */
#define OS_ERRNO_HWI_CB_UNAVAILABLE              LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x02)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Insufficient memory for hardware interrupt initialization.
 *
 * Value: 0x02000903
 *
 * Solution: Expand the configured memory.
 */
#define OS_ERRNO_HWI_NO_MEMORY                   LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x03)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: The interrupt has already been created.
 *
 * Value: 0x02000904
 *
 * Solution: Check whether the interrupt specified by the passed-in interrupt number has already been created.
 */
#define OS_ERRNO_HWI_ALREADY_CREATED             LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x04)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Invalid interrupt priority.
 *
 * Value: 0x02000905
 *
 * Solution: Ensure that the interrupt priority is valid.
 */
#define OS_ERRNO_HWI_PRIO_INVALID                LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x05)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: Incorrect interrupt creation mode.
 *
 * Value: 0x02000906
 *
 * Solution: The interrupt creation mode can be only set to ECLIC_NON_VECTOR_INTERRUPT or ECLIC_VECTOR_INTERRUPT of which the
 * value can be 0 or 1.
 */
#define OS_ERRNO_HWI_MODE_INVALID                LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x06)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: The interrupt has already been created as a fast interrupt.
 *
 * Value: 0x02000907
 *
 * Solution: Check whether the interrupt specified by the passed-in interrupt number has already been created.
 */
#define OS_ERRNO_HWI_FASTMODE_ALREADY_CREATED    LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x07)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code: The API is called during an interrupt, which is forbidden.
 *
 * Value: 0x02000908
 *
 * * Solution: Do not call the API during an interrupt.
 */
#define OS_ERRNO_HWI_INTERR LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x08)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code:the hwi support SHARED error.
 *
 * Value: 0x02000909
 *
 * * Solution:check the input params hwiMode and irqParam of HalHwiCreate or HalHwiDelete whether adapt the current
 * hwi.
 */
#define OS_ERRNO_HWI_SHARED_ERROR LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x09)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code:Invalid interrupt Arg.
 *
 * Value: 0x0200090a
 *
 * * Solution:check the interrupt Arg, Arg should only be ECLIC_LEVEL_TRIGGER, ECLIC_POSTIVE_EDGE_TRIGGER or
 *  ECLIC_NEGTIVE_EDGE_TRIGGER.
 */
#define OS_ERRNO_HWI_ARG_INVALID LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x0a)

/**
 * @ingroup los_hwi
 * Hardware interrupt error code:The interrupt corresponded to the hwi number or devid  has not been created.
 *
 * Value: 0x0200090b
 *
 * * Solution:check the hwi number or devid, make sure the hwi number or devid need to delete.
 */
#define OS_ERRNO_HWI_HWINUM_UNCREATE LOS_ERRNO_OS_ERROR(LOS_MOD_HWI, 0x0b)

extern UINT32 HalUnalignedAccessFix(UINTPTR mcause, UINTPTR mepc, UINTPTR mtval, VOID *sp);

extern VOID DisplayTaskInfo(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_HWI_H */
