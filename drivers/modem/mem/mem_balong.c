
/*lint -save -e537*/
#include "mem_balong.h"
#include "osl_barrier.h"
/*lint -restore*/


/**************************************************************************/
/*lint -save -e413 -e19*/
/**************************************************************************/
/**************************************************************************
  �궨��
**************************************************************************/
#ifndef __BSP_MEM_DEBUG__
#define __BSP_MEM_DEBUG__
#endif/*__BSP_MEM_DEBUG__*/

#ifndef CHIP_BB_6920CS
#define CHIP_BB_6920CS
#endif/*CHIP_BB_6920CS*/

/* ÿ���ڴ�ڵ�����ֵ���� */
#define MEM_NODE_MGR_SIZE           32

/* MAGIC������ */
#define MEM_MAGIC_NUM               0x11223344

/* �ڴ�ߴ����� */
u32 sg_AllocListSize[]  =   {32,    128,   512,    1024,   2048, 4096, 8192,   0x4000, 0x8000, 0x10000, 0x20000};
u32 sg_AllocMinNum[]    =   {512,   256,   100,    10,     1,     2,    10,     10,      1,      1,        1};
u32 sg_AllocMaxNum[]    =   {1024,  1024,  1024,   1024,   2048, 100 , 80,     40,      10,     4,        1};

#define MEM_ALLOC_LIST_NUM          (sizeof(sg_AllocListSize) / sizeof(u32))
#define FAIL                        -1

/**************************************************************************
  ���Ͷ���
**************************************************************************/
/* �ڴ�ڵ�״̬���� */
typedef enum tagMEM_STATUS
{
    MEM_FREE = 0,
    MEM_ALLOC = 1
}MEM_STATUS;

/* �ڴ�ع�����Ϣ */
typedef struct tagMEM_POOL_INFO
{
    u32 u32BaseAddr;            /* �ڴ�ػ���ַ */
    u32 u32Size;                /* �ڴ���ܴ�С */
    u32 u32CurPosAddr;          /* �ڴ�ص�ǰ���䵽��λ�� */
    u32 u32Left;                /* �ڴ��ʣ���С */
    u32 u32MgrSize;             /* �����ṹ��С */
}MEM_POOL_INFO;

/* ÿ���ڴ�ڵ�Ĺ�����Ϣ(ע��,���Ҫ���� 32bytes) */
typedef struct tagMEM_MGR_INFO
{
    u32 u32MagicNumber;         /* ���ڼ�鵱ǰ�ڴ���Ƿ���Ч������Debugģʽ��д��ã�*/
    u32 u32Next;                /*����ָ�򱾽ڵ����һ���ڴ�*/
    u32 u32Size;                /*�������ڴ��Ĵ�С*/
    u32 u32Flags;               /*�������ڴ������ԣ�˫��ʱ��Ҫ����������AXI����DDR��*/
#ifdef __BSP_MEM_DEBUG__
    u32 u32FileName;            /*ʹ�ø��ڴ��� .c �ļ�������Debugģʽ��д��ã�*/
    u32 u32Line;                /*ʹ�ø��ڴ���� .c �ļ��е�����*/
    u32 u32Status;              /*��¼��ǰ�ڴ���ʹ��״̬����Malloc״̬����Free״̬*/
#endif
}MEM_MGR_INFO;

/* ÿ���ڴ�ڵ��ʹ�ü��� */
typedef struct tagMEM_USED_INFO
{
    u32 u32CurNum;              /* ��ǰʹ�ø������� */
    u32 u32MaxNum;              /* ʹ�ø�����ֵ */
    u32 u32TotalMallocNum;      /* �ۻ�malloc���� */
    u32 u32TotalFreeNum;        /* �ۻ�free���� */
}MEM_USED_INFO;

/* �ڴ���������Ϣ */
typedef struct tagMEM_ALLOC_INFO
{
    void*           allocList[MEM_ALLOC_LIST_NUM];          /* �������յ���Ӧ�ڴ�ڵ�Ĵ�С*/
    u32         allocNum[MEM_ALLOC_LIST_NUM];            /*�Ѿ������������Ӧ�ڴ�ڵ������*/
    MEM_USED_INFO   allocUsedInfoList[MEM_ALLOC_LIST_NUM];  /*����ʹ��*/
    MEM_POOL_INFO   memPoolInfo;                            /*�ڴ����Ϣ*/
    u32         mostUsedItem;                           /*��Ƶ��ʹ�õĳߴ�ı�־*/
    u32         u32AllocFailCnt;                        /*����ʧ�ܵĴ���*/
}MEM_ALLOC_INFO;


/**************************************************************************
  ȫ�ֱ���
**************************************************************************/
LOCAL MEM_ALLOC_INFO sg_stLocalAllocInfo[1];

#define AXI_MEM_ADDR        (HI_SRAM_MEM_ADDR_VIRT)
#define AXI_MEM_SIZE        (HI_SRAM_MEM_SIZE)
#define PHY_TO_VIRT(addr)   HI_IO_ADDRESS(addr)

#define MEM_CTX_RESERVED    4

u32* sg_pAllocSizeTbl = NULL;
MEM_ALLOC_INFO* sg_pIccAllocInfo = NULL;
u32* g_mem_init_mark = NULL;

