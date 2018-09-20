
#ifndef __DRV_MAILBOX_CFG_H__
#define __DRV_MAILBOX_CFG_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/

/* Modified by c64416 for hifi mailbox, 2013/09/24, begin */
#include "bsp_shared_ddr.h"
#include "drv_ipc_enum.h"
/* Modified by c64416 for hifi mailbox, 2013/09/24, end */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* 定义源CPU 往 目标CPU的邮箱通道, 每通道对应独立的邮箱缓存 */

/* 按规则生成枚举项的宏定义 */
#define MAILBOX_CHANNEL_BEGIN(src, dst) \
    enum MAILBOX_CHANNEL_##src##2##dst##_ENUM \
    { \
        MAILBOX_CHANNEL_##src##2##dst##_RSERVED = -1,

#define MAILBOX_CHANNEL_ITEM(src, dst, channel)  \
    MAILBOX_CHANNEL_##src##2##dst##_##channel

#define MAILBOX_CHANNEL_END(src, dst) \
        MAILBOX_CHANNEL_##src##2##dst##_BUTT \
    };

/* CCPU -> HIFI */
MAILBOX_CHANNEL_BEGIN(CCPU, HIFI)
    /* 以下各枚举项按规则生成, 形如: MAILBOX_CHANNEL_CCPU2HIFI_DEFAULT */
    MAILBOX_CHANNEL_ITEM(CCPU, HIFI, MSG),
MAILBOX_CHANNEL_END(CCPU, HIFI)

/* CCPU -> MCU */
MAILBOX_CHANNEL_BEGIN(CCPU, MCU)
    MAILBOX_CHANNEL_ITEM(CCPU, MCU, MSG),
    MAILBOX_CHANNEL_ITEM(CCPU, MCU, IFC),
MAILBOX_CHANNEL_END(CCPU, MCU)

/* CCPU -> ACPU */
MAILBOX_CHANNEL_BEGIN(CCPU, ACPU)
    MAILBOX_CHANNEL_ITEM(CCPU, ACPU, MSG),
    MAILBOX_CHANNEL_ITEM(CCPU, ACPU, IFC),
MAILBOX_CHANNEL_END(CCPU, ACPU)

/* ACPU -> CCPU */
MAILBOX_CHANNEL_BEGIN(ACPU, CCPU)
    MAILBOX_CHANNEL_ITEM(ACPU, CCPU, MSG),
    MAILBOX_CHANNEL_ITEM(ACPU, CCPU, IFC),
MAILBOX_CHANNEL_END(ACPU, CCPU)

/* ACPU -> MCU */
MAILBOX_CHANNEL_BEGIN(ACPU, MCU)
    MAILBOX_CHANNEL_ITEM(ACPU, MCU, MSG),
    MAILBOX_CHANNEL_ITEM(ACPU, MCU, IFC),
MAILBOX_CHANNEL_END(ACPU, MCU)

/* ACPU -> HIFI */
MAILBOX_CHANNEL_BEGIN(ACPU, HIFI)
    MAILBOX_CHANNEL_ITEM(ACPU, HIFI, MSG),
MAILBOX_CHANNEL_END(ACPU, HIFI)

/* HIFI -> ACPU */
MAILBOX_CHANNEL_BEGIN(HIFI, ACPU)
    MAILBOX_CHANNEL_ITEM(HIFI, ACPU, MSG),
MAILBOX_CHANNEL_END(HIFI, ACPU)

/* HIFI -> CCPU */
MAILBOX_CHANNEL_BEGIN(HIFI, CCPU)
    MAILBOX_CHANNEL_ITEM(HIFI, CCPU, MSG),
MAILBOX_CHANNEL_END(HIFI, CCPU)

/* HIFI -> BBE16 */
MAILBOX_CHANNEL_BEGIN(HIFI, BBE16)
    MAILBOX_CHANNEL_ITEM(HIFI, BBE16, MSG),
MAILBOX_CHANNEL_END(HIFI, BBE16)

/* MCU -> ACPU */
MAILBOX_CHANNEL_BEGIN(MCU, ACPU)
    MAILBOX_CHANNEL_ITEM(MCU, ACPU, MSG),
    MAILBOX_CHANNEL_ITEM(MCU, ACPU, IFC),
MAILBOX_CHANNEL_END(MCU, ACPU)

/* MCU -> CCPU */
MAILBOX_CHANNEL_BEGIN(MCU, CCPU)
    MAILBOX_CHANNEL_ITEM(MCU, CCPU, MSG),
    MAILBOX_CHANNEL_ITEM(MCU, CCPU, IFC),
MAILBOX_CHANNEL_END(MCU, CCPU)

/* BBE16 -> HIFI */
MAILBOX_CHANNEL_BEGIN(BBE16, HIFI)
    MAILBOX_CHANNEL_ITEM(BBE16, HIFI, MSG),
MAILBOX_CHANNEL_END(BBE16, HIFI)

/* 有助SI解析, 利于阅读代码 */
enum MAILBOX_GAP_FOR_SI_PARSE {MAILBOX_GAP_FOR_SI_BUTT};

#define MAILBOX_ID_SRC_CPU_OFFSET       (24)        /*邮件ID中的源CPU ID bit位偏移地址*/
#define MAILBOX_ID_DST_CPU_OFFSET       (16)        /*邮件ID中的源CPU ID bit位偏移地址*/
#define MAILBOX_ID_CHANNEL_OFFSET       (8)         /*邮件ID中邮箱内存通道 ID bit位偏移地址*/

