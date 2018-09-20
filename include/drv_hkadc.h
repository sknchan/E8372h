

#ifndef __DRV_HKADC_H__
#define __DRV_HKADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_comm.h"


/*****************************************************************************
* 函 数 名  : BSP_HKADC_PaValueGet
*
* 功能描述  :
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_HKADC_PaValueGet( BSP_U16 *pusValue );


/*************************************************
 函 数 名   : DRV_HKADC_BAT_VOLT_GET
 功能描述   : 返回当前电池电压值
 输入参数   : pslData : 电池电压值
 输出参数   : pslData : 电池电压值
 返 回 值   :0:获取成功
            -1:获取失败
*************************************************/
BSP_S32 DRV_HKADC_BAT_VOLT_GET(BSP_S32 *ps32Data);


/*****************************************************************************
 函 数 名  : hkadcBatADCRead
 功能描述  : 获取电池电压采样值
 输入参数  : 无
 输出参数  : pTemp：        指向电池温度的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
BSP_S32 DRV_GET_BATTERY_ADC(BSP_S32 * pslData);

#ifdef __cplusplus
}
#endif
#endif

