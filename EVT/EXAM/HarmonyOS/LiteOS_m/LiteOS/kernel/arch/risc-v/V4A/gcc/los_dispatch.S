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

#include "riscv_encoding.h"

//#define ARCH_RISCV_FPU

#ifndef __riscv_32e
#define portRegNum          30
#else
#define portRegNum          14
#endif

#ifdef  ARCH_RISCV_FPU
#define portFloatRegNum     32
#endif

#define portCONTEXT_SIZE    ( portRegNum * REGBYTES )

    .section .text,"ax",@progbits
    .align 2
    .type HalIntLock, %function
    .global HalIntLock
HalIntLock:
    csrr    a0, mstatus  /* return value */
    li      t0, (MSTATUS_MIE|MSTATUS_MPIE) /* mie|mpie */
    csrrc   zero, mstatus, t0
    ret

    .type HalIntUnLock, %function
    .global HalIntUnLock
HalIntUnLock:
    csrr    a0, mstatus  /* return value*/
    li      t0, (MSTATUS_MIE|MSTATUS_MPIE) /* mie|mpie */
    csrrs   zero, mstatus, t0
    ret

    .type HalIntRestore, %function
    .global HalIntRestore
HalIntRestore:
    csrw mstatus, a0
    ret


/* Start the first task.  This also clears the bit that indicates the FPU is
    in use in case the FPU was used before the scheduler was started - which
    would otherwise result in the unnecessary leaving of space in the stack
    for lazy saving of FPU registers. */
    .type HalStartToRun, %function
    .global HalStartToRun
    .align 2
HalStartToRun:
    /* Setup Interrupt Stack using
       The stack that was used by main()
       before the scheduler is started is
       no longer required after the scheduler is started.
       Interrupt stack pointer is stored in CSR_MSCRATCH */
    la t0, _eusrstack
    addi t0,t0,-512
    csrw CSR_MSCRATCH, t0
    /* get stack pointer */
    la t0, g_losTask
    LOAD t1, 0x0(t0)
    LOAD      sp, 0(t1)    /* TCB.sp->sp */

    /* Pop PC from stack and set MEPC */
    LOAD t0,  0  * REGBYTES(sp)
    csrw CSR_MEPC, t0
    /* Pop mstatus from stack and set it */
    LOAD t0,  (portRegNum - 1)  * REGBYTES(sp)
    csrw CSR_MSTATUS, t0
    /* Interrupt still disable here */
    /* Restore Registers from Stack */
    LOAD x1,  1  * REGBYTES(sp)    /* RA */
    LOAD x5,  2  * REGBYTES(sp)
    LOAD x6,  3  * REGBYTES(sp)
    LOAD x7,  4  * REGBYTES(sp)
    LOAD x8,  5  * REGBYTES(sp)
    LOAD x9,  6  * REGBYTES(sp)
    LOAD x10, 7  * REGBYTES(sp)
    LOAD x11, 8  * REGBYTES(sp)
    LOAD x12, 9  * REGBYTES(sp)
    LOAD x13, 10 * REGBYTES(sp)
    LOAD x14, 11 * REGBYTES(sp)
    LOAD x15, 12 * REGBYTES(sp)
#ifndef __riscv_32e
    LOAD x16, 13 * REGBYTES(sp)
    LOAD x17, 14 * REGBYTES(sp)
    LOAD x18, 15 * REGBYTES(sp)
    LOAD x19, 16 * REGBYTES(sp)
    LOAD x20, 17 * REGBYTES(sp)
    LOAD x21, 18 * REGBYTES(sp)
    LOAD x22, 19 * REGBYTES(sp)
    LOAD x23, 20 * REGBYTES(sp)
    LOAD x24, 21 * REGBYTES(sp)
    LOAD x25, 22 * REGBYTES(sp)
    LOAD x26, 23 * REGBYTES(sp)
    LOAD x27, 24 * REGBYTES(sp)
    LOAD x28, 25 * REGBYTES(sp)
    LOAD x29, 26 * REGBYTES(sp)
    LOAD x30, 27 * REGBYTES(sp)
    LOAD x31, 28 * REGBYTES(sp)