/* 邮编起始编号宏定义 */
#define MAILBOX_MAILCODE_CHANNEL(src, dst, channel) \
    ( ((unsigned long)(src) << MAILBOX_ID_SRC_CPU_OFFSET) \
    | ((unsigned long)(dst) << MAILBOX_ID_DST_CPU_OFFSET) \
    | ((unsigned long)(channel) << MAILBOX_ID_CHANNEL_OFFSET) )

#define MAILBOX_CPUID(cpu)   MAILBOX_CPUID_##cpu

#define MAILBOX_MAILCODE_RESERVED(src, dst, channel) \
    MAILBOX_MAILCODE_CHANNEL(MAILBOX_CPUID(src), \
                             MAILBOX_CPUID(dst), \
                            MAILBOX_CHANNEL_ITEM(src, dst, channel))

#define MAILBOX_MAILCODE_ITEM_RESERVED(src, dst, channel) \
    MAILBOX_MAILCODE_##src##2##dst##_##channel##_RESERVED

#define MAILBOX_MAILCODE_ITEM_END(src, dst, channel)    \
    MAILBOX_MAILCODE_##src##2##dst##_##channel##_BUTT

#define MAILBOX_MAILCODE_ITEM_BEGIN(src, dst, channel)  \
    MAILBOX_MAILCODE_ITEM_RESERVED(src, dst, channel) = MAILBOX_MAILCODE_RESERVED(src, dst, channel)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
 实 体 名  : MAILBOX_CPUID_E
 功能描述  : 定义邮箱涉及CPU编号
*****************************************************************************/
/* Modified by c64416 for hifi mailbox, 2013/09/24, begin */
enum MAILBOX_CPUID_ENUM
{
    MAILBOX_CPUID_RESERVED  =   -1,
    MAILBOX_CPUID_ACPU      =   IPC_CORE_ACORE,
    MAILBOX_CPUID_CCPU      =   IPC_CORE_CCORE,
    MAILBOX_CPUID_MCU       =   IPC_CORE_ACORE,
    MAILBOX_CPUID_BBE16     =   IPC_CORE_LDSP,
    MAILBOX_CPUID_HIFI      =   IPC_CORE_HiFi,
    MAILBOX_CPUID_BUTT
};
/* Modified by c64416 for hifi mailbox, 2013/09/24, end */