/**************************************************************************
  ��ʵ��
**************************************************************************/
#define MEM_GET_ALLOC_SIZE(i)       (*(sg_pAllocSizeTbl+i))
#define MEM_GET_ALLOC_INFO(type)    (((type) >= MEM_ICC_DDR_POOL) ? \
                                    (((MEM_ALLOC_INFO*)(sg_pIccAllocInfo)) + ((u32)(type)-(u32)MEM_ICC_DDR_POOL)) : \
                                    (&sg_stLocalAllocInfo[MEM_NORM_DDR_POOL]))
#define MEM_MGR_SIZE_FOR_CACHE      MEM_NODE_MGR_SIZE


#define MEM_GET_ALLOC_ADDR(x)       ((void*)((u32)(x)-MEM_NODE_MGR_SIZE))
#define MEM_OFFSET_OF(type, member) ((u32) (&((type *)0)->member))
#define MEM_ITEM_NEXT(x)            (*(u32*)(((u32)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32Next))))
#define MEM_ITEM_SIZE(x)            (*(u32*)(((u32)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32Size))))
#define MEM_ITEM_FLAGS(x)           (*(u32*)(((u32)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32Flags))))
#define MEM_ITEM_MAGIC(x)           (*(u32*)(((u32)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32MagicNumber))))
#define MEM_ITEM_FILE_NAME(x)       (*(u32*)(((u32)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32FileName))))
#define MEM_ITEM_LINE(x)            (*(u32*)(((u32)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32Line))))
#define MEM_ITEM_STATUS(x)          (*(u32*)(((u32)(x)-MEM_NODE_MGR_SIZE+MEM_OFFSET_OF(MEM_MGR_INFO, u32Status))))
#define MEM_IS_AXI_ADDR(ptr) \
((u32)(ptr) >= (u32)AXI_MEM_ADDR && (u32)(ptr) < (u32)(AXI_MEM_ADDR+AXI_MEM_SIZE))

spinlock_t g_ulMemSpinLock;
unsigned long g_ulMemSpinFlag = 0;

#define MEM_LOCAL_LOCK() \
do{\
    spin_lock_irqsave(&g_ulMemSpinLock, g_ulMemSpinFlag);\
}while(0)

#define MEM_LOCAL_UNLOCK() spin_unlock_irqrestore(&g_ulMemSpinLock, g_ulMemSpinFlag)

#define MEM_SPIN_LOCK() \
do{\
    MEM_LOCAL_LOCK();\
    bsp_ipc_spin_lock((u32)IPC_SEM_MEM);\
}while(0)

#define MEM_SPIN_UNLOCK() \
do{\
    bsp_ipc_spin_unlock((u32)IPC_SEM_MEM);\
    MEM_LOCAL_UNLOCK();\
}while(0)

#define MEM_LOCK_BY_TYPE(type)  \
    do{\
    if ((MEM_POOL_TYPE)type >= MEM_ICC_DDR_POOL)\
    {\
        MEM_SPIN_LOCK();\
    }\
    else\
    {\
        MEM_LOCAL_LOCK();\
    }\
}while(0)

#define MEM_UNLOCK_BY_TYPE(type) \
do{\
    if ((MEM_POOL_TYPE)type >= MEM_ICC_DDR_POOL)\
    {\
        MEM_SPIN_UNLOCK();\
    }\
    else\
    {\
        MEM_LOCAL_UNLOCK();\
    }\
}while(0)

/*#define MEM_FLUSH_CACHE(ptr, size)  (void)flush_kernel_vmap_range(ptr,size)*/
#define MEM_FLUSH_CACHE(ptr, size)   mb()
#define MEM_INVALID_CACHE(ptr, size) (void)invalidate_kernel_vmap_range(ptr, size)
#define MEM_FLUSH_CACHE_BY_TYPE(ptr, size, type) \
do{\
    if ((MEM_POOL_TYPE)type == MEM_ICC_DDR_POOL)\
    {\
        MEM_FLUSH_CACHE(ptr, size);\
    }\
}while(0)

#define MEM_INVALID_CACHE_BY_TYPE(ptr, size, type)\
do{\
    if ((MEM_POOL_TYPE)type == MEM_ICC_DDR_POOL)\
    {\
        MEM_INVALID_CACHE(ptr, size);\
    }\
}while(0)

#define MEM_DEFINE_TIMES()
#define MEM_INC_TIMES()
#define MEM_PRINT_TIMES(size)

/* ��size list �в��Һ��ʵ��ڴ�ڵ�,����Ҳ����򷵻� MEM_ALLOC_LIST_NUM */
#define MEM_FIND_RIGHT_ITEM(item, size, most_used) \
do {\
    MEM_DEFINE_TIMES();\
    if ((size) > MEM_GET_ALLOC_SIZE(most_used))\
    {\
        MEM_INC_TIMES();\
        for ((item) = (most_used+1); (item) < MEM_ALLOC_LIST_NUM && size > MEM_GET_ALLOC_SIZE(item); (item)++)\
        {\
            MEM_INC_TIMES();\
        }\
    }\
    else\
    {\
        MEM_INC_TIMES();\
        for ((item) = 0; (item) <= (most_used) && (size) > MEM_GET_ALLOC_SIZE(item); (item)++)\
        {\
             MEM_INC_TIMES();\
        }\
        /* �����Ч, ����Чֵ��Ϊ MEM_ALLOC_LIST_NUM */\
        if ((item) > (most_used))\
        {\
            (item) = MEM_ALLOC_LIST_NUM;\
        }\
    }\
    MEM_PRINT_TIMES(size);\
}while(0)