#endif
    addi sp, sp, portCONTEXT_SIZE

#ifdef ARCH_RISCV_FPU
    FPLOAD   f0, 0 * FPREGBYTES(sp)
    FPLOAD   f1, 1 * FPREGBYTES(sp)
    FPLOAD   f2, 2 * FPREGBYTES(sp)
    FPLOAD   f3, 3 * FPREGBYTES(sp)
    FPLOAD   f4, 4 * FPREGBYTES(sp)
    FPLOAD   f5, 5 * FPREGBYTES(sp)
    FPLOAD   f6, 6 * FPREGBYTES(sp)
    FPLOAD   f7, 7 * FPREGBYTES(sp)
    FPLOAD   f8, 8 * FPREGBYTES(sp)
    FPLOAD   f9, 9 * FPREGBYTES(sp)
    FPLOAD   f10, 10 * FPREGBYTES(sp)
    FPLOAD   f11, 11 * FPREGBYTES(sp)
    FPLOAD   f12, 12 * FPREGBYTES(sp)
    FPLOAD   f13, 13 * FPREGBYTES(sp)
    FPLOAD   f14, 14 * FPREGBYTES(sp)
    FPLOAD   f15, 15 * FPREGBYTES(sp)
    FPLOAD   f16, 16 * FPREGBYTES(sp)
    FPLOAD   f17, 17 * FPREGBYTES(sp)
    FPLOAD   f18, 18 * FPREGBYTES(sp)
    FPLOAD   f19, 19 * FPREGBYTES(sp)
    FPLOAD   f20, 20 * FPREGBYTES(sp)
    FPLOAD   f21, 21 * FPREGBYTES(sp)
    FPLOAD   f22, 22 * FPREGBYTES(sp)
    FPLOAD   f23, 23 * FPREGBYTES(sp)
    FPLOAD   f24, 24 * FPREGBYTES(sp)
    FPLOAD   f25, 25 * FPREGBYTES(sp)
    FPLOAD   f26, 26 * FPREGBYTES(sp)
    FPLOAD   f27, 27 * FPREGBYTES(sp)
    FPLOAD   f28, 28 * FPREGBYTES(sp)
    FPLOAD   f29, 29 * FPREGBYTES(sp)
    FPLOAD   f30, 30 * FPREGBYTES(sp)
    FPLOAD   f31, 31 * FPREGBYTES(sp)
    addi    sp, sp, portFloatRegNum * FPREGBYTES
#endif

    mret


.extern HalTaskSwitch

.section    .text.vector_handler, "ax", @progbits
.global SW_Handler
.align 2
SW_Handler:

#ifdef ARCH_RISCV_FPU
    addi    sp, sp, -portFloatRegNum * FPREGBYTES
    FPSTORE  f0, 0 * FPREGBYTES(sp)
    FPSTORE  f1, 1 * FPREGBYTES(sp)
    FPSTORE  f2, 2 * FPREGBYTES(sp)
    FPSTORE  f3, 3 * FPREGBYTES(sp)
    FPSTORE  f4, 4 * FPREGBYTES(sp)
    FPSTORE  f5, 5 * FPREGBYTES(sp)
    FPSTORE  f6, 6 * FPREGBYTES(sp)
    FPSTORE  f7, 7 * FPREGBYTES(sp)
    FPSTORE  f8, 8 * FPREGBYTES(sp)
    FPSTORE  f9, 9 * FPREGBYTES(sp)
    FPSTORE  f10, 10 * FPREGBYTES(sp)
    FPSTORE  f11, 11 * FPREGBYTES(sp)
    FPSTORE  f12, 12 * FPREGBYTES(sp)
    FPSTORE  f13, 13 * FPREGBYTES(sp)
    FPSTORE  f14, 14 * FPREGBYTES(sp)
    FPSTORE  f15, 15 * FPREGBYTES(sp)
    FPSTORE  f16, 16 * FPREGBYTES(sp)
    FPSTORE  f17, 17 * FPREGBYTES(sp)
    FPSTORE  f18, 18 * FPREGBYTES(sp)
    FPSTORE  f19, 19 * FPREGBYTES(sp)
    FPSTORE  f20, 20 * FPREGBYTES(sp)
    FPSTORE  f21, 21 * FPREGBYTES(sp)
    FPSTORE  f22, 22 * FPREGBYTES(sp)
    FPSTORE  f23, 23 * FPREGBYTES(sp)
    FPSTORE  f24, 24 * FPREGBYTES(sp)
    FPSTORE  f25, 25 * FPREGBYTES(sp)
    FPSTORE  f26, 26 * FPREGBYTES(sp)
    FPSTORE  f27, 27 * FPREGBYTES(sp)
    FPSTORE  f28, 28 * FPREGBYTES(sp)
    FPSTORE  f29, 29 * FPREGBYTES(sp)
    FPSTORE  f30, 30 * FPREGBYTES(sp)
    FPSTORE  f31, 31 * FPREGBYTES(sp)