/*****************************************************************************
 实体名称  : MAILBOX_MAILCODE_ENUM
 功能描述  : 邮编枚举定义, 邮编唯一标识邮件发送目标, 也即邮件数据接收处理者
*****************************************************************************/
enum MAILBOX_MAILCODE_ENUM
{
    /* CCPU->MCU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(CCPU, MCU, MSG),
    MAILBOX_MAILCODE_CCPU_TO_MCU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_CCPU_TO_MCU_VOS_MSG_URGENT,
    BSP_MAILBOX_CHANNEL_CCPU_TO_MCU_MCA_CH,

    MAILBOX_MAILCODE_ITEM_END(CCPU, MCU, MSG),

    /* CCPU->MCU IFC调用号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(CCPU, MCU, IFC),
    MAILBOX_IFC_CCPU_TO_MCU_TEST_CMP,
    MAILBOX_IFC_CCPU_TO_MCU_TEST,
    MAILBOX_IFC_CCPU_TO_MCU_MCA,
    MAILBOX_MAILCODE_ITEM_END(CCPU, MCU, IFC),

    /* CCPU->HIFI 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(CCPU, HIFI, MSG),
    MAILBOX_MAILCODE_CCPU_TO_HIFI_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_CCPU_TO_HIFI_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_ITEM_END(CCPU, HIFI, MSG),

    /* CCPU->ACPU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(CCPU, ACPU, MSG),
    MAILBOX_MAILCODE_ITEM_END(CCPU, ACPU, MSG),

    /* CCPU->ACPU IFC编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(CCPU, ACPU, IFC),
    MAILBOX_IFC_CCPU_TO_ACPU_TEST_CMP,
    MAILBOX_IFC_CCPU_TO_ACPU_TEST,
    MAILBOX_IFC_CCPU_TO_ACPU_PRINT,
    MAILBOX_IFC_CCPU_TO_ACPU_FOPEN,
    MAILBOX_IFC_CCPU_TO_ACPU_FCLOSE,
    MAILBOX_IFC_CCPU_TO_ACPU_FREAD,
    MAILBOX_IFC_CCPU_TO_ACPU_FWRITE,
    MAILBOX_IFC_CCPU_TO_ACPU_FSEEK,
    MAILBOX_IFC_CCPU_TO_ACPU_REMOVE,
    MAILBOX_IFC_CCPU_TO_ACPU_FTELL,
    MAILBOX_IFC_CCPU_TO_ACPU_RENAME,
    MAILBOX_IFC_CCPU_TO_ACPU_ACCESS,
    MAILBOX_IFC_CCPU_TO_ACPU_MKDIR,
    MAILBOX_IFC_CCPU_TO_ACPU_RMDIR,
    MAILBOX_IFC_ACPU_TO_CCPU_PMIC_IRQEVENT_REPO,
    MAILBOX_MAILCODE_ITEM_END(CCPU, ACPU, IFC),

    /* ACPU->MCU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(ACPU, MCU, MSG),
    MAILBOX_MAILCODE_ACPU_TO_MCU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_ACPU_TO_MCU_VOS_MSG_URGENT,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_IFC_CH,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_IFC_RESPONSE_CH,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_SENSOR_CH,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_TP_CH,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_MCA_CH,
    BSP_MAILBOX_CHANNEL_ACPU_TO_MCU_RST_CH,
    MAILBOX_MAILCODE_ITEM_END(ACPU, MCU, MSG),

    /* ACPU->MCU IFC远程函数调用邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(ACPU, MCU, IFC),
    MAILBOX_IFC_ACPU_TO_MCU_TEST_CMP,
    MAILBOX_IFC_ACPU_TO_MCU_TEST,
    MAILBOX_IFC_ACPU_TO_MCU_HUTAF_HLT,
	MAILBOX_IFC_ACPU_TO_MCU_MCA,
    MAILBOX_IFC_ACPU_TO_MCU_MNTN,
    MAILBOX_IFC_ACPU_TO_MCU_RUN_CMD,
    MAILBOX_MAILCODE_ITEM_END(ACPU, MCU, IFC),


    /* ACPU->HIFI 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(ACPU, HIFI, MSG),
    MAILBOX_MAILCODE_ACPU_TO_HIFI_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_ACPU_TO_HIFI_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_ACPU_TO_HIFI_AUDIO,
    MAILBOX_MAILCODE_ACPU_TO_HIFI_MISC,
    MAILBOX_MAILCODE_ACPU_TO_HIFI_CCORE_RESET_ID,
    MAILBOX_MAILCODE_ITEM_END(ACPU, HIFI, MSG),

    /* ACPU->CCPU MSG号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(ACPU, CCPU, MSG),
    MAILBOX_IFC_ACPU_TO_CCPU_CSHELL_START,
    MAILBOX_MAILCODE_ITEM_END(ACPU, CCPU, MSG),

    /* ACPU->CCPU IFC号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(ACPU, CCPU, IFC),
    MAILBOX_IFC_ACPU_TO_CCPU_TEST_CMP,
    MAILBOX_IFC_ACPU_TO_CCPU_TEST,
    MAILBOX_IFC_ACPU_TO_CCPU_PRINT,
    MAILBOX_IFC_ACPU_TO_CCPU_FOPEN,
    MAILBOX_IFC_ACPU_TO_CCPU_FCLOSE,
    MAILBOX_IFC_ACPU_TO_CCPU_FREAD,
    MAILBOX_IFC_ACPU_TO_CCPU_FWRITE,
    MAILBOX_IFC_ACPU_TO_CCPU_FSEEK,
    MAILBOX_IFC_ACPU_TO_CCPU_REMOVE,
    MAILBOX_IFC_ACPU_TO_CCPU_FTELL,
    MAILBOX_IFC_ACPU_TO_CCPU_RENAME,
    MAILBOX_IFC_ACPU_TO_CCPU_ACCESS,
    MAILBOX_IFC_ACPU_TO_CCPU_MKDIR,
    MAILBOX_IFC_ACPU_TO_CCPU_RMDIR,
    MAILBOX_IFC_ACPU_TO_CCPU_BASE_TEST2,
    MAILBOX_IFC_ACPU_TO_CCPU_BASE_TEST1,
	MAILBOX_IFC_ACPU_TO_CCPU_PMIC_IRQEVENT,
    MAILBOX_IFC_ACPU_TO_CCPU_READ_EFUSE,
    MAILBOX_IFC_ACPU_TO_CCPU_SYSTEMERROR,
    MAILBOX_MAILCODE_ITEM_END(ACPU, CCPU, IFC),

    /* HIFI->CCPU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(HIFI, CCPU, MSG),
    MAILBOX_MAILCODE_HIFI_TO_CCPU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_HIFI_TO_CCPU_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_ITEM_END(HIFI, CCPU, MSG),

    /* HIFI->ACPU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(HIFI, ACPU, MSG),
    MAILBOX_MAILCODE_HIFI_TO_ACPU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_HIFI_TO_ACPU_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_HIFI_TO_ACPU_AUDIO,
    MAILBOX_MAILCODE_HIFI_TO_ACPU_MISC,
    MAILBOX_MAILCODE_HIFI_TO_ACPU_CCORE_RESET_ID,
    MAILBOX_MAILCODE_ITEM_END(HIFI, ACPU, MSG),

    /* HIFI->BBE16 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(HIFI, BBE16, MSG),
    MAILBOX_MAILCODE_HIFI_TO_BBE16_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_HIFI_TO_BBE16_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_ITEM_END(HIFI, BBE16, MSG),

    /* MCU->CCPU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(MCU, CCPU, MSG),
    MAILBOX_MAILCODE_MCU_TO_CCPU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_MCU_TO_CCPU_VOS_MSG_URGENT,
    BSP_MAILBOX_CHANNEL_MCU_TO_CCPU_MCA_CH,
    MAILBOX_MAILCODE_ITEM_END(MCU, CCPU, MSG),

    /* MCU->CCPU ifc邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(MCU, CCPU, IFC),
    MAILBOX_IFC_MCU_TO_CCPU_TEST_CMP,
    MAILBOX_IFC_MCU_TO_CCPU_BASE_TEST2,
    MAILBOX_IFC_MCU_TO_CCPU_BASE_TEST1,
    MAILBOX_IFC_MCU_TO_CCPU_TEST,
    MAILBOX_MAILCODE_ITEM_END(MCU, CCPU, IFC),

    /* MCU->ACPU 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(MCU, ACPU, MSG),
    MAILBOX_MAILCODE_MCU_TO_ACPU_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_MCU_TO_ACPU_VOS_MSG_URGENT,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_IFC_CH,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_IFC_RESPONSE_CH,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_SENSOR_CH,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_TP_CH,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_MCA_CH,
    BSP_MAILBOX_CHANNEL_MCU_TO_ACPU_MNTN_CH,
    MAILBOX_IFC_MCU_TO_ACPU_HUTAF_HLT,
    MAILBOX_MAILCODE_MCU_TO_ACPU_CCORE_RESET_ID,
    MAILBOX_MAILCODE_ITEM_END(MCU, ACPU, MSG),

    /* MCU->ACPU 远程函数调用邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(MCU, ACPU, IFC),
    MAILBOX_IFC_MCU_TO_ACPU_TEST_CMP,
    MAILBOX_IFC_MCU_TO_ACPU_TEST,
    MAILBOX_IFC_MCU_TO_ACPU_PRINT,
    MAILBOX_MAILCODE_ITEM_END(MCU, ACPU, IFC),

    /* BBE16 -> HIFI 邮编号定义在这里 */
    MAILBOX_MAILCODE_ITEM_BEGIN(BBE16, HIFI, MSG),
    MAILBOX_MAILCODE_BBE16_TO_HIFI_VOS_MSG_NORMAL,
    MAILBOX_MAILCODE_BBE16_TO_HIFI_VOS_MSG_URGENT,
    MAILBOX_MAILCODE_ITEM_END(BBE16, HIFI, MSG),
};

