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

#include "los_sortlink.h"
#include "los_sched.h"
#include "los_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

STATIC SortLinkAttribute g_taskSortLink;
STATIC SortLinkAttribute g_swtmrSortLink;

UINT32 OsSortLinkInit(SortLinkAttribute *sortLinkHeader)
{
    LOS_ListInit(&sortLinkHeader->sortLink);
    return LOS_OK;
}

STATIC INLINE VOID OsAddNode2SortLink(SortLinkAttribute *sortLinkHeader, SortLinkList *sortList)
{
    LOS_DL_LIST *head = (LOS_DL_LIST *)&sortLinkHeader->sortLink;

    if (LOS_ListEmpty(head)) {
        LOS_ListAdd(head, &sortList->sortLinkNode);
        return;
    }

    SortLinkList *listSorted = LOS_DL_LIST_ENTRY(head->pstNext, SortLinkList, sortLinkNode);
    if (listSorted->responseTime > sortList->responseTime) {
        LOS_ListAdd(head, &sortList->sortLinkNode);
        return;
    } else if (listSorted->responseTime == sortList->responseTime) {
        LOS_ListAdd(head->pstNext, &sortList->sortLinkNode);
        return;
    }

    LOS_DL_LIST *prevNode = head->pstPrev;
    do {
        listSorted = LOS_DL_LIST_ENTRY(prevNode, SortLinkList, sortLinkNode);
        if (listSorted->responseTime <= sortList->responseTime) {
            LOS_ListAdd(prevNode, &sortList->sortLinkNode);
            break;
        }

        prevNode = prevNode->pstPrev;
    } while (1);
}

VOID OsDeleteNodeSortLink(SortLinkAttribute *sortLinkHeader, SortLinkList *sortList)
{
    LOS_ListDelete(&sortList->sortLinkNode);
    SET_SORTLIST_VALUE(sortList, OS_SORT_LINK_INVALID_TIME);
}

STATIC INLINE UINT64 OsGetSortLinkNextExpireTime(SortLinkAttribute *sortHeader, UINT64 startTime)
{
    UINT64 expirTime = 0;
    UINT64 nextExpirTime = 0;
    LOS_DL_LIST *head = &sortHeader->sortLink;
    LOS_DL_LIST *list = head->pstNext;

    if (LOS_ListEmpty(head)) {
        return OS_SCHED_MAX_RESPONSE_TIME - OS_CYCLE_PER_TICK;
    }

    do {
        SortLinkList *listSorted = LOS_DL_LIST_ENTRY(list, SortLinkList, sortLinkNode);
        if (listSorted->responseTime <= startTime) {
            expirTime = startTime;
            list = list->pstNext;
        } else {
            nextExpirTime = listSorted->responseTime;
            break;
        }
    } while (list != head);

    if (expirTime == 0) {
        return nextExpirTime;
    }

    if (nextExpirTime == 0) {
        return expirTime;
    }

    if ((nextExpirTime - expirTime) <= OS_US_PER_TICK) {
        return nextExpirTime;
    }

    return expirTime;
}

VOID OsAdd2SortLink(SortLinkList *node, UINT64 startTime, UINT32 waitTicks, SortLinkType type)
{
    UINT32 intSave;
    SortLinkAttribute *sortLinkHeader = NULL;

    if (type == OS_SORT_LINK_TASK) {
        sortLinkHeader = &g_taskSortLink;
    } else if (type == OS_SORT_LINK_SWTMR) {
        sortLinkHeader = &g_swtmrSortLink;
    } else {
        LOS_Panic("Sort link type error : %u\n", type);
    }

    intSave = LOS_IntLock();
    SET_SORTLIST_VALUE(node, startTime + (UINT64)waitTicks * OS_CYCLE_PER_TICK);
    OsAddNode2SortLink(sortLinkHeader, node);
    LOS_IntRestore(intSave);
}

VOID OsDeleteSortLink(SortLinkList *node, SortLinkType type)
{
    UINT32 intSave;
    SortLinkAttribute *sortLinkHeader = NULL;

    if (type == OS_SORT_LINK_TASK) {
        sortLinkHeader = &g_taskSortLink;
    } else if (type == OS_SORT_LINK_SWTMR) {
        sortLinkHeader = &g_swtmrSortLink;
    } else {
        LOS_Panic("Sort link type error : %u\n", type);
    }

    intSave = LOS_IntLock();
    if (node->responseTime != OS_SORT_LINK_INVALID_TIME) {
        OsDeleteNodeSortLink(sortLinkHeader, node);
    }
    LOS_IntRestore(intSave);
}

SortLinkAttribute *OsGetSortLinkAttribute(SortLinkType type)
{
    if (type == OS_SORT_LINK_TASK) {
        return &g_taskSortLink;
    } else if (type == OS_SORT_LINK_SWTMR) {
        return &g_swtmrSortLink;
    }

    PRINT_ERR("Invalid sort link type!\n");
    return NULL;
}

UINT64 OsGetNextExpireTime(UINT64 startTime)
{
    UINT32 intSave;
    SortLinkAttribute *taskHeader = &g_taskSortLink;
    SortLinkAttribute *swtmrHeader = &g_swtmrSortLink;

    intSave = LOS_IntLock();
    UINT64 taskExpirTime = OsGetSortLinkNextExpireTime(taskHeader, startTime);
    UINT64 swtmrExpirTime = OsGetSortLinkNextExpireTime(swtmrHeader, startTime);
    LOS_IntRestore(intSave);

    return (taskExpirTime < swtmrExpirTime) ? taskExpirTime : swtmrExpirTime;
}

UINT32 OsSortLinkGetTargetExpireTime(UINT64 currTime, const SortLinkList *targetSortList)
{
    if (currTime >= targetSortList->responseTime) {
        return 0;
    }

    return (UINT32)(((targetSortList->responseTime - currTime) * LOSCFG_BASE_CORE_TICK_PER_SECOND) / OS_SYS_CLOCK);
}

UINT32 OsSortLinkGetNextExpireTime(const SortLinkAttribute *sortLinkHeader)
{
    LOS_DL_LIST *head = (LOS_DL_LIST *)&sortLinkHeader->sortLink;

    if (LOS_ListEmpty(head)) {
        return 0;
    }

    SortLinkList *listSorted = LOS_DL_LIST_ENTRY(head->pstNext, SortLinkList, sortLinkNode);
    return OsSortLinkGetTargetExpireTime(OsGetCurrSchedTimeCycle(), listSorted);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