/**************************************************************************
  �ڲ�����
**************************************************************************/
s32 bsp_init_poll(u32 u32PoolType)
{
    MEM_ALLOC_INFO* pAllocInfo = MEM_GET_ALLOC_INFO(u32PoolType);

    /* �������ַ�ʹ�С */
    switch((MEM_POOL_TYPE)u32PoolType)
    {
    case MEM_NORM_DDR_POOL:
        {
 /*           pAllocInfo->memPoolInfo.u32CurPosAddr =
            pAllocInfo->memPoolInfo.u32BaseAddr = (u32)MEM_NORM_DDR_POOL_BASE_ADDR;
            pAllocInfo->memPoolInfo.u32Left =
            pAllocInfo->memPoolInfo.u32Size = MEM_NORM_DDR_POOL_SIZE;
            pAllocInfo->memPoolInfo.u32MgrSize = MEM_MGR_SIZE_FOR_CACHE;
*/
       }
        break;

    case MEM_ICC_DDR_POOL:
        {
            pAllocInfo->memPoolInfo.u32CurPosAddr =
            pAllocInfo->memPoolInfo.u32BaseAddr = (u32)SHD_DDR_V2P(MEM_ICC_DDR_POOL_BASE_ADDR);
            pAllocInfo->memPoolInfo.u32Left =
            pAllocInfo->memPoolInfo.u32Size = MEM_ICC_DDR_POOL_SIZE;
            pAllocInfo->memPoolInfo.u32MgrSize = MEM_MGR_SIZE_FOR_CACHE;
        }
        break;
    case MEM_ICC_AXI_POOL:
        {
 /*           pAllocInfo->memPoolInfo.u32CurPosAddr =
            pAllocInfo->memPoolInfo.u32BaseAddr = (u32)DRV_AXI_VIRT_TO_PHY(MEM_ICC_AXI_POOL_BASE_ADDR);
            pAllocInfo->memPoolInfo.u32Left =
            pAllocInfo->memPoolInfo.u32Size = MEM_ICC_AXI_POOL_SIZE;
            pAllocInfo->memPoolInfo.u32MgrSize = MEM_MGR_SIZE_FOR_CACHE;
*/
        }
        break;

    default:
        printk("Invalid pool type:%d, line:%d\n",  u32PoolType,  __LINE__);
        return FAIL;
    }
    if(u32PoolType == MEM_ICC_DDR_POOL)
    {
        if (!pAllocInfo->memPoolInfo.u32BaseAddr )
        {
            printk("Invalid pool ptr, line:%d\n", __LINE__);
            return FAIL;
        }
        /* ��ʼ������ȫ�ֱ��� */
        pAllocInfo->mostUsedItem = 0;
    }
    return OK;
}

BSP_BOOL bsp_ptr_invalid(const void* pMem)
{
    u32 u32Type;
    MEM_POOL_INFO* pPoolInfo;
    u32 u32FindMem = 0;
    if(NULL == pMem)
    {
        return FALSE;
    }
    for (u32Type = MEM_NORM_DDR_POOL; u32Type < MEM_POOL_MAX; u32Type++)
    {
        pPoolInfo = &(MEM_GET_ALLOC_INFO(u32Type)->memPoolInfo);
        if ((u32)pMem >= pPoolInfo->u32BaseAddr ||
            (u32)pMem < pPoolInfo->u32BaseAddr + pPoolInfo->u32Size)
        {
            u32FindMem = 1;
        }
    }
    if (!u32FindMem ||
        MEM_MAGIC_NUM != MEM_ITEM_MAGIC(pMem) ||
        MEM_ITEM_FLAGS(pMem) >= (u32)MEM_POOL_MAX)
    {
        return TRUE;
    }
    return FALSE;
}

void* bsp_pool_alloc(MEM_ALLOC_INFO* pAllocInfo, u32 u32Size)
{
   u32 u32RetAddr = 0;

   if (pAllocInfo->memPoolInfo.u32Left < u32Size)
   {
        mem_print_error("alloc fail! left size = %x alloc size = %x", pAllocInfo->memPoolInfo.u32Left, u32Size);
        return NULL;
   }

   u32RetAddr = pAllocInfo->memPoolInfo.u32CurPosAddr;
   if (u32RetAddr < 0xF0000000)
   {
   	    u32RetAddr = IO_ADDRESS(u32RetAddr);
   }

   pAllocInfo->memPoolInfo.u32CurPosAddr += u32Size;
   pAllocInfo->memPoolInfo.u32Left -= u32Size;

   return (void*)(u32RetAddr+pAllocInfo->memPoolInfo.u32MgrSize);
}

