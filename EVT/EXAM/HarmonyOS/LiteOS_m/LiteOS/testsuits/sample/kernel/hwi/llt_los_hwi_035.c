
#include "osTest.h" 
#include "it_los_hwi.h"


static VOID TaskF01(VOID)
{
}
 
VOID ItHwiM3B001F001()
{
    return;
}

UINT32 g_testTaskIdHwi = 0xFFFF;

static UINT32 Testcase(VOID)
{
    UINT32           ret;
    UINT32           loop;
    HWI_PRIOR_T      hwiPrio = 2;
    HWI_MODE_T       mode    = 0;
    HWI_ARG_T        arg     = 0;
    TSK_INIT_PARAM_S task1 = {0};

    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task1.pcName       = "HWI_035";
    task1.usTaskPrio   = 4; // set new task priority is 4
    task1.uwStackSize  = 0x200;
    task1.uwResved   = LOS_TASK_STATUS_DETACHED;

    ret = LOS_TaskCreate(&g_testTaskIdHwi, &task1);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_HwiCreate(HWI_NUM_INT0, hwiPrio, mode, (HWI_PROC_FUNC)ItHwiM3B001F001, arg);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);    

    return LOS_OK;

EXIT:
    LOS_TaskDelete(g_testTaskIdHwi);
    TestHwiDelete(HWI_NUM_TEST);

    return LOS_OK;
}
 /*
**********
Testcase brief in English
**********
*/

VOID LltLosHwi035(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("LltLosHwi035", Testcase, TEST_LOS, TEST_HWI, TEST_LEVEL3, TEST_PRESSURE);
} 
        