/*****************************************************************************
 实 体 名  : MAILBOX_RET_ENUM
 功能描述  : 邮箱接口返回值
*****************************************************************************/
#define    MAILBOX_OK                       0
#define    MAILBOX_ERRO                     0xF7654321
#define    MAILBOX_FULL                     0xF7654322
#define    MAILBOX_NOT_READY                0xF7654323   /*目标CPU邮箱未初始化*/
#define    MAILBOX_TARGET_NOT_READY         MAILBOX_NOT_READY   /*目标CPU邮箱未初始化*/
#define    MAILBOX_TIME_OUT                 0xF7654324     /*发送等待返回超时*/

/* 邮件序列号的初始值 */
#define MAILBOX_SEQNUM_START            (0)

/* Modified by c64416 for hifi mailbox, 2013/09/24, begin */
/* 邮箱占用memory基地址 */
#define MAILBOX_MEM_BASEADDR            SHD_DDR_V2P(SHM_MEM_HIFI_MBX_ADDR)

/* 邮箱占用memory预留总长度, 包括邮箱控制头和邮箱队列缓存 */
#define MAILBOX_MEM_LENGTH              SHM_MEM_HIFI_MBX_SIZE
/* Modified by c64416 for hifi mailbox, 2013/09/24, end */

/*****************************************************************************
 实 体 名  : struct mb_head
 功能描述  : 核间邮箱缓存头, 通过保护字判断邮箱通道有没有被初始化, 此功能需要
             fastboot同步修改:上电初始化对邮箱共享内存头数据清零
*****************************************************************************/
typedef struct mb_head
{
    unsigned long       ulProtectWord1;     /*保护字 0x55AA55AA*/
    unsigned long       ulProtectWord2;     /*保护字 0x5A5A5A5A*/
    unsigned long       ulFront;            /*队列待写单元距离队列(不含保护字)头的长度，单位32bit */
    unsigned long       ulRear;             /*队列待读单元距离队列(不含保护字)头的长度，单位32bit */
    unsigned long       ulFrontslice;       /*更新邮箱环形队列的头指针系统时间*/
    unsigned long       ulRearslice;        /*更新邮箱环形队列的尾指针系统时间*/
    unsigned short      ausReserve[4];      /*保留*/
    unsigned long       ulProtectWord3;     /*保护字 0x55AA55AA*/
    unsigned long       ulProtectWord4;     /*保护字 0x5A5A5A5A*/
} MAILBOX_HEAD_STRU;

/* 邮箱头占用空间总长度定义*/
#define MAILBOX_HEAD_LEN                (sizeof(struct mb_head))
#define MAILBOX_MAX_CHANNEL             (30)
#define MAILBOX_MEM_HEAD_LEN            (MAILBOX_MAX_CHANNEL * MAILBOX_HEAD_LEN)

/* 各邮箱数据队列长度定义 */
#define MAILBOX_QUEUE_SIZE(src, dst, channel) \
    MAILBOX_QUEUE_SIZE_##src##2##dst##_##channel
enum MAILBOX_QUEUE_SIZE_ENUM
{
/* Modified by c64416 for hifi mailbox, 2013/09/24, begin */
    /* 以下各枚举项按规则生成, 形如: MAILBOX_QUEUE_SIZE_MCU2ACPU_DEFAULT */
    MAILBOX_QUEUE_SIZE(MCU,  ACPU, MSG) = 0x00000000,
    MAILBOX_QUEUE_SIZE(ACPU, MCU,  MSG) = 0x00000000,
    MAILBOX_QUEUE_SIZE(MCU, ACPU, IFC)  = 0x00000000,
    MAILBOX_QUEUE_SIZE(ACPU, MCU, IFC)  = 0x00000000,

    MAILBOX_QUEUE_SIZE(MCU,  CCPU, MSG) = 0x00000000,
    MAILBOX_QUEUE_SIZE(CCPU, MCU,  MSG) = 0x00000000,
    MAILBOX_QUEUE_SIZE(MCU, CCPU, IFC)  = 0x00000000,
    MAILBOX_QUEUE_SIZE(CCPU, MCU, IFC)  = 0x00000000,

    MAILBOX_QUEUE_SIZE(ACPU, CCPU, MSG) = 0x00000000,
    MAILBOX_QUEUE_SIZE(CCPU, ACPU, MSG) = 0x00000000,
    MAILBOX_QUEUE_SIZE(ACPU, CCPU, IFC) = 0x00000000,
    MAILBOX_QUEUE_SIZE(CCPU, ACPU, IFC) = 0x00000000,

