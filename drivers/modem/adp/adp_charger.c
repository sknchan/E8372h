
#include <osl_types.h>
#include "drv_chg.h"
/*****************************************************************************
 函 数 名  : BSP_CHG_ChargingStatus
 功能描述  :查询目前是否正在充电中
 输入参数  :无
 输出参数  :无
 返回值：   0 未充电
                          1 充电中
*****************************************************************************/
int DRV_CHG_GET_CHARGING_STATUS(void)
{
    return -1;
}

/*****************************************************************************
 函 数 名  : BSP_CHG_StateGet
 功能描述  :chg_state_set(UINT32 ulstate)在A核的映射函数,查询是否需要补电
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
int BSP_CHG_StateGet(void)
{
    return -1;
}
/*****************************************************************************
 函 数 名  : BSP_CHG_GetCbcState
 功能描述  : 返回电池状态和电量
 输入参数  : pusBcs 0:电池在供电 1:与电池连接电池未供电 2:没有与电池连接
             pucBcl  0:电量不足或没有与电池连接
 输出参数  : pusBcs 0:电池在供电 1:与电池连接电池未供电 2:没有与电池连接
             pucBcl  0:电量不足或没有与电池连接
 返回值：    0 操作成功
             -1操作失败
*****************************************************************************/
int DRV_CHG_GET_CBC_STATE(unsigned char *pusBcs,unsigned char *pucBcl)
{
    return -1;
}


/*****************************************************************************
 函 数 名  : DRV_CHG_GET_BATTERY_STATE
 功能描述  : 获取底层电池状态信息
 输入参数  : battery_state 电量信息
 输出参数  : battery_state 电量信息
 返回值：    0 操作成功
             -1操作失败
*****************************************************************************/
int DRV_CHG_GET_BATTERY_STATE(BATT_STATE_T *bttery_state)
{
    return -1;
}

/*****************************************************************************
 函 数 名  : BSP_CHG_Sply
 功能描述  :chg_state_set(UINT32 ulstate)在A核的映射函数,查询是否需要补电
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
int BSP_CHG_Sply( void)
{
    return BSP_ERROR;
}

/*****************************************************************************
 函 数 名  : DRV_CHG_STATE_SET
 功能描述  : 使能或者禁止充电
 输入参数  : ulState    0:禁止充电
 						1:使能充电
 输出参数  : 无
 返回值：    无
*****************************************************************************/
void  DRV_CHG_STATE_SET(unsigned long ulState)
{

}