void* bsp_get_item(MEM_ALLOC_INFO* pAllocInfo, u32 cnt, u32 u32PoolType, u32 u32Size)
{
    void *pItem;
    void **ppHead = &(pAllocInfo->allocList[cnt]);

    /* ���������û�нڵ�,����ڴ���з��� */
    if (!*ppHead)
    {
        /* �ж��Ƿ�ﵽ������ */
        if((u32PoolType != MEM_ICC_DDR_POOL) || (pAllocInfo->allocNum[cnt] < sg_AllocMaxNum[cnt]))
        {
	        /* ע����ڴ���з���ĳߴ�Ҫ������� MGR �Ĳ��� */
	        pItem = bsp_pool_alloc(pAllocInfo, u32Size+pAllocInfo->memPoolInfo.u32MgrSize);
	        if (NULL == pItem)
	        {
	            pAllocInfo->u32AllocFailCnt++;
	            return NULL;
	        }
	        MEM_ITEM_MAGIC(pItem) = (u32)MEM_MAGIC_NUM;
	        MEM_ITEM_SIZE(pItem) = u32Size;
	        MEM_ITEM_FLAGS(pItem) = u32PoolType;
            if(MEM_ICC_DDR_POOL == u32PoolType)
            {
                pAllocInfo->allocNum[cnt]++;
            }

	 #ifdef __BSP_MEM_DEBUG__
	        MEM_ITEM_FILE_NAME(pItem) = 0;
	        MEM_ITEM_LINE(pItem) = 0;
	 #endif
        }
        else
        {
            pAllocInfo->u32AllocFailCnt++;
            return NULL;
        }
    }
    /* ��������ȡ���ڵ� */
    else
    {
	    pItem = (void*)PHY_TO_VIRT((unsigned int)*ppHead);

        /* Invalid Cache */
        MEM_INVALID_CACHE_BY_TYPE(MEM_GET_ALLOC_ADDR(pItem), MEM_MGR_SIZE_FOR_CACHE, u32PoolType);
        *ppHead = (void*)MEM_ITEM_NEXT(pItem);
    }
    return pItem;
}

u8* bsp_memory_alloc(u32 u32PoolType, u32 u32Size)
{
    u32 cnt;
    void *pItem;
    MEM_ALLOC_INFO* pAllocInfo = MEM_GET_ALLOC_INFO(u32PoolType);
    u32 u32MostUsedItem = pAllocInfo->mostUsedItem;

    /* �Ȳ���AllocList���Ƿ��п��õ��ڴ�ڵ� */
    MEM_FIND_RIGHT_ITEM(cnt, u32Size, u32MostUsedItem);

    /* ���û���ҵ���ֱ�ӷ���ʧ�� */
    if (cnt >= MEM_ALLOC_LIST_NUM)
    {
        printk(
                  "Invalid malloc size:%d, line:%d\n", u32Size, __LINE__);
        return NULL;
    }


    /* ����sizeΪ�б��е�size */
    u32Size = MEM_GET_ALLOC_SIZE(cnt);
    /*lint -save -e718 -e746*/
    MEM_LOCK_BY_TYPE(u32PoolType);
    /*lint -restore*/
    pItem = bsp_get_item(pAllocInfo, cnt, u32PoolType, u32Size);

    if (NULL != pItem)
    {
#ifdef __BSP_MEM_DEBUG__
        pAllocInfo->allocUsedInfoList[cnt].u32CurNum++;
        pAllocInfo->allocUsedInfoList[cnt].u32TotalMallocNum++;

        if (pAllocInfo->allocUsedInfoList[cnt].u32CurNum >
            pAllocInfo->allocUsedInfoList[cnt].u32MaxNum)
        {
            pAllocInfo->allocUsedInfoList[cnt].u32MaxNum =
                pAllocInfo->allocUsedInfoList[cnt].u32CurNum;
        }
        MEM_ITEM_STATUS(pItem) = MEM_ALLOC;
#endif
        /* ���Ҫ Flush Cache, ȷ��������Ϣд�� */
        MEM_FLUSH_CACHE_BY_TYPE(MEM_GET_ALLOC_ADDR(pItem), MEM_MGR_SIZE_FOR_CACHE, u32PoolType);
    }

    MEM_UNLOCK_BY_TYPE(u32PoolType);

    return pItem;
}

void bsp_memory_free(u32 u32PoolType, void* pMem, u32 u32Size)
{

    u32 cnt;
    u32 u32MostUsedItem;

    MEM_ALLOC_INFO* pAllocInfo = MEM_GET_ALLOC_INFO(u32PoolType);

    u32MostUsedItem = pAllocInfo->mostUsedItem;
    /* �Ȳ���AllocList���Ƿ��п��õ��ڴ�ڵ� */
    MEM_FIND_RIGHT_ITEM(cnt, u32Size, u32MostUsedItem);
#ifdef __BSP_MEM_DEBUG__
    /* �жϸýڵ��Ƿ���Ч */
    if (cnt >= MEM_ALLOC_LIST_NUM)
    {
        printk("bsp_pool_alloc Fail, size:%d, line:%d\n", u32Size, __LINE__);
        return;
    }
#endif

    MEM_LOCK_BY_TYPE(u32PoolType);

    /* ��item�һص����� */
    if (MEM_ICC_AXI_POOL == u32PoolType)
    {
        MEM_ITEM_NEXT(pMem) = (u32)(pAllocInfo->allocList[cnt]);
        pAllocInfo->allocList[cnt] = (void*)(DRV_AXI_VIRT_TO_PHY((u32)pMem));
    }
    else if (MEM_ICC_DDR_POOL == u32PoolType)
    {
        MEM_ITEM_NEXT(pMem) = (u32)(pAllocInfo->allocList[cnt]);
        pAllocInfo->allocList[cnt] = (void *)SHD_DDR_V2P((u32)pMem);
    }

#ifdef __BSP_MEM_DEBUG__
    pAllocInfo->allocUsedInfoList[cnt].u32CurNum--;
    pAllocInfo->allocUsedInfoList[cnt].u32TotalFreeNum++;
    MEM_ITEM_STATUS(pMem) = MEM_FREE;
#endif
    /* Flush Cache */
    MEM_FLUSH_CACHE_BY_TYPE(MEM_GET_ALLOC_ADDR(pMem), MEM_MGR_SIZE_FOR_CACHE, u32PoolType);

    MEM_UNLOCK_BY_TYPE(u32PoolType);
    return;
}