    MAILBOX_QUEUE_SIZE(ACPU, HIFI, MSG) = 0x00001800,
    MAILBOX_QUEUE_SIZE(HIFI, ACPU, MSG) = 0x00001800,

    MAILBOX_QUEUE_SIZE(CCPU, HIFI, MSG) = 0x00001800,
    MAILBOX_QUEUE_SIZE(HIFI, CCPU, MSG) = 0x00001800,

    MAILBOX_QUEUE_SIZE(BBE16, HIFI, MSG) = 0x00001800,
    MAILBOX_QUEUE_SIZE(HIFI, BBE16, MSG) = 0x00001800
/* Modified by c64416 for hifi mailbox, 2013/09/24, end */
};

/* 各邮箱控制头地址分配 */
#define MAILBOX_HEAD_ADDR(src, dst, channel) \
    MAILBOX_HEAD_ADDR_##src##2##dst##_##channel
enum MAILBOX_HEAD_ADDR_ENUM
{
    /* 以下各枚举项按规则生成, 形如: MAILBOX_HEAD_ADDR_MCU2ACPU_DEFAULT */
    MAILBOX_HEAD_ADDR(MCU,  ACPU, MSG)  = MAILBOX_MEM_BASEADDR,
    MAILBOX_HEAD_ADDR(ACPU, MCU,  MSG)  = MAILBOX_HEAD_ADDR(MCU,  ACPU, MSG) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(ACPU, HIFI, MSG)  = MAILBOX_HEAD_ADDR(ACPU, MCU,  MSG) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(HIFI, ACPU, MSG)  = MAILBOX_HEAD_ADDR(ACPU, HIFI, MSG) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(MCU,  CCPU, MSG)  = MAILBOX_HEAD_ADDR(HIFI, ACPU, MSG) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(CCPU, MCU,  MSG)  = MAILBOX_HEAD_ADDR(MCU,  CCPU, MSG) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(CCPU, HIFI, MSG)  = MAILBOX_HEAD_ADDR(CCPU, MCU,  MSG) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(HIFI, CCPU, MSG)  = MAILBOX_HEAD_ADDR(CCPU, HIFI, MSG) + MAILBOX_HEAD_LEN,

    MAILBOX_HEAD_ADDR(ACPU, CCPU, MSG)  = MAILBOX_HEAD_ADDR(HIFI, CCPU, MSG) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(CCPU, ACPU, MSG)  = MAILBOX_HEAD_ADDR(ACPU, CCPU, MSG) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(CCPU, ACPU, IFC)  = MAILBOX_HEAD_ADDR(CCPU, ACPU, MSG) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(ACPU, CCPU, IFC)  = MAILBOX_HEAD_ADDR(CCPU, ACPU, IFC) + MAILBOX_HEAD_LEN,

    MAILBOX_HEAD_ADDR(CCPU, MCU, IFC)   = MAILBOX_HEAD_ADDR(ACPU, CCPU, IFC) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(MCU, CCPU, IFC)   = MAILBOX_HEAD_ADDR(CCPU, MCU,  IFC) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(ACPU, MCU, IFC)   = MAILBOX_HEAD_ADDR(MCU,  CCPU, IFC) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(MCU, ACPU, IFC)   = MAILBOX_HEAD_ADDR(ACPU, MCU,  IFC) + MAILBOX_HEAD_LEN,

    MAILBOX_HEAD_ADDR(BBE16, HIFI, MSG) = MAILBOX_HEAD_ADDR(MCU, ACPU,  IFC) + MAILBOX_HEAD_LEN,
    MAILBOX_HEAD_ADDR(HIFI, BBE16, MSG) = MAILBOX_HEAD_ADDR(BBE16,HIFI, MSG) + MAILBOX_HEAD_LEN,

    MAILBOX_HEAD_BOTTOM_ADDR            = MAILBOX_HEAD_ADDR(HIFI, BBE16,MSG) + MAILBOX_HEAD_LEN

};

/* 各邮箱数据队列地址分配 */
#define MAILBOX_QUEUE_ADDR(src, dst, channel) \
    MAILBOX_QUEUE_ADDR_##src##2##dst##_##channel
#define MAILBOX_QUEUE_BOTTOM_ADDR(src, dst, channel) \
    (MAILBOX_QUEUE_ADDR(src, dst, channel) + MAILBOX_QUEUE_SIZE(src, dst, channel))
enum MAILBOX_QUEUE_ADDR_ENUM
{
    /* 以下各枚举项按规则生成, 形如: MAILBOX_QUEUE_ADDR_MCU2ACPU_DEFAULT */
    MAILBOX_QUEUE_ADDR(MCU,  ACPU, MSG)  = MAILBOX_MEM_BASEADDR + MAILBOX_MEM_HEAD_LEN,
    MAILBOX_QUEUE_ADDR(ACPU, MCU,  MSG)  = MAILBOX_QUEUE_BOTTOM_ADDR(MCU,  ACPU, MSG),
    MAILBOX_QUEUE_ADDR(ACPU, HIFI, MSG)  = MAILBOX_QUEUE_BOTTOM_ADDR(ACPU, MCU,  MSG),
    MAILBOX_QUEUE_ADDR(HIFI, ACPU, MSG)  = MAILBOX_QUEUE_BOTTOM_ADDR(ACPU, HIFI, MSG),
    MAILBOX_QUEUE_ADDR(MCU,  CCPU, MSG)  = MAILBOX_QUEUE_BOTTOM_ADDR(HIFI, ACPU, MSG) ,
    MAILBOX_QUEUE_ADDR(CCPU, MCU,  MSG)  = MAILBOX_QUEUE_BOTTOM_ADDR(MCU,  CCPU, MSG),
    MAILBOX_QUEUE_ADDR(CCPU, HIFI, MSG)  = MAILBOX_QUEUE_BOTTOM_ADDR(CCPU, MCU,  MSG),
    MAILBOX_QUEUE_ADDR(HIFI, CCPU, MSG)  = MAILBOX_QUEUE_BOTTOM_ADDR(CCPU, HIFI, MSG),

