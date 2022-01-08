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

#include "osTest.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define SELF_DELETED 0
#define SYS_EXIST_SWTMR 1

#define TEST_HWI_RUNTIME 0x100000
#define TASK_LOOP_NUM 0x100000

#if (LOSCFG_BASE_CORE_SWTMR == 1)
#define TASK_EXISTED_NUM 3
#else
#define TASK_EXISTED_NUM 2
#endif

#define TASK_EXISTED_D_NUM TASK_EXISTED_NUM
#define TASK_NAME_NUM 10
#define IT_TASK_LOOP 20

extern EVENT_CB_S g_stEventCB;
extern EVENT_CB_S g_stEventCB1;
extern EVENT_CB_S g_stEventCB2;
extern EVENT_CB_S g_stEventCB3;

extern UINT32 g_testTaskID01;
extern UINT32 g_testTaskID02;
extern UINT32 g_testTaskID03;
extern UINT32 g_testQueueID01;
extern UINT32 g_idleTaskID;
extern UINT32 g_testTskHandle;
extern UINT32 g_usSwTmrMaxNum;

extern UINT32 g_leavingTaskNum;

extern VOID ItLosTask001(VOID);
extern VOID ItLosTask002(VOID);
extern VOID ItLosTask003(VOID);
extern VOID ItLosTask004(VOID);
extern VOID ItLosTask005(VOID);
extern VOID ItLosTask006(VOID);
extern VOID ItLosTask007(VOID);
extern VOID ItLosTask008(VOID);
extern VOID ItLosTask010(VOID);
extern VOID ItLosTask011(VOID);
extern VOID ItLosTask012(VOID);
extern VOID ItLosTask013(VOID);
extern VOID ItLosTask014(VOID);
extern VOID ItLosTask015(VOID);
extern VOID ItLosTask016(VOID);
extern VOID ItLosTask017(VOID);
extern VOID ItLosTask018(VOID);
extern VOID ItLosTask019(VOID);
extern VOID ItLosTask020(VOID);
extern VOID ItLosTask021(VOID);
extern VOID ItLosTask022(VOID);
extern VOID ItLosTask023(VOID);
extern VOID ItLosTask024(VOID);
extern VOID ItLosTask025(VOID);
extern VOID ItLosTask026(VOID);
extern VOID ItLosTask027(VOID);
extern VOID ItLosTask028(VOID);
extern VOID ItLosTask029(VOID);
extern VOID ItLosTask030(VOID);
extern VOID ItLosTask031(VOID);
extern VOID ItLosTask032(VOID);
extern VOID ItLosTask033(VOID);
extern VOID ItLosTask034(VOID);
extern VOID ItLosTask035(VOID);
extern VOID ItLosTask036(VOID);
extern VOID ItLosTask037(VOID);
extern VOID ItLosTask038(VOID);
extern VOID ItLosTask039(VOID);
extern VOID ItLosTask040(VOID);
extern VOID ItLosTask041(VOID);
extern VOID ItLosTask042(VOID);
extern VOID ItLosTask043(VOID);
extern VOID ItLosTask046(VOID);
extern VOID ItLosTask047(VOID);
extern VOID ItLosTask048(VOID);
extern VOID ItLosTask049(VOID);
extern VOID ItLosTask050(VOID);
extern VOID ItLosTask051(VOID);
extern VOID ItLosTask052(VOID);
extern VOID ItLosTask053(VOID);
extern VOID ItLosTask054(VOID);
extern VOID ItLosTask055(VOID);
extern VOID ItLosTask056(VOID);
extern VOID ItLosTask057(VOID);
extern VOID ItLosTask058(VOID);
extern VOID ItLosTask059(VOID);
extern VOID ItLosTask060(VOID);
extern VOID ItLosTask061(VOID);
extern VOID ItLosTask062(VOID);
extern VOID ItLosTask063(VOID);
extern VOID ItLosTask064(VOID);
extern VOID ItLosTask065(VOID);
extern VOID ItLosTask066(VOID);
extern VOID ItLosTask067(VOID);
extern VOID ItLosTask068(VOID);
extern VOID ItLosTask069(VOID);
extern VOID ItLosTask070(VOID);
extern VOID ItLosTask071(VOID);
extern VOID ItLosTask072(VOID);
extern VOID ItLosTask073(VOID);
extern VOID ItLosTask074(VOID);
extern VOID ItLosTask075(VOID);
extern VOID ItLosTask076(VOID);
extern VOID ItLosTask077(VOID);
extern VOID ItLosTask078(VOID);
extern VOID ItLosTask079(VOID);
extern VOID ItLosTask080(VOID);
extern VOID ItLosTask081(VOID);
extern VOID ItLosTask082(VOID);
extern VOID ItLosTask083(VOID);
extern VOID ItLosTask085(VOID);
extern VOID ItLosTask086(VOID);
extern VOID ItLosTask087(VOID);
extern VOID ItLosTask088(VOID);
extern VOID ItLosTask089(VOID);
extern VOID ItLosTask090(VOID);
extern VOID ItLosTask092(VOID);
extern VOID ItLosTask093(VOID);
extern VOID ItLosTask094(VOID);
extern VOID ItLosTask095(VOID);
extern VOID ItLosTask097(VOID);
extern VOID ItLosTask098(VOID);
extern VOID ItLosTask099(VOID);
extern VOID ItLosTask100(VOID);
extern VOID ItLosTask101(VOID);
extern VOID ItLosTask102(VOID);
extern VOID ItLosTask103(VOID);
extern VOID ItLosTask104(VOID);
extern VOID ItLosTask105(VOID);
extern VOID ItLosTask106(VOID);
extern VOID ItLosTask107(VOID);
extern VOID ItLosTask108(VOID);
extern VOID ItLosTask109(VOID);
extern VOID ItLosTask110(VOID);
extern VOID ItLosTask111(VOID);
extern VOID ItLosTask112(VOID);
extern VOID ItLosTask113(VOID);
extern VOID ItLosTask114(VOID);
extern VOID ItLosTask115(VOID);
extern VOID ItLosTask116(VOID);
extern VOID ItLosTask117(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