/* ��ʼ���Զ������ */
int bsp_usr_init(void)
{
    return 0;
}

s32 bsp_mem_ccore_reset_cb(DRV_RESET_CALLCBFUN_MOMENT enParam, int userdata)
{
    u32 u32PoolType = 0;
    u32 u32MaxInitNum = 0;
    if(DRV_RESET_CALLCBFUN_RESET_BEFORE == enParam)
    {
        *g_mem_init_mark = 0;
        MEM_LOCK_BY_TYPE(MEM_ICC_DDR_POOL);
        memset((void*)sg_pIccAllocInfo, 0, (sizeof(MEM_ALLOC_INFO)));
        u32MaxInitNum = MEM_POOL_MAX;
        for (u32PoolType = (u32)MEM_NORM_DDR_POOL; u32PoolType < (u32)u32MaxInitNum; u32PoolType++)
        {
            /* �����ڴ�� */
            (void)bsp_init_poll(u32PoolType);
            (void)bsp_set_most_used_size(512, u32PoolType);
        }
        *g_mem_init_mark = 1;
        MEM_UNLOCK_BY_TYPE(MEM_ICC_DDR_POOL);
    }
    return BSP_OK;
}
/**************************************************************************
  �ӿ�ʵ��
**************************************************************************/
s32 bsp_mem_init(void)
{
    u32 u32PoolType = 0;
    u32 u32MaxInitNum = 0;

    memset((void *)SHM_MEMMGR_FLAG_ADDR, 0, SHM_MEMMGR_FLAG_SIZE);

    g_mem_init_mark = (u32* )MEM_CTX_ADDR;
    sg_pAllocSizeTbl = (u32*)(MEM_CTX_ADDR + MEM_CTX_RESERVED);
    sg_pIccAllocInfo = (MEM_ALLOC_INFO*)(MEM_CTX_ADDR + sizeof(sg_AllocListSize) + MEM_CTX_RESERVED);

    memset((void*)sg_pIccAllocInfo, 0, (sizeof(MEM_ALLOC_INFO)));
    memcpy(sg_pAllocSizeTbl, sg_AllocListSize, sizeof(sg_AllocListSize));
    mb();

    u32MaxInitNum = MEM_POOL_MAX;
    if (bsp_usr_init() != OK)
    {
        printk("bsp_usr_init call fail, line:%d\n", __LINE__);
    }

    for (u32PoolType = (u32)MEM_NORM_DDR_POOL; u32PoolType < (u32)u32MaxInitNum; u32PoolType++)
    {
        /* �����ڴ�� */
        if (OK != bsp_init_poll(u32PoolType))
        {
            return ERROR;
        }

        (void)bsp_set_most_used_size(512, u32PoolType);
        mb();
    }
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_MEM, "[mem]: <bsp_mem_init> memory init OK!\r\n");
    /*��ʼ���ɹ���ʾ*/
    *g_mem_init_mark = 1;
    mb();
    return OK;
}
module_init(bsp_mem_init);

s32 bsp_set_most_used_size(u32 u32Size, u32 u32PoolType)
{
    u32 u32Item;

    if (u32PoolType != MEM_ICC_DDR_POOL)
    {
        return ERROR;
    }

    for ((u32Item) = 0; (u32Item) < MEM_ALLOC_LIST_NUM && u32Size > MEM_GET_ALLOC_SIZE(u32Item); (u32Item)++)
        ;

    if (u32Item >= MEM_ALLOC_LIST_NUM)
    {
        printk("invalid size:%d, line:%d\n", u32Size, __LINE__);
        return ERROR;
    }

    /* ����ʱҪ���� MostItem - 1, ������� */
    MEM_GET_ALLOC_INFO(u32PoolType)->mostUsedItem  = (u32Item == 0) ? (0) : (u32Item-1);

    return OK;
}

/*****************************************************************************
* �� �� ��  : bsp_malloc
*
* ��������  : BSP ��̬�ڴ����
*
* �������  : u32Size: ����Ĵ�С(byte)
*             enFlags: �ڴ�����(�ݲ�ʹ��,Ԥ��)
* �������  : ��
* �� �� ֵ  : ����������ڴ�ָ��
*****************************************************************************/
void* bsp_malloc(u32 u32Size, MEM_POOL_TYPE enFlags)
{
    u8 *pItem;
    pItem = (u8 *)kmalloc(u32Size, GFP_KERNEL);
    return (void*)pItem;
}
EXPORT_SYMBOL(bsp_malloc);

