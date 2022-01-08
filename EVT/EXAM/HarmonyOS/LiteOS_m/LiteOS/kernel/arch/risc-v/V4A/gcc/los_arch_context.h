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

#ifndef _LOS_ARCH_CONTEXT_H
#define _LOS_ARCH_CONTEXT_H

#include "los_compiler.h"
#include "los_context.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

//#define ARCH_RISCV_FPU

/**
 * @ingroup los_hw
 */
typedef unsigned long STACK_TYPE;

typedef struct {
    STACK_TYPE epc;        /* epc - epc    - program counter                     */
    STACK_TYPE ra;         /* x1  - ra     - return address for jumps            */
    STACK_TYPE t0;         /* x5  - t0     - temporary register 0                */
    STACK_TYPE t1;         /* x6  - t1     - temporary register 1                */
    STACK_TYPE t2;         /* x7  - t2     - temporary register 2                */
    STACK_TYPE s0_fp;      /* x8  - s0/fp  - saved register 0 or frame pointer   */
    STACK_TYPE s1;         /* x9  - s1     - saved register 1                    */
    STACK_TYPE a0;         /* x10 - a0     - return value or function argument 0 */
    STACK_TYPE a1;         /* x11 - a1     - return value or function argument 1 */
    STACK_TYPE a2;         /* x12 - a2     - function argument 2                 */
    STACK_TYPE a3;         /* x13 - a3     - function argument 3                 */
    STACK_TYPE a4;         /* x14 - a4     - function argument 4                 */
    STACK_TYPE a5;         /* x15 - a5     - function argument 5                 */
#ifndef __riscv_32e
    STACK_TYPE a6;         /* x16 - a6     - function argument 6                 */
    STACK_TYPE a7;         /* x17 - s7     - function argument 7                 */
    STACK_TYPE s2;         /* x18 - s2     - saved register 2                    */
    STACK_TYPE s3;         /* x19 - s3     - saved register 3                    */
    STACK_TYPE s4;         /* x20 - s4     - saved register 4                    */
    STACK_TYPE s5;         /* x21 - s5     - saved register 5                    */
    STACK_TYPE s6;         /* x22 - s6     - saved register 6                    */
    STACK_TYPE s7;         /* x23 - s7     - saved register 7                    */
    STACK_TYPE s8;         /* x24 - s8     - saved register 8                    */
    STACK_TYPE s9;         /* x25 - s9     - saved register 9                    */
    STACK_TYPE s10;        /* x26 - s10    - saved register 10                   */
    STACK_TYPE s11;        /* x27 - s11    - saved register 11                   */
    STACK_TYPE t3;         /* x28 - t3     - temporary register 3                */
    STACK_TYPE t4;         /* x29 - t4     - temporary register 4                */
    STACK_TYPE t5;         /* x30 - t5     - temporary register 5                */
    STACK_TYPE t6;         /* x31 - t6     - temporary register 6                */
#endif
    STACK_TYPE mstatus;    /*              - machine status register             */
#ifdef ARCH_RISCV_FPU
    STACK_TYPE  f0;
    STACK_TYPE  f1;
    STACK_TYPE  f2;
    STACK_TYPE  f3;
    STACK_TYPE  f4;
    STACK_TYPE  f5;
    STACK_TYPE  f6;
    STACK_TYPE  f7;
    STACK_TYPE  f8;
    STACK_TYPE  f9;
    STACK_TYPE  f10;
    STACK_TYPE  f11;
    STACK_TYPE  f12;
    STACK_TYPE  f13;
    STACK_TYPE  f14;
    STACK_TYPE  f15;
    STACK_TYPE  f16;
    STACK_TYPE  f17;
    STACK_TYPE  f18;
    STACK_TYPE  f19;
    STACK_TYPE  f20;
    STACK_TYPE  f21;
    STACK_TYPE  f22;
    STACK_TYPE  f23;
    STACK_TYPE  f24;
    STACK_TYPE  f25;
    STACK_TYPE  f26;
    STACK_TYPE  f27;
    STACK_TYPE  f28;
    STACK_TYPE  f29;
    STACK_TYPE  f30;
    STACK_TYPE  f31;
#endif
} TaskContext;

extern VOID HalStartToRun(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_HW_H */
