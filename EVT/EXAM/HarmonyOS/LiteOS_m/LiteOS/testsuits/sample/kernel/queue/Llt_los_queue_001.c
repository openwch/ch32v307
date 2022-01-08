
#include "It_los_queue.h"



static VOID TaskF01(VOID)
{
    UINT32 ret;
    CHAR   buff1[QUEUE_SHORT_BUFFER_LENGTH] = "UniDSP";
    CHAR   buff2[QUEUE_SHORT_BUFFER_LENGTH] = " ";

    ret = LOS_QueueRead(g_testQueueID01, &buff2, QUEUE_BASE_MSGSIZE, LOS_WAIT_FOREVER);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_QueueWrite(g_testQueueID01, &buff1, QUEUE_BASE_MSGSIZE, 0);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_QueueWrite(g_testQueueID01, &buff1, QUEUE_BASE_MSGSIZE, LOS_WAIT_FOREVER);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

EXIT:
    ret = LOS_QueueDelete(g_testQueueID01);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    ret = LOS_TaskDelete(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    return;
}

static UINT32 Testcase(VOID)
{
    UINT32 ret;
    QUEUE_INFO_S queueInfo;
    UINT32 i;
    UINT32 queueID[LOSCFG_BASE_IPC_QUEUE_LIMIT + 1];
    CHAR   buff1[QUEUE_SHORT_BUFFER_LENGTH] = "UniDSP";
    CHAR   buff2[QUEUE_SHORT_BUFFER_LENGTH] = " ";

    TSK_INIT_PARAM_S task1 = {0};
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.uwStackSize  = TASK_STACK_SIZE_TEST;
    task1.pcName       = "TskTstA";
    task1.usTaskPrio   = TASK_PRIO_TEST - 2; // TASK_PRIO_TEST - 2, Set the priority according to the task purpose,a smaller number means a higher priority.
    task1.uwResved   = LOS_TASK_STATUS_DETACHED;

    ret = LOS_QueueInfoGet(0, NULL);
    ICUNIT_ASSERT_EQUAL(ret, LOS_ERRNO_QUEUE_PTR_NULL, ret);

    ret = LOS_QueueCreate("Q1", 1, &g_testQueueID01, 0, QUEUE_BASE_MSGSIZE);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    ret = LOS_TaskCreate(&g_testTaskID01, &task1);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT1);

    ret = LOS_QueueInfoGet(g_testQueueID01, &queueInfo);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);
    ICUNIT_GOTO_EQUAL(queueInfo.waitReadTask, (1 << g_testTaskID01), queueInfo.waitReadTask, EXIT2);

    ret = LOS_QueueWrite(g_testQueueID01, &buff1, QUEUE_BASE_MSGSIZE, 0);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);

    ret = LOS_QueueInfoGet(g_testQueueID01, &queueInfo);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT2);
    ICUNIT_GOTO_EQUAL(queueInfo.waitWriteTask, (1 << g_testTaskID01), queueInfo.waitWriteTask, EXIT2);

EXIT2:
    ret = LOS_TaskDelete(g_testTaskID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
EXIT1:
    ret = LOS_QueueDelete(g_testQueueID01);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);
EXIT:
    return LOS_OK;
}

VOID LlTLosQueue001(VOID)
{
    TEST_ADD_CASE("LlTLosQueue001", Testcase, TEST_LOS, TEST_QUE, TEST_LEVEL0, TEST_FUNCTION);
}