/*****************************************************************************
* �� �� ��  : bsp_malloc_dbg
*
* ��������  : BSP ��̬�ڴ����(Debug�ӿ�)
*
* �������  : u32Size: ����Ĵ�С(byte)
*             enFlags: �ڴ�����(�ݲ�ʹ��,Ԥ��)
*             pFileName: ʹ�õ�Դ�ļ�
*             u32Line:   �����ļ����к�
* �������  : ��
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
void* bsp_malloc_dbg(u32 u32Size, MEM_POOL_TYPE enFlags, u8* pFileName, u32 u32Line)
{
    u8 *pItem;

    /* �����ڴ� */
    pItem = bsp_memory_alloc(MEM_NORM_DDR_POOL, (u32)u32Size);

#ifdef __BSP_MEM_DEBUG__
    if (NULL != pItem)
    {
        /* DebugģʽҪ����MGR ��Ϣ */
        MEM_ITEM_LINE(pItem) = u32Line;
        MEM_ITEM_FILE_NAME(pItem) = (u32)pFileName;
    }
#endif
    return (void*)pItem;
}

/*****************************************************************************
* �� �� ��  : bsp_free
*
* ��������  : BSP ��̬�ڴ��ͷ�
*
* �������  : pMem: ��̬�ڴ�ָ��
* �������  : ��
* �� �� ֵ  : ��
*****************************************************************************/
void  bsp_free(void* pMem)
{
	if(pMem)
        /* coverity[freed_arg] */
	    kfree(pMem);
}
EXPORT_SYMBOL(bsp_free);

/*****************************************************************************
* �� �� ��  : BSP_Free
*
* ��������  : BSP ��̬�ڴ��ͷ�(Debug�ӿ�)
*
* �������  : pMem: ��̬�ڴ�ָ��
*             pFileName: ʹ�õ�Դ�ļ�
*             u32Line:   �����ļ����к�
*
* �������  : ��
* �� �� ֵ  : ��
*****************************************************************************/
void  bsp_free_dbg(void* pMem, u8* pFileName, u32 u32Line)
{
	/* coverity[USE_AFTER_FREE] */
#ifdef __BSP_MEM_DEBUG__
    /* ��鵱ǰ�ڴ��Ƿ���Ч */
    if (bsp_ptr_invalid(pMem))
    {
        printk("invalid mem block, ptr:0x%x, line:%d\n", (unsigned int)pMem, __LINE__);
        return;
    }

    if (MEM_FREE == MEM_ITEM_STATUS(pMem) ||
        MEM_NORM_DDR_POOL != MEM_ITEM_FLAGS(pMem))
    {
        printk("warning! ptr:0x%x, may free twice, or wrong mem flags line:%d\n", (unsigned int)pMem, __LINE__);
        return;
    }
#endif
	/* coverity[freed_arg] */
    bsp_free(pMem);

#ifdef __BSP_MEM_DEBUG__
    /* ����MGR Debug��Ϣ */
	/* coverity[use_after_free] */
    MEM_ITEM_LINE(pMem) = u32Line;
    MEM_ITEM_FILE_NAME(pMem) = (u32)pFileName;
#endif

    return;
}

/*****************************************************************************
* �� �� ��  : bsp_smalloc
*
* ��������  : BSP ��̬�ڴ����(��spin lock����,��˳���ʹ��)
*
* �������  : u32Size: ����Ĵ�С(byte)
*             enFlags: �ڴ�����(�ݲ�ʹ��,Ԥ��)
* �������  : ��
* �� �� ֵ  : ����������ڴ�ָ��
*****************************************************************************/
void* bsp_smalloc(u32 u32Size, MEM_POOL_TYPE enFlags)
{
    u8 *pItem;
    if(0 == *g_mem_init_mark)
    {
        return NULL;
    }
    /* �����ڴ� */
    pItem = bsp_memory_alloc((u32)enFlags, u32Size);

    return (void*)pItem;
}
EXPORT_SYMBOL(bsp_smalloc);

/*****************************************************************************
* �� �� ��  : bsp_smalloc_dbg
*
* ��������  : BSP ��̬�ڴ����(��spin lock����,��˳���ʹ��)(Debug�ӿ�)
*
* �������  : u32Size: ����Ĵ�С(byte)
*             enFlags: �ڴ�����(�ݲ�ʹ��,Ԥ��)
*             pFileName: ʹ�õ�Դ�ļ�
*             u32Line:   �����ļ����к�
* �������  : ��
* �� �� ֵ  : �ɹ�/ʧ��
*****************************************************************************/
void* bsp_smalloc_dbg(u32 u32Size, MEM_POOL_TYPE enFlags, u8* pFileName, u32 u32Line)
{
    u8 *pItem;
    if(0 == *g_mem_init_mark)
    {
        return NULL;
    }

#ifdef __BSP_MEM_DEBUG__
    if ((u32)enFlags >= MEM_POOL_MAX)
    {
        printk("invalid mem enFlags:%d, line:%d\n", (u32)enFlags, __LINE__);
        return NULL;
    }
#endif

    /* �����ڴ� */
    pItem = bsp_memory_alloc((u32)enFlags, u32Size);

    return (void*)pItem;
}

