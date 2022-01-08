/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
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
#ifndef IT_LOS_HWI_H
#define IT_LOS_HWI_H

#include "osTest.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef __RISC_V__
#define OS_USER_HWI_MAX (26)
#define OS_HWI_MAX_USED_NUM (26)
#define OS_USER_HWI_MIN OS_RISCV_SYS_VECTOR_CNT
#else
#define OS_USER_HWI_MAX 10
#define OS_HWI_MAX_USED_NUM 10
#define OS_USER_HWI_MIN 0
#endif

extern VOID ItLosHwi001(VOID);
extern VOID ItLosHwi002(VOID);
extern VOID ItLosHwi003(VOID);
extern VOID ItLosHwi004(VOID);
extern VOID ItLosHwi005(VOID);
extern VOID ItLosHwi006(VOID);
extern VOID ItLosHwi007(VOID);
extern VOID ItLosHwi008(VOID);
extern VOID ItLosHwi009(VOID);
extern VOID ItLosHwi010(VOID);
extern VOID ItLosHwi011(VOID);
extern VOID ItLosHwi012(VOID);
extern VOID ItLosHwi013(VOID);
extern VOID ItLosHwi014(VOID);
extern VOID ItLosHwi015(VOID);
extern VOID ItLosHwi016(VOID);
extern VOID ItLosHwi017(VOID);
extern VOID ItLosHwi018(VOID);
extern VOID ItLosHwi019(VOID);
extern VOID ItLosHwi020(VOID);
extern VOID ItLosHwi021(VOID);
extern VOID ItLosHwi022(VOID);
extern VOID ItLosHwi023(VOID);
extern VOID ItLosHwi024(VOID);
extern VOID ItLosHwi025(VOID);
extern VOID ItLosHwi026(VOID);
extern VOID ItLosHwi027(VOID);
extern VOID ItLosHwi028(VOID);
extern VOID ItLosHwi029(VOID);
extern VOID ItLosHwi030(VOID);
extern VOID ItLosHwi031(VOID);
extern VOID ItLosHwi032(VOID);
extern VOID ItLosHwi033(VOID);
extern VOID ItLosHwi034(VOID);
extern VOID LltLosHwi035(VOID);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* IT_LOS_HWI_H */