    MAILBOX_QUEUE_ADDR(ACPU, CCPU, MSG)  = MAILBOX_QUEUE_BOTTOM_ADDR(HIFI, CCPU, MSG),
    MAILBOX_QUEUE_ADDR(CCPU, ACPU, MSG)  = MAILBOX_QUEUE_BOTTOM_ADDR(ACPU, CCPU, MSG),
    MAILBOX_QUEUE_ADDR(CCPU, ACPU, IFC)  = MAILBOX_QUEUE_BOTTOM_ADDR(CCPU, ACPU, MSG),
    MAILBOX_QUEUE_ADDR(ACPU, CCPU, IFC)  = MAILBOX_QUEUE_BOTTOM_ADDR(CCPU, ACPU, IFC),

    MAILBOX_QUEUE_ADDR(CCPU, MCU, IFC)   = MAILBOX_QUEUE_BOTTOM_ADDR(ACPU, CCPU, IFC),
    MAILBOX_QUEUE_ADDR(MCU, CCPU, IFC)   = MAILBOX_QUEUE_BOTTOM_ADDR(CCPU, MCU, IFC),
    MAILBOX_QUEUE_ADDR(ACPU, MCU, IFC)   = MAILBOX_QUEUE_BOTTOM_ADDR(MCU, CCPU, IFC),
    MAILBOX_QUEUE_ADDR(MCU, ACPU, IFC)   = MAILBOX_QUEUE_BOTTOM_ADDR(ACPU, MCU, IFC),

    MAILBOX_QUEUE_ADDR(BBE16, HIFI, MSG) = MAILBOX_QUEUE_BOTTOM_ADDR(MCU, ACPU, IFC),
    MAILBOX_QUEUE_ADDR(HIFI, BBE16, MSG) = MAILBOX_QUEUE_BOTTOM_ADDR(BBE16, HIFI, MSG),

    MAILBOX_MEMORY_BOTTOM_ADDR           = MAILBOX_QUEUE_BOTTOM_ADDR(HIFI, BBE16, MSG)
};

/*共享内存中的保护字定义*/
#define MAILBOX_PROTECT1                (0x55AA55AA)    /* 邮箱保护字1，用于邮箱头及邮箱, 同时表示邮箱刚初始化没有接收过数据*/
#define MAILBOX_PROTECT2                (0x5A5A5A5A)    /* 邮箱保护字2，用于邮箱头及邮箱 */
#define MAILBOX_PROTECT_LEN             (sizeof(long))  /* 邮箱保护字长度，单位byte，邮箱头及邮箱头、尾均有两个这样的保护字 */
#define MAILBOX_MSGHEAD_NUMBER          (0xA5A5A5A5)    /* 跨核消息消息分隔字 */

#define MAILBOX_DATA_BASE_PROTECT_NUM         (2)    /* 邮箱通道共享内存数据区以首地址开始的保护字个数*/
#define MAILBOX_DATA_TAIL_PROTECT_NUM         (2)    /* 邮箱通道共享内存数据区以尾地址结束的保护字个数*/

/* 邮箱通道共享内存数据区长度中包含的保护字的个数，包括首地址开始的2个保护字，和末地址结束的2个保护字*/
#define MAILBOX_DATA_LEN_PROTECT_NUM          (MAILBOX_DATA_BASE_PROTECT_NUM + MAILBOX_DATA_TAIL_PROTECT_NUM)


/* hifi邮箱地址信息保护字 */
#define HIFI_MB_ADDR_PROTECT            (0x5a5a5a5a)

/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/*****************************************************************************
  定义邮箱核与核之间的IPC中断号
*****************************************************************************/
#define MAILBOX_IPC_INT_NUM(src, dst, channel) \
    MAILBOX_IPC_INT_##src##2##dst##_##channel
enum IPC_MAILBOX_INT_ENUM
{
/* Modified by c64416 for hifi mailbox, 2013/09/24, begin */
#if 0
    /* 以下各枚举项按规则生成, 形如: MAILBOX_IPC_INT_CCPU2MCU_DEFAULT */
    MAILBOX_IPC_INT_NUM(CCPU, MCU, MSG)  = IPC_MCU_INT_SRC_CCPU_MSG,
    MAILBOX_IPC_INT_NUM(MCU, CCPU, MSG)  = IPC_CCPU_INT_SRC_MCU_MSG,

    MAILBOX_IPC_INT_NUM(CCPU, HIFI, MSG) = IPC_HIFI_INT_SRC_CCPU_MSG,
    MAILBOX_IPC_INT_NUM(HIFI, CCPU, MSG) = IPC_CCPU_INT_SRC_HIFI_MSG,

    MAILBOX_IPC_INT_NUM(ACPU, MCU, MSG)  = IPC_MCU_INT_SRC_ACPU_MSG,
    MAILBOX_IPC_INT_NUM(MCU, ACPU, MSG)  = IPC_ACPU_INT_SRC_MCU_MSG,

    MAILBOX_IPC_INT_NUM(ACPU, HIFI, MSG) = IPC_HIFI_INT_SRC_ACPU_MSG,
    MAILBOX_IPC_INT_NUM(HIFI, ACPU, MSG) = IPC_ACPU_INT_SRC_HIFI_MSG,