/*****************************************************************************
* �� �� ��  : bsp_sfree
*
* ��������  : BSP ��̬�ڴ��ͷ�(��spin lock����,��˳���ʹ��)
*
* �������  : pMem: ��̬�ڴ�ָ��
* �������  : ��
* �� �� ֵ  : ��
*****************************************************************************/
void  bsp_sfree(void* pMem)
{
    u32 u32Size;
    u32 u32Flags;

    if(0 == *g_mem_init_mark)
    {
        return;
    }

    /* Invalid Cache */
    if (!MEM_IS_AXI_ADDR(pMem))
    {
        MEM_INVALID_CACHE(MEM_GET_ALLOC_ADDR(pMem), MEM_MGR_SIZE_FOR_CACHE);
    }

#ifdef __BSP_MEM_DEBUG__
    /* ��鵱ǰ�ڴ��Ƿ���Ч */
    if (bsp_ptr_invalid(pMem)                     ||
        MEM_FREE == MEM_ITEM_STATUS(pMem)       ||
        MEM_ITEM_FLAGS(pMem) == MEM_NORM_DDR_POOL)
    {
        printk("warning! ptr:0x%x, invalid mem block, or may free twice, or wrong mem flags line:%d\n", (unsigned int)pMem, __LINE__);
        return;
    }
#endif

    u32Size = MEM_ITEM_SIZE(pMem);
    u32Flags = MEM_ITEM_FLAGS(pMem);

    bsp_memory_free(u32Flags, pMem, u32Size);
    return;
}
EXPORT_SYMBOL(bsp_sfree);

/*****************************************************************************
* �� �� ��  : BSP_SFree
*
* ��������  : BSP ��̬�ڴ��ͷ�(��spin lock����,��˳���ʹ��)(Debug�ӿ�)
*
* �������  : pMem: ��̬�ڴ�ָ��
*             pFileName: ʹ�õ�Դ�ļ�
*             u32Line:   �����ļ����к�
*
* �������  : ��
* �� �� ֵ  : ��
*****************************************************************************/
void  bsp_sfree_dbg(void* pMem, u8* pFileName, u32 u32Line)
{
    if(0 == *g_mem_init_mark)
    {
        return;
    }
    bsp_sfree(pMem);
    return;
}

/**************************************************************************
  ������Ϣʵ��
**************************************************************************/
static const char* g_memPoolName[] =
{
    "MEM_NORM_DDR_POOL",
    "MEM_ICC_DDR_POOL",
    "MEM_ICC_AXI_POOL",

    "MEM_POOL_MAX"
};

s32 bsp_mem_info(u32 u32MemType)
{
    u32 u32Item;
    MEM_ALLOC_INFO* pAllocInfo;

    if (u32MemType >= MEM_POOL_MAX)
    {
        printk("invalid pool type:%d\n", u32MemType);
        return ERROR;
    }

        pAllocInfo = MEM_GET_ALLOC_INFO(u32MemType);
        printk("Dump Mem (%s):\n", g_memPoolName[u32MemType]);
        printk("================================\n");
        printk("mem pool info:\n");
        printk("--------------------------------\n");
        printk("Base Addr:          0x%x\n", pAllocInfo->memPoolInfo.u32BaseAddr);
        printk("Total Size:         %d(0x%x)\n", pAllocInfo->memPoolInfo.u32Size, pAllocInfo->memPoolInfo.u32Size);
        printk("Left Size:          %d(0x%x)\n", pAllocInfo->memPoolInfo.u32Left, pAllocInfo->memPoolInfo.u32Left);
        printk("Cur Pos Addr:       0x%x\n", pAllocInfo->memPoolInfo.u32CurPosAddr);
        printk("Mgr Size:           %d\n", pAllocInfo->memPoolInfo.u32MgrSize);
        printk("Most Used Item:     %d\n", pAllocInfo->mostUsedItem);
        printk("Alloc Fail Count:   %d\n", pAllocInfo->u32AllocFailCnt);
        printk("\n");
        printk("================================\n");
        printk("mem list used info:\n");
        printk("--------------------------------\n");
        for (u32Item = 0; u32Item < MEM_ALLOC_LIST_NUM; u32Item++)
        {
            printk("+-- dump size:%d list info:\n", MEM_GET_ALLOC_SIZE(u32Item));
            printk("|-- cur alloc num:  %d\n", pAllocInfo->allocUsedInfoList[u32Item].u32CurNum);
            printk("|-- max alloc num:  %d\n", pAllocInfo->allocUsedInfoList[u32Item].u32MaxNum);
	     if(u32MemType== MEM_ICC_DDR_POOL)
	     {
	         printk("|-- min num line:   %d\n", sg_AllocMinNum[u32Item]);
	         printk("|-- max num line:   %d\n", sg_AllocMaxNum[u32Item]);
	     }
            printk("|-- total alloc num:%d\n", pAllocInfo->allocUsedInfoList[u32Item].u32TotalMallocNum);
            printk("|-- total free num: %d\n", pAllocInfo->allocUsedInfoList[u32Item].u32TotalFreeNum);
            printk("+-- \n");
        }
        printk("================================\n");
    return 0;
}

