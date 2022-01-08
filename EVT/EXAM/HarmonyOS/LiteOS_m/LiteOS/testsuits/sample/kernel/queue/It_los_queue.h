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
#ifndef IT_LOS_QUEUE_H
#define IT_LOS_QUEUE_H
#include "osTest.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define QUEUE_SHORT_BUFFER_LENGTH 12
#define QUEUE_STANDARD_BUFFER_LENGTH 50
#define QUEUE_BASE_NUM 3
#define QUEUE_BASE_MSGSIZE 8

#define PER_ADDED_VALUE 1

extern UINT32 g_testTaskID01;
extern UINT32 g_testTaskID02;
extern UINT32 g_testQueueID01;
extern UINT32 g_testQueueID02;
extern UINT32 g_testQueueID03;

extern VOID ItLosQueue001(VOID);
extern VOID ItLosQueue002(VOID);
extern VOID ItLosQueue003(VOID);
extern VOID ItLosQueue004(VOID);
extern VOID ItLosQueue005(VOID);
extern VOID ItLosQueue006(VOID);
extern VOID ItLosQueue007(VOID);
extern VOID ItLosQueue008(VOID);
extern VOID ItLosQueue009(VOID);
extern VOID ItLosQueue010(VOID);
extern VOID ItLosQueue011(VOID);
extern VOID ItLosQueue012(VOID);
extern VOID ItLosQueue013(VOID);
extern VOID ItLosQueue014(VOID);
extern VOID ItLosQueue015(VOID);
extern VOID ItLosQueue016(VOID);
extern VOID ItLosQueue017(VOID);
extern VOID ItLosQueue018(VOID);
extern VOID ItLosQueue019(VOID);
extern VOID ItLosQueue020(VOID);
extern VOID ItLosQueue021(VOID);
extern VOID ItLosQueue022(VOID);
extern VOID ItLosQueue025(VOID);
extern VOID ItLosQueue026(VOID);
extern VOID ItLosQueue027(VOID);
extern VOID ItLosQueue028(VOID);
extern VOID ItLosQueue029(VOID);
extern VOID ItLosQueue030(VOID);
extern VOID ItLosQueue031(VOID);
extern VOID ItLosQueue032(VOID);
extern VOID ItLosQueue033(VOID);
extern VOID ItLosQueue034(VOID);
extern VOID ItLosQueue035(VOID);
extern VOID ItLosQueue036(VOID);
extern VOID ItLosQueue037(VOID);
extern VOID ItLosQueue038(VOID);
extern VOID ItLosQueue039(VOID);
extern VOID ItLosQueue040(VOID);
extern VOID ItLosQueue041(VOID);
extern VOID ItLosQueue042(VOID);
extern VOID ItLosQueue043(VOID);
extern VOID ItLosQueue044(VOID);
extern VOID ItLosQueue045(VOID);
extern VOID ItLosQueue046(VOID);
extern VOID ItLosQueue047(VOID);
extern VOID ItLosQueue048(VOID);
extern VOID ItLosQueue050(VOID);
extern VOID ItLosQueue051(VOID);
extern VOID ItLosQueue052(VOID);
extern VOID ItLosQueue053(VOID);
extern VOID ItLosQueue054(VOID);
extern VOID ItLosQueue055(VOID);
extern VOID ItLosQueue056(VOID);
extern VOID ItLosQueue057(VOID);
extern VOID ItLosQueue058(VOID);
extern VOID ItLosQueue059(VOID);
extern VOID ItLosQueue060(VOID);
extern VOID ItLosQueue061(VOID);
extern VOID ItLosQueue062(VOID);
extern VOID ItLosQueue063(VOID);
extern VOID ItLosQueue064(VOID);
extern VOID ItLosQueue065(VOID);
extern VOID ItLosQueue066(VOID);
extern VOID ItLosQueue067(VOID);
extern VOID ItLosQueue068(VOID);
extern VOID ItLosQueue069(VOID);
extern VOID ItLosQueue070(VOID);
extern VOID ItLosQueue071(VOID);
extern VOID ItLosQueue072(VOID);
extern VOID ItLosQueue073(VOID);
extern VOID ItLosQueue074(VOID);
extern VOID ItLosQueue078(VOID);
extern VOID ItLosQueue079(VOID);
extern VOID ItLosQueue080(VOID);
extern VOID ItLosQueue081(VOID);
extern VOID ItLosQueue082(VOID);
extern VOID ItLosQueue083(VOID);
extern VOID ItLosQueue084(VOID);
extern VOID ItLosQueue085(VOID);
extern VOID ItLosQueue086(VOID);
extern VOID ItLosQueue087(VOID);
extern VOID ItLosQueue088(VOID);
extern VOID ItLosQueue090(VOID);
extern VOID ItLosQueue091(VOID);
extern VOID ItLosQueue092(VOID);
extern VOID ItLosQueue093(VOID);
extern VOID ItLosQueue094(VOID);
extern VOID ItLosQueue095(VOID);
extern VOID ItLosQueue096(VOID);
extern VOID ItLosQueue097(VOID);
extern VOID ItLosQueue103(VOID);
extern VOID ItLosQueue104(VOID);
extern VOID ItLosQueue106(VOID);
extern VOID ItLosQueue107(VOID);
extern VOID ItLosQueue108(VOID);
extern VOID ItLosQueue109(VOID);
extern VOID ItLosQueue110(VOID);
extern VOID ItLosQueue114(VOID);
extern VOID ItLosQueueHead001(VOID);
extern VOID ItLosQueueHead002(VOID);
extern VOID ItLosQueueHead003(VOID);
extern VOID ItLosQueueHead004(VOID);
extern VOID ItLosQueueHead005(VOID);
extern VOID ItLosQueueHead006(VOID);
extern VOID ItLosQueueHead007(VOID);
extern VOID ItLosQueueHead008(VOID);
extern VOID ItLosQueueHead009(VOID);
extern VOID ItLosQueueHead010(VOID);
extern VOID ItLosQueueHead011(VOID);
extern VOID ItLosQueueHead012(VOID);
extern VOID ItLosQueueHead013(VOID);
extern VOID ItLosQueueHead014(VOID);
extern VOID ItLosQueueHead015(VOID);
extern VOID ItLosQueueHead016(VOID);
extern VOID ItLosQueueHead019(VOID);
extern VOID ItLosQueueHead020(VOID);
extern VOID ItLosQueueHead021(VOID);
extern VOID ItLosQueueHead022(VOID);
extern VOID ItLosQueueHead023(VOID);
extern VOID ItLosQueueHead024(VOID);
extern VOID ItLosQueueHead025(VOID);
extern VOID ItLosQueueHead026(VOID);
extern VOID ItLosQueueHead027(VOID);
extern VOID ItLosQueueHead028(VOID);
extern VOID ItLosQueueHead029(VOID);
extern VOID ItLosQueueHead030(VOID);
extern VOID ItLosQueueHead031(VOID);
extern VOID ItLosQueueHead032(VOID);
extern VOID ItLosQueueHead038(VOID);
extern VOID ItLosQueueHead039(VOID);
extern VOID ItLosQueueHead040(VOID);
extern VOID ItLosQueueHead041(VOID);
extern VOID ItLosQueueHead042(VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif /* IT_LOS_QUEUE_H */