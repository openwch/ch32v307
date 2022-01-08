
#include "It_los_queue.h"



#define LOS_MEMBOX_MAGIC_SIZE 4
static UINT32 g_uwQueueID122 = 0;
static unsigned char g_aucMailBoxPool[40 + LOS_MEMBOX_MAGIC_SIZE] = {0};
static VOID *g_pMailBox = NULL;
static BOOL  g_bTaskFinish1 = FALSE;
static BOOL  g_bTaskFinish2 = FALSE;


static VOID StTaskAllocWait(VOID)
{
    UINT32 ret;
    VOID *memBox = NULL;

    memBox = OsQueueMailAlloc(g_uwQueueID122, (VOID *)g_aucMailBoxPool, 20); // 20, set timeout
    ICUNIT_GOTO_EQUAL(memBox, NULL, memBox, EXIT);

    memBox = OsQueueMailAlloc(g_uwQueueID122, (VOID *)g_aucMailBoxPool, 50); // 50, set timeout
    ICUNIT_GOTO_NOT_EQUAL(memBox, NULL, memBox, EXIT);

    ret = OsQueueMailFree(g_uwQueueID122, (VOID *)g_aucMailBoxPool, memBox);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

EXIT:
    g_bTaskFinish1 = TRUE;
    return;
}


static VOID StTaskAllocNoWait(VOID)
{
    UINT32 ret;
    VOID *memBox = NULL;

    memBox = OsQueueMailAlloc(g_uwQueueID122, (VOID *)g_aucMailBoxPool, LOS_NO_WAIT);
    ICUNIT_GOTO_EQUAL(memBox, NULL, memBox, EXIT);

EXIT:
    g_bTaskFinish2 = TRUE;
    return;
}


static UINT32 Testcase(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1221 = {0};
    TSK_INIT_PARAM_S task1222 = {0};
    UINT32 taskID1221, taskID1222;
    QUEUE_INFO_S queueInfo;
    VOID *memBox = NULL;

    task1221.pfnTaskEntry = (TSK_ENTRY_FUNC)StTaskAllocWait;
    task1221.pcName       = "TskName122_1";
    task1221.uwStackSize  = TASK_STACK_SIZE_TEST;
    task1221.usTaskPrio   = 23; // 23, Set the priority according to the task purpose,a smaller number means a higher priority.
    task1221.uwResved     = LOS_TASK_STATUS_DETACHED;

    task1222.pfnTaskEntry = (TSK_ENTRY_FUNC)StTaskAllocNoWait;
    task1222.pcName       = "TskName122_2";
    task1222.uwStackSize  = TASK_STACK_SIZE_TEST;
    task1222.usTaskPrio   = 23; // 23, Set the priority according to the task purpose,a smaller number means a higher priority.
    task1222.uwResved     = LOS_TASK_STATUS_DETACHED;

    ret = LOS_QueueCreate("Q122", 1, &g_uwQueueID122, 0, QUEUE_BASE_MSGSIZE);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_MemboxInit((VOID *)g_aucMailBoxPool, sizeof(g_aucMailBoxPool) * 1, sizeof(g_aucMailBoxPool));
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    memBox = OsQueueMailAlloc(g_uwQueueID122, (VOID *)g_aucMailBoxPool, LOS_NO_WAIT);
    ICUNIT_GOTO_NOT_EQUAL(memBox, NULL, memBox, EXIT);

    ret = LOS_TaskCreate(&taskID1221, &task1221);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_TaskCreate(&taskID1222, &task1222);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    /* make sure the first OsQueueMailAlloc is failed of task1 and task2. */
    LOS_TaskDelay(10); // 10, set delay time

    ret = LOS_QueueInfoGet(g_uwQueueID122, &queueInfo);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_QueueDelete(g_uwQueueID122);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_QUEUE_IN_TSKUSE, ret);

    LOS_TaskDelay(20); // 20, set delay time

    ret = OsQueueMailFree(g_uwQueueID122, (VOID *)g_aucMailBoxPool, memBox);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    /* keep waitting until task1 and task2 exit. */
    while (!(g_bTaskFinish1 && g_bTaskFinish2)) {
        LOS_TaskDelay(5); // 5, set delay time
    }

EXIT:
    ret = LOS_QueueDelete(g_uwQueueID122);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    return LOS_OK;
}

VOID LltLosQueue003(VOID)
{
    TEST_ADD_CASE("LltLosQueue003", Testcase, TEST_LOS, TEST_QUE, TEST_LEVEL1, TEST_FUNCTION);
}


