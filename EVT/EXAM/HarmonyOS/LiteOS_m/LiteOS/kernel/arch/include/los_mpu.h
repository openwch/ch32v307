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
 * @defgroup memory protection
 * @ingroup kernel
 */

#ifndef _LOS_MPU_H
#define _LOS_MPU_H

#include "los_compiler.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    MPU_RW_BY_PRIVILEGED_ONLY = 0,
    MPU_RW_ANY = 1,
    MPU_RO_BY_PRIVILEGED_ONLY = 2,
    MPU_RO_ANY = 3,
} MpuAccessPermission;

typedef enum {
    MPU_EXECUTABLE = 0,
    MPU_NON_EXECUTABLE = 1,
} MpuExecutable;

typedef enum {
    MPU_NO_SHARE = 0,
    MPU_SHARE = 1,
} MpuShareability;

typedef enum {
    MPU_MEM_ON_CHIP_ROM = 0,
    MPU_MEM_ON_CHIP_RAM = 1,
    MPU_MEM_XIP_PSRAM = 2,
    MPU_MEM_XIP_NOR_FLASH = 3,
    MPU_MEM_SHARE_MEM = 4,
} MpuMemType;

typedef struct {
    UINT32 baseAddr;
    UINT64 size; /* armv7 size == 2^x (5 <= x <= 32)  128B - 4GB */
    MpuAccessPermission permission;
    MpuExecutable executable;
    MpuShareability shareability;
    MpuMemType memType;
} MPU_CFG_PARA;

VOID HalMpuEnable(UINT32 defaultRegionEnable);
VOID HalMpuDisable();
UINT32 HalMpuSetRegion(UINT32 regionId, MPU_CFG_PARA *para);
UINT32 HalMpuDisableRegion(UINT32 regionId);
INT32 HalMpuUnusedRegionGet(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_MPU_H */
