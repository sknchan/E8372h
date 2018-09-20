



#ifndef REB_CONFIG_H
#define REB_CONFIG_H

#define YES  1
#define NO   0

#define Reb_Platform_9x25 (NO)
#define Reb_Platform_V7R2 (YES)
#define MBB_FEATURE_REB_DFT (YES)
/*系统启动过程不对文件系统操作功能*/
#define MBB_REB_WRITE_PROTECT   (YES)
/*假关机失败进入真关机功能*/
#define MBB_REB_FAST_POWEROFF   (YES)
/*Flash病危块扫描功能*/
#define MBB_REB_BADZONE_SCAN  (NO)
/*Flash病危块回写功能*/
#define MBB_REB_BADZONE_REWRITE (NO)
/*分区写越界保护使能*/
#define MBB_REB_ZONE_WRITE_PROTECT (YES)

#define REB_OS_DEBUG
#if ( YES == Reb_Platform_9x25)
#include <linux/huawei_feature.h>
#ifdef  CONFIG_MBB_FAST_ON_OFF
#define MBB_FAST_ON_OFF FEATURE_ON
#endif
#endif
#if ( YES == Reb_Platform_V7R2)
#include <product_config.h>
#endif
#endif
