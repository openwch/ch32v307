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
#ifndef IT_LOS_SWTMR_H
#define IT_LOS_SWTMR_H

#include "osTest.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define SELF_DELETED 0
#define SYS_EXIST_SWTMR 1
#define TEST_HWI_RUNTIME 0x100000

extern EVENT_CB_S g_eventCB0;
extern EVENT_CB_S g_eventCB1;
extern EVENT_CB_S g_eventCB2;
extern EVENT_CB_S g_eventCB3;

#define TIMER_LOS_EXPIRATION3 10
#define TIMER_LOS_EXPIRATION1 4

#define TIMER_LOS_HANDLER_PARAMETER 0xFF
#define TIMER_LOS_SELF_DELETED 1

extern UINT32 g_idleTaskID;
static UINT32 g_usSwTmrMaxNum;

static UINT32 g_swtmrId1;
static UINT32 g_swtmrId2;
static UINT32 g_swtmrId3;

static UINT32 g_swtmrCountA;
static UINT32 g_swtmrCountB;
static UINT32 g_swtmrCountC;

static UINT32 g_uwsTick1;
static UINT32 g_uwsTick2;
static UINT32 g_uwsTick3;

extern VOID LOS_GetCpuTick(UINT32 *puwCntHi, UINT32 *puwCntLo);
extern VOID ItSuiteLosSwtmr();
extern VOID ItLosSwtmr001(VOID);
extern VOID ItLosSwtmr002(VOID);
extern VOID ItLosSwtmr003(VOID);
extern VOID ItLosSwtmr004(VOID);
extern VOID ItLosSwtmr005(VOID);
extern VOID ItLosSwtmr006(VOID);
extern VOID ItLosSwtmr007(VOID);
extern VOID ItLosSwtmr008(VOID);
extern VOID ItLosSwtmr009(VOID);
extern VOID ItLosSwtmr010(VOID);
extern VOID ItLosSwtmr011(VOID);
extern VOID ItLosSwtmr012(VOID);
extern VOID ItLosSwtmr013(VOID);
extern VOID ItLosSwtmr014(VOID);
extern VOID ItLosSwtmr015(VOID);
extern VOID ItLosSwtmr016(VOID);
extern VOID ItLosSwtmr017(VOID);
extern VOID ItLosSwtmr018(VOID);
extern VOID ItLosSwtmr019(VOID);
extern VOID ItLosSwtmr020(VOID);
extern VOID ItLosSwtmr021(VOID);
extern VOID ItLosSwtmr022(VOID);
extern VOID ItLosSwtmr023(VOID);
extern VOID ItLosSwtmr024(VOID);
extern VOID ItLosSwtmr025(VOID);
extern VOID ItLosSwtmr026(VOID);
extern VOID ItLosSwtmr027(VOID);
extern VOID ItLosSwtmr028(VOID);
extern VOID ItLosSwtmr029(VOID);
extern VOID ItLosSwtmr030(VOID);
extern VOID ItLosSwtmr031(VOID);
extern VOID ItLosSwtmr032(VOID);
extern VOID ItLosSwtmr033(VOID);
extern VOID ItLosSwtmr034(VOID);
extern VOID ItLosSwtmr035(VOID);
extern VOID ItLosSwtmr036(VOID);
extern VOID ItLosSwtmr037(VOID);
extern VOID ItLosSwtmr038(VOID);
extern VOID ItLosSwtmr039(VOID);
extern VOID ItLosSwtmr040(VOID);
extern VOID ItLosSwtmr041(VOID);
extern VOID ItLosSwtmr042(VOID);
extern VOID ItLosSwtmr043(VOID);
extern VOID ItLosSwtmr044(VOID);
extern VOID ItLosSwtmr045(VOID);
extern VOID ItLosSwtmr046(VOID);
extern VOID ItLosSwtmr047(VOID);
extern VOID ItLosSwtmr048(VOID);
extern VOID ItLosSwtmr049(VOID);
extern VOID ItLosSwtmr050(VOID);
extern VOID ItLosSwtmr051(VOID);
extern VOID ItLosSwtmr052(VOID);
extern VOID ItLosSwtmr053(VOID);
extern VOID ItLosSwtmr054(VOID);
extern VOID ItLosSwtmr055(VOID);
extern VOID ItLosSwtmr056(VOID);
extern VOID ItLosSwtmr057(VOID);
extern VOID ItLosSwtmr058(VOID);
extern VOID ItLosSwtmr059(VOID);
extern VOID ItLosSwtmr060(VOID);
extern VOID ItLosSwtmr061(VOID);
extern VOID ItLosSwtmr062(VOID);
extern VOID ItLosSwtmr063(VOID);
extern VOID ItLosSwtmr064(VOID);
extern VOID ItLosSwtmr065(VOID);
extern VOID ItLosSwtmr066(VOID);
extern VOID ItLosSwtmr067(VOID);
extern VOID ItLosSwtmr068(VOID);
extern VOID ItLosSwtmr069(VOID);
extern VOID ItLosSwtmr071(VOID);
extern VOID ItLosSwtmr072(VOID);
extern VOID ItLosSwtmr073(VOID);
extern VOID ItLosSwtmr074(VOID);
extern VOID ItLosSwtmr075(VOID);
extern VOID ItLosSwtmr076(VOID);
extern VOID ItLosSwtmr077(VOID);
extern VOID ItLosSwtmr078(VOID);

extern VOID ItLosSwtmrAlign001(VOID);
extern VOID ItLosSwtmrAlign002(VOID);
extern VOID ItLosSwtmrAlign003(VOID);
extern VOID ItLosSwtmrAlign004(VOID);
extern VOID ItLosSwtmrAlign005(VOID);
extern VOID ItLosSwtmrAlign006(VOID);
extern VOID ItLosSwtmrAlign007(VOID);
extern VOID ItLosSwtmrAlign008(VOID);
extern VOID ItLosSwtmrAlign009(VOID);
extern VOID ItLosSwtmrAlign010(VOID);
extern VOID ItLosSwtmrAlign011(VOID);
extern VOID ItLosSwtmrAlign012(VOID);
extern VOID ItLosSwtmrAlign013(VOID);
extern VOID ItLosSwtmrAlign014(VOID);
extern VOID ItLosSwtmrAlign015(VOID);
extern VOID ItLosSwtmrAlign016(VOID);
extern VOID ItLosSwtmrAlign017(VOID);
extern VOID ItLosSwtmrAlign018(VOID);
extern VOID ItLosSwtmrAlign019(VOID);
extern VOID ItLosSwtmrAlign020(VOID);
extern VOID ItLosSwtmrAlign021(VOID);
extern VOID ItLosSwtmrAlign022(VOID);
extern VOID ItLosSwtmrAlign023(VOID);
extern VOID ItLosSwtmrAlign024(VOID);
extern VOID ItLosSwtmrAlign025(VOID);
extern VOID ItLosSwtmrAlign026(VOID);
extern VOID ItLosSwtmrAlign027(VOID);
extern VOID ItLosSwtmrAlign028(VOID);
extern VOID ItLosSwtmrAlign029(VOID);
extern VOID ItLosSwtmrAlign030(VOID);
extern VOID ItLosSwtmrAlign031(VOID);
extern VOID ItLosSwtmrDelay001(VOID);
extern VOID ItLosSwtmrDelay002(VOID);
extern VOID ItLosSwtmrDelay003(VOID);
extern VOID ItLosSwtmrDelay004(VOID);
extern VOID ItLosSwtmrDelay005(VOID);


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* IT_LOS_SWTMR_H */