    MAILBOX_IPC_INT_NUM(HIFI, MCU, MSG)  = IPC_MCU_INT_SRC_HIFI_MSG,
    MAILBOX_IPC_INT_NUM(MCU, HIFI, MSG)  = IPC_HIFI_INT_SRC_MCU_MSG,

    MAILBOX_IPC_INT_NUM(CCPU, ACPU, MSG)    = IPC_ACPU_INT_SRC_CCPU_MSG,
    MAILBOX_IPC_INT_NUM(ACPU, CCPU, MSG)    = IPC_CCPU_INT_SRC_ACPU_MSG,

    MAILBOX_IPC_INT_NUM(CCPU, ACPU, IFC)    = IPC_ACPU_INT_SRC_CCPU_IFC,
    MAILBOX_IPC_INT_NUM(ACPU, CCPU, IFC)    = IPC_CCPU_INT_SRC_ACPU_IFC,

    MAILBOX_IPC_INT_NUM(CCPU, MCU, IFC)    = IPC_MCU_INT_SRC_CCPU_IFC,
    MAILBOX_IPC_INT_NUM(MCU, CCPU, IFC)    = IPC_CCPU_INT_SRC_MCU_IFC,

    MAILBOX_IPC_INT_NUM(ACPU, MCU, IFC)    = IPC_MCU_INT_SRC_ACPU_IFC,
    MAILBOX_IPC_INT_NUM(MCU, ACPU, IFC)    = IPC_ACPU_INT_SRC_MCU_IFC,

    MAILBOX_IPC_INT_NUM(BBE16, HIFI, MSG) = IPC_HIFI_INT_SRC_BBE_MSG,
    MAILBOX_IPC_INT_NUM(HIFI, BBE16, MSG) = IPC_BBE16_INT_SRC_HIFI_MSG

#else
    MAILBOX_IPC_INT_NUM(CCPU, HIFI, MSG) = IPC_HIFI_INT_SRC_CCPU_MSG,
    MAILBOX_IPC_INT_NUM(HIFI, CCPU, MSG) = IPC_CCPU_INT_SRC_HIFI_MSG,

    MAILBOX_IPC_INT_NUM(ACPU, HIFI, MSG) = IPC_HIFI_INT_SRC_ACPU_MSG,
    MAILBOX_IPC_INT_NUM(HIFI, ACPU, MSG) = IPC_ACPU_INT_SRC_HIFI_MSG,
#endif
/* Modified by c64416 for hifi mailbox, 2013/09/24, end */
};

/* 各邮箱数据单个邮件最大长度定义 */
#define MAILBOX_MAILSIZE_MAX(src, dst, channel) \
    MAILBOX_MAILSIZE_MAX_##src##2##dst##_##channel
enum MAILBOX_MAILSIZE_MAX_ENUM
{
    /* 以下各枚举项按规则生成, 形如: MAILBOX_MAILSIZE_MAX_MCU2ACPU_DEFAULT */
    MAILBOX_MAILSIZE_MAX(MCU,  ACPU ,MSG)  = MAILBOX_QUEUE_SIZE(MCU,  ACPU ,MSG)/4, //DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(ACPU, MCU  ,MSG)  = MAILBOX_QUEUE_SIZE(ACPU, MCU  ,MSG)/4, //DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(ACPU, HIFI ,MSG)  = MAILBOX_QUEUE_SIZE(ACPU ,HIFI ,MSG)/4, //DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(HIFI, ACPU ,MSG)  = MAILBOX_QUEUE_SIZE(HIFI ,ACPU ,MSG)/4, //DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(MCU , CCPU ,MSG)  = MAILBOX_QUEUE_SIZE(MCU  ,CCPU ,MSG)/4,//DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(CCPU, MCU  ,MSG)  = MAILBOX_QUEUE_SIZE(CCPU ,MCU  ,MSG)/4,//DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(CCPU, HIFI ,MSG)  = MAILBOX_QUEUE_SIZE(CCPU ,HIFI ,MSG)/4,//DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(HIFI, CCPU ,MSG)  = MAILBOX_QUEUE_SIZE(HIFI ,CCPU ,MSG)/4,//DOTO:具体值需要分析

    MAILBOX_MAILSIZE_MAX(CCPU, ACPU ,MSG)  = MAILBOX_QUEUE_SIZE(CCPU, ACPU ,MSG)/4,//DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(ACPU, CCPU ,MSG)  = MAILBOX_QUEUE_SIZE(ACPU, CCPU ,MSG)/4,//DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(CCPU, ACPU ,IFC)  = MAILBOX_QUEUE_SIZE(CCPU, ACPU ,IFC)/4,//DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(ACPU, CCPU ,IFC)  = MAILBOX_QUEUE_SIZE(ACPU, CCPU ,IFC)/4,//DOTO:具体值需要分析

    MAILBOX_MAILSIZE_MAX(CCPU, MCU ,IFC)  = MAILBOX_QUEUE_SIZE(CCPU, MCU ,IFC)/4,//DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(MCU, CCPU ,IFC)  = MAILBOX_QUEUE_SIZE(MCU, CCPU ,IFC)/4,//DOTO:具体值需要分析

    MAILBOX_MAILSIZE_MAX(ACPU, MCU ,IFC)  = MAILBOX_QUEUE_SIZE(ACPU, MCU ,IFC)/4,//DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(MCU, ACPU ,IFC)  = MAILBOX_QUEUE_SIZE(MCU, ACPU ,IFC)/4,//DOTO:具体值需要分析