s32 bsp_mem_dump_block(u32 u32Addr)
{
    BSP_BOOL bBlockInvalid = FALSE;

    if (!u32Addr)
    {
        printk("can't dump the block, the input ptr is NULL\n");
        return ERROR;
    }

    if (MEM_MAGIC_NUM != MEM_ITEM_MAGIC(u32Addr))
    {
        bBlockInvalid = TRUE;
        printk("The Block:0x%x may Invalid!\n", u32Addr);
    }

    /* Ϊȷ����ȷ,ͳһˢ��Cache */
    MEM_INVALID_CACHE(MEM_GET_ALLOC_ADDR(u32Addr), MEM_MGR_SIZE_FOR_CACHE);

    printk("================================\n");
    printk("dump mem block info:\n");
    printk("================================\n");
    printk("usr ptr:            0x%x\n",    u32Addr);
    printk("alloc ptr:          0x%x\n",    (u32)MEM_GET_ALLOC_ADDR(u32Addr));
    printk("magic num:          0x%x\n",    MEM_ITEM_MAGIC(u32Addr));
    printk("block size:         %d\n",      MEM_ITEM_SIZE(u32Addr));
    printk("mem flags:          0x%x\n",    MEM_ITEM_FLAGS(u32Addr));
    printk("mem item next:      0x%x\n",    MEM_ITEM_NEXT(u32Addr));
#ifdef __BSP_MEM_DEBUG__
    printk("used file name:     %s\n",      (bBlockInvalid) ? ("invalid") : (BSP_CHAR*)MEM_ITEM_FILE_NAME(u32Addr));
    printk("used line:          %d\n",      MEM_ITEM_LINE(u32Addr));
    printk("mem status:         %s\n",      (MEM_ITEM_STATUS(u32Addr)) ? ("ALLOC") : ("FREE"));
#endif
    printk("================================\n");
    return 0;
}

s32 bsp_mem_dump_alloc_list(u32 u32Size, u32 u32PoolType)
{
    u32 u32Item;
    u32 u32MostUsedItem;
    void* pCurAddr;
    u32 u32Num = 0;
    MEM_ALLOC_INFO* pAllocInfo;

    if (u32PoolType >= MEM_POOL_MAX)
    {
        printk("invalid pool type:%d\n", u32PoolType);
        return ERROR;
    }
    pAllocInfo = MEM_GET_ALLOC_INFO(u32PoolType);
    u32MostUsedItem = pAllocInfo->mostUsedItem;
    MEM_FIND_RIGHT_ITEM(u32Item, u32Size, u32MostUsedItem);

    if (u32Item >= MEM_ALLOC_LIST_NUM)
    {
        printk("invalid size:%d\n", u32Size);
        return -1;
    }

    printk("dump alloc list beg:\n");

    pCurAddr = (void*)PHY_TO_VIRT((unsigned int)pAllocInfo->allocList[u32Item]);
    for (;NULL != pCurAddr; pCurAddr = (void*)MEM_ITEM_NEXT(pCurAddr))
    {
        (void)bsp_mem_dump_block((u32)pCurAddr);
        u32Num++;
    }

    printk("dump alloc list end, num:%d\n", u32Num);
    return 0;
}

s32 bsp_mem_scan(u32 u32PoolType)
{
    MEM_ALLOC_INFO* pAllocInfo = NULL;
    u32 u32CurScan;
    u32 u32EndAddr;
    u32 u32MgrSize;

    pAllocInfo = MEM_GET_ALLOC_INFO(u32PoolType);
    if (u32PoolType >= MEM_POOL_MAX)
    {
        printk("invalid pool type:%d\n", u32PoolType);
        return ERROR;
    }
    u32CurScan = pAllocInfo->memPoolInfo.u32BaseAddr;
    u32MgrSize = pAllocInfo->memPoolInfo.u32MgrSize;
    u32EndAddr = pAllocInfo->memPoolInfo.u32CurPosAddr;

    printk("Beg Scan Mem (%s):\n", g_memPoolName[u32PoolType]);
    printk("================================\n");
    printk("mem pool info:\n");
    printk("--------------------------------\n");
    printk("Base Addr:          0x%x\n", pAllocInfo->memPoolInfo.u32BaseAddr);
    printk("Total Size:         %d(0x%x)\n", pAllocInfo->memPoolInfo.u32Size, pAllocInfo->memPoolInfo.u32Size);
    printk("Left Size:          %d(0x%x)\n", pAllocInfo->memPoolInfo.u32Left, pAllocInfo->memPoolInfo.u32Left);
    printk("Cur Pos Addr:       0x%x\n", pAllocInfo->memPoolInfo.u32CurPosAddr);
    printk("Mgr Size:           %d\n", pAllocInfo->memPoolInfo.u32MgrSize);
    printk("Most Used Item:     %d\n", pAllocInfo->mostUsedItem);
    printk("\n");

    /* ɨ�� MemPool */
    while(u32CurScan < u32EndAddr)
    {
        /* ���õ�usr ptr */
        u32CurScan += u32MgrSize;

        /* ��ӡ��ǰ�ڴ����Ϣ */
        (void)bsp_mem_dump_block(u32CurScan);

        /* �ƶ�����һ���ڴ�� */
        u32CurScan += MEM_ITEM_SIZE(u32CurScan);

        printk("\npress \'Enter\' to continue, press \'q\' to stop scan\n");
    }

    return OK;
}
/*lint -restore +e413 +e19*/
/**************************************************************************
**************************************************************************/



