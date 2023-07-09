/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

#include <linux/timer.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <drm/drm_device.h>
#if defined(OS_DRM_DRMP_H_EXIST)
#include <drm/drmP.h>
#else
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#endif
#ifdef SUPPORT_ION
#include "ion/ion.h"
#endif

#include "pvrsrv.h"
#include "mtgpu_mdev.h"
#include "mtgpu_drm_drv.h"
#include "mtgpu_drm_gem.h"
#include "mtvpu_api.h"
#include "mtvpu_drv.h"
#include "misc.h"

bool is_guest_cmds = false;

struct mt_timer_struct {
       struct timer_list timer;
       void *data;
};

struct file_operations vinfo_fops = {
	.owner = THIS_MODULE,
	.read = vpu_info_read,
};

struct file_operations fwinfo_fops = {
	.owner = THIS_MODULE,
	.read = fw_info_read,
};

struct file_operations *get_vinfo_fops(void)
{
	return &vinfo_fops;
}

struct file_operations *get_fwinfo_fops(void)
{
	return &fwinfo_fops;
}

void VLOG(int level, const char *fmt, ...)
{
	va_list args;
	int r;

	// Eliminate log level bigger than warning.
	// For debug, you can change 'WARN' to 'MAX_LOG_LEVEL'
	if (level > WARN)
		return;

	va_start(args, fmt);
	r = vprintk(fmt, args);
	va_end(args);

	return;
}

Uint64 osal_gettime(void)
{
	u64 tv = ktime_get_ns();

	return ((Uint64)tv / 1000000);
}

void *osal_malloc(int size)
{
	return kmalloc(size, GFP_KERNEL);
}

void osal_free(void *p)
{
	kfree(p);
}

void osal_memcpy(void *dst, const void *src, int count)
{
	memcpy(dst, src, count);
}

void *osal_memset(void *dst, int val, int count)
{
	return memset(dst, val, count);
}

struct timer_list *malloc_mt_timer(void)
{
	struct mt_timer_struct *mt_timer;

	return kzalloc(sizeof(*mt_timer), GFP_KERNEL);
}

void set_mt_timer_data(struct timer_list *timer, void *data)
{
	struct mt_timer_struct *mt_timer = (struct mt_timer_struct *)timer;

	if (mt_timer)
		mt_timer->data = data;
}

void *get_mt_timer_data(struct timer_list *timer)
{
	struct mt_timer_struct *mt_timer = (struct mt_timer_struct *)timer;

	if (!mt_timer)
		return NULL;

	return mt_timer->data;
}

struct mt_chip *to_chip(struct drm_device *drm)
{
	struct mtgpu_drm_private *drm_private = drm->dev_private;

	if (!drm_private)
		return NULL;

	return drm_private->chip;
}

struct mtgpu_gem_object *mtvpu_alloc_mtgpu_obj(void)
{
	struct mtgpu_gem_object *mtgpu_obj;

	mtgpu_obj = kzalloc(sizeof(struct mtgpu_gem_object), GFP_KERNEL);
	if (!mtgpu_obj)
		return NULL;

	return mtgpu_obj;
}

struct mtgpu_gem_object *mtvpu_to_mtgpu_obj(struct drm_gem_object *obj)
{
	if (!obj)
		return NULL;

	return to_mtgpu_obj(obj);
}

struct drm_gem_object *get_mtgpu_obj_base(struct mtgpu_gem_object *mtgpu_obj)
{
	if (!mtgpu_obj)
		return NULL;

	return &mtgpu_obj->base;
}

size_t get_mtgpu_obj_size(struct mtgpu_gem_object *mtgpu_obj)
{
	return mtgpu_obj->base.size;
}

u64 get_mtgpu_obj_addr(struct mtgpu_gem_object *mtgpu_obj)
{
	return mtgpu_obj->dev_addr;
}

void *get_mtgpu_obj_handle(struct mtgpu_gem_object *mtgpu_obj)
{
	return mtgpu_obj->handle;
}

struct sg_table *get_mtgpu_obj_sgt(struct mtgpu_gem_object *mtgpu_obj)
{
	return mtgpu_obj->sgt;
}

size_t get_mt_node_size(struct mt_node *node)
{
	return node->obj->size;
}

u64 get_mt_node_addr(struct mt_node *node)
{
	return node->dev_addr;
}

struct mt_file *os_get_drm_file_private_data(struct drm_file *file)
{
	struct mtgpu_drm_file *drv_priv = file->driver_priv;

	return drv_priv->vpu_priv;
}

void os_set_drm_file_private_data(struct drm_file *file, struct mt_file *priv)
{
	struct mtgpu_drm_file *drv_priv = file->driver_priv;

	if (!drv_priv)
		return;

	drv_priv->vpu_priv = priv;
}

int mtvpu_vram_alloc(struct drm_device *drm,
			int group_id,
			size_t size,
			u32 mem_type,
			struct mtgpu_gem_object *mtgpu_obj)
{
	struct mt_chip *chip = to_chip(drm);

	if (chip && chip->conf.type == TYPE_QUYU2 && vpu_fixed_128M_mem(mem_type))
		return mtgpu_vram_alloc(drm, MTGPU_VGPU_VPU_NON_GROUP, size,
					&mtgpu_obj->dev_addr, &mtgpu_obj->handle);
	else
		return mtgpu_vram_alloc(drm, group_id, size, &mtgpu_obj->dev_addr, &mtgpu_obj->handle);
}

void mtvpu_vram_free(void *handle)
{
	mtgpu_vram_free(handle);
}

int mtvpu_get_drm_dev_id(struct drm_device *drm)
{
	struct mt_chip *chip = to_chip(drm);
	int i;

	if (!chip)
		return -1;

	for (i = 0; i < chip->drm_dev_cnt; i++)
		if (drm == chip->drm_dev[i])
			return i;

	return -1;
}

int mtvpu_get_drm_group_id(struct drm_device *drm)
{
	struct mt_chip *chip = to_chip(drm);
	int dev_id;

	if (!chip)
		return -1;

	dev_id = mtvpu_get_drm_dev_id(drm);

	if (dev_id < 0)
		return -1;

	return chip->drm_video_group_idx[dev_id];
}

bool mtvpu_drm_core_valid(struct mt_chip *chip, struct drm_device *drm, u32 core_idx)
{
	struct mt_core *core = &chip->core[core_idx];

	if (!core)
		return false;

	if (core->serve_all)
		return true;
	else if (mtvpu_get_drm_group_id(drm) == mtvpu_get_drm_group_id(core->drm_dev))
		return true;
	else
		return false;
}

void *mtvpu_get_dev_node(struct drm_device *drm)
{
	struct mtgpu_drm_private *drm_private = drm->dev_private;

	return drm_private->pvr_private.dev_node;
}

void mtvpu_get_drm_mode_args(struct drm_mode_create_dumb *args,
			     u32 *height, u32 *width, u32 *bpp,
			     u32 *flags)
{
	*height = args->height;
	*width = args->width;
	*bpp = args->bpp;
	*flags = args->flags;
}

void mtvpu_set_drm_mode_args(struct drm_mode_create_dumb *args, u32 handle, u32 pitch, u32 size)
{
	args->handle = handle;
	args->pitch = pitch;
	args->size = size;
}