    MAILBOX_MAILSIZE_MAX(BBE16, HIFI ,MSG) = MAILBOX_QUEUE_SIZE(BBE16 ,HIFI ,MSG)/4,//DOTO:具体值需要分析
    MAILBOX_MAILSIZE_MAX(HIFI, BBE16 ,MSG) = MAILBOX_QUEUE_SIZE(HIFI ,BBE16 ,MSG)/4,//DOTO:具体值需要分析
};

/*****************************************************************************
  4 消息头定义
*****************************************************************************/


/*****************************************************************************
  5 消息定义
*****************************************************************************/


/*****************************************************************************
  6 STRUCT定义
*****************************************************************************/

/*****************************************************************************
 实 体 名  : struct mb_mail
 功能描述  : 核间邮件头
*****************************************************************************/
typedef struct mb_mail
{
    unsigned long       ulPartition;    /*消息分隔字 0xA5A5A5A5*/
    unsigned long       ulWriteSlice;   /*消息写入系统时间信息 */
    unsigned long       ulReadSlice;    /*消息读出系统时间信息 */
    unsigned long       ulSeqNum;       /*消息序列号 */
    unsigned long       ulPriority;     /*消息优先级：紧急、普通, 废弃*/
    unsigned long       ulMailCode;     /*邮编*/
    unsigned long       ausReserve[2];  /*保留*/
    unsigned long       ulMsgLength;    /*自ulMsgLength字段(不含本身)往后的消息长度*/
} MAILBOX_MSG_HEADER;

/************************************************************************
 结构名    : IPC_MAILBOX_QUEUE_STRU
 结构说明  : 核间邮箱缓存
 ************************************************************************/
/* 邮箱结构示意:
typedef struct
{
    unsigned long      ulProtectWord1;     //保护字 0x55aa55aa
    unsigned long      ulProtectWord2;     //保护字 0a5a5a5a5a
    unsigned long      aulMsgQueue[队列长度-4];//环形队列存放若干封邮件
    unsigned long      ulProtectWord3;     //保护字 0x55aa55aa
    unsigned long      ulProtectWord4;     //保护字 0x5a5a5a5a
} MAILBOX_QUEUE_STRU;
*/

/*****************************************************************************
 结构名    : AARM_HIFI_DYN_ADDR_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :AARM和HIFI的共享地址，OAM需要赋值并拷贝至Hifi共享数据中的相应结构体
*****************************************************************************/
typedef struct
{
    unsigned long uwProtectWord;                /*0x5a5a5a5a*/
    unsigned long uwHifi2AarmMailBoxLen;        /* hifi到Aarm跨核邮箱长度 */
    unsigned long uwAarm2HifiMailBoxLen;        /* Aarm到hifi跨核邮箱长度 */
    unsigned long uwHifiAarmHeadAddr;
    unsigned long uwHifiAarmBodyAddr;
    unsigned long uwAarmHifiHeadAddr;
    unsigned long uwAarmHifiBodyAddr;
    unsigned long uwReserved[2];
}AARM_HIFI_MAILBOX_STRU;

/*****************************************************************************
 结构名    : CARM_HIFI_DYN_ADDR_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :CARM和HIFI的共享地址，OAM需要赋值并拷贝至Hifi共享数据中的相应结构体
*****************************************************************************/
typedef struct
{
    unsigned long uwProtectWord;                /*0x5a5a5a5a*/
    unsigned long uwHifi2CarmMailBoxLen;        /* hifi到Carm跨核邮箱长度 */
    unsigned long uwCarm2HifiMailBoxLen;        /* Carm到hifi跨核邮箱长度 */
    unsigned long uwHifiCarmHeadAddr;
    unsigned long uwHifiCarmBodyAddr;
    unsigned long uwCarmHifiHeadAddr;
    unsigned long uwCarmHifiBodyAddr;
    unsigned long uwReserved[2];
}CARM_HIFI_MAILBOX_STRU;

/*****************************************************************************
 结构名    : CARM_HIFI_DYN_ADDR_SHARE_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  :CARM和HIFI的共享地址，OAM需要赋值并拷贝至Hifi共享数据中的相应结构体
*****************************************************************************/
typedef struct
{
    unsigned long uwProtectWord;            /*0x5a5a5a5a*/
    CARM_HIFI_MAILBOX_STRU stCarmHifiMB;    /*CARM和HIFI的邮箱共享区定义*/
    AARM_HIFI_MAILBOX_STRU stAarmHifiMB;    /*AARM和HIFI的邮箱共享区定义*/
    unsigned long uwNvBaseAddrPhy;          /*DDR上NV备份区首地址的物理地址*/
    unsigned long uwNvBaseAddrVirt;         /*DDR上NV备份区首地址的ARM虚拟地址*/
    unsigned long uwReserved[2];
}CARM_HIFI_DYN_ADDR_SHARE_STRU;

/*****************************************************************************
  7 UNION定义
*****************************************************************************/


/*****************************************************************************
  8 OTHERS定义
*****************************************************************************/


/*****************************************************************************
  9 全局变量声明
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/


typedef void (*mb_msg_cb)(
                void                   *user_handle,
                void                   *mail_handle,
                unsigned long           mail_len);


unsigned long DRV_MAILBOX_SENDMAIL(
                unsigned long           MailCode,
                void                   *pData,
                unsigned long           Length);


unsigned long DRV_MAILBOX_REGISTERRECVFUNC(
                unsigned long           MailCode,
                mb_msg_cb               pFun,
                void                   *UserHandle);


unsigned long DRV_MAILBOX_READMAILDATA(
                void                   *MailHandle,
                unsigned char          *pData,
                unsigned long          *pSize);


void drv_hifi_fill_mb_info(unsigned int addr);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of drv_mailbox_cfg.h */