#endif

    addi sp, sp, -portCONTEXT_SIZE
    STORE x1,  1  * REGBYTES(sp)    /* RA */
    STORE x5,  2  * REGBYTES(sp)
    STORE x6,  3  * REGBYTES(sp)
    STORE x7,  4  * REGBYTES(sp)
    STORE x8,  5  * REGBYTES(sp)
    STORE x9,  6  * REGBYTES(sp)
    STORE x10, 7  * REGBYTES(sp)
    STORE x11, 8  * REGBYTES(sp)
    STORE x12, 9  * REGBYTES(sp)
    STORE x13, 10 * REGBYTES(sp)
    STORE x14, 11 * REGBYTES(sp)
    STORE x15, 12 * REGBYTES(sp)
#ifndef __riscv_32e
    STORE x16, 13 * REGBYTES(sp)
    STORE x17, 14 * REGBYTES(sp)
    STORE x18, 15 * REGBYTES(sp)
    STORE x19, 16 * REGBYTES(sp)
    STORE x20, 17 * REGBYTES(sp)
    STORE x21, 18 * REGBYTES(sp)
    STORE x22, 19 * REGBYTES(sp)
    STORE x23, 20 * REGBYTES(sp)
    STORE x24, 21 * REGBYTES(sp)
    STORE x25, 22 * REGBYTES(sp)
    STORE x26, 23 * REGBYTES(sp)
    STORE x27, 24 * REGBYTES(sp)
    STORE x28, 25 * REGBYTES(sp)
    STORE x29, 26 * REGBYTES(sp)
    STORE x30, 27 * REGBYTES(sp)
    STORE x31, 28 * REGBYTES(sp)
#endif

    li   t0,    0x20
	csrs 0x804, t0

    /* Push mstatus to stack */
    csrr t0, CSR_MSTATUS
    STORE t0,  (portRegNum - 1)  * REGBYTES(sp)

    /* Push additional registers */

    /* Store sp to task stack */
    la t0, g_losTask
    LOAD t0, 0(t0)
    STORE sp, 0(t0)

    csrr t0, CSR_MEPC
    STORE t0, 0(sp)

    /* Switch task context */
    jal HalTaskSwitch
    /* Load new task */
    la t0, g_losTask
    LOAD t0, 0(t0)
    LOAD sp, 0x0(t0)                /* Read sp from first TCB member */

    /* Pop PC from stack and set MEPC */
    LOAD t0,  0  * REGBYTES(sp)
    csrw CSR_MEPC, t0
    /* Pop additional registers */

    /* Pop mstatus from stack and set it */
    LOAD t0,  (portRegNum - 1)  * REGBYTES(sp)
    csrw CSR_MSTATUS, t0
    /* Interrupt still disable here */
    /* Restore Registers from Stack */
    LOAD x1,  1  * REGBYTES(sp)    /* RA */
    LOAD x5,  2  * REGBYTES(sp)
    LOAD x6,  3  * REGBYTES(sp)
    LOAD x7,  4  * REGBYTES(sp)
    LOAD x8,  5  * REGBYTES(sp)
    LOAD x9,  6  * REGBYTES(sp)
    LOAD x10, 7  * REGBYTES(sp)
    LOAD x11, 8  * REGBYTES(sp)
    LOAD x12, 9  * REGBYTES(sp)
    LOAD x13, 10 * REGBYTES(sp)
    LOAD x14, 11 * REGBYTES(sp)
    LOAD x15, 12 * REGBYTES(sp)
