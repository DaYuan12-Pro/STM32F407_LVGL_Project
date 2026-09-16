#ifndef __MALLOC_H
#define __MALLOC_H

#include <stdint.h>
#include <stddef.h>
#include "sys.h"

/* 内存池编号 */
#define SRAMIN      0      /* 内部 SRAM */
#define SRAMCCM     1      /* CCM，仅 CPU 可访问，不可用于 DMA */
#define SRAMEX      2      /* 外部 SRAM */
#define SRAMBANK    3

/* 内存管理表元素类型 */
#define MT_TYPE     uint16_t

/* 内部 SRAM：20 KB 内存池 */
#define MEM1_BLOCK_SIZE         32
#define MEM1_MAX_SIZE           (20 * 1024)
#define MEM1_ALLOC_TABLE_SIZE   (MEM1_MAX_SIZE / MEM1_BLOCK_SIZE)

/* CCM：60 KB 内存池 */
#define MEM2_BLOCK_SIZE         32
#define MEM2_MAX_SIZE           (20 * 1024)
#define MEM2_ALLOC_TABLE_SIZE   (MEM2_MAX_SIZE / MEM2_BLOCK_SIZE)

/* 外部 SRAM：50 KB 内存池 */
#define MEM3_BLOCK_SIZE         32
#define MEM3_MAX_SIZE           (963 *1024)
#define MEM3_ALLOC_TABLE_SIZE   (MEM3_MAX_SIZE / MEM3_BLOCK_SIZE)

/* 各内存区域还需要额外容纳对应的管理表 */
struct _m_mallco_dev
{
    void (*init)(uint8_t);
    uint16_t (*perused)(uint8_t);

    uint8_t *membase[SRAMBANK];
    MT_TYPE *memmap[SRAMBANK];
    uint8_t memrdy[SRAMBANK];
};

extern struct _m_mallco_dev mallco_dev;

/* 初始化内存管理表；外部 SRAM 必须先完成硬件初始化 */
void my_mem_init(uint8_t memx);

/* 使用率：0～1000，对应 0.0%～100.0% */
uint16_t my_mem_perused(uint8_t memx);

void my_mem_set(void *s, uint8_t c, uint32_t count);
void my_mem_copy(void *des, void *src, uint32_t n);

void myfree(uint8_t memx, void *ptr);
void *mymalloc(uint8_t memx, uint32_t size);
void *myrealloc(uint8_t memx, void *ptr, uint32_t size);

#endif /* __MALLOC_H */