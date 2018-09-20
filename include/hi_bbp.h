
#ifndef __HI_BBP_H__
#define __HI_BBP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* this macro is used to handle the diff betwen V711 and v7r2  */
#define BBP_V711
#define BBP_SINGLE_MODEM

/*控制rf_resetn信号的保存恢复是否需要在M3上操作,回片后调试*/
#define RF_RESETN_MSAVE
#ifdef __cplusplus
}
#endif

#endif

