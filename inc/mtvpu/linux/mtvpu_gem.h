/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

#ifndef _MTVPU_GEM_H_
#define _MTVPU_GEM_H_

#include "mtvpu_drv.h"

extern u32 gpu_page_size;
extern u32 gpu_page_shift;

struct mt_node *gem_malloc_internal(struct mt_chip *chip, int idx, struct drm_device *drm, u64 size, u32 mem_type);
void gem_free_internal(struct mt_chip *chip, struct mt_node *node);
void gem_free_node(struct mt_chip *chip, struct mt_node *node);

struct mt_node *gem_malloc(struct mt_chip *chip, int idx, int instIdx, u64 size, u32 mem_type,
			   struct list_head *mm_head);
void gem_free(struct mt_chip *chip, u64 dev_addr, struct list_head *mm_head);

void gem_free_all(struct mt_chip *chip, struct list_head *mm_head);
void gem_clear(struct mt_core *core, u64 dev_addr);

#ifdef SUPPORT_ION
struct mt_node *ion_malloc_internal(struct mt_chip *chip, int idx, struct drm_device *drm, u64 size);
int ion_free_node(struct mt_node *);
u64 get_dev_addr_dma_buf(struct dma_buf *psDmaBuf);
#endif

#endif /* _MTVPU_GEM_H_ */
