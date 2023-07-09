/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

#include <linux/slab.h>
#include <linux/device.h>

#include <drm/drm_file.h>
#if defined(OS_DRM_DRMP_H_EXIST)
#include <drm/drmP.h>
#else
#include <drm/drm_device.h>
#include <drm/drm_ioctl.h>
#endif

#ifdef SUPPORT_ION
#include "ion/ion.h"
#endif

#include "mtgpu_mdev.h"
#include "mtgpu_drm_gem.h"
#include "mtgpu_drm_drv.h"
#include "mtgpu_drv.h"

#include "osfunc.h"

#include "mtvpu_drv.h"
#include "mtvpu_gem.h"
#include "vpuapifunc.h"

#include "misc.h"

#ifdef SUPPORT_ION
u64 get_dev_addr_dma_buf(struct dma_buf *psDmaBuf)
{
	struct ion_buffer *buffer;
	struct sg_table *table;
	ion_phys_addr_t paddr;

	if (!psDmaBuf)
		return 0;
	buffer = psDmaBuf->priv;
	if (!buffer)
		return 0;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 8, 0))
	table = buffer->priv_virt;
#else
	table = buffer->sg_table;
#endif
	paddr = sg_dma_address(table->sgl);
	return paddr;
}

int ion_free_node(struct mt_node *node)
{
	struct dma_buf *psDmaBuf;
	struct ion_buffer *ion_buf;

	if (!node)
		return -1;
	psDmaBuf = node->ion_buf;
	if (!psDmaBuf)
		return -1;
	ion_buf = psDmaBuf->priv;

	return ion_free(ion_buf);
}

struct mt_node *ion_malloc_internal(struct mt_chip *chip, int idx, struct drm_device *drm, u64 size)
{
	struct mt_node *node;
	struct dma_buf *ion_buf;

	size = ALIGN(size, gpu_page_size);
	node = kzalloc(sizeof(*node), GFP_KERNEL);
	if (!node)
		return NULL;

	node->obj = kzalloc(sizeof(*node->obj), GFP_KERNEL);
	if (!node->obj) {
		kfree(node);
		return NULL;
	}

	ion_buf = ion_alloc(chip->ion_dev[mtvpu_get_drm_dev_id(drm)], size, 1 << chip->core[idx].heap_id, 0);
	if (!ion_buf) {
		kfree(node->obj);
		kfree(node);
		pr_err("ion allocate size %lld failed\n", size);
		return NULL;
	}
	node->ion_buf = ion_buf;
	node->dev_addr = get_dev_addr_dma_buf(ion_buf);
	node->obj->size = ion_buf->size;
	pr_info("core %d ion malloc internal 0x%llx\n", idx, node->dev_addr);
	return node;
}
#endif

struct mt_node *gem_malloc_internal(struct mt_chip *chip, int idx, struct drm_device *drm, u64 size, u32 mem_type)
{
	struct mt_core *core = &chip->core[idx];
	struct mt_node *node;
	u64 dev_addr;
	size_t obj_size;
	int ret = 0;

	size = ALIGN(size, gpu_page_size);
	node = kzalloc(sizeof(*node), GFP_KERNEL);
	if (!node)
		return NULL;

	node->obj = kzalloc(sizeof(struct drm_gem_object), GFP_KERNEL);
	if (!node->obj) {
		kfree(node);
		return NULL;
	}

	drm_gem_private_object_init(drm, node->obj, size);

	if (chip->conf.type == TYPE_QUYU2 && vpu_fixed_128M_mem(mem_type))
		ret = mtgpu_vram_alloc(drm, MTGPU_VGPU_VPU_NON_GROUP, size,
					   &node->dev_addr, &node->handle);
	else
		ret = mtgpu_vram_alloc(drm, core->mem_group_id, size,
					  &node->dev_addr, &node->handle);

	if (ret) {
		pr_err("error, mtgpu_vram_alloc\n");
		goto unref;
	}

	dev_addr = get_mt_node_addr(node);
	obj_size = get_mt_node_size(node);

	return node;

unref:
	drm_gem_object_release(node->obj);
	kfree(node->obj);
	kfree(node);
	return NULL;
}

/* check work buffer, task buffer cross with code buffer.
 * refer: https://confluence.mthreads.com/display/VD/VPU+buf+require#
 */
struct mt_node *gem_malloc(struct mt_chip *chip, int idx, int instIdx, u64 size,
			u32 mem_type, struct list_head *mm_head)
{
	struct mt_node *node = NULL;
	int bak_count = 0;
	CodecInst *pCodecInst;

	pCodecInst = (CodecInst *)chip->core[idx].pool.codecInstPool[instIdx];

	mutex_lock(chip->mm_lock);
	do {
		if (node) {
			bak_count++;
			pr_info("bak_count: %d, dev_addr: 0x%llx\n", bak_count, node->dev_addr);
			list_add(&node->list, mm_head);
		}
#ifdef SUPPORT_ION
		node = ion_malloc_internal(chip, idx, pCodecInst->drm, size);
#else
		node = gem_malloc_internal(chip, idx, pCodecInst->drm, size, mem_type);
#endif
		if (!node) {
			mutex_unlock(chip->mm_lock);
			return NULL;
		}
	} while ((node->dev_addr & 0xffffffffUL) < (WAVE5_MAX_CODE_BUF_SIZE + FW_LOG_BUFFER_SIZE) ||
		((node->dev_addr + size) & 0xffffffffUL) >= W_VCPU_SPM_ADDR);

	list_add(&node->list, mm_head);

	mutex_unlock(chip->mm_lock);

	return node;
}

/* for ANDROID, gem malloc cannot use ION during insmod */
void gem_free_internal(struct mt_chip *chip, struct mt_node *node)
{
	if (node->vir_addr)
		iounmap(node->vir_addr);

	if (node->bak_addr)
		vfree(node->bak_addr);

	mtgpu_vram_free(node->handle);
	drm_gem_object_release(node->obj);
}

void gem_free_node(struct mt_chip *chip, struct mt_node *node)
{
	if (node->vir_addr)
		iounmap(node->vir_addr);

	if (node->bak_addr)
		vfree(node->bak_addr);

#ifdef SUPPORT_ION
	ion_free_node(node);
#else
	mtgpu_vram_free(node->handle);
	drm_gem_object_release(node->obj);
#endif
}

void gem_free_all(struct mt_chip *chip, struct list_head *mm_head)
{
	struct mt_node *node, *next;

	mutex_lock(chip->mm_lock);

	list_for_each_entry_safe(node, next, mm_head, list) {
		gem_free_node(chip, node);

		list_del(&node->list);
		kfree(node->obj);
		kfree(node);
	}

	mutex_unlock(chip->mm_lock);
}

void gem_free(struct mt_chip *chip, u64 dev_addr, struct list_head *mm_head)
{
	struct mt_node *node;

	mutex_lock(chip->mm_lock);

	list_for_each_entry(node, mm_head, list) {
		if (node->dev_addr == dev_addr) {
			gem_free_node(chip, node);

			list_del(&node->list);
			kfree(node->obj);
			kfree(node);
			break;
		}
	}

	mutex_unlock(chip->mm_lock);
}

void gem_clear(struct mt_core *core, u64 dev_addr)
{
	struct mt_node *node;
	int inst;

	for (inst = 0; inst < INST_MAX_SIZE; inst ++) {
		list_for_each_entry(node, &core->mm_head[inst], list) {
			if (node->dev_addr == dev_addr) {
				memset_io(node->vir_addr, 0, node->obj->size);
				return;
			}
		}
	}
}
