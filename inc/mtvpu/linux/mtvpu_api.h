/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

#ifndef _MTVPU_API_H_
#define _MTVPU_API_H_

#include "linux-types.h"

struct mt_node;
struct mt_file;
struct mt_virm;
struct file;
struct device;
struct timer_list;
struct drm_device;
struct drm_file;
struct drm_gem_object;
struct drm_mode_create_dumb;

extern struct platform_driver vpu_driver;
extern bool is_guest_cmds;

enum {NONE=0, ERR, WARN, INFO, TRACE, MAX_LOG_LEVEL};

void VLOG(int level, const char *fmt, ...);

u64 osal_gettime(void);
void *osal_malloc(int size);
void osal_free(void *p);
void osal_memcpy(void *dst, const void *src, int count);
void *osal_memset(void *dst, int val, int count);

struct mt_chip *to_chip(struct drm_device *drm);

struct mt_file *os_get_drm_file_private_data(struct drm_file *file);
void os_set_drm_file_private_data(struct drm_file *file, struct mt_file *priv);

struct mtgpu_gem_object *mtvpu_to_mtgpu_obj(struct drm_gem_object *obj);
u64 get_mtgpu_obj_addr(struct mtgpu_gem_object *mtgpu_obj);
void *get_mtgpu_obj_handle(struct mtgpu_gem_object *mtgpu_obj);
struct sg_table *get_mtgpu_obj_sgt(struct mtgpu_gem_object *mtgpu_obj);

size_t get_mt_node_size(struct mt_node *node);
u64 get_mt_node_addr(struct mt_node *node);

struct mtgpu_gem_object *mtvpu_alloc_mtgpu_obj(void);

struct drm_gem_object *get_mtgpu_obj_base(struct mtgpu_gem_object *mtgpu_obj);
size_t get_mtgpu_obj_size(struct mtgpu_gem_object *mtgpu_obj);

void mtvpu_set_drm_mode_args(struct drm_mode_create_dumb *args, u32 handle, u32 pitch, u32 size);
void mtvpu_get_drm_mode_args(struct drm_mode_create_dumb *args, u32 *height, u32 *width, u32 *bpp, u32 *flags);

struct timer_list *malloc_mt_timer(void);
void *get_mt_timer_data(struct timer_list *timer);
void set_mt_timer_data(struct timer_list *timer, void *data);

int mtvpu_get_drm_group_id(struct drm_device *drm);
int mtvpu_get_drm_dev_id(struct drm_device *drm);
bool mtvpu_drm_core_valid(struct mt_chip *chip, struct drm_device *drm, u32 core_idx);
void *mtvpu_get_dev_node(struct drm_device *priv);

ssize_t vpu_info_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
ssize_t fw_info_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
struct file_operations *get_vinfo_fops(void);
struct file_operations *get_fwinfo_fops(void);

void mtvpu_vram_free(void *handle);
int mtvpu_vram_alloc(struct drm_device *drm, int segment_id, size_t size, u32 mem_type,
		     struct mtgpu_gem_object *mtgpu_obj);

int mtvpu_drm_open(struct drm_device *drm, struct drm_file *file);
void mtvpu_drm_release(struct drm_device *drm, struct drm_file *file);

#endif /* _MTVPU_API_H_ */