#ifndef __riscv_32e
    LOAD x16, 13 * REGBYTES(sp)
    LOAD x17, 14 * REGBYTES(sp)
    LOAD x18, 15 * REGBYTES(sp)
    LOAD x19, 16 * REGBYTES(sp)
    LOAD x20, 17 * REGBYTES(sp)
    LOAD x21, 18 * REGBYTES(sp)
    LOAD x22, 19 * REGBYTES(sp)
    LOAD x23, 20 * REGBYTES(sp)
    LOAD x24, 21 * REGBYTES(sp)
    LOAD x25, 22 * REGBYTES(sp)
    LOAD x26, 23 * REGBYTES(sp)
    LOAD x27, 24 * REGBYTES(sp)
    LOAD x28, 25 * REGBYTES(sp)
    LOAD x29, 26 * REGBYTES(sp)
    LOAD x30, 27 * REGBYTES(sp)
    LOAD x31, 28 * REGBYTES(sp)
#endif
    addi sp, sp, portCONTEXT_SIZE

#ifdef ARCH_RISCV_FPU
    FPLOAD   f0, 0 * FPREGBYTES(sp)
    FPLOAD   f1, 1 * FPREGBYTES(sp)
    FPLOAD   f2, 2 * FPREGBYTES(sp)
    FPLOAD   f3, 3 * FPREGBYTES(sp)
    FPLOAD   f4, 4 * FPREGBYTES(sp)
    FPLOAD   f5, 5 * FPREGBYTES(sp)
    FPLOAD   f6, 6 * FPREGBYTES(sp)
    FPLOAD   f7, 7 * FPREGBYTES(sp)
    FPLOAD   f8, 8 * FPREGBYTES(sp)
    FPLOAD   f9, 9 * FPREGBYTES(sp)
    FPLOAD   f10, 10 * FPREGBYTES(sp)
    FPLOAD   f11, 11 * FPREGBYTES(sp)
    FPLOAD   f12, 12 * FPREGBYTES(sp)
    FPLOAD   f13, 13 * FPREGBYTES(sp)
    FPLOAD   f14, 14 * FPREGBYTES(sp)
    FPLOAD   f15, 15 * FPREGBYTES(sp)
    FPLOAD   f16, 16 * FPREGBYTES(sp)
    FPLOAD   f17, 17 * FPREGBYTES(sp)
    FPLOAD   f18, 18 * FPREGBYTES(sp)
    FPLOAD   f19, 19 * FPREGBYTES(sp)
    FPLOAD   f20, 20 * FPREGBYTES(sp)
    FPLOAD   f21, 21 * FPREGBYTES(sp)
    FPLOAD   f22, 22 * FPREGBYTES(sp)
    FPLOAD   f23, 23 * FPREGBYTES(sp)
    FPLOAD   f24, 24 * FPREGBYTES(sp)
    FPLOAD   f25, 25 * FPREGBYTES(sp)
    FPLOAD   f26, 26 * FPREGBYTES(sp)
    FPLOAD   f27, 27 * FPREGBYTES(sp)
    FPLOAD   f28, 28 * FPREGBYTES(sp)
    FPLOAD   f29, 29 * FPREGBYTES(sp)
    FPLOAD   f30, 30 * FPREGBYTES(sp)
    FPLOAD   f31, 31 * FPREGBYTES(sp)
    addi    sp, sp, portFloatRegNum * FPREGBYTES
#endif

    mret

