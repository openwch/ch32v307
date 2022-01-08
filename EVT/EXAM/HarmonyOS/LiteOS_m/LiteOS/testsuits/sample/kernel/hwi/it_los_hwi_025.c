
#include "osTest.h" 
#include "it_los_hwi.h"


static VOID HwiF01(VOID)
{
    UINT32 ret;

    TestHwiClear(HWI_NUM_TEST);

    ret = LOS_SemPend(g_usSemID, LOS_WAIT_FOREVER);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);
    ret = LOS_SemPend(g_usSemID, LOS_WAIT_FOREVER);
    ICUNIT_GOTO_EQUAL(ret, LOS_ERRNO_SEM_PEND_INTERR, ret, EXIT);

    return;

EXIT:
    ret = LOS_SemDelete(g_usSemID);
    ICUNIT_ASSERT_EQUAL_VOID(ret, LOS_OK, ret);

    return;
}

static UINT32 Testcase(VOID)
{
    UINT32           ret;
    UINT32           semCount  = 1;
    HWI_PRIOR_T      hwiPrio = 7;
    HWI_MODE_T       mode    = 0;
    HWI_ARG_T        arg     = 0;

    ret = LOS_SemCreate(semCount, &g_usSemID);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    ret = LOS_HwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)HwiF01, arg);
    ICUNIT_GOTO_EQUAL(ret, LOS_OK, ret, EXIT);

    TestHwiTrigger(HWI_NUM_TEST);

    TestHwiDelete(HWI_NUM_TEST);

EXIT:
    ret = LOS_SemDelete(g_usSemID);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    return LOS_OK;
}
 /*
**********
Testcase brief in English
**********
*/

VOID ItLosHwi025(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosHwi025", Testcase, TEST_LOS, TEST_HWI, TEST_LEVEL2, TEST_FUNCTION);
} 
        
