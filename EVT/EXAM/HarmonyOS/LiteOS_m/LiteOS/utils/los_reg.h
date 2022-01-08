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
 * @defgroup kernel Kernel
 * @defgroup los_reg Basic definitions
 * @ingroup kernel
 */

#ifndef _LOS_REG_H
#define _LOS_REG_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @ingroup los_base
 * Read a UINT8 value from addr and stroed in value.
 */
#define READ_UINT8(value, addr)      ((value) = *((volatile UINT8 *)(addr)))
/**
 * @ingroup los_base
 * Read a UINT16 value from addr and stroed in addr.
 */
#define READ_UINT16(value, addr)     ((value) = *((volatile UINT16 *)(addr)))
/**
 * @ingroup los_base
 * Read a UINT32 value from addr and stroed in value.
 */
#define READ_UINT32(value, addr)     ((value) = *((volatile UINT32 *)(addr)))
/**
 * @ingroup los_base
 * Read a UINT64 value from addr and stroed in value.
 */
#define READ_UINT64(value, addr)     ((value) = *((volatile UINT64 *)(addr)))

/**
 * @ingroup los_base
 * Get a UINT8 value from addr.
 */
#define GET_UINT8(addr)                (*((volatile UINT8 *)(addr)))
/**
 * @ingroup los_base
 * Get a UINT16 value from addr.
 */
#define GET_UINT16(addr)               (*((volatile UINT16 *)(addr)))
/**
 * @ingroup los_base
 * Get a UINT32 value from addr.
 */
#define GET_UINT32(addr)               (*((volatile UINT32 *)(addr)))
/**
 * @ingroup los_base
 * Get a UINT64 value from addr.
 */
#define GET_UINT64(addr)               (*((volatile UINT64 *)(addr)))

/**
 * @ingroup los_base
 * Write a UINT8 value to addr.
 */
#define WRITE_UINT8(value, addr)     (*((volatile UINT8 *)(addr)) = (value))
/**
 * @ingroup los_base
 * Write a UINT16 value to addr.
 */
#define WRITE_UINT16(value, addr)    (*((volatile UINT16 *)(addr)) = (value))
/**
 * @ingroup los_base
 * Write a UINT32 value to addr.
 */
#define WRITE_UINT32(value, addr)    (*((volatile UINT32 *)(addr)) = (value))
/**
 * @ingroup los_base
 * Write a UINT64 addr to addr.
 */
#define WRITE_UINT64(value, addr) (*((volatile UINT64 *)(addr)) = (value))

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_REG_H */